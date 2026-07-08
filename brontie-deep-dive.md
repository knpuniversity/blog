# How We Built Brontie, Our Comment-Thread AI

The SymfonyCasts team is amazing, but we can't be online all the time! To help
us more quickly answer questions on our courses, we've introduced **Brontie**: 
a Retrieval-Augmented Generation (RAG) chatbot that lives right 
in the comment thread. Mention `@Brontie` to get instant help that will also be
reviewed by the humans on the team when we're online! 

For the really interesting part let's tour the architecture, tech and trade-offs!

## Brontie Deliverables

To start any new feature you first need a list of requirements. Here are ours:

- **Speed** - watching a spinner on your browser can be meditative, but no one
  really wants to sit and stare while an LLM thinks for around ten seconds!
- **Quality** Answers have to come from *our* content: the courses and the blog,
  not from a model confidently inventing a plausible-sounding API that has never existed.
- **Auditable** When an answer is great, we want to know why. When it's bad, we
  *really* want to know why. Which is the reason behind our rating system - if 
  a user doesn't like what came from Brontie we want that feedback!

## The layers and an async hand-off

Let's take a look at the four single-responsibility layers that make up Brontie:

1. **Detection** — this is the logic. Was Brontie mentioned? Is this a reply
   *to* Brontie? Has this thread already hit its reply limit?
2. **Orchestration** — this is the access control, rate limiting, creating the
   placeholder, and kicking off the real work.
3. **Context assembly** — this gathers the article context, the thread history,
   and the embedding search results into a value object.
4. **Agent execution** — finally, this is what assembles the LLM message stack
   and calls the model.

Why these four layers? For us, it's to make sure each piece is
*independently testable*. Detection is the functions over comments. Context
assembly is a deterministic builder. The agent is the *only* piece that talks to
the network. So when an answer comes out weird, the layering tells us exactly
*where* to go looking.

### Never block the response on the LLM

An important piece in creating Brontie was to **never block the HTTP response on the model.**

When Brontie is triggered, the request synchronously drops a placeholder comment —
*"Brontie is thinking…"* — and returns immediately. The real work gets dispatched
as a Symfony Messenger message that's picked up by a background worker.
That worker does the thinking and then updates the placeholder.

```
HTTP request ──► create "Brontie is thinking…" placeholder ──► 200 OK
                             │
                             └─► dispatch Messenger message (async)
                                           │
                      worker: build context → call LLM → update placeholder
```

Once the worker is finished, the placeholder gets updated to show the answer, 
no page refresh needed! The page just quietly polls in the background and swaps the
answer at the moment it's ready. Boring, reliable, done. The whole point of this dance is to
get the *latency out of the request path*: the HTTP response comes back in
milliseconds, while the slower model call happens entirely in the
background. The user sees "Brontie is thinking…" and a moment later, a real
answer. Nice!

## The service stack

Before we dive deeper, meet the two AI services doing the heavy lifting:

- **LLM: OpenAI** — generates the embeddings we search with *and* writes the actual answers.
- **Vector store: Pinecone** — a managed vector database. It stores our content
  as embeddings and finds the chunks most similar to a question.

That's the whole list! Everything else is regular Symfony code — more on that
in a bit.

## The RAG pipeline

Okay here is the fun part!

Naive RAG goes like this: embed the question, grab the top-k nearest vectors,
stuff them into the prompt and... hope. It's *easy* to build. It also produces
mediocre answers. These kinds of answers will confidently quote a chunk of text that 
only *looks* relevant. Almost all of our engineering went into the retrieval pipeline
that sits *between* the question and the prompt. Let's walk through it.

### 1. Classify the question — without an LLM call

Before we search for anything, we sort the question into one of four intents:

- `DEBUGGING` — error messages, stack traces, "not working"
- `CONFIGURATION` — YAML, env vars, setup
- `CONCEPT` — "what is", "explain", "difference between"
- `IMPLEMENTATION` — how to actually write it (the default)

