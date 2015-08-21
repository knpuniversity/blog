# REST Revisited

Two weeks ago, I wrote [What the REST?](/blog/what-the-rest), asking for help answering
questions around REST that I thought were incomplete. With all the beauty
of REST and HATEOAS, I was coming across very real challenges and gaps when
trying to write a tutorial on how to do it *well*. It seemed like there was
a lot of talk, but not a lot of real-world proven examples to back this all
up.

Wonderfully, a lot of people showed up to offer their thoughts, which you
can see as comments on [that post](/blog/what-the-rest). In this entry,
I wanted to summarize the problems and the solutions we found. I may still
say some wrong things, and hopefully people will once again come to my rescue.

Overall, one big message is that you don't really want to get REST perfect.
You want to get as close as you can, then bend the rules happily when it
makes sense. Also, the "best practices" don't really exist, and there really
*are* holes in how this should all be done. So if you can't find out the "right"
way to do something, it may be because there isn't a "right" way. Instead,
do your best to make an educated decision.

***TIP
If you want to know when the REST tutorial is released, follow us on
[Twitter](https://twitter.com/knpuniversity) or [Register](http://knpuniversity.com/signup/) and add yourself to the email list.
***

## 1. Links versus Resources

[Original discussion](/blog/what-the-rest#blog-what-the-rest-links-resources)

Since a link contains a URI, and a URI is an address to a resource, the difference
between a link and a resource can be fuzzy. In fact, if your API were entirely
made up of *only* so-called CRUD operations (e.g. creating a user, viewing
a representation of that user, editing that user, deleting the user), then
each resource will probably only have one link to it.

But as soon as you have some special, non-standard action that needs to be
taken on a resource, then you may have 2, 3 or more links to that resource.
I mentioned that with a [HAL example](/blog/what-the-rest#blog-what-the-rest-original-links).
This is what I argued in my first post, and I think it's correct, or close
enough.

## 2. Self-Describing APIs and "rels"

One of the seeming advantages of a truly RESTful API is that it has the potential
of being self-describing, which means that you can learn everything you need
to know about the API *from* the API. After talking with people (especially
[Luke Stokes](https://twitter.com/lukestokes) and [Larry Garfield](http://knpuniversity.com/blog/what-the-rest#comment-1038006806)), I think the importance of this is over-emphasized.
And worse, it's probably very difficult to achieve.

Remember that HATEOAS (at least how we're using it) emphasizes links and says
that the `rel` of the link describes its importance. Instead of coding
our clients to the URIs, we code them to recognize the `rel` of a link
and know how to process it. To quote Larry:

    It's not reasonable to expect a client to see a new rel it doesn't understand
    and somehow derive meaning from it. However, if it sees a rel that it *does*
    understand, in concept, then it should be able to know what it means
    and figure out logically what it means in relation to the source of the
    link.

    To use the example you already cited, a client that knows what "next"
    and "prev" mean can automatically "know" what it should do with those
    links if it sees them on a resource. If it sees "bob", though, it doesn't
    know what "bob" means. It should therefore ignore it until a human comes
    along and programs in the Meaning of Bob. Documentation is for a human.

So the lifecycle of developing an API client might look like this:

A. A human crawls the API by starting on the homepage and observing the links.
   When the human sees an interesting link, the `rel` is used to look up
   the human documentation, which tells him the HTTP methods available, fields,
   and other information (see [The 4 Missing Pieces of a Link](/blog/what-the-rest#blog-what-the-rest-4-missing)).

B. The human programs the API client to go to the homepage and look for a link
   with the "rel" that he looked up previously. The client uses the URI from
   that link along with information that the human has hardcoded into the
   client (like the HTTP method to use and fields to send). This hardcoded
   information came from the human documentation.

C. A month later, the API author changes the URI behind one of the links.
   The client doesn't notice or care, since it's looking at the "rel" of the
   link and using whatever URI is there. This is all a bit theoretical, but
   hopefully it makes sense.

D. 3 months later, a new link relation is created on the homepage. The client
   initially doesn't see it or care about it, since it's not hardcoded to
   look for the "rel" on this link.

E. The human reads a blog post about a sweet new feature to the API, which
   is exposed via the above-mentioned new link. He surfs the API, notices
   the link and looks up the human documentation via its "rel". He then adds
   code to the client to look for this "rel" and tells the client exactly
   what to do when it follows it (e.g. make a PUT request with the following
   parameters). He looked up those details in the human API.

That's it! Rinse and repeat. As long as a human can find documentation for
a "rel", then we're in good shape. The client responds to "rel"s that it
recognizes, because a human has looked up the documentation on that rel and
filled in the [missing pieces](/blog/what-the-rest#blog-what-the-rest-4-missing).

So the "rel" becomes the new all-important *key*, instead of the URI. But
really, both are very similar. Both the "rel" in a HATEOAS API and a URI
in an older API somehow give you a pointer to the documentation. And in both
cases, a human is needed to read that and figure out exactly *how* to make
a request.

***TIP More information-rich formats like JSON-LD
Like I mentioned in my previous post, there are other formats like [JSON-LD](http://json-ld.org/)
that seem to try to offer even more information about the link, like
what fields are in it and how that information should be sent in the
request (e.g. as simple `application/json` or `application/x-www-form-urlencoded`).
I think this is really interesting. However, I still think that a human
needs to be involved. Even if you can programmatically determine that
an endpoint needs `firstName` and `lastName` fields, your API client
will need to be programmed to figure out the significance of these fields
and what data goes into which field. Your client *could* give you warnings
if something changes in the future (e.g. suddenly `firstName` is missing
from the field list), but an API could also return a 400 validation error
if you made a breaking change like this. In other words, I think this
is cool, but I'm not sure I really see whether or not it gets us a whole
lot further.
***

## 3. What happens when we're missing a link to the docs?

In my previous post, I mentioned 2 situations where I end up with
[only the URI without its rel](/blog/what-the-rest#blog-what-the-rest-only-uri).

For me, this was a serious problem. Even if we're relying on a human to find
external documentation, the API should be easy for a human to use. This means
that whenever the API isn't self-describing, it should tell us where the
documentation lives. The "rel" is the pointer to the documentation, except
that it's missing in these [2 cases](/blog/what-the-rest#blog-what-the-rest-only-uri).

It turns out that this is maybe ok. What!? Let's revisit the first situation:
I POST to create a new user resource. The response contains a 201 status
code with a `Location` header to `/users/5`, but no rel.

After talking with [Luke Stokes](https://twitter.com/lukestokes), he pointed out that in order to even know
*how* to POST to create the user, a human would have needed to look at the
documentation for the users rel (something like `https://api.example.com/rels/users`,
which we would have discovered by walking the API). As long as that documentation
clearly states that POST'ing will create a user resource and that the "main rel"
to that resource is `https://api.example.com/rels/user`, then we're in
business! The user can then look up that documentation to figure out what
to do with the URI in the `Location` header.

### Embedded Resources: Not as Clean

The same could be argued for the second place this problem shows up, embedded
resources ([example](/blog/what-the-rest#blog-what-the-rest-collection-missing-rel)). In
other words, you should look at the "https://api.example.com/rels/users" rel
documentation to see that the embedded `users` key contains items whose
"main rel" is `https://api.example.com/rels/user`.

But this "smells" to me a little bit. I think a link should always give me
enough information to follow it. In our API, that means a URI and a rel so
that we can look up the rest of the information in the human docs. This is
missing from embedded resources, and I think that's unfortunate.

This also affects how we program our API client. When we see these links,
we don't know if we recognize how to use them because the rel is missing.
Instead of hardcoding the "rel" and looking for it, we would need to hardcode
the fact that the embedded `users` resource after following a `https://api.example.com/rels/users`
link contains links whose "self" is `https://api.example.com/rels/user`.
That's a bummer.

### A Better Way?

First, this problem doesn't need to be solved. All the information is there
for the human to understand the API and for the client to use it. I think
the API could be more useable for the human and a little cleaner for the client,
but it's not the end of the world.

I think that a link should always give us enough information to follow it,
even if that means just pointing us to the docs. And for the simplicity of
the API client, I think every link should have a "rel" so that we know if
this is a link that we have already programmed the client to understand.

One suggestion that [Raul Fraile from ServerGrove](http://knpuniversity.com/blog/what-the-rest#comment-1032280776) suggested is to add a
header on the 201 response when creating a resource (e.g. `X-Location-Rel: https://api.example.com/rels/user`).
For me, this is kind of cool because if we think of the response as a "link",
it now contains the URI (`Location` header) and the rel (`X-Location-Rel`
header). The only downside is that it's odd to invent things like this when
this is clearly a problem shared by many people.

But what about the embedded resources issue? For this, I don't know. Could
we duplicate the "self" link by adding another link with the true "rel"?
Should it be more clear that the "users" key will contain resources whose
"main rel" is `https://api.example.com/rels/user`? Where would we put this?

On this issue, I'm still a little dissatisfied.

***TIP The "main rel" of a resource
I've said "main rel" a few times to mean the link to a resource that represents
its CRUD operations. I'm not sure this is totally correct, but I invented
this term because in practice, there is always a "main" link to a resource,
which includes the implied GET operation that you can do on any resource.
This link is represented as the "self" rel of an embedded resource.
***

## 4. Walking the API - Caching

One of the key assumptions of a REST API is that it will be used by REST
API clients. This means that your API clients will *not* hardcode your URIs,
but will instead "browse" your API whenever it needs to do something, looking
for link rels that it recognizes.

In reality, while you *may* have some true REST clients, if your API is used
by many people, a lot of them will probably hardcode your URLs. I think that's
life, and as long as we've made the API easy to understand for these people,
then it's ok. This includes explaining clearly that the documentation is connected
to the "rel" and (ideally) making sure people don't get stranded without
a rel, like I talked about in the previous section.

But if you *do* build an API client, this means that it will always start
from the homepage of the API and browse to where it needs to go. At first,
this seems like a REST client could never be fast. Instead of hardcoding
a URI and making 1 request, we browse the API and maybe make 2, 3 or more
requests.

But Luke pointed out that this is where HTTP caching comes into play. If
you've designed your REST API well, then you're returning HTTP caching headers
that allow the client to cache the responses. This means that even though
your code may *look* like it's making 4 API requests, the first 3 that browse
the API are cached, meaning no request is actually made.

This sounds complicated, but if you use [Guzzle](http://guzzlephp.org) to make the API requests
in your client, then it happens automatically by using their [HTTP Cache Plugin](http://guzzlephp.org/plugins/cache-plugin.html).
So if "making too many requests" was one of your worries, it may not be such
a big deal.

## 5. Custom Actions

One of the most difficult things to figure out is how custom "actions" should
work on a resource. The basic operations are covered by the HTTP verbs GET,
POST, PUT and DELETE. But what if I have an endpoint to `/users` that sends
an invitation email to anyone that hasn't confirmed their registration yet?
How should that look? Once again, [Larry helped here](http://knpuniversity.com/blog/what-the-rest#comment-1039347270) by mentioning a few
good points:

A) This is where REST starts to break down, so cheating here is not so bad.

B) POST is a great "fall-back" method to use for custom actions.

C) You *can* sort of, "invent" new URIs (i.e. resources) for these actions.

Larry gave 2 examples in [his comment](http://knpuniversity.com/blog/what-the-rest#comment-1039347270), and I'll give 2 more possibilities
for my "resend" idea, which is a little bit less clean since we're operating
on a collection resource. So, check out [his comment](http://knpuniversity.com/blog/what-the-rest#comment-1039347270) and then come back:

> PUT /users/reinvite (bad!)
>
> POST /users/reinvite (better!)

In both cases, I used a new URI instead of POST'ing to `/users` with some
special request body that indicated that I want to reinvite users instead
of creating a new user resource. That point is debatable, but this seems cleaner.

Can you spot the problem with the first? It works in Larry's example
(`PUT /article/1/published` with a body containing "1"), but in our example,
this wouldn't be idempotent. That's an overused word, but I should always
be able to issue a PUT request multiple times without adverse affects. In
this example, making this request multiple times will probably email people
multiple times. For that reason, POST is probably better.

My point here was to give a few examples that probably *cheat* a little with
REST and show how the thinking on these endpoints is always a little fuzzy.
I typically feel that someone will be able to come along and suggest a better
way to format a custom verb. I hope they do. But in your API, choose something
and live with it :).

## To the Tutorial!

After all of this, I *am* once again working on the REST tutorial. In fact,
we'll probably have 2: one for PHP talking about all the difficult things
we've discussed here, and another for Symfony, using FOSRestBundle, and probably
bundles like FSCHateoasBundle.

You can watch progress and contribute (that would be awesome) to the upcoming
tutorial on [GitHub](https://github.com/knpuniversity/rest). Or follow us on [Twitter](https://twitter.com/knpuniversity) or [Register](http://knpuniversity.com/signup/) and add yourself
to the email list for a poke when it comes out.

Cheers!

*Title image courtesy of http://www.flickr.com/photos/hmk/1442578687/*
