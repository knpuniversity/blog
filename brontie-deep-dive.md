# How We Built Brontie, Our Comment-Thread AI

Every day, students leave questions in the comments under our tutorials. And most
of the time, the answer *already exists* somewhere in our content — the chapter
they're watching, another course, a blog post, the upstream Symfony and Doctrine
docs. Having the answer was never the problem. Being on every thread at once was.

So we built something that can. Meet **Brontie**: a Retrieval-Augmented
Generation (RAG) chatbot that lives right in the comment thread. Mention
`@Brontie` and it writes back, grounded in our *actual* content and aware of
which page you're on. This post is the behind-the-scenes tour — architecture,
tech, trade-offs. Spoiler: the model is the easy part.

## What Brontie had to be

We boiled Brontie down to four non-negotiables. Almost every decision in this
post falls out of one of them:

- **Fast.** Nobody wants to stare at a spinner while an LLM thinks for eight
  seconds — and an HTTP request *really* can't sit and block on that. Whatever
  Brontie did, it had to get out of the request's way and let the page answer
  instantly.
- **Grounded.** Answers have to come from *our* content — the courses, the blog,
  the docs — not from a model cheerfully inventing a plausible-sounding API that
  has never existed. A confident wrong answer is worse than no answer at all.
- **Gated.** Every reply is a real (paid) API call, so Brontie can't be a
  free-for-all. Only paying subscribers — and, at first, a closed beta — get
  through the door.
- **Auditable.** When an answer is great, we want to know why. When it's bad, we
  *really* want to know why. So every prompt and every retrieved chunk gets
  logged — no shrugging at a mystery reply.

Keep those four in your back pocket. They're the *why* behind everything below.

## The shape of it: four layers and an async hand-off

It would have been *so* easy to write Brontie as one giant do-everything service.
We didn't. We split it into four single-responsibility layers instead:

1. **Detection** — `BrontieMentionDetector`: pure logic. Was Brontie mentioned?
   Is this a reply *to* Brontie? Has this thread already hit its reply limit?
2. **Orchestration** — `BrontieChatbotService`: access control, rate limiting,
   creating the placeholder, and kicking off the real work.
3. **Context assembly** — `BrontieContextBuilder`, which builds a
   `BrontiePromptContext`: it gathers the article context, the thread history,
   and the embedding search results into a value object.
4. **Agent execution** — `BrontieChatbotAgent`: assembles the LLM message stack
   and calls the model.

This isn't architecture astronautics — each layer earns its keep by being
*independently testable*. Detection is pure functions over comments. Context
assembly is a deterministic builder. The agent is the *only* piece that talks to
the network. So when an answer comes out weird, the layering tells us exactly
*where* to go looking.

### Never block the response on the LLM

Here's the single most important decision in the whole project: **never block the
HTTP response on the model.**

When you trigger Brontie, the request synchronously drops a placeholder comment —
*"Brontie is thinking…"* — and returns immediately. The real work gets dispatched
as a Symfony Messenger message, `GenerateBrontieReply`, and picked up by a
background worker. That worker does the thinking and then **updates the
placeholder in place**.

```
HTTP request ──► create "Brontie is thinking…" placeholder ──► 200 OK
                             │
                             └─► dispatch GenerateBrontieReply (async)
                                           │
                      worker: build context → call LLM → update placeholder
```

Once the worker finishes, the placeholder updates in place and the answer shows
up when it's ready — no page refresh needed. The whole point of the dance is to
get the *latency out of the request path*: the HTTP response comes back in
milliseconds, while that twelve-second model call happens entirely in the
background. The user sees "Brontie is thinking…" and, a moment later, a real
answer. Nice!

## The RAG pipeline

This is the fun part.

Naive RAG goes like this: embed the question, grab the top-k nearest vectors,
stuff them into the prompt, and hope. It's *easy* to build. It also produces
mediocre, hand-wavy answers — the kind that confidently quote a chunk that only
*looks* relevant. Almost all of our engineering went into the retrieval pipeline
that sits *between* the question and the prompt. Let's walk through it.

### 1. Classify the question — without an LLM call

Before we search for anything, we sort the question into one of four intents with
`QueryIntentClassifier` — a quick read of what the question is *actually* asking:

- `DEBUGGING` — error messages, stack traces, "not working"
- `CONFIGURATION` — YAML, env vars, setup
- `CONCEPT` — "what is", "explain", "difference between"
- `IMPLEMENTATION` — how to actually write it (the default)

We handled this in the traditional way (no LLM involved), keeping the process
simple and fast. We're keeping an eye on it, though — if a plain classifier turns
out not to be good enough, handing the job off to an LLM is always on the table.

### 2. Enrich the query with the page you're on

Before we vectorize the question, we sneak the **article title** onto the front
of it:

```
{articleTitle}: {userQuestion}
```