We handled this in the traditional way (no LLM involved), keeping the process
simple and fast. However, if a plain classifier turns out not to be good enough, 
handing the job off to an LLM is always on the table.

### 2. Enrich the query with the page you're on

Before we vectorize the question, we add some critical information about the
article (like the title) onto the front of it.

This tiny change nudges the vector search toward the topic of the
page the student is *actually* on. So, a question like: "why isn't this saving?" 
asked under a Doctrine chapter pulls completely different context than the 
same question asked under a Forms chapter.

### 3. Search, scoped by technology

First, we narrow the field of search by sniffing out which topic the question
is about: Doctrine, Turbo, Twig, etc. Then we ask Pinecone for the `top_k: 12`
nearest chunks inside those filters.

Twelve feels like a lot, right? That's because it totally is. In the next step we'll 
be throwing most of them out.

### 4. Re-rank twelve hits down to four

Those twelve raw hits get whittled down to at most **four** top-scoring chunks.
That's the re-ranker's whole job. Here are the real knobs, straight from the code:

```php
private const float BOOST_MULTIPLIER = 1.15;  // +15% for preferred content types
private const float SCORE_THRESHOLD  = 0.80;  // drop anything less relevant
private const int   MAX_CHUNKS       = 4;     // final cap injected into the prompt
private const int   MAX_PER_FILE     = 3;     // diversity
private const int   MAX_PER_TOPIC    = 2;     // diversity
```

To make the cut, a chunk has to survive four passes:

1. **Boost.** Here is where the intents from step 1 pay off. The
   content types our intent prefers get a 15% bump.
2. **Threshold.** Score below `0.80`? Gone. It's more preferable to
   hand the model nothing than feed it with noise.
3. **Diversity caps.** No more than 3 chunks from one file, 2 from one topic. 
   This prevents a single chapter from dominating the whole context window.
4. **Group and slice.** Group by file, sort by score, grab the top four.

Keep in mind, none of those numbers are set in stone. We're *still* nudging them around
and then watching what actually makes Brontie's answers better.

This is where "grounded, not hallucinated" really comes from. No magic, just
four constantly reviewed steps.

### Pre-computed chapter summaries

One last grounding trick. Our tutorial scripts are *long* since they're video
transcripts. These are best consumed through watching one of the course videos, so they
can be rough to hand an LLM. It's a *lot* of tokens for not much signal.

So, we don't hand it the transcript. Instead, a background job boils each
chapter down to a dense ~400-word summary. *That's* what Brontie reads first,
not the raw script. This creates a better signal, fewer tokens, and a happier model.

And chapters aren't the only thing available. Brontie searches three sources
side by side: course chapters, blog posts (even this one), and the 
docs for Symfony, Doctrine, PHPUnit, Foundry, etc. If the answer lives in the official docs
and not in our own content, Brontie has access to it.

## Symfony AI holds it all together

Remember that "regular Symfony code"? None of it talks to OpenAI or Pinecone
by hand: that's all [Symfony AI](https://github.com/symfony/ai), the first-party AI framework.
We lean on three of its packages:

- **Platform** (`symfony/ai-platform`) — one consistent interface in front of
  the model provider. Chat completions and embeddings both go through it, so
  "which model" is configuration, not code.
- **Agent** (`symfony/ai-agent`) — runs the conversation: it assembles the
  system prompt, article context, thread history, and the question into a
  `MessageBag` and sends it through the Platform.
- **Store** (`symfony/ai-store`) — the vector-store abstraction. Its Pinecone
  bridge handles both indexing our content and the similarity search from the
  pipeline above.

And since these are Symfony components, they wire up through the container like
any other service. If we ever swap the model, or even the vector database,
Brontie's code barely notices.

## Prompting: tone, caching, and injection defense

