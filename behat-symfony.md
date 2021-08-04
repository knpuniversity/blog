# Setting up Behat on Symfony 5

If you're interested in Behat, we have a [great tutorial](https://symfonycasts.com/screencast/behat)
about it! Except that... well... that tutorial is getting old when it comes to
getting Behat installed and configured into a modern app.

So how *can* you use Behat with Symfony 5? Easy Peasy! Just find a Symfony 5 project,
install and configure Behat, then profit!

Too fast? Ok, let's look at each part, step-by-step.

## Install Behat with everything you need

To get a minimal setup, let's install some packages:

```terminal
composer require friends-of-behat/mink-extension friends-of-behat/mink-browserkit-driver friends-of-behat/symfony-extension --dev
```

As you can see, we're using packages from the `friends-of-behat` organization.
This setup uses the Symfony extension to make requests directly through Symfony's Client.

***TIP
To use Goutte to make real HTTP requests - use this command instead:

```terminal
composer require friends-of-behat/mink-extension friends-of-behat/mink-browserkit-driver behat/mink-goutte-driver --dev
```
***

You may see this warning:

> The recipe for this package comes from the "contrib" repository, which is open to
> community contributions.

Hit "yes" to continue the installation. This recipe has everything you need to run Behat:

```
|config/
| - services_test.yaml
|features/
| - demo.feature
|tests/
| - Behat/
|  - DemoContext.php
|behat.yml.dist
```

## Configure it

But it's still not enough. By default `FriendsOfBehat\SymfonyExtension` tries to load
`config/bootstrap.php`. But in Symfony 5, we don't have that file! No problem.

Because Behat is a test system, we can use the same configuration as, for example, PHPUnit.
In this case, we can use the same bootstrap file. The only problem is that, unless you already
have PHPUnit installed, you won't have this file!

Run:

```terminal
composer require phpunit --dev
```

This installs a few packages including a recipe with a shiny new `tests/bootstrap.php`
file that we can use.

Now we are unstoppable! Adjust your `behat.yaml.dist` to use this:

```yaml
default:
    # ...

    extensions:
        FriendsOfBehat\SymfonyExtension:
            bootstrap: tests/bootstrap.php
```

Now we are ready to test it. Check the `features/demo.feature` file out:

```gherkin
Feature:
    In order to prove that the Behat Symfony extension is correctly installed
    As a user
    I want to have a demo scenario

    Scenario: It receives a response from Symfony's kernel
        When a demo scenario sends a request to "/"
        Then the response should be received
```

It's not too complex, but enough to see if everything works! Run:

```terminal
./vendor/bin/behat
```

Woohoo we have Behat superpowers!

## PHPUnit Power

You may also want to use PHPUnit's assert functions inside Behat. Cool! We've
already installed PHPUnit, so we can immediately use its assertions:

```php
use PHPUnit\Framework\Assert;

final class DemoContext implements Context
{
    //...
    
    /**
     * @Then the response should be received
     */
    public function theResponseShouldBeReceived(): void
    {
        Assert::assertNotNull($this->response);
    }
}
```

Go to the console and run things again:

```terminal
./vendor/bin/behat
```

It's still green! That's a WIN!

***TIP
If you have `symfony/phpunit-bridge` installed instead of `phpunit/phpunit`,
using its built-in assertions is a bit trickier. We recommend installing
`phpunit/phpunit` directly.
***

## Real Browser Power

You may ask:

> Hey, what about testing in a real browser!?

Let's do it!

For this, we *will* need some additional tools: Java, Selenium and a browser to test with,
like Chrome or Firefox.

***TIP
Instead of using Selenium, you could also use Panther via https://github.com/robertfausk/behat-panther-extension.
We haven't tried this extension out yet, but in general, working with Panther is great and a bit
easier than Selenium.
***

The "browser" part of the setup is the easiest thing. You probably already have one installed.
But it *does* require a driver to work properly. For Chrome and Chromium-based browser, you
need `chromedriver`. For Firefox, you need `geckodriver`.

There are multiple ways to get the driver you need! The easiest is: [dbrekelmans/browser-driver-installer](https://github.com/dbrekelmans/browser-driver-installer) -
this package will check your system and automatically download supported drivers into a `drivers/`
directory in your app. All you need to run is:

```terminal
composer require --dev dbrekelmans/bdi
./vendor/bin/bdi detect drivers
```

Otherwise, use your favorite package manager or download it manually from the
official [ChromeDriver](https://sites.google.com/a/chromium.org/chromedriver/) and [GeckoDriver](https://github.com/mozilla/geckodriver)
repositories.

Then, before running your tests, start it:

```terminal
# if you downloaded it as a file with dbi
./drivers/chromedriver -v

# or if you installed it with your favorite package manager
chromedriver -v
```

Next up: selenium server! You can download it from https://www.selenium.dev/downloads/.
Move the downloaded file (e.g. `selenium-server-standalone-3.141.59.jar`) to an easily-accessible
folder so you can jump to it.

Executing `jar` files requires the Java Runtime to be installed. Check
if it's installed with:

```terminal
java -version
```

If you see an error like "Command not found", then install it. Installation, of course,
is one of those wonderful things that varies depending on your operating system.

Ok, so we have Java, Selenium and a browser with a driver! Next,
open a terminal, go to wherever you put the Selenium file, and run:

```terminal
java -jar selenium-server-standalone-3.141.59.jar
```

That will start Selenium, which means that Behat can now operate with your browser.
Now we need to tell Behat that we want to use Selenium instead of using Goutte
or the Symfony2 extension.

To do this, install the Selenium2 driver:

```terminal
composer require behat/mink-selenium2-driver --dev
```

After this finishes, add the following config to `behat.yml.dist`:

```yaml
default:
    # ...
    extensions:
        # ...
        Behat\MinkExtension:
            # adapt this to whatever the real URL is to your local site
            base_url: http://127.0.0.1:8000/
            # or use "goutte"
            default_session: symfony
            javascript_session: selenium2
            browser_name: chrome
            symfony: ~
            selenium2: ~
```

Make your context files extend `Behat\MinkExtension\Context\MinkContext`:

```php
use Behat\MinkExtension\Context\MinkContext;

final class DemoContext extends MinkContext
{
    // ...
}
```

Now we are ready to make some real queries to our website. Do it!
Open `features/demo.feature` and add a new scenario:

```gherkin
Feature:
    # ...

    Scenario: I can navigate to the homepage
        When I am on the homepage
        Then the response status code should be 200
```

Try it with:

```terminal
./vendor/bin/behat
```

Hey! Where is our browser? Add one more scenario to `features/demo.feature`,
but this time add `@javascript` above it:

```gherkin
Feature:
    # ...

    @javascript
    Scenario: I can open the homepage in the Browser
        When I am on the homepage
        Then I should see a "body" element
```

Try it again:

```terminal
./vendor/bin/behat
```

And... celebrate with a green pass mark on all tests!

If you have any issues, feel free to ask us in the comments below or anywhere
on the [Behat tutorial](https://symfonycasts.com/screencast/behat). We think Behat
and that tutorial are great... as long as you can get it installed.

That's all folks!
