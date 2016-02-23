# Introducing KnpUOAuth2ClientBundle: Making Social Easy

    **tl;dr** [https://github.com/knpuniversity/oauth2-client-bundle](KnpUOAuth2ClientBundle)
    has hit 1.0 - use it for all sorts of "social connect" functionality in Symfony
    without a headache.

Logging in with Facebook, connecting with your Twitter account, or registering via
GitHub - integrating with "Social" networks is now a standard feature of many sites.
But in Symfony, it wasn't *easy* enough to build this. The most popular solution - 
HWIOAuthBundle - is good and has a *lot* of built-in OAuth providers. But, it's also
confusing to use and frustrating to extend. I know, we use it here on KnpUniversity.
I wanted a better option.

Today, I'm *thrilled* to announce the 1.0 release of [https://github.com/knpuniversity/oauth2-client-bundle](KnpUOAuth2ClientBundle),
which is built on the backs of the wonderful [OAuth 2.0 Client Library](https://github.com/thephpleague/oauth2-client)
from the PHP League. In other words: take the best from the PHP world and make it
*sing* inside of Symfony.

* Easily [configure OAuth2 client services](#config-usage)
* Integrate with [Guard Auth](#guard-auth) for authentication
* Support for "finish registration" when a brand new user connects to a service
* Automatic support for OAuth "state"

<a name="config-usage"></a>

## Simple to use: You're in Control

In a nut-shell, here's how it works:

1. You configure a provider. This gives you a "client" service:

```yml
# app/config/config.yml
knpu_oauth2_client:
    clients:
        # will create a service: knpu.oauth2.client.facebook_main
        facebook_main:
            type: facebook
            client_id: %facebook_app_id%
            client_secret: %facebook_app_secret%
            # see below
            redirect_route: connect_facebook_check
```

2. Use the new service to redirect to Facebook and do some cool stuff when the
   user comes back:

```php
// ...

class FacebookController extends Controller
{
    /**
     * Link to this controller to start the "connect" process
     *
     * @Route("/connect/facebook")
     */
    public function connectAction()
    {
        return $this->get('oauth2.registry')
            ->getClient('facebook_main')
            ->redirect();
    }

    /**
     * Facebook redirects to back here afterwards
     *
     * @Route("/connect/facebook/check", name="connect_facebook_check")
     */
    public function connectCheckAction(Request $request)
    {
        $client = $this->get('oauth2.registry')
            ->getClient('facebook_main');

        $user = $client->fetchUser();
        // do something with all this new power!
        $user->getFirstName();
    }
}
```

Simple, right?

<a name="guard-auth"></a>

## Guard Auth Integration

You can use this new power to just "get some user information" *or* actually *authenticate*
your user. For that, the bundles comes with integration for Symfony's [Guard](https://knpuniversity.com/screencast/guard)
security. This includes the ability to "finish registration": i.e. redirect a new
user to a registration form before logging them in. See the tutorial below for more
info.

## Tutorial & Documentation

Ready to try it? Let's do this!

A) [https://github.com/knpuniversity/oauth2-client-bundle/blob/master/README.md](Official Documentation)

B) [https://github.com/knpuniversity/guard-tutorial/tree/finished](Example Project)

C) [https://knpuniversity.com/screencast/guard](And soon, A full tutorial!)

Like everything, this is a community project meant to make our collective lives
easier (and honestly, more fun). If you have some ideas or problems - please
[open an issue](https://github.com/knpuniversity/oauth2-client-bundle/issues).

Cheers!
