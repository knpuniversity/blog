# How we Upgraded to Symfony 2.7 (+ deprecation notices)

***SEEALSO
When you're ready, also check out [How to Upgrade to Symfony 2.8, then 3.0!](http://knpuniversity.com/screencast/symfony3-upgrade)
***

Symfony 2.7 - the next LTS release - came out on Saturday, with bells and
whistles like [100+ new features/enhancements](http://symfony.com/blog/symfony-2-7-0-released) and a surprise new bridge
component to [PSR-7](http://symfony.com/blog/psr-7-support-in-symfony-is-here).

So, we decided to upgrade immediately and report back. Let's go!

## Upgrading composer.json

Since Symfony protects backwards-compatibility, upgrading is *mostly* easy,
but we did hit a few minor things. Btw, there's a new [upgrade](http://symfony.com/doc/current/cookbook/upgrade/index.html) section
on the docs - tell your friends!

Start by updating your `composer.json` changes to allow for `2.7.*`:

```json
{
    "require": {
        "php": ">=5.3.3",
        "symfony/symfony" : "2.7.*",
        "...": "..."
    }
}
```

Now update this with composer. The command may surprise you:

```bash
composer update symfony/symfony sensio/distribution-bundle --with-dependencies
```

This will upgrade your project, but there are two important things happening:

### 1. You need to upgrade `sensio/distribution-bundle`

The version doesn't matter, just any new tag, `2.3.14` or higher. Why?
Symfony 2.7 comes with [deprecation warnings](http://symfony.com/doc/current/cookbook/upgrade/major_version.html#make-your-code-deprecation-free), which are awesome for knowing
what deprecated features you're using. But, it also means that you need to
silence `E_USER_DEPRECATED` warnings. The latest version of
`sensio/distribution-bundle` builds an `app/bootstrap.php.cache` file
that silences `E_USER_DEPRECATED` warnings for you. Don't worry: you'll
still see the deprecated warnings in your web debug toolbar (see the image
on this post).

So, if you see a friend who's getting a lot messages that look like this:

    Deprecated: The Definition::setFactoryMethod method is deprecated since
    version 2.6 and will be removed in 3.0.

Tell them to upgrade their `sensio/distribution-bundle`. And of course,
to eventually fix these deprecated calls.

### 2. You Need --with-dependencies

This flag tells composer to update `symfony/symfony` *and* all libraries
that it depends on. You *may* not need this, but without it, we got a dependency
error involving `twig/twig`. The version of Twig in our project was not
compatible with Symfony and needed to be upgraded. To allow for this, you have
two options: use `--with-dependencies`, or explicitly upgrade the library
in question (e.g. `twig/twig`):

```bash
composer update symfony/symfony twig/twig sensio/distribution-bundle
```

## So What Broke?

Yay, you upgraded! Now, what broke? In our case, not much: in fact, only
things in third-party bundles.

### Upgrading FOSUserBundle

A new enhancement in Symfony 2.7 caused an accidental [bug](https://github.com/FriendsOfSymfony/FOSUserBundle/issues/1775) in FOSUserBundle.
This was fixed two months ago, but you'll need to upgrade FOSUserBundle to
get it.

If you're using a `1.3` release, just run the `update` command below
(a tag was *just* released with the fix). If you're using the master (or 2.0)
branch, that's not stable yet, and has no tag. Make sure your `composer.json`
file has something like `"friendsofsymfony/user-bundle": "~2.0@dev"`.
Then let composer work its magic:

```bash
composer update friendsofsymfony/user-bundle
```

But check the UPGRADE log on the bundle to see what might have changed.

### Fixing Behat 2.5

We're using Behat 2.5, which suffered from a minor BC break in Symfony 2.7.
If you get this error:

    Runtime Notice: Declaration of InputDefinition::getSynopsis()
    should be compatible with InputDefinition::getSynopsis($short = false)  

then welcome to the club! Fortunately, this issue *has* been fixed
(thanks to Stof for the fast release), so you just need to upgrade:

```bash
composer update behat/behat
```

If you're using the symfony2 driver, Behat may also explode on the new deprecated
notices. To fix this, add the following at the top of your `FeatureContext`
class::

    define('BEHAT_ERROR_REPORTING', E_ALL & ~E_USER_DEPRECATED);

Back to the tests! And welcome to Symfony 2.7.

If you hit other issues, comment below and maybe we can help others.

Cheers!
