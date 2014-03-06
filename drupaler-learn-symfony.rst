Want to be a Drupal 8 Expert? Learn Symfony
===========================================

.. note::

    Image courtesy of `Ekino`_, who has a bundle for using Drupal in Symfony.

I'm not a Drupal developer, but I *do* know **a lot** about Drupal 8. I know
how the event system works, what a service is, how it relates to a dependency
injection container and how the deepest and darkest of Drupal's request-response
workflow looks.

How?

Because I'm a Symfony developer. And if you want to get a jumpstart on Drupal 8,
you should be to. I'm *not* saying use Symfony instead of Drupal - they each
solve very different problems.

Use both.

What you can Learn from Symfony
-------------------------------

How about a list?

1) Code
~~~~~~~

Simply put: Drupal 8 and Symfony share *a lot* of code, called components
(think, little PHP libraries). You can see these right inside the `Drupal core on GitHub`_
and Symfony documents these individually (`Symfony Components`_). If you
use Symfony right now, you're using a lot of code from Drupal 8.

2) Double your Weaponry
~~~~~~~~~~~~~~~~~~~~~~~

Symfony isn't always the right tool for a job, and neither is Drupal. But
when you learn Symfony, you're doubling the arsenal that you can throw at
any problem.

And because so much is shared, you get these 2 tools at a discounted learning
price. Value!

3) PHP Namespaces
~~~~~~~~~~~~~~~~~

Drupal 8 uses PHP namespaces. But great news! These aren't that hard, and
after using Symfony for a bit (or any modern PHP framework or library), this
learning curve will be a thing of the past.

Bonus: if you have 2 minutes, we have a screencast for you: `PHP Namespaces in 120 Seconds`_.

4) Object-Oriented-ness
~~~~~~~~~~~~~~~~~~~~~~~

Yay, objects! Interfaces! Flexibility!

Objects are everywhere in Symfony and Drupal (and again, in almost every PHP
library these days). Use Symfony for a day to get *way* ahead on this.

5) The Container
~~~~~~~~~~~~~~~~

If you're a geek, one of the most exciting things about Drupal 8 is its use
of a "dependency injection container". Quick, take 20 minutes and learn about
dependency injection and containers: `Dependency Injection and the art of services and containers`_.
I'll wait.

In Symfony, everything stops and starts with the container. And in Drupal 8,
things will be much the same. The container (and good use of object-oriented
principles) allows you to modify *any* part of the Drupal core without - as
Larry Garfield puts it - killing any kittens (see `D8FTW: Hacking Core Without Killing Kittens`_).

Get into Symfony and you'll get right into the container. We even intro it
early in our Symfony series (`Rendering a Template`_) cause it's just so
darn important.

6) HttpKernel
~~~~~~~~~~~~~

HttpKernel: the beating heart of Symfony2 and Drupal 8.

It's a Symfony component (little PHP library!) and I'd tell you what it does,
but then I'd have to kill you, or at least dive you into a bunch of deep
code over coffee.

But basically: it's the code that starts with request information and transforms
it into a response. It involves dispatching Symfony events (another concept!),
executing a controller and returning a Response. Geek out here: `The HttpKernel Component`_.

So the code-sharing between Symfony and Drupal 8 doesn't include frivolous
pieces: they share the most fundamental code that makes things go. No matter
how different 2 different cars are, under the hood, they move using the same
basic mechanics. (Bad pun:) So take Symfony for a drive.

Start using Symfony!
--------------------

If you want to step through a real project, do yourself a favor and go through
our Symfony2 series and then build something. I've listed the 4 episodes,
with highlights that directly affect Drupalers:

* `Symfony2 Ep1`_ (install, bundles, routing, controller, services, Composer)
* `Symfony2 Ep2`_ (a lot more controllers)
* `Symfony2 Ep3`_ (JSON, dependency injection container, services, DI tags)
* `Symfony2 Ep4`_ (Assetic, Deployment)

And for an even lower barrier to entry, try `Silex`_! Here's a Silex app::

    require_once __DIR__.'/../vendor/autoload.php'; 

    $app = new Silex\Application(); 

    $app->get('/drupal/{version}', function($version) use($app) { 
        return 'You\'re using Drupal '.$version; 
    });

    $app->run(); 

And this *still* uses all the most important components used in Drupal 8 -
HttpKernel, EventDispatcher and a dependency injection container.

So start kicking butt in Drupal 8 right now and add another tool to your
arsenal.

Happy Drupal-Symfonying!

.. _`Drupal core on GitHub`: https://github.com/drupal/drupal/tree/8.x/core/vendor/symfony
.. _`Symfony Components`: http://symfony.com/doc/current/components/index.html
.. _`PHP Namespaces in 120 Seconds`: knpuniversity.com/screencast/php-namespaces-in-120-seconds
.. _`Dependency Injection and the art of services and containers`: http://knpuniversity.com/screencast/dependency-injection
.. _`D8FTW: Hacking Core Without Killing Kittens`: http://www.palantir.net/blog/d8ftw-hacking-core-without-killing-kittens
.. _`The HttpKernel Component`: http://symfony.com/doc/current/components/http_kernel/introduction.html
.. _`Ekino`: http://www.ekino.com/drupal-and-symfony2-dont-wait-for-drupal8/
.. _`Rendering a Template`: http://knpuniversity.com/screencast/symfony2-ep1/controller#rendering-a-template
.. _`Symfony2 Ep1`: http://knpuniversity.com/screencast/symfony2-ep1
.. _`Symfony2 Ep2`: http://knpuniversity.com/screencast/symfony2-ep2
.. _`Symfony2 Ep3`: http://knpuniversity.com/screencast/symfony2-ep3
.. _`Symfony2 Ep4`: http://knpuniversity.com/screencast/symfony2-ep4
.. _`Silex`: http://silex.sensiolabs.org/
