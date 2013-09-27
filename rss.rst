We can has RSS?
===============

We have an `RSS feed`_! Ok, that's not earth-shattering 2013 news, but you if
you've enjoyed our posts about REST and OSS licenses, then subscribe and
keep updated. We also use the blog to ask a lot of questions to help make
sure that our tutorials represent the best-practices and opinions of a larger
community. If you're one of those smart guys or gals, we definitely hope
you'll show up in the future.

http://feeds.feedburner.com/knpuniversity

Simple Tech: \\Suin\\RSSWriter
------------------------------

Creating an RSS feed is simple and was made even easier by `\\Suin\\RSSWriter`_.
It's built with PHP namespaces, is `tested in Travis`_, is all setup with
Composer and does nothing more than help dump RSS. There are also some Symfony2
bundles for RSS, but creating a feed is so simple that these seem like overkill.

So if you have a simple RSS problem, this simple library solves it nicely.
It doesn't support all of the fields possible in `RSS 2.0`_ (something that's
easily fixed), and you can certainly argue that Atom is better, but it got
us up and running fast.

Like something better? Let us know :).

Cheers!

.. _`RSS feed`: http://feeds.feedburner.com/knpuniversity
.. _`\\Suin\\RSSWriter`: https://github.com/suin/php-rss-writer
.. _`tested in Travis`: https://travis-ci.org/suin/php-rss-writer
.. _`RSS 2.0`: http://en.wikipedia.org/wiki/RSS#RSS_Compared_to_Atom