Tiny change, real difference. It nudges the vector search toward the topic of the
page the student is *actually* on. So "why isn't this saving?" asked under a
Doctrine chapter pulls completely different context than the same question asked
under a Forms chapter.

### 3. Search, scoped by technology

We *could* throw the question at the entire vector store and hope the right
chunks float to the top. But why make Pinecone sift through every Twig tip when
the question is clearly about Doctrine?

So first, we narrow the field. `EmbeddingsContextProvider` sniffs out which techs
the question is about, Doctrine, Turbo, Twig, etc. Then we ask Pinecone for the `top_k: 12` nearest
chunks inside those filters.

Twelve feels like a lot, right? It is — on purpose. The next step earns its
paycheck by throwing most of them out.

### 4. Re-rank twelve hits down to four

Now for the fun part. Those twelve raw hits get whittled down to at most **four**
hand-picked chunks — that's `ChunkReRanker`'s whole reason for existing. Here are
the real knobs, straight from the code:

```php
// ChunkReRanker.php
private const float BOOST_MULTIPLIER = 1.15;  // +15% for preferred content types
private const float SCORE_THRESHOLD  = 0.80;  // drop anything less relevant
private const int   MAX_CHUNKS       = 4;     // final cap injected into the prompt
private const int   MAX_PER_FILE     = 3;     // diversity
private const int   MAX_PER_TOPIC    = 2;     // diversity
```

To make the cut, a chunk has to survive four passes:

1. **Boost.** Remember those intents from step 1? Here's where they pay off. The
   content types our intent prefers get a 15% bump.
2. **Threshold.** Score below `0.80`? Gone. Harsh, sure — but we'd *much* rather
   hand the model nothing than feed it with noise.
3. **Diversity caps.** No more than 3 chunks from one file, 2 from one topic — so
   a single rambling chapter can't hog the whole context window.
4. **Group and slice.** Group by file, sort by score, grab the top four. Done.

A confession: none of those numbers are sacred. We're *still* nudging them around
— boost up a little, threshold down a little — then watching what actually makes
answers better, hunting for the sweet spot on each. Tuning a RAG pipeline is less
"set it and forget it" and more "season to taste."

This is where "grounded, not hallucinated" really comes from. No magic — just
four unglamorous (and constantly re-seasoned) steps.

### Pre-computed chapter summaries

One last grounding trick. Our tutorial scripts are *long* — they're video
transcripts, so they ramble, double back, and toss in the odd joke (guilty as
charged). Great to watch; rough to hand an LLM. It's a *lot* of tokens for not
much signal.

So we don't hand it the transcript. Instead, a background job —
`GenerateChapterAiSummaryMessage` — boils each chapter down to a dense ~400-word
summary and stashes it on the `CourseChapter` entity. *That's* what Brontie reads
first, not the raw script. Better signal, fewer tokens, happier model.

And chapters aren't the only thing in the store. Brontie searches three sources
side by side: course chapters, blog posts (yes — including this one), and the
upstream docs for Symfony, Doctrine, PHPUnit, Behat, Foundry, and EasyAdmin, each
wired in through a shared `AbstractRstLibraryProcessor`. So when the answer lives
in the official docs and not in our own content, Brontie can reach right past our
walls and grab it.

## The tech stack, and why

Here's what's under the hood, and the reasoning behind each pick:

| Concern | Choice | Why |
|---|---|---|
| AI framework | **Symfony AI** (0.9.x) | First-party abstractions (`Agent`, `MessageBag`, `Platform`, `Store`) that fit the rest of the app. |
| LLM (hot path) | **OpenAI `gpt-5.4-mini`** | Brontie's answers must be cheap and fast at volume; the mini model is plenty for grounded, short answers. |
| LLM (offline) | `gpt-5.2` | Reserved for the heavier challenge generator, not Brontie's hot path. |
| Embeddings | **`text-embedding-3-small`** (1536-dim) | Strong quality-per-dollar for retrieval. |
| Vector store | **Pinecone** (index `main`, namespace `chunk-650`) | Managed, fast, scales with our content. |
| Async | **Symfony Messenger** | Already in the app; the natural home for the placeholder hand-off and the indexing jobs. |
| Frontend | **Stimulus + Turbo** (Hotwire) | Matches the existing stack; ratings submit through native Turbo forms and Streams. |

There's a deliberate *non*-choice hiding in that table: we run the **fast, cheap
model on the hot path** and save the smarter one for offline work. We can get
away with that because most of Brontie's quality comes from *retrieval*, not from
raw model horsepower. We spent our energy on the pipeline, not on buying a bigger
brain.

## Prompting: tone, caching, and injection defense

The system prompt has *opinions*. Brontie's voice is "a senior dev
pair-programming with a colleague" — casual, but never sloppy. And a handful of
hard rules keep every answer tight: no filler openings (no "Great question!" —
yes, we feel the irony of writing that rule *into* an AI), a few sentences plus
*one* minimal code snippet, never ask a follow-up, commit to a single approach,
always answer in English, and skip the boilerplate `use` statements. Oh, and a
firm ceiling on length — Brontie answers questions, it doesn't write essays.

