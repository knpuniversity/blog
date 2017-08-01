# Changes to Symfony Tutorials for Symfony 3.3

Symfony 3.3 comes with some pretty revolutionary changes to autowiring and how
services are registered and named. Woo! We talk about *all* of these cool things
in our [Symfony 3.3: Upgrade, Autowiring & Autoconfigure](http://knpuniversity.com/screencast/symfony-3.3)
tutorial.

## Problems with Existing Tutorials?

All these new features are *optional*.. but if you start a *new* project today,
these features are activated *automatically*. Why? Because a new Symfony 3.3 (or 3.4)
project comes with this code in `app/config/services.yml`:

<a name="services-yml"></a>

```yml
# app/config/services.yml
services:
    _defaults:
        autowire: true
        autoconfigure: true
        public: false

    AppBundle\:
        resource: '../../src/AppBundle/*'
        exclude: '../../src/AppBundle/{Entity,Repository,Tests}'

    AppBundle\Controller\:
        resource: '../../src/AppBundle/Controller'
        public: true
        tags: ['controller.service_arguments']
```

I *love* these new [features](http://knpuniversity.com/screencast/symfony-3.3).
But, most tutorials on KnpU were created *before* Symfony 3.3: so *before* these
features existed. This can cause a few issues!

## The Fix

If you like to code along with our tutorials (yay!), your best option is to *always*
download the course code from the tutorial page and use the `start/` directory. That
way, you're coding along with the *exact* code we used in the tutorials.

But, if you'd rather code along with a new project, then, to avoid problems, *comment out*
all of the [new code in services.yml](#services-yml) (unless we say in the tutorial
that we're using the new Symfony 3.3 autowiring goodness).

Yep, that's it! Then later, when you're curious, check out the
[Symfony 3.3: Upgrade, Autowiring & Autoconfigure](http://knpuniversity.com/screencast/symfony-3.3)
tutorial to learn about this new stuff!

And of course, if you ever have any questions at all, just add a comment below the
video: we're happy to help!

## Autowiring Deprecation Warnings

Oh, one last thing! If you're following the pre-Symfony 3.3 tutorials, when you
take advantage or [autowiring](http://knpuniversity.com/screencast/symfony-services/autowiring-madness),
you may start to see deprecation warnings. Don't worry about this! The way autowiring
works has changed a little bit in Symfony 3.3. You can learn more about these deprecations
by watching [Autowiring Deprecations](http://knpuniversity.com/screencast/symfony-3.3/deprecated-autowiring).

Have fun!
