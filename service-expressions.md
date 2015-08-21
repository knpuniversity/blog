# Symfony Service Expressions: Do things you thought Impossible

Did you know you can do this with Symfony's service container?

```yaml
# app/config/services.yml
services:
    app_user_manager:
        class: AppBundle\Service\UserManager
        arguments:
            - "@=service('doctrine').getRepository('AppBundle:User')"
            # ... other arguments
```

The `@=` means that you're using Symfony's [Expression Language](http://symfony.com/doc/current/components/expression_language/syntax.html),
which let's you mix dynamic logic into your normally-static service definitions.

Normally, if you want to inject a repository, you need to register it as
a service first, using a [factory](http://symfony.com/doc/current/components/dependency_injection/factories.html). And while that's fine (and probably
better if you're injecting the factory a lot), using the expression language
is well, kinda cool.

In the [compiled container](http://knpuniversity.com/screencast/symfony-journey-di/symfony-builds-the-container#the-cached-container), it beautifully generates with exactly the code
you would've used directly::

    // app/cache/appDevDebugProjectContainer.php
    protected function getAppUserManagerService()
    {
        return $this->services['app_user_manager'] = new AppBundle\Service\UserManager(
            $this->get('doctrine')->getRepository('AppBundle:User')
        );
    }

And from an object-oriented, dependency-injection perspective, that feels
good, and makes up for the slightly-wonky service expression syntax. The
expression language isn't new, but I think a lot of people don't really know
its power.

## Passing Database Configuration as a Service Argument

How about reading configuration from the database and passing those as arguments?
Normally, you'd need create a service that reads the configuration and inject
the *whole* thing in:

```yaml
services:
    # some service that can read configuration values from the database
    app_configuration_reader:
        class: AppBundle\Config\ConfigurationReader
        arguments: ["@doctrine.orm_entity_manager"]

    # some service that helps email things
    app_mailer:
        class: AppBundle\Service\Mailer:
        arguments:
            - "@mailer"
            - "@app_configuration_reader"
```

Suppose our `app_mailer` service needs some database configuration value
called `email_from_username`, which it uses as the `from` address when
sending emails. To accomplish this, you'd usually need inject the entire
`app_configuration_reader` service (the service you create to read config
values). But with expressions, you can inject *only* what you need:

```yaml
    services:
        # some service that can read configuration values from the database
        app_configuration_reader:
            class: AppBundle\Config\ConfigurationReader
            arguments: ["@doctrine.orm_entity_manager"]

        # some service that helps email things
        app_mailer:
            class: AppBundle\Service\Mailer:
            arguments:
                - "@mailer"
                - "@=service('app_configuration_reader').get('email_from_username')"
```

And in addition to the `service()` function you also have a `parameter`
function, and all the normal syntax (including `if` statement logic) from
the [Expression Language](http://symfony.com/doc/current/components/expression_language/syntax.html). See [Using the Expression Language](http://symfony.com/doc/current/book/service_container.html#using-the-expression-language) for a few
more details about using it with services.

Now, go do something cool with this :).
