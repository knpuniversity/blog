# Playwright-PHP Changes the Game for Symfony Testing

> "We migrated our browser test suite from Panther to Playwright...
>
> Went from ~39 seconds to ~18 seconds. And now it's possible to run this suite with paratest...
> since we can parallelize it now!"
>
> * [Jon Wage](https://x.com/jwage/status/2091699483553743034)

A test suite that relies on JavaScript, cut from ~39 seconds to ~18 seconds -
**~50% faster?!**

And now it can run in parallel with ParaTest?!

I've been working with [Simon André](https://github.com/smnandre) on this new
Playwright + Symfony setup, and the whole thing is kind of blowing my mind.

The architecture is incredibly clever and unlocks some things I didn't think
were possible with end-to-end tests.

Let's start at the bottom.

## Playwright

[Playwright](https://playwright.dev/) is a browser automation tool from
Microsoft.

It controls real Chromium, Firefox, and WebKit browsers, so your tests can click
around, fill out forms, execute JavaScript and interact with your app like a
real user.

The idea isn't new. We've had Selenium/WebDriver-style end-to-end testing for
years.

The problem? It wasn't a great experience. It was slow, flaky, and generally
painful to work with.

Playwright is a much more modern take on browser automation: fast, powerful, and
built around today's web apps.

The official library is JavaScript/TypeScript.

But we're PHP developers. So...

## Playwright-PHP

That's where
[`playwright-php/playwright`](https://github.com/playwright-php/playwright)
comes in.

Created by Simon along with other amazing contributors, its job - for our
purposes - is basically to get Playwright installed and usable for PHP
developers.

It gives us a PHP API for controlling Playwright and writing end-to-end tests
from PHP.

**Awesome!**

## Playwright + Symfony 🤯

But this is where things get **really** interesting.

Because Simon didn't stop at "Playwright, but for PHP".

With
[`playwright-php/playwright-symfony`](https://github.com/playwright-php/playwright-symfony),
he asked:

**What if we could make Playwright work *with* Symfony's kernel?**

With traditional end-to-end testing - including tools like [Symfony
Panther](https://github.com/symfony/panther) - the architecture looks like you'd
expect:

```text
Browser -> Web Server -> Symfony Kernel
```

The browser is a separate process, your app is behind a web server, and they
talk over HTTP.

Totally normal.

But Playwright has a powerful trick up its sleeve: it can **intercept network
requests and provide the response itself**.

And `playwright-symfony` takes that idea somewhere I absolutely did not expect.

### From Browser Request to Symfony Request

When the browser requests a page from your app, the request is intercepted
**before it ever reaches a web server**.

It's converted into a Symfony `Request`, passed **directly to the Symfony kernel
running inside your test**, and the resulting Symfony `Response` is handed
straight back to the browser.

So this:

```text
Browser -> Web Server -> Symfony Kernel
```

becomes this:

```text
Browser -> Symfony Kernel
```

**THE WEB SERVER IS A LIE!!!**

🤯🤯🤯

And this isn't a fake browser or some DOM simulation.

It's still a **real Chromium, Firefox or WebKit browser**.

It's rendering the page.

It's executing the JavaScript.

Fetch requests, Turbo, Stimulus, whatever your frontend is doing - it's
happening in a real browser.

But the Symfony requests are being handled **in-process by your kernel**.

That combination is incredible:

**Real browser + real JavaScript + in-process Symfony kernel.**

And suddenly the boundary that normally separates an E2E test from your Symfony
test environment basically disappears. Your test and your application share the
same process, so:

The container is right there.

The profiler is right there.

Your mocked services are right there.

This feels less like "another browser testing tool" and more like a new kind of
Symfony test: the browser realism of an end-to-end test with the superpowers we
normally only get from kernel testing.

That is just **so cool**.

## Zenstruck Browser

And... this is where I got involved.

Once I understood what was happening here, it felt **game changing** for Symfony
end-to-end testing. I wanted to push it further and get it into the hands of
more people.

So I brought Playwright support to
[`zenstruck/browser`](https://github.com/zenstruck/browser).

Zenstruck Browser is an expressive testing layer for Symfony apps. It gives you
a fluent API for clicking links, filling forms, making assertions,
authenticating users, accessing the container, working with the profiler and a
bunch more.

For years, when you needed a **browser that actually runs your JavaScript**, we
supported [Symfony Panther](https://github.com/symfony/panther).

Panther has been awesome for us. It brought end-to-end testing into the Symfony
ecosystem and made something that was traditionally pretty painful much easier
to use.

But with all this new Playwright + Symfony magic available, I made a pretty big
change:

**I added Playwright support... and deprecated Panther.**

And the really exciting part isn't just that Zenstruck Browser can now drive
Playwright. It's how much of the experience from our normal kernel tests we can
bring along with it.

Want to authenticate a user without visiting the login page?

```php
$this->playwrightBrowser()
    ->actingAs($user)
    ->visit('/account')
;
```

That logs the user in **directly through Symfony's session**, while the real
browser gets the resulting authentication cookie.

Want to test redirects? You can stop on them, assert exactly where they're
going, then follow them:

```php
$this->playwrightBrowser()
    ->interceptRedirects()
    ->visit('/account')
    ->assertRedirectedTo('/login')
;
```

And exceptions are especially cool.

Normally, once a real browser and web server are involved, an exception happens
on the other side of that boundary. But because Playwright's requests are
running directly through our kernel, Zenstruck Browser can actually **catch the
real PHP exception**:

```php
$this->playwrightBrowser()
    ->expectException(\DomainException::class)
    ->visit('/boom')
;
```

Not "the page returned a 500".

**The actual exception!**

> `DomainException: You cannot purchase this product.`

We also get container access, profiler support, shared assertions, cookie
handling and a bunch of the other conveniences that make kernel tests so nice to
work with.

So instead of choosing between:

**"nice Symfony functional test"** and **"real browser with JavaScript"**

...we can get remarkably close to having both.

And that's what made me want to push this forward.

### Wait... DAMA Works?!

My favorite part? What this means for your database.

If you write Doctrine tests, you might use
[DAMADoctrineTestBundle](https://github.com/dmaicher/doctrine-test-bundle). It
wraps each test in a transaction and rolls it back when the test finishes. Your
database is clean again instantly, and you never pay to rebuild it.

It's fantastic. But with end-to-end tests, **it never worked**.

It couldn't. Your test opens a transaction on its connection, but the browser
talks to a web server in a *different process*, holding a *different*
connection. That process can't see your uncommitted data. So the transaction
trick was off the table and you were back to purging tables or reloading
fixtures between every single test.

But now the request is handled **inside your test process, on the same
connection**.

So DAMA just... works.

```php
// persisted in your test - never committed
ProductFactory::createOne(['name' => 'Widget']);

$this->playwrightBrowser()
    ->visit('/products')
    ->assertSeeIn('#products', 'Widget') // the browser sees it!
;
```

The real browser renders that product. Your JavaScript can fetch it. And when
the test ends, it all disappears - nothing ever hits the database.

It works in the other direction too: if the *request* writes something, your
test sees it afterwards, and it's still rolled back at the end.

So we've dropped the web server, and now we've dropped the
rebuild-the-database-every-test tax with it. And we haven't even parallelized
anything yet.

### And... ParaTest

Remember the other part of Jon's quote?

> "And now it's possible to run this suite with paratest... since we can parallelize it now!"

Because the Symfony app is running inside the test process instead of behind a
shared web server, these Playwright tests can play nicely with
[ParaTest](https://github.com/paratestphp/paratest).

So now we're not just talking about fast Playwright tests.

We're talking about **a Playwright suite you can actually run in parallel** -
without giving up any of the above.

That's how a Playwright suite goes from something you're nervous to grow into
something you can actually keep adding to.

## This Changes the Equation

Browser tests have always come with trade-offs. You want a browser that actually
runs your JavaScript? Great. But now you're dealing with a web server, separate
processes, slower tests and less access to Symfony itself.

This new Playwright + Symfony setup changes that equation. For the most part,
you stop choosing.

And, as Jon Wage showed us right at the beginning:

> "...Went from ~39 seconds to ~18 seconds..."

That's not a small improvement.

I think Simon and the contributors to Playwright-PHP are building something
genuinely special here, and I'm incredibly proud to be part of it and excited to
see where it goes next.

If you're testing Symfony apps in a browser, **you should absolutely be paying
attention to this project**.
