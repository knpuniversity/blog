OMG REST! Questions and thoughts on our upcoming Course
=======================================================

Great news! We're already well-underway on our next screencast/course, which
is all about REST. Bad news! This stuff is *really* tough, and no amount
of hours reading blog posts, watching videos, reading CFPs or talking with
some smart people (thanks to `Luke Stokes`_ `Lukas Smith`_) seems to totally
clear things up. In light of how complex this stuff is, we'll actually have
at least 2 courses, one for REST basics (and a project built in Silex) and
another one about doing all of this in Symfony2.

RFC on some RESTful Statements
------------------------------

But this blog post is actually more for the experts in REST: those of you
who have been in the trenches, writing a REST API for your application. My
hope is that I can write some bold and direct statements here, and you can
tell me where I might be wrong or misdirected. There are *a lot* of options
in REST and a lot of strong opinions. But in a tutorial, we have to pick
one *really good* path and recommend it.

Everything we need to know about a Resource
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Imagine you're working with an API. At each stage ("application state"),
you need to know what to do next. With HATEOAS, we somehow have links to
other resources, which explain to us what we can do next. Since these links
are URIs, they point to a resource (e.g. ``/users/weaverryan``). But before
we *really* know what to do next, we need to know several things about that
resource:

.. _blog-rest-resource-questions:

1) What HTTP methods does the resource support?

2) What fields should be in the request body for each method? If I send a
   PUT request, what fields do I need?

3) What format can those fields be in (e.g. ``application/json``, ``application/x-www-form-urlencoded``)?

4) Are there some non-standard actions (e.g. "publishing a user")? If so,
   what should the request body look like for them? Are they POST, something
   else?

5) What response media types are supported?

The big question is, where does this information live? And are we missing
anything?

In an "old-school" API, it would be in the external documentation. 

In a "new-school" API (in theory), it could live entirely in the API itself,
making it "self-describing".

I'm curious about a "new-school", but pragmatic approach, where this information
is probably spread between external documentation and the API itself. Of
course, any information kept external should be pointed at by the API, but
we'll get to that.

A Proposed HAL-based RESTful API
--------------------------------

I've chosen `HAL`_ as the hypermedia type of the API in our course because
it's one of the easiest, leading to a (hopefully) pragmatic, but still nice
API. I've looked at `JSON-LD`_ and `Siren`_ as well, and find these to be
really interesting, and potential candidates for a future addition to the
course.

A lot of the approach taken here is thanks to the still-WIP API being built
by `Foxycart`_. They use the `HAL Browser`_, which is helpful to see what
we're talking about.

In the `HAL Browser`_, you can clearly see hypermedia links to other resources
at each step. Again, one of my biggest questions is, how should my API client
know :ref:`what to do with that resource<blog-rest-resource-questions>`.
Largely, the answer is by using real URLs as the "rel" of the links, which
leads to real documentation.

The Homepage
~~~~~~~~~~~~

First, I'll propose a homepage to the API, complete with nothing but links
and a friendly message:

.. code-block:: json

    {
      "_links": {
        "self": {
          "href": "https://example.com/",
          "title": "Your API starting point."
        },
        "http://api.example.com/rels/user": {
          "href": "https://example.com/users",
          "title": "Users in the system"
        }
      },
      "message": "Well hallo there! Please check out our docs at http://api.example.com/docs"
    }

The "users" relation tells me where the users collection resource is located
and the ``rel`` value (``http://api.example.com/rels/user``) should be a real
URL that the client can follow to get human-readable information about this
relation. Like FoxyCart's documentation (`user example`_), the ``rel`` docs
*must* contain at least:

* A human description of the relation
* Supported HTTP methods
* The properties/fields expected

With that information, I could, for example, construct the ``POST`` request
needed to create a new use (I have the URI from the hypermedia link and the
fields from the ``rel`` API docs).


The Users Collection Resource
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If we instead made a GET request to the users resource (i.e. ``/users``),
then we would get back something that looks like this (similar to `Foxycart Stores`_):

.. code-block:: json

    {
      "_links": {
        "self": {
          "href": "http://example.com/users",
          "title": "This Collection"
        },
        "first": {
          "href": "http://example.com/users?offset=0",
          "title": "First Page of this Collection"
        }
      },
      "_embedded": {
        "users": [
          {
            "_links": {
              "self": {
                "href": "http://example.com/users/weaverryan",
                "title": "This user"
              }
            },
            "username": "weaverryan",
            "published": false
          }
        ]
      }
    }

.. note::

    I've left of some details - like ``prev``, ``next``, ``last`` links.

->> how the hell do I know what to do with the user link? There is no rel
    on it???

- roll up CRUD operations into one
- hypermedia versus rel
- OPTIONS



A RESTful API is designed around the idea of a resource and allowing the
client to change the state of that resource.

- where can I go from here?
- what methods does a relation support? How should I know that?
- what custom actions does a relation support? How should I know that?
- what fields does each action support? Where are the docs?
- what formats does it support in the request body?
- what should a custom action look like? POST with a different body?
- should OPTIONS be used? Should it return links?

- list of the thins that you need to know at each decision point
- where is the documentation?
- custom actions
- LSmith and Foxycart



- /users (CRUD)
- /users/{username} (CRUD)
- /users/{username} (report as spam) -> /users/{username}/report? Custom POST body? Same/different controllers?
- /users/{username}/statuses (CRUD)
- /users/{username}/following -> the LINK of user to user (UNLINK?)

- think about how the custom actions would work
- think about how we'd document all of the link relations for each resource
    and the fields+methods for each, etc
- If I POST to create a new resource and I get back the Location header,
    how do I know what the rel is for the new resource? How can I look
    up its docs?
- if you had a custom media type on each link, then that *would* answer
    what fields are needed on that LINK. With an OPTIONS, you could see
    if it supports POST. A custom action would be another link with another
    media-type (and thus, different documented fields)

.. _`Luke Stokes`: https://github.com/lukestokes
.. _`Lukas Smith`: https://github.com/lsmith77
.. _`HAL`: http://stateless.co/hal_specification.html
.. _`JSON-LD`: http://json-ld.org/
.. _`Siren`: https://github.com/kevinswiber/siren
.. _`Foxycart`: http://wiki.foxycart.com/v/0.0.0/hypermedia_api
.. _`HAL Browser`: https://api-sandbox.foxycart.com/hal-browser/hal_browser.html#/
.. _`user example`: https://api.foxycart.com/rels/user
.. _`Foxycart stores`: https://api-sandbox.foxycart.com/hal-browser/hal_browser.html#https://api-sandbox.foxycart.com/users/2/stores