# Preloading Assets for Fun & Performance

> Read the entire series about LAST Stack:
> * [Your LAST Stack](https://symfonycasts.com/blog/last-stack)
> * [Myth: JS imports need a Build System](https://symfonycasts.com/blog/myth-imports-need-build)
> * [Stop Combining CSS & JS](/blog/stop-combining-files)
> * Preloading Assets
>
> And look out for the [30 Days with Last Stack](https://symfonycasts.com/screencast/30-days-last)
> tutorial.

tl;dr - AssetMapper 6.4 automatically preloads your CSS & JS files for you
after installing `symfony/web-link`. This makes your site faster 🚀. And that's nice!

We've learned to [Stop Combining CSS & JS](/blog/stop-combining-files) because
HTTP/2+ allows our browser to fetch all the assets in parallel. But before
our browser downloads a file, it needs to learn that it *needs* to download that file.  

For example, the simplest way to run modern JavaScript looks like this:

```html
<script type="module">
    import '/assets/app.js';
</script>
```

Notice: your browser doesn't know that it needs to download `app.js`
until it reaches this `<script>` tag. But because the `<script>` tag is in
the `<head>` (and because `module` script tags don't block the page render anyway),
this is no huge deal.

But now let's peek inside `app.js`:

```js
// /assets/app.js
import Alien from './aliens.js';
```

Uh oh! Another file to download! But now the problem is worse! The browser
doesn't know that it needs to download `aliens.js` until it *finishes* downloading
`app.js`! And what if `aliens.js` imports another module... which imports another?
The result is a waterfall of requests:

```
* [0ms-20ms] `app.js` downloads in 20ms
  * [20ms-40ms]`alien.js` downloads in 20ms
    * [40ms-60ms]`borg-collective.js` downloads in 20ms
      * [60ms-80ms]`borg-assimiliation.js` downloads in 20ms
```

In this example, each file only takes 20ms to download. But the 4th file - 
`borg-assimiliation.js` - doesn't even start downloading until 60ms after the
page load started! It's not until that moment that the browser *discovers* that
it needs to download that file. You can see this if you profile your page load
with LightHouse: https://developer.chrome.com/docs/lighthouse/performance/critical-request-chains.
I'm not a fan of the borg, but you must admit that they wouldn't tolerate this
kind of inefficiency.

## Hello Preloading

*This* is where preloading comes in. It's a way to hint to the browser that it
needs to download a file, even though it hasn't yet downloaded the code that
*references* that file. It's like a "heads up" to the browser. It looks like
this:

```html
<link rel="modulepreload" href="/assets/app.js">
<link rel="modulepreload" href="/assets/aliens.js">
<link rel="modulepreload" href="/assets/borg-collective.js">
<link rel="modulepreload" href="/assets/borg-assimilation.js">
```

This is a *huge* performance win, though you *do* need to be careful: you
should only preload files that you *know* will be needed immediately (or
very soon) after the page loads. Otherwise, you're not only wasting bandwidth,
you may prioritize downloading a file that isn't needed until later over
a file that *is* needed immediately.

## AssetMapper and Preloading

Because proper preloading is so important, in AssetMapper 6.4, it's automatic.
Suppose your `base.html.twig` renders the normal `app` importmap:

```twig
{# templates/base.html.twig #}

{{ importmap('app') }}
```

Behind the scenes, AssetMapper parses the `assets/app.js` file looking for
all ([non-dynamic](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Operators/import))
JavaScript imports. It then parses *those* files recursively, looking for more imports.
The final result - the list of all JavaScript files needed immediately on page load -
are rendered as `<link rel="modulepreload">` tags in the `<head>`. Done!

## Preloading CSS

What about CSS tags? These are *more* important! Unlike the JavaScript files,
when your browser reaches a `<link rel="stylesheet">` tag, it *blocks* the page
render until the CSS file is downloaded and parsed. This is why it's so important
to put your CSS tags in your `<head>` tag: we want the browser to *discover* that
it needs to download the CSS file as soon as possible.

But what if we could tell the browser to download the CSS file *before* it reaches
the `<link rel="stylesheet">` tag? Instead of putting a preload tag in the `<head>`,
we can return it as a header from the server:

```
Link: </assets/app.css>; rel=preload; as=style
```

The way this works is not as clear and may vary depending on the browser (please
tell me if you have more in-depth details!). But the idea is simple:
the browser sees the `Link` header and starts downloading the CSS file
immediately... even before it reaches the `<link rel="stylesheet">` tag.
For render-blocking resources like CSS, this is a *huge* performance win.

> Note: in earlier versions of Chrome, this may have also caused a "server push"
> to happen, which is a way to send the CSS file to the browser *before* it even
> asks for it. This is no longer the case and wasn't a good idea anyway as it
> could cause the browser to download a file that it already had in cache.

## Preloading CSS with AssetMapper

Like the way CSS preloading sounds? Me too! And so, in AssetMapper 6.4, it's automatic,
as soon as you install the WebLink component:

```
composer require symfony/web-link
```

That's it! Now, when you render the importmap, AssetMapper will automatically
add the `Link` header for you. To see an example, check out the response headers
on https://ux.symfony.com. That's just one of the reasons it scores 100 on
Lighthouse's performance score ⚡️.