**Prompt tone.** We've set Brontie's voice to be in the theme of a "senior dev pair-programming 
with a colleague", very casual. But it also has a handful of rules keep every 
answer tight: no filler openings like, "Great question!". It uses a few sentences 
plus *one* minimal code snippet, never asks a follow-up, commits
to a single approach, always answers in English, skips the boilerplate `use` 
statements and has a firm limit on length of response.

**Prompt caching.** We order the message stack for OpenAI's prefix caching: the
fully static system prompt first (cached), then the per-page article context
(cached per page), then the dynamic stuff last: embeddings, thread history, and
the question itself.

**Prompt-injection defense.** One classic injection trick: if a prompt wraps
user input in an obvious tag like `<question>`, an attacker types `</question>`
themselves to "close" it, then writes instructions as if they came from us.
Our counter is straightforward: we wrap each message in a tag with a weird, unguessable name that only
exists server-side, so anyone trying that has to guess the name blind. It's not
a force field — no delimiter trick is — but it makes the most common breakout
a lot harder, basically for free.

## Access control and rate limiting

Every Brontie reply costs us a real, paid API call — so not just *anyone* can
summon it, and nobody can summon it *endlessly*. Three gates stand between a
comment and the model:

1. **Paid access.** Brontie lives behind our paywall. You'll need an active
   subscription.

2. **Beta access.** For now a subscription gets you *in line*, not necessarily *in*.
   Brontie is still a closed beta while we fine-tune its answers.

3. **Rate limits.** These run in two layers: a short-term burst limiter
   that stops users from machine-gunning questions back-to-back, and a
   longer-term fair-use quota so that one *very* enthusiastic student doesn't quietly
   run up the entire monthly bill. Hit the cap and Brontie will politely tell you
   when the service is available again.

And one last guard rail, any single comment thread is capped at four Brontie replies.
Otherwise, a stray "...are you sure?" could nudge Brontie into an infinite argument with itself.

## A quick word on feedback

We *really* want readers to tell us when Brontie gets it wrong, so every answer
carries a thumbs up / thumbs down. Anyone logged in can rate any answer, not
just whoever asked, and you can change your vote later. A thumbs-down pops a
little Turbo modal with context-specific reasons ("Solved my problem", "Incorrect
or incomplete", and friends) plus an optional free-text note.

## Observability

Since answer quality is really important, every Brontie reply gets logged: the
full serialized prompt, every ranked chunk with its score, the classified intent,
and the tech filters we used. On top of that sits an analytics dashboard with
daily usage, failure rates, per-course breakdowns, and the running monthly
OpenAI spend.

Here's the goal, when an answer is bad, we don't sit around theorizing. We open the
log and see *exactly* what context the model was handed, making debugging answer quality
more transparent.

## What we learned

1. **Most of RAG quality is in retrieval, not the model.** Intent biasing, query
   enrichment, thresholding, diversity caps, adjacent-chunk merging. All of it
   moved the needle more than any model upgrade would have.
2. **Hide latency, don't fight it.** The placeholder-then-update pattern makes an
   eight-second LLM call *feel* instant.
3. **Pre-compute when you can.** Dense chapter summaries beat raw transcripts as
   context *and* cost fewer tokens.
4. **Ship gated and instrumented.** A closed beta plus full prompt-and-chunk
   logging has let us iterate on real questions with a safety net.

## Try it

So that's Brontie: a genuinely modern RAG agent, retrofitted into a Symfony app
that's been enjoyed by users like you for over a *decade*!

It's in beta for subscribers right now. Once you're in, head to any chapter,
mention `@Brontie` in the comments with a real question, and rate the answer
with a thumbs up or down.
That feedback is *exactly* what we're feeding back into the retrieval pipeline next.

And that's what's next: closing the feedback loop. We'll be turning those thumbs-down
ratings into smarter retrieval and prompts, and we'll continue adjusting as the library
grows.

Because when you get right down to it, Brontie is a careful retrieval pipeline
with a friendly voice bolted to the front. The model? That's the easy part. The
*engineering* is everything around it.

Happy asking! 🦕
