---
published: xxxx-xx-xx
category: tech
preview: |-
  Stuck on PHPUnit 9.6 because of the Symfony PHPUnit Bridge's awesome deprecation report? PHPUnit 13
  has (almost) caught up. Here's how to rebuild `max[self]=0&max[direct]=0` with PHPUnit's own
  deprecation handling - and what's still missing.
---

# Upgrading from PHPUnit 9.6: Keeping Symfony's Deprecation Reporting

Confession time: a lot of my projects stayed on PHPUnit 9.6 far longer than they should have.
PHPUnit 10 came out in early 2023, and every so often I'd try the upgrade, look at the test
output... and quietly put it off for another day.

The reason wasn't PHPUnit itself. It was the deprecation report from Symfony's
[PHPUnit Bridge](https://symfony.com/doc/current/components/phpunit_bridge.html). With a single
`SYMFONY_DEPRECATIONS_HELPER` environment variable, my test suite would fail on deprecations *I*
caused, ignore the noise from vendor code calling other vendor code, and print a tidy,
deduplicated summary at the end. It's a big part of how we
[upgrade Symfony without breaking things](https://symfonycasts.com/blog/deprecations-explained).

PHPUnit 10 took over error handling, and the bridge's report stopped working. PHPUnit's own
deprecation handling was still young: Symfony's silenced deprecations were invisible, there was no
way to tell *my* deprecations from a vendor's, and failing on deprecations was all or nothing.

The good news? PHPUnit has been busy. Release by release, those gaps have been filled in, and as
of PHPUnit 13.3, we have *almost* feature parity. We'll get to the "almost".

And the bridge isn't going anywhere - it just has a new job. Let's look at what my old setup did,
rebuild it on PHPUnit 13, and finally leave 9.6 behind.

## The Setup I Wanted to Keep

Here's the line from my old `phpunit.xml.dist` that I wasn't willing to give up:

```xml
<server name="SYMFONY_DEPRECATIONS_HELPER" value="max[self]=0&amp;max[direct]=0&amp;quiet[]=indirect&amp;quiet[]=other"/>
```

The bridge sorts every deprecation into a group, based on *where* it was triggered and *who*
called it. This config sets a rule for each one:

* `max[self]=0`: **self** deprecations are triggered by my own code - like a method in `src/`
  that I've marked as deprecated. Allow zero of them: fail the build.
* `max[direct]=0`: **direct** deprecations happen when my code calls something deprecated in a
  vendor package. These are mine to fix, so allow zero of them too.
* `quiet[]=indirect`: **indirect** deprecations happen when vendor code calls *other* vendor code
  that's deprecated. Not my problem (yet) - I just need to wait for that package to update. They
  don't fail the build, and `quiet` shrinks them down to a single count.
* `quiet[]=other`: deprecations the bridge couldn't attribute to anyone - usually from config files
  or the cached container. Same deal: show the count, nothing more.

To see this in action, I built a small Symfony 7.4 app with a healthy collection of deprecations:
a deprecated method of my own, a controller calling `Request::get()`, a command that uses
`Application::add()`, a class extending a `@final` Symfony class, a third-party bundle that's
behind on its own upgrades, and a few more. Here's the PHPUnit 9.6 output:

```
PHPUnit 9.6.38 by Sebastian Bergmann and contributors.

Testing
..............                                                    14 / 14 (100%)

Time: 00:00.209, Memory: 32.00 MB

OK (14 tests, 22 assertions)

Remaining self deprecation notices (4)

  2x: Since app 1.1: Method "App\Greeter::hello()" is deprecated, use "greet()" instead.
    1x in GreeterTest::testHello from App\Tests
    1x in GreeterTest::testHelloAgain from App\Tests

  1x: Method "Symfony\Component\Console\Command\Command::configure()" might add "void" as a native return type declaration in the future. Do the same in child class "App\Command\GreetCommand" now to avoid errors or add an explicit @return annotation to suppress this message.
    1x in GreetCommandTest::testExecute from App\Tests\Command

  1x: App\Greeter::greet(): Implicitly marking parameter $punctuation as nullable is deprecated, the explicit nullable type must be used instead
    1x in GreetCommandTest::testExecute from App\Tests\Command

Remaining direct deprecation notices (11)

  9x: Since symfony/http-foundation 7.4: Request::get() is deprecated, use properties ->attributes, query or request directly instead.
    8x in GreetControllerTest::testGreet from App\Tests\Controller
    1x in GreetControllerTest::testLocale from App\Tests\Controller

  1x: Since symfony/console 7.4: The "Symfony\Component\Console\Application::add()" method is deprecated and will be removed in Symfony 8.0, use "Symfony\Component\Console\Application::addCommand()" instead.
    1x in GreetCommandTest::testExecute from App\Tests\Command

  1x: The "Symfony\Bundle\FrameworkBundle\Controller\RedirectController" class is considered final. It may change without further notice as of its next major version. You should not extend it from "App\Controller\HomepageRedirectController".
    1x in GreetCommandTest::testRedirectControllerExists from App\Tests\Command

Remaining indirect deprecation notices (11)

Other deprecation notices (1)
```

Everything I need to fix, with nothing I can't. Each deprecation is listed *once*, with a count
and the tests that triggered it: `Request::get()` was hit 9 times across 8 data sets and another
test, and that's just 3 lines. The indirect and "other" deprecations shrink to a count, and since
there are self and direct deprecations, the build fails.

That's the bar. Let's see how PHPUnit 13 measures up.

## How PHPUnit Caught Up

PHPUnit didn't close the gap in one big release - it's been chipping away at it for three years.
The highlights:

* **10.2**: `ignoreSuppressionOfDeprecations` - PHPUnit can finally *see* Symfony's deprecations.
* **11.1**: The self/direct/indirect distinction arrives, plus `<deprecationTrigger>` to make it
  work with Symfony.
* **13.1**: Custom "issue trigger resolvers" - a hook for tools like the bridge to help classify
  tricky deprecations.
* **13.3**: The big one: `failOnSelfDeprecation`, `failOnDirectDeprecation` and friends - fail on
  *my* deprecations while still showing the rest.

That 13.3 release is the one that made me finally take the plunge. Interestingly, its
[pull request](https://github.com/sebastianbergmann/phpunit/pull/6827) calls out projects "that
follow the workflow of Symfony's PHPUnit Bridge" as the motivation. Thanks, Sebastian!

## Upgrading (and a Sneaky Trap)

First, the easy part. PHPUnit 13 requires PHP 8.4, so make sure you're there, then:

```terminal
composer require --dev phpunit/phpunit:^13.4 symfony/phpunit-bridge:^8.2 -W
```

***TIP
Moving from 9.6 to 13 comes with other chores too - annotations like `@dataProvider` are now
attributes, for example. [Rector](https://getrector.com/) has PHPUnit rule sets that handle most of
that, so I'll stay focused on deprecations.
***

Now, run the tests with the *old* `phpunit.xml.dist`:

```
PHPUnit 13.4.5 by Sebastian Bergmann and contributors.

Runtime:       PHP 8.4.27
Configuration: phpunit.xml.dist

D.............                                                    14 / 14 (100%)

Time: 00:00.197, Memory: 44.50 MB

OK, but there were issues!
Tests: 14, Assertions: 22, Deprecations: 1, PHPUnit Deprecations: 1.
```

Wait... *one* deprecation? We had 27 a minute ago! And the exit code is `0`: the build passes.

This is the sneaky trap. PHPUnit 13 doesn't know about `SYMFONY_DEPRECATIONS_HELPER`, so it's
ignored completely. And by default, PHPUnit ignores *silenced* deprecations - which is how Symfony
triggers nearly all of them. The only one that survived is a PHP deprecation. (The "PHPUnit
Deprecation" is PHPUnit politely asking us to update our config file.)

No errors, no warnings - just a quiet, green build. Yikes.

### Updating the Recipes

The fix is in the Flex recipes, which ship a PHPUnit 13-ready config:

```terminal
composer recipes:update phpunit/phpunit
composer recipes:update symfony/phpunit-bridge
```

Expect a conflict or two in `phpunit.xml.dist` around the old `SYMFONY_*` variables - those can
all go.

***IMPORTANT
**Review the diff carefully** before committing: there's an
[open Flex bug](https://github.com/symfony/flex/issues/1108) where updating the
`symfony/phpunit-bridge` recipe *deletes* files it shouldn't, like your entire `.gitignore`. It
bit me while writing this post!
***

Let's run the tests again:

```
DDDDDDDDDDD.DD                                                    14 / 14 (100%)

1 test triggered 1 PHP deprecation:

1) src/Greeter.php:7
App\Greeter::greet(): Implicitly marking parameter $punctuation as nullable is deprecated, ...

13 tests triggered 5 deprecations:

1) vendor/symfony/error-handler/DebugClassLoader.php:363
Method "Symfony\Component\Console\Command\Command::configure()" might add "void" as a native return type ...

2) vendor/symfony/console/Application.php:559
Since symfony/console 7.4: The "Symfony\Component\Console\Application::add()" method is deprecated ...

3) vendor/symfony/error-handler/DebugClassLoader.php:363
The "Symfony\Bundle\FrameworkBundle\Controller\RedirectController" class is considered final. ...

4) vendor/symfony/http-foundation/Request.php:775
Since symfony/http-foundation 7.4: Request::get() is deprecated, ...

5) src/Greeter.php:17
Since app 1.1: Method "App\Greeter::hello()" is deprecated, use "greet()" instead.

OK, but there were issues!
Tests: 14, Assertions: 22, Deprecations: 6.
```

***NOTE
I've trimmed the messages and the list of tests under each deprecation.
***

Now *that's* more like it! The build fails, and these are the exact six deprecations the bridge
reported as self and direct. Out of the box, the recipe gives us `max[self]=0&max[direct]=0`.

What's missing are the indirect deprecations: the bridge counted 11 indirect and 1 "other", and
here... nothing. Not even a count. That takes one small tweak.

## The New Config

Here are the deprecation-related parts of the config the recipe gave us:

```xml
<phpunit
    failOnDeprecation="true"
    failOnNotice="true"
    failOnWarning="true"
    ...
>
    <!-- ... -->

    <source ignoreSuppressionOfDeprecations="true"
            ignoreIndirectDeprecations="true"
            restrictNotices="true"
            restrictWarnings="true"
    >
        <include>
            <directory>src</directory>
        </include>

        <deprecationTrigger>
            <function>trigger_deprecation</function>
        </deprecationTrigger>

        <issueTriggerResolvers>
            <issueTriggerResolver className="Symfony\Bridge\PhpUnit\DebugClassLoaderIssueTriggerResolver" />
        </issueTriggerResolvers>
    </source>

    <extensions>
        <bootstrap class="Symfony\Bridge\PhpUnit\SymfonyExtension">
            <parameter name="clock-mock-namespaces" value="App" />
            <parameter name="dns-mock-namespaces" value="App" />
        </bootstrap>
    </extensions>
</phpunit>
```

Each piece is doing important work, so let's walk through it - then make the tweak.

### `<source>`

PHPUnit decides what's "mine" using `<source>`: anything in `src/` is first-party code. Test code
counts too, so a test calling a deprecated method directly is a *direct* deprecation, just like with
the bridge.

### `ignoreSuppressionOfDeprecations`

This is the critical one. Symfony triggers deprecations with
[`trigger_deprecation()`](https://github.com/symfony/deprecation-contracts/blob/main/function.php),
which *silences* them with `@`. That way they don't show up as noise in production. But by default,
PHPUnit skips silenced deprecations. Remove this option and every Symfony deprecation vanishes...
along with your failing build. That's the trap we just fell into.

### `<deprecationTrigger>`

When Symfony deprecates a method, the deprecation is actually *triggered* inside
`trigger_deprecation()`, a vendor function. Without this element, PHPUnit sees vendor code
triggering a deprecation and has to guess who's responsible. It guesses wrong: direct deprecations
get classified as indirect - so they stop failing the build - and every deprecation points at
`vendor/symfony/deprecation-contracts/function.php`.

Listing `trigger_deprecation` here tells PHPUnit to look one step further up the call stack, at
whoever *called* it.

***TIP
Using Doctrine? The recipe also adds Doctrine's `Deprecation::trigger()` and
`Deprecation::delegateTriggerToBackend()` here.
***

### `SymfonyExtension`

This is the bridge's new job. Instead of reporting deprecations, it's a PHPUnit extension that
turns on Symfony's `DebugClassLoader`. That's the thing behind deprecations like "this method might
add a `void` return type" or "this class is considered final". Skip the extension, and those
deprecations are never even triggered.

It also registers the bridge's
[`ClockMock`](https://symfony.com/doc/current/components/phpunit_bridge.html#clock-mocking) and
[`DnsMock`](https://symfony.com/doc/current/components/phpunit_bridge.html#dns-sensitive-tests)
helpers.

### `<issueTriggerResolvers>`

`DebugClassLoader` checks each class as it's autoloaded: if your class extends a `@final` class,
or overrides a method without a return type the parent will soon require, it triggers a
deprecation. But it triggers it from *inside the autoloader*, deep in vendor code, so PHPUnit files
it as indirect - and those never fail the build.

PHPUnit 13.1 added a hook for exactly this: issue trigger resolvers. And the bridge has one:
`DebugClassLoaderIssueTriggerResolver` works out which of *your* classes caused the deprecation, so
it's correctly classified as self or direct. It's new in `symfony/phpunit-bridge` 8.2.

***TIP
The bridge supports older Symfony versions, so you can use bridge 8.2 on a Symfony 7.4 app too.
***

### The Tweak: `failOnSelfDeprecation` & `failOnDirectDeprecation`

I *do* care about indirect deprecations, just not enough to fail the build. They're a heads-up that
a package needs an update before I can move to the next major version.

The recipe's `ignoreIndirectDeprecations="true"` hides them completely, so I removed it. But then
`failOnDeprecation="true"` would fail the build on them, so I swapped it for the 13.3
trigger-specific options:

```diff
 <phpunit
-    failOnDeprecation="true"
+    failOnSelfDeprecation="true"
+    failOnDirectDeprecation="true"
     ...
 >
     <source ignoreSuppressionOfDeprecations="true"
-            ignoreIndirectDeprecations="true"
             ...
     >
```

Self and direct deprecations still fail the build, and indirect ones are reported without failing
it.

***NOTE
You might be tempted to keep `failOnDeprecation="true"` and add
`failOnIndirectDeprecation="false"`. That doesn't work - PHPUnit ignores it and prints a warning
telling you to use the `--do-not-fail-on-indirect-deprecation` CLI option instead.
***

## Side by Side

So how does PHPUnit 13 compare to my old bridge setup? The good news first: the build fails on
*exactly* the same deprecations as before. My own deprecations, my calls to deprecated code and the
`DebugClassLoader` deprecations all fail the build, and the indirect ones are there without failing
it. Behavior-wise, we have parity!

But the report is a different story: it's 116 lines for 14 tests, compared to the bridge's 37.
The bridge summed up `Request::get()` in 3 lines. Here's PHPUnit's version:

```
7) vendor/symfony/http-foundation/Request.php:775
Since symfony/http-foundation 7.4: Request::get() is deprecated, use properties ->attributes, query or request directly instead.

Triggered by:

* App\Tests\Controller\GreetControllerTest::testGreet#Beau (2 times)
  tests/Controller/GreetControllerTest.php:14

* App\Tests\Controller\GreetControllerTest::testGreet#Diane (2 times)
  tests/Controller/GreetControllerTest.php:14

* App\Tests\Controller\GreetControllerTest::testGreet#Jerry (2 times)
  tests/Controller/GreetControllerTest.php:14

* App\Tests\Controller\GreetControllerTest::testGreet#Kevin (2 times)
  tests/Controller/GreetControllerTest.php:14

* App\Tests\Controller\GreetControllerTest::testGreet#Leannapelli (2 times)
  tests/Controller/GreetControllerTest.php:14

* App\Tests\Controller\GreetControllerTest::testGreet#Rachel (2 times)
  tests/Controller/GreetControllerTest.php:14

* App\Tests\Controller\GreetControllerTest::testGreet#Ryan (2 times)
  tests/Controller/GreetControllerTest.php:14

* App\Tests\Controller\GreetControllerTest::testGreet#Victor (2 times)
  tests/Controller/GreetControllerTest.php:14

* App\Tests\Controller\GreetControllerTest::testLocale (2 times)
  tests/Controller/GreetControllerTest.php:30
```

Every data set gets its own entry. Now imagine a deprecation that's hit by 300 tests. The
*behavior* is there, but the *report* is a lot noisier. That's the "almost" - more on it in a bit.

## Translating the Rest of the Bridge

`SYMFONY_DEPRECATIONS_HELPER` wasn't the bridge's only deprecation tool. Here's where the rest of
them went:

* **`@group legacy`** → `#[IgnoreDeprecations]` on a test method or class. Since PHPUnit 12.4, you
  can pass a pattern to only ignore specific deprecations - with no regex delimiters:
  `#[IgnoreDeprecations('Greeter::hello\(\)')]`.
* **`expectDeprecation()`** → `$this->expectUserDeprecationMessage('...')`, or
  `expectUserDeprecationMessageMatches()` for a regex. Just like the bridge needed `@group legacy`
  here, you still need `#[IgnoreDeprecations]`: otherwise the expected deprecation is *also*
  reported.
* **`generateBaseline`** → `vendor/bin/phpunit --generate-baseline phpunit-baseline.xml`, then
  `<source baseline="phpunit-baseline.xml">`. Existing deprecations are ignored, and only *new* ones
  fail the build.
* **`SYMFONY_DEPRECATIONS_HELPER=/regex/`** → `--stop-on-deprecation="RedirectController"`: stop at
  the first deprecation containing that text and print its stack trace.
* **`weak` & `disabled`** → `--do-not-fail-on-self-deprecation --do-not-fail-on-direct-deprecation`.
  The shorter `--do-not-fail-on-deprecation` does *not* override the trigger-specific options in our
  config, so the build would still fail.
* **`ignoreFile`** → a deprecation filter (PHPUnit 13.3): a class that decides whether to ignore
  each deprecation.

That last one needs a bit of code. Implement `DeprecationFilter`:

```php
use PHPUnit\Event\Code\IssueTrigger\IssueTrigger;
use PHPUnit\Runner\DeprecationFilter;

final class IgnoreFileDeprecationFilter implements DeprecationFilter
{
    public function ignores(string $message, string $file, int $line, IssueTrigger $trigger): bool
    {
        // e.g. return true if $message matches a regex from your old ignore file
        return false;
    }
}
```

And register it inside `<source>`:

```xml
<deprecationFilters>
    <deprecationFilter className="App\Tests\IgnoreFileDeprecationFilter" />
</deprecationFilters>
```

Since the filter also receives the file, line and trigger type, you can get fancier than the
bridge ever could: ignore deprecations from a specific package, for example.

## The "Almost"

So, what's still missing? Nothing that changes *what* fails the build - the gaps are all about
the report.

* **No deduplication.** Every data set and every test gets its own entry, with no "8x in
  `testGreet`" summary and no limit. That's a deliberate choice: PHPUnit reports deprecations per
  test, consistent with how it reports everything else, while the bridge counted them across the
  whole test suite. Consistent... but on a big test suite, a single deprecation can scroll right
  off your screen.
* **No "count only" mode.** `quiet[]=indirect` showed a count and nothing else. PHPUnit gives you
  all the details or nothing at all.
* **No groups.** Self, direct and indirect deprecations are all in one list, so you can't see at a
  glance which ones broke the build. Even worse: that "(2 times)" above is my controller calling
  `Request::get()` (direct) *and* the third-party bundle calling it (indirect). PHPUnit groups
  deprecations by where they're triggered, so both land in one entry.
* **No thresholds.** The bridge let you allow *some* deprecations - like `max[indirect]=10`.
  PHPUnit's options are on or off. A baseline is the closest substitute.

### Your Config Deprecations Are "Indirect"

Here's one that isn't new, but surprised me while writing this post. My demo app loads an XML
service file and uses fluent config builders, both deprecated in Symfony 7.4. Those are *my*
fault - but PHPUnit files them as indirect (the bridge did too, or as "other"), so they never fail
the build.

Why? The deprecation is triggered while Symfony loads the config: Symfony code calling Symfony
code. My config file is what's deprecated, but it isn't code in `src/` - an XML file isn't PHP code
at all. With the bridge, `quiet[]` was hiding these from me for... who knows how long. With PHPUnit
13 and indirect deprecations visible, I can at least *see* them. So keep an eye on that indirect
list: not everything in it is someone else's problem.

***TIP
Config deprecations are only triggered when the container is compiled. If your `var/cache/test` is
already warm, they won't show up at all. That's true with the bridge too, and on CI you start with
an empty cache anyway, but it's worth knowing when you run the tests locally.
***

## Bye Bye, 9.6

After years of putting it off, the upgrade turned out to be pretty painless. PHPUnit now does the
deprecation *work* the bridge used to do, and the bridge sticks around to fill in the
Symfony-specific gaps. The report is noisier, but it fails in all the right places - and that's what
keeps an upgrade safe.

Here's the checklist:

1. Require `phpunit/phpunit:^13.4` and `symfony/phpunit-bridge:^8.2`.
2. Update the `phpunit/phpunit` and `symfony/phpunit-bridge` Flex recipes, and check the diff
   (keep that `.gitignore`!).
3. Want to see indirect deprecations? Remove `ignoreIndirectDeprecations` and swap
   `failOnDeprecation` for `failOnSelfDeprecation` & `failOnDirectDeprecation`.
4. Translate `@group legacy`, `expectDeprecation()`, your ignore file and your baseline.
5. Update any CI jobs that used `weak` or `disabled`.
6. Clear your cache, run the tests and make sure they fail on the deprecations you expect - remember
   that sneaky green build!

***SEEALSO
Once the build is green for the *right* reasons, check out the
[Symfony 8 Upgrade](https://symfonycasts.com/screencast/symfony8-upgrade) course to put all those
deprecations to work.
***

Still on 9.6? Hit a gap I didn't mention? Let us know in the comments below!
