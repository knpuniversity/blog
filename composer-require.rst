Update your Docs for "composer require", then Celebrate with a Sandwich
=======================================================================

Pretty much every package manager works the same: run the executable (``apt-get``),
add the command ``install``, pass it a library name ``php5``, then make a
sandwich:

.. code-block:: bash

    apt-get install php5
    # ... makes a sandwich

But not Composer, right? Typically, the docs for a a PHP library looks like
this:

1. Add the following to your ``composer.json`` file:

.. code-block:: json

    {
        "require": {
            "my-cool-name/sandwich-maker": "~1.1.0"
        }
    }

2. Run the following command:

.. code-block:: bash

    composer update my-cool-name/sandwich-maker

Here are the problems:

- Needing 2 steps is a bummer.
- Manually updating ``composer.json`` can confuse beginners: "I already
  have a ``require`` key, should I replace what I have with this?".
- The latest version of my sandwich maker is ``2.5.4``, but since I forgot
  to update the docs, people are still installing the ancient version... which
  only makes `grilled cheese`_.

One Step Installation
---------------------

Thanks to some help from `Jordi`_ (friends call him "Mr Composer"), we merged
a `update to Composer`_ on Sep 23rd to make Composer's ``require`` command
automatically select the latest version constraint for you.

This means I can update my installation docs to say this:

    Run `composer require my-cool-name/sandwich-maker`.

That's it! Composer automatically selects the latest version (using the ``~``
constraint when possible), updates ``composer.json`` and runs the update.
This means you get ``~2.5.4`` in ``composer.json`` and a sandwhich maker
that cooks `fancy sandwiches`_.

Got it? Now let's go update our docs or open pull requests for libraries
that we use... and then have a sandwich.

.. _`Jordi`: https://twitter.com/seldaek
.. _`update to Composer`: https://github.com/composer/composer/pull/3096
.. _`grilled cheese`: http://lifeasmodernwife.files.wordpress.com/2011/12/img_2240.jpg
.. _`fancy sandwiches`: http://everyoneneedsaplanb.com/wp-content/uploads/2012/08/Fancy-Sandwich.jpg
