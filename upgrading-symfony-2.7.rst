How we Upgraded to Symfony 2.7 (+ deprecation notices)
======================================================

Symfony 2.7 - the next LTS release - came out on Saturday, with bells and
whistles like `100+ new features/enhancements`_ and a surprise new bridge
component to `PSR-7`_.

So, we decided to upgrade immediately and report back. Let's go!

Upgrading composer.json
-----------------------

Since Symfony protects backwards-compatability, upgrading is *mostly* easy,
but we did hit a few minor things. Btw, there's a new `upgrade`_ section
on the docs about it - tell your friends!

Start by updating your ``composer.json`` changes to allow for ``2.7.*``:

.. code-block:: json

{
    "require": {
        "php": ">=5.3.3",
        "symfony/symfony" : "2.7.*",
        "...": "..."
    }
}

Now update this with composer. The command may surprise you:

.. code-block:: bash

    composer update symfony/symfony sensio/distribution-bundle --with-dependencies

This will upgrade your project, but there are two important things happening:

1. You need to upgrade ``sensio/distribution-bundle``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The version doesn't matter, just any new tag, ``2.3.14`` or higher? Why?
Symfony 2.7 comes with `deprecation warnings`_, which are awesome for knowing
what deprecated features you're using. But, it also means that you need to
silence ``E_USER_DEPRECATED`` warnings. The latest version of
``sensio/distribution-bundle`` builds an ``app/bootstrap.php.cache`` file
that silences ``E_USER_DEPRECATED`` warnings for you. You'll still see the
deprecated warnings in your web debug toolbar (awesome).

So, if you see a friend who's getting a lot messages that look like this:

    Deprecated: The Symfony\Component\DependencyInjection\Definition::setFactoryMethod
    method is deprecated since version 2.6 and will be removed in 3.0.

Tell them to upgrade their ``sensio/distribution-bundle``. And of course,
to eventually fix these deprecated calls.

2. You Need --with-dependencies
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This flag tells composer to update ``symfony/symfony`` *and* all libraries
that it depends on. You *may* not need this, but without it, we got a dependency
error involving ``twig/twig``. The version of Twig in our project was not
compatible with Symfony and needs to be upgraded. To allow for this, you have
two options: use ``--with-dependencies``, or explictly upgrade the library
in question (e.g. ``twig/twig``):

.. code-block:: bash

    composer update symfony/symfony twig/twig sensio/distribution-bundle

So What Broke?
--------------

Yay, you upgraded! Now, what broke? In our case, not much: in fact, only
things in third-party bundles.

Upgrading FOSUserBundle
~~~~~~~~~~~~~~~~~~~~~~~

A new enhancement in Symfony 2.7 caused an accidental `bug`_ in FOSUserBundle.
This was fixed two months ago, but you'll need to upgrade FOSUserBundle to
get it. Unfortunately, there's no tag yet with the fix. Hopefully a new tag
will be `created soon`_.

Until then, you'll need to upgrade to the bleeding edge of whatever branch
you're using. If you're using a ``1.3`` release, update your ``composer.json``
with ``"friendsofsymfony/user-bundle": "1.3.x-dev"``. If you're using the
master (or 2.0) branch, update with ``"friendsofsymfony/user-bundle": "~2.0@dev"``.
Then, update:

.. code-block:: bash
    
    composer update friendsofsymfony/user-bundle

But check the UPGRADE log on the bundle to see what might have changed!

Fixing Behat 2.5
~~~~~~~~~~~~~~~~

We're using Behat 2.5, which suffered from a minor BC break in Symfony 2.7.
If you get this error:

    Runtime Notice: Declaration of Behat\Behat\Console\Input\InputDefinition::getSynopsis()
    should be compatible with Symfony\Component\Console\Input\InputDefinition::getSynopsis($short = false)  

then welcome to the club. There are already two pull requests (`#749`_, `#750`_),
so assuming these are merged, you'll just need to upgrade.

But if you're impatient, you can hack around it. In our case, we copied
the ``InputDefinition`` from Behat, pasted it into ``src/Behat/Behat/Console/Input/InputDefinition.php``,
then applied the patch in `#749`_. By adding a small line to ``composer.json``,
you can get Composer to load *our* file instead of the original one:

.. code-block:: json

{
    "autoload": {
        "psr-0": {
            "...": "...",
            "Behat\\Behat\\Console\\Input": "src/"
        }
    },
}

File this under the category of "do not do, but I did it anyways". This is
a big hack, but I'm comfortable, because I'm hacking a testing tool only.
Dump the autoloader, and your Behat tests should start running again:

.. code-block:: bash

    composer dump-autoload

If you're using the symfony2 driver, Behat may also explode on the new deprecated
notices. To fix this, add the following at the top of your ``FeatureContext``
class::

    define('BEHAT_ERROR_REPORTING', E_ALL & ~E_USER_DEPRECATED);

Back to the tests!

.. _`100+ new features/enhancements`: http://symfony.com/blog/symfony-2-7-0-released
.. _`PSR-7`: http://symfony.com/blog/psr-7-support-in-symfony-is-here
.. _`upgrade`: http://symfony.com/doc/current/cookbook/upgrade/index.html
.. _`deprecation warnings`: http://symfony.com/doc/current/cookbook/upgrade/major_version.html#make-your-code-deprecation-free
.. _`#749`: https://github.com/Behat/Behat/pull/749
.. _`#750`: https://github.com/Behat/Behat/pull/750
.. _`bug`: https://github.com/FriendsOfSymfony/FOSUserBundle/issues/1775
.. _`created soon`: https://github.com/FriendsOfSymfony/FOSUserBundle/issues/1844
