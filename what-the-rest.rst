What the REST?
==============

When I talked recently about :doc:`being collaborative and open</blog/knp-you>`,
I mentioned that I was weeks into research around building RESTful API's.
I've been like Luke training on Dagobah, except replace Jedi powers with
REST and Yoda with the Internet, RFCs and dozens of blog posts. And despite
everything, my mind is clouded. When it comes to new tech, sometimes the more
you learn about the pieces, the less you can see how they could ever fit together.
This post is an experiment to see if we can fix that.

REST is as deep as the `rabbit hole`_, with varied approaches, undefined
best-practices, and flamewars along the way. Should I use custom hypermedia
types or something like `HAL`_ or `JSON-LD`_? Should I implement OPTIONS,
and if so, should it tell me only what methods to use or much more? What role
should the API documentation play and what information should be described
inside the API itself? How do custom verbs like banning a user or linking
to a friend look?

.. tip::

    I owe a big thanks to `Luke Stokes`_ and his team's WIP API for `FoxyCart`_,
    which I'll bring up in this post. Luke let me bother him with questions
    (repeatedly) and was wonderful about it.

Eventually, best-practices will rise to the surface. But I (and maybe you)
want to build a high-quality "REST" API right now. Curiously, HATEOAS - the
missing RESTful piece for many APIs - has made things more difficult for me
so far. The idea is beautiful: return links in your API to help the client know
what to do next. But links, which are basically a URL with a "rel", only
tell part of the story. What methods does that link support? What fields
should I send to it? And where should all this information live?

In the next sections, I'll say some things about REST that are just wrong.
And hopefully, I'll be corrected. I'll pour out my RESTful heart, and invite
you all to crush it and build it back up. I've heard that the Internet is
good at correcting people, so I thought, why not tap into that? :).

By the end, I hope to have *one*, *good*, well-defined path that can be written
up and shared openly.

HATEOAS: What you can do next?
------------------------------

When I get a response from a RESTful API, it should contain links. Like on
a webpage, these tell me what I might do next (i.e. what actions I can take).

Links versus Resources
~~~~~~~~~~~~~~~~~~~~~~

