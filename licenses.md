# Finding our OS Content License

A :doc:`couple of weeks ago </blog/knp-you>`, I talked about making KnpUniversity
open, transparent, and full of the rainbows and sunshine that come with sharing
and collaborating. We already offer the content of our screencasts for free
right here on the site. Our hope is that we can make money selling the full
package (screencast + in-browser coding activities) while still giving away
good content to everyone.

But we want to go further by making the content available on GitHub ([done!](https://github.com/knpuniversity)),
which means figuring out how to license it. Of course, this is where things
get legal, which means complicated... and boring -_- zzzz.

If you're not too familiar with licenses, I'll give a brief background.
But my real goal is to get your opinion! If you're an expert or already bored,
at least :ref:`scroll down <blog-license-choosing>` to the good part and let
us know what you think.

## Creative Commons to the Rescue

GitHub has a great site ([choosealicense.com](http://choosealicense.com/)) to help you license your
*code*, but not necessarily your content. If you peak at the footer,
you'll see that the site itself is licensed as "Creative Commons Attribution 3.0
Unported License". In fact, this is also the license used for
[Symfony's documentation](http://symfony.com/doc/current/contributing/code/license.html)  and Creative Commons is the go-to provider of
licenses when it comes to protecting *content*. They even have an awesome
[license "wizard"](http://creativecommons.org/choose/), which you can see a screenshot of at the top of this
post.

.. _blog-license-choosing:

## But which License is for Us?

There are basically two big decision points to selecting the license:

1) **Allow modifications of your work?**

Yes! Locking down information is silly, and if someone can improve on our
work, more power to them. The license specifies that the modification must
attribute the original author, which seems reasonable.

There's also a "Yes, as long as others share alike" option, which is even
more interesting because it means that any modifications must have the same
license as the original content (similar or equal to "copyleft"). This is
what we prefer.

2) **Allow commercial uses of your work?**

And this is where the debate starts, which comes down to two conflicting,
but good arguments:

A) "Yes: We should let anyone do whatever they want with the content. Let's
not be the "man" and control them. Peace and openness!"

B) "No: I *love* being open, but I want to protect someone from scamming
people by taking this content and selling it. It's free (as in beer) and
we should make sure it's always that way!"

Saying "Yes" is more open, but saying "No" protects the content from being
used in a way that we don't really want. The decision may never matter, but
choosing a license lets us plan ahead... just in case.

So what do you think?