Two details under the hood are worth pulling out.

**Prompt caching.** We order the message stack for OpenAI's prefix caching: the
fully static system prompt first (cached), then the per-page article context
(cached per page), then the dynamic stuff last — embeddings, thread history, and
the question itself.

***IMPORTANT
Keeping that system message *fully static* is the entire trick. The *moment* you
interpolate per-call data into it, you bust the cached prefix — for *everyone*.
So we just... don't. Anything that changes per call lives later in the stack,
every single time.
***

**Prompt-injection defense.** Here's a cheap little trick we're fond of. We wrap
each user's message in a deliberately *non-obvious* tag name — nothing guessable
like `<student_message>`. And since the system prompt never leaves the server,
anyone trying to "close the tag" and break out has to guess that name completely
*blind*. It's not a force field — but it makes the most common injection trick a
whole lot harder, for basically free. Good luck closing a tag you can't see.

## Access control and rate limiting

Every Brontie reply costs us a real, paid API call — so not just *anyone* can
summon it, and nobody can summon it *endlessly*. Three gates stand between a
comment and the model:

1. **Paid access.** Brontie lives behind the paywall. You need an active
   subscription — no subscription, no Brontie. It's the "Gated" constraint from
   the top of the post, now with teeth.

2. **Beta access.** A subscription gets you *in line*, not necessarily *in*.
   Brontie is still a closed beta while we tune its answers, so we open it up
   gradually instead of flipping the switch for everyone at once — we'd much
   rather watch quality with a small crowd than firefight it in front of a big
   one.

3. **Rate limits.** Even once you're through the door, you can't hammer the
   thing. `BrontieChatbotRateLimiter` runs two layers: a short-term burst limiter
   that stops anyone from machine-gunning questions back-to-back, and a
   longer-term fair-use quota so one *very* enthusiastic student doesn't quietly
   run up the entire monthly bill. Hit that cap and Brontie politely tells you
   when to come back.

And one last guard rail — less about cost, more about sanity: any single comment
thread is capped at four Brontie replies. Otherwise a stray "...are you sure?"
could nudge Brontie into an infinite argument with itself.

## A quick word on feedback

We *really* wanted readers to tell us when Brontie gets it wrong, so every answer
carries a thumbs up / thumbs down. Anyone logged in can rate any answer — not
just whoever asked — and you can change your vote later. A thumbs-down pops a
little Turbo modal with context-specific reasons ("Solved my problem", "Incorrect
or incomplete", and friends) plus an optional free-text note.

Those tallies feed our analytics but stay admin-only — we don't show vote counts
to readers. And full confession: this is the *second* version of the ratings. The
first one only let the *original asker* rate, and during review we realized that
quietly threw away feedback from every *other* reader on the thread. Rebuilding it
before launch was a lot cheaper than shipping the wrong thing and learning the
hard way.

## Observability

Since answer quality is the *entire* game, every Brontie reply gets logged in
`BrontieEmbeddingsLog`: the full serialized prompt, every ranked chunk with its
score, the classified intent, and the tech filters we used. On top of that sits a
`BrontieAnalyticsService` dashboard — daily usage, failure rates, per-course
breakdowns, and the running monthly OpenAI spend via `OpenAiUsageService`.

The payoff: when an answer is bad, we don't sit around theorizing. We open the
log and see *exactly* what context the model was handed. Debugging answer quality
stops being a séance.

## What we learned

1. **Most of RAG quality is in retrieval, not the model.** Intent biasing, query
   enrichment, thresholding, diversity caps, adjacent-chunk merging — all of it
   moved the needle more than any model upgrade would have.
2. **Hide latency, don't fight it.** The placeholder-then-update pattern makes an
   eight-second LLM call *feel* instant.
3. **Pre-compute when you can.** Dense chapter summaries beat raw transcripts as
   context *and* cost fewer tokens.
4. **Ship gated and instrumented.** A closed beta plus full prompt-and-chunk
   logging let us iterate on real questions with a safety net.

## Try it

So that's Brontie: a genuinely modern RAG agent, retrofitted into a Symfony app
that's been humming along for over a *decade*. New-school brain, old-school
codebase — and they get along great.

It's in beta for subscribers right now. Head to any chapter, mention `@Brontie`
in the comments with a real question, and give the answer a thumbs up or down.
That feedback is *exactly* what we're feeding back into the retrieval pipeline
next.

And that's what's next: closing the feedback loop — turning those thumbs-down
ratings into smarter retrieval and prompts — plus continued tuning as the library
grows.

Because when you get right down to it, Brontie is a careful retrieval pipeline
with a friendly voice bolted to the front. The model? That's the easy part. The
*engineering* is everything around it.

Happy asking! 🦕