As I understand it, it's not exactly correct to say that links are links to resources.
Of course a link has a URI, and each URI is an address to a resource, but
you could have multiple hyperlinks to the same URI. For example, imagine
there are multiple actions I can take on that resource. Suppose that when
we go to the homepage of our API it returns the following links (I'm using
HAL to represent the links, but that's not important):

.. _blog-what-the-rest-original-links:

.. code-block:: json

    {
      "_links": {
        "self": {
          "href": "/"
        },
        "http://api.example.com/rels/users": {
          "href": "http://api.example.com/users",
          "title": "Users in the system"
        },
        "http://api.example.com/rels/users_reinvite": {
          "href": "http://api.example.com/users",
          "title": "Re-invite unregistered users to the system"
        },
      },
      "...": "... other stuff ..."
    }

In this case, we have 2 different links (actions) but each link points to
the same resource. This is because there are 2 actions that can be taken on
that resource. This is my understanding of links versus resources, a distinction
which is important because it means that a link is much more than just a URI.

.. note::

    I recommend using the `HAL Browser`_ for the FoxyCart API if you want
    to see how this really looks for an API.

.. sidebar:: A link for *every* CRUD operation?

    It also *seems* to be safe that the "main" link to the user resource is
    *probably* being used for CRUD operations. Luke suggested that some APIs
    break each operation into its own link (1 link for the read/GET, 1 for
    create/POST, 1 for the update/PUT and 1 for the delete/DELETE). This
    fits exactly with my understanding above. But it also feels like overkill.
    Resources versus links feel tricky.

We don't know exactly what to do with those links yet, but that's next.

Hypermedia Links: What they tell Us
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Exactly how these links look depends on your hypermedia type (e.g. ``HAL``
something custom, etc), but a link has the following information:

1. A URI;
2. A ``rel`` to explain the significance of this link;
3. (optional) A ``type``, which defines the hypermedia type to be returned
   by the link;
4. (optional) A nice human title for the link;

.. note::

    In the links above, the ``rel`` is the key (e.g. ``http://api.example.com/rels/users_reinvite``)
    and there is no ``type``, since it's HAL and you assume that the link
    will also be able to return HAL.

In a browser, we click links (i.e. GET) or submit a form (i.e. POST). The
form contains the fields we need to send right inside of it and there's an
web-wide standard of sending that data in a certain format (i.e. ``application/x-www-form-urlencoded``).
So when we're using a browser, each link contains all the information needed
to follow it.

But that's not true at all with an API. We need more information than we
have from simply looking at the link, which I'll call:

.. _blog-what-the-rest-4-missing:

The 4 Missing Pieces of a Link
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

1. What HTTP methods can I use with this link?
2. What fields should I send if I'm POST'ing/PUT'ing?
3. How should I encode that data in the request (e.g. ``application/json``, ``application/x-www-form-urlencoded``, etc)?
4. What media type(s) should I expect back from the response?

HATEOAS tells us to use these links to determine what our next step, or
action is (they `induce application flow`_). In the HTTP world, they should
tell us what possible HTTP *requests* we can take next.

So if this information isn't found in the hyperlinks, where does it live?
Where can an API client find these details and how does she know to look there?
This is at the heart of the trouble I have in understanding RESTful APIs.

.. note::

    Some hypermedia formats like `JSON-LD`_ seem to have a more sophisticated
    method where "link data" is exposed that answers some or all of these
    questions. I'm very interested in this, but first I want to know how
    this should look in simpler cases, like when using HAL. If people are
    using HAL, then there should be a "right" place to define this information.

Finding the Missing Pieces to make the next Request
---------------------------------------------------

Your API also needs to choose how its hypermedia type(s) look like. Will
each link have its own type (e.g. ``application/vnd.com.users+xml``) or
will you use one hypermedia type like HAL?

Right now, we're missing :ref:`4 pieces of information<blog-what-the-rest-4-missing>`
before we can really make the next request. In REST, you often read that
the only thing you should need to document is your hypermedia types. In
that model, does *every* link have its own hypermedia type? And do the docs
for that hypermedia type really tell us what HTTP methods can be used and
what fields can be POST'ed? That would seem odd, especially because the data
is not sent (e.g. POST'ed) using the hypermedia type. Usually data is sent
with something simpler like ``application/json``, and the special hypermedia
is only used in the response.

So let's look at the 2 approaches (custom hypermedia type versus HAL) and
try to see how a client would answer the :ref:`4 questions<blog-what-the-rest-4-missing>`
standing between us and the next API request:

+--------------+-------------------------------------------+--------------------------------------+
|              | Custom hypermedia type                    | HAL (or similar)                     |
+--------------+-------------------------------------------+--------------------------------------+
| HTTP Methods | Find docs based on the hypermedia type??  | Find docs based on the link ``rel``  |
+--------------+-------------------------------------------+--------------------------------------+
| Fields       | Find docs based on the hypermedia type??  | Find docs based on the link ``rel``  |
+--------------+-------------------------------------------+--------------------------------------+
| Request      | Find docs based on the hypermedia type??  | Find docs based on the link ``rel``  |
| encoding     |                                           |                                      |
+--------------+-------------------------------------------+--------------------------------------+
| Response     | Read the link ``type`` attribute          | Assume HAL                           |
| media type   |                                           |                                      |
+--------------+-------------------------------------------+--------------------------------------+

.. note::

    The ``rel`` could literally be a URL to the documentation. FoxyCart uses
    this (e.g. `https://api.foxycart.com/rels/users`_).

It *seems* that both methods will probably rely on some external documentation.
This is known as "out-of-band information", which we shouldn't need in theory.
But I'd argue that creating an API that's fully self-describing is still
very hard, and quite possibly not worth it.

Some pieces of information could probably be answered globally for your API (e.g.
Request encoding). Very literally, you might just say on the homepage of your
docs that all endpoints support ``application/json`` and ``application/x-www-form-urlencoded``.

But usually, we will need to look up a specific API docs page for each link.
For a custom hypermedia approach, the docs *seem* to be for each hypermedia
type (right?, wrong?). For HAL, since we only have one hypermedia type, we
rely entirely on the link ``rel``. This could all totally be wrong, but if
it is, then how should the client be answering our :ref:`4 questions<blog-what-the-rest-4-missing>`
for each link? If we have docs, how do they know which doc to look at for
each link?

Value in OPTIONS?
-----------------

What if we made an ``OPTIONS`` request to ``/users``? This actually seems
very unhelpful, as the ``OPTIONS`` is for a single URI, which could actually
have multiple links to it. The ``OPTIONS`` response may say we can POST,
but how would a client know what to POST? In our :ref:`example<blog-what-the-rest-original-links>`,
there will be 2 POST actions (one for creating a new user resource and another
for "re-inviting" users), each which has its own documentation on how the
fields should look.

``OPTIONS`` may be helpful if it returns the links that I would receive
if I made a ``GET`` request to the resource. In our example, we'd have:

.. include:: includes/what-the-rest/_users_hal_links.rst.inc

Of course, I could just make a ``GET`` for this information. So is there
value in ``OPTIONS``?

Problems: When I have only a URI
--------------------------------

One important consequence about a REST API is that we're not really supposed
to start with (i.e. bookmark) a URI. In fact, with just a URI, we don't really
have enough information to know what to do with it.

.. note::

    In this section, I'll look only at HAL. If you're using a custom hypermedia
    type, then I think you also have a problem. If you only have the URI,
    you don't know what ``Accept`` header to use in your request nor would
    you know really how to process the response. You simply don't know what
    hypermedia type you'll get back.

For example, if I know nothing about ``/users`` and I make a ``GET`` request,
I get back these 2 links (among other things):

.. include:: includes/what-the-rest/_users_hal_links.rst.inc

Notice ``self``, which is a standard `IANA Link Relation`_, but which no
longer includes the helpful ``http://api.example.com/rels/users`` "rel".
If the rel is the key to finding the docs, we're stuck.

Of course, according to the rules, we shouldn't be hardcoding URIs', we should
always start back on the homepage and follow the link, with its nice rel.

.. note::

    DHH famously argues against this idea that an API client would always
    start from the homepage and dynamically crawl instead of hardcoding
    URLs (`Getting hyper about hypermedia APIs`_). I'm pretty sure this
    pissed a lot of people off, but I think he makes a lot of sense.

But this scenario *does* show up during normal client-server interaction in
at least two places:

1. After POST'ing to create a new resource, the ``location`` header gives
   us the URI to the resource, but without a ``rel``;

2. When GET'ing a collection resource, the embedded children don't have a
   specific rel value (they have ``self``):

.. code-block:: json

    {
      "_embedded": {
        "users": [
          {
            "username": "weaverryan",
            "_links": {
              "self": {
                "href": "http://api.example.com/user/weaverryan",
                "title": "Users in the system"
              },
              "...": "... other links ..."
            }
          },
          "... other users ..."
        ]
      },
      "...": "... other stuff, links, data, etc ..."
    }

In both cases, we can GET the resource, but we're never given the pointer
(``rel`` in HAL) to the docs, which answer our questions. I need some "in-band"
information that tells me where to find ("out-of-band") the API documentation.
Said more simply, if I'm presented with a link to this user, how should our
client know what to do with it?

FoxyCart's solution is to include this information in the "originating" docs.
In the 2 scenarios above, I'm POST'ing and GET'ing to ``/users`` and we already
have its rel (``http://api.example.com/users``). This means we also have its
docs. On that page, the documentation from ``http://api.example.com/user``
(the "rel" for the new resource) could be embedded (see "Embedded Resource: user"
at `https://api.foxycart.com/rels/users`_).

Is there a better way? How should the API client know what to do with this
new resource? What can be safely assumed?

Opinions, Experiences, Please!
------------------------------

Again, a huge thanks to `Luke Stokes`_ and `FoxyCart`_ for pioneering so
much of this and answering my questions. If you're someone who's been in
the trenches with REST, comment, enlighten and correct me please! Anything
we accomplish here will ultimately turn into code, a script (both of which
will be available publicly) and a screencast.

But one more thought! The further I get into REST, the more rules I see.
Things like hypermedia (linking to what I can do next) seem very pragmatic
and wonderful. But immediately after, I find myself (as a client) having a
difficult time navigating a RESTful API and knowing where to find the docs
(whether those docs are part of the API or externally). I want to be able
to leverage all the great ideas behind REST, but not be constrained by them.
At the end of the day, we need an API that I can build and that our friendly
API client can understand quickly. In some ways, then, the path of DHH
(`Getting hyper about hypermedia APIs`_) does make sense: use what's good,
leave what's complex, break some rules, and move forward.

But, maybe I'm just missing a few, final, beautiful pieces to get out of
the swamp and up to blissful RESTland.

Thanks!

.. _`rabbit hole`: http://www.urbandictionary.com/define.php?term=rabbit%20hole
.. _`Luke Stokes`: https://github.com/lukestokes
.. _`HAL`: http://stateless.co/hal_specification.html
.. _`JSON-LD`: http://json-ld.org/
.. _`HAL Browser`: https://api-sandbox.foxycart.com/hal-browser/hal_browser.html#/
.. _`Foxycart`: http://wiki.foxycart.com/v/0.0.0/hypermedia_api
.. _`induce application flow`: http://amundsen.com/hypermedia/
.. _`https://api.foxycart.com/rels/users`: https://api.foxycart.com/rels/users
.. _`IANA Link Relation`: http://www.iana.org/assignments/link-relations/link-relations.xhtml
.. _`Getting hyper about hypermedia APIs`: http://37signals.com/svn/posts/3373-getting-hyper-about-hypermedia-apis
