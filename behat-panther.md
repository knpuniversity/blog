# Wanna see some Panther

In [the previous blog post](https://symfonycasts.com/blog/behat-symfony), we successfully configured Behat with Symfony 5.
Everything was super cool, and we used Selenium to operate our tests that required JavaScript.
But what about using [Panther](https://github.com/symfony/panther) instead? Panther is
an alternative to Selenium that can be a bit easier to use *and* faster, because
it allows you to run tests in a "headless" browser (i.e. the tests run in a browser
but without visually *opening* the browser).

## Installing Symfony Panther extension for Behat

Ok, we need a **Panther**! So... _let's go to the Zoo_! Or, maybe just install the digital
version with Composer:

```terminal
composer require robertfausk/behat-panther-extension --dev
```

This library integrates Panther with Behat... and though it's still pretty young, it works
really well!

After it finishes, you won't find many changes to your project: only the `composer.json`
and `composer.lock` files have changed. But you *will* see that the `symfony/panther` recipe
post-install prints a recommendation:

> Install ChromeDriver or geckodriver

Woohoo! That's something we already did! If you missed it - check out the
[previous blog post](https://symfonycasts.com/blog/behat-symfony#real-browser-power).

The post-install text also says:

> To use Panther with PHPUnit, uncomment the snippet registering the Panther extension
> in `phpunit.xml.dist`

We won't be using Panther inside PHPUnit, so you can ignore this.

## Configuring PantherExtension

We now have a PantherExtension in our app, which is like a "plugin" between Behat
and Panther. To tell Behat to use it, update your `behat.yaml.dist` file:

```yaml
default:
    # ...
    extensions:
        # ...
        Robertfausk\Behat\PantherExtension: ~ # no configuration here
        Behat\MinkExtension:
        # ...
```

Now we need to configure `Behat\MinkExtension` to use **Panther** for the JavaScript session.
Here is the minimum configuration:

```yaml
default:
    # ...
    extensions:
        # ...
        Behat\MinkExtension:
            # ...
            javascript_session: panther
            # ...
            panther:
                options:
                    browser: 'chrome'
```

To run tests, open your terminal and execute:

```terminal
vendor/bin/behat
```

And... woohoo, everything should work! You won't *see* the browser open (like you
did with Selenium), but your tests *are* being run in a real browser. If you get an exception like:

> The port 9080 is already in use. (RuntimeException)

No problem, just configure another port for **Panther**:

```yaml
default:
    # ...
    extensions:
        # ...
        Behat\MinkExtension:
            # ...
            panther:
                options:
                    port: 9081  # or another port you like
                    browser: 'chrome'
```

By the way, you can find all available configuration options in
[PantherTestCaseTrait::$defaultOptions](https://github.com/symfony/panther/blob/e53feac1df95f2022979e86f40b2540306581c3c/src/PantherTestCaseTrait.php#L70-L79).

As you can see, configuring and using **Panther** is a bit easier than **Selenium**,
which we like. You can find more usage examples in the [robertfausk/behat-panther-extension repository](https://github.com/robertfausk/behat-panther-extension#usage-example)

That's all folks!
