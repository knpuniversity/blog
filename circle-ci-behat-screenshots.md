# Behat on CircleCI 2.0 with Failure Screenshots

***TIP
**tl;dr** By combining Behat and CircleCI, you can automatically take screenshots
on failures and see a list of them on each build. This is madness!
***

***TIP
This blog post was updated to support CircleCI 2.0 because CircleCI 1.0
will no longer be available after August 31, 2018.
***

For KnpUniversity, we use both [Behat](https://knpuniversity.com/screencast/behat)
and [CircleCI](https://knpuniversity.com/screencast/phpunit/continuous-integration)
for continuous integration. The combination works *great*, except for
debugging. If you've done functional testing in a CI environment before, then
you're probably familiar with (yikes!) *phantom failures*: those tests that *only*
seem to fail on the CI server, making them nearly impossible to debug.

I don't like to lose time debugging tests. And thanks to Behat and CircleCI, you
can easily take screenshots of the browser at the moment of those failures and see
them on the build afterwards. Now, when you forget to install `wkhtmltopdf` on your
CI server, you'll see a big error screenshot that tells you so. Sweet!

## CircleCI Setup

To run our `@javascript` Behat scenarios, we use Selenium 2. Our CircleCI 2.0 `config.yml` setup
(without the screenshot magic) looks like this:

```yml
# .circleci/config.yml
version: 2
jobs:
  build:
    docker:
      - image: php:7
      - image: mysql:5
        environment:
          MYSQL_ALLOW_EMPTY_PASSWORD: yes
      - image: selenium/standalone-chrome:3
    working_directory: ~/your-repository-name
    steps:
      - checkout

      # Update Composer and install dependencies
      - run: composer self-update
      - run: composer install --prefer-dist

      # Setup database
      - run: php bin/console doctrine:database:create --env=test
      - run: php bin/console doctrine:schema:create --env=test

      - run:
          name: Run web server
          command: php bin/console server:run --env=test -vvv localhost:8080 > server.log 2>&1:
          background: true

      # Run tests
      - run: php vendor/bin/behat
      # and maybe some unit tests
```

CircleCI 2.0 allows you to add Docker images, so you don't need to take care of a lot of
details that we [previously](https://knpuniversity.com/screencast/question-answer-day/travis-ci)
needed to worry about, like setting up `xvfb` and installing a browser (Chrome).

The `behat.yml` file doesn't need anything special, except that the web server port
needs to match the `8080` used above and you should use the `chrome` browser:

```yml
default:
    # ...
    extensions:
        Behat\MinkExtension:
            base_url: http://localhost:8080
            goutte: ~
            selenium2: ~
            browser_name: chrome
        Behat\Symfony2Extension: ~
```

That's it for CircleCI setup.

## Taking Screenshots on Failure

Next, we need to take a screenshot each time a scenario fails. Behat does most of
the work for you. Just add this to your `FeatureContext`:

```php
// FeatureContext.php
use Behat\Behat\Hook\Scope\AfterStepScope;
use Behat\Mink\Driver\Selenium2Driver;
use Behat\MinkExtension\Context\RawMinkContext

// ...

class FeatureContext extends RawMinkContext
{
    /**
     * @AfterStep
     */
    public function printLastResponseOnError(AfterStepScope $event)
    {
        if (!$event->getTestResult()->isPassed()) {
            $this->saveDebugScreenshot();
        }
    }

    /**
     * @Then /^save screenshot$/
     */
    public function saveDebugScreenshot()
    {
        $driver = $this->getSession()->getDriver();

        if (!$driver instanceof Selenium2Driver) {
            return;
        }

        if (!getenv('BEHAT_SCREENSHOTS')) {
            return;
        }

        $filename = microtime(true).'.png';
        $path = $this->getContainer()
            ->getParameter('kernel.root_dir').'/../behat_screenshots';

        if (!file_exists($path)) {
            mkdir($path);
        }

        $this->saveScreenshot($filename, $path);
    }
}
```

This uses the [@AfterStep hook](https://knpuniversity.com/screencast/behat/behat-hooks-background)
to save a timestamped screenshot into a `behat_screenshots/` directory on failure.
The `saveScreenshot()` method comes directly from the `RawMinkContext` class that's
available from [MinkExtension](http://knpuniversity.com/screencast/behat/behat-loves-mink).

***TIP
With a little more work, you could probably use the `AfterStepScope` object to save
a screenshot that's derived from the scenario's name.
***

It also *only* does this if it sees a `BEHAT_SCREENSHOTS` environmental variable:
I don't need screenshots when I'm working locally.

To finish things, we need to tweak the CircleCI setup to make sure this directory
exists and to set the `BEHAT_SCREENSHOTS` environment variable that activates everything:

```yml
# .circleci/config.yml
version: 2
jobs:
  build:
    # ...
    steps:
      # ...

      # Run tests
      - run: mkdir var/behat_screenshots/
      # add an empty file so that the directory isn't empty (else copy may fail)
      - run: touch var/behat_screenshots/empty.txt

      - run:
          name: Run Behat tests
          command: php vendor/bin/behat
          # Export environment variable for this single command shell
          environment:
            BEHAT_SCREENSHOTS: '1'
      # and maybe some unit tests

      - store_artifacts:
          path: var/behat_screenshots/
```

The `store_artifacts` command is special to CircleCI 2.0: it uploads a file or a directory so
that, when the job finishes, you can view those files in the Artifacts tab of the Job page in
your browser. There is no limit on the number of `store_artifacts` steps a job can run. Artifacts
will be available after the job finishes.

To become a Behat & Mink expert, check out our full
[BDD, Behat, Mink and other Wonderful Things](https://knpuniversity.com/screencast/behat)
tutorial.

Have fun!
