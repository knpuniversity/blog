# Stop Combining CSS & JS! + Performance Revisited

> Read the entire series about LAST Stack:
> * [Your LAST Stack](https://symfonycasts.com/blog/last-stack)
> * [Myth: JS imports need a Build System](https://symfonycasts.com/blog/myth-imports-need-build)
> * Stop Combining CSS & JS
> * [Preloading Assets](/blog/preloading)
>
> And look out for an upcoming [30 Days with Last Stack](https://symfonycasts.com/screencast/30-days-last)
> tutorial. Or join me for my [UX + AssetMapper SymfonyCon Workshop](https://live.symfony.com/2023-brussels-con/workshop/having-fun-and-being-productive-with-symfony-ux-and-assetmapper)!

You know the all-important rule of frontend performance. Heck, it's probably
the first thing you think about when profiling your site via Lighthouse.
Repeat after me: combine your CSS and JS files to minimize the number of HTTP
requests.

But wait, plot twist! That's not true anymore. Heck, **it's been a myth for a while**. Even
Lighthouse doesn't seem to care. The one [metric that mentions it](https://developer.chrome.com/docs/lighthouse/performance/resource-summary/)
doesn't affect your score... and the "solutions" relate to avoiding "blocking"
resources, not eliminating requests.

## Don't Care about Requests

What changed? Simple: HTTP/2 (and HTTP/3). **With HTTP/2, browsers can make
multiple requests (via a single connection) to the server at the same time**.
It's that simple. Game over. Oh, and HTTP/2 is supported by all browsers and
all web servers. And if you're lazy (like us!), you can also put a service like
Cloudflare in front of your site to automatically enable HTTP/2+.

Is there still some theoretical number of requests that's too many? Probably!
But it's also probably a lot higher than you think. And that's the point:
**HTTP/2+ allows us to stop worrying about the number of requests**. If you
*did* hit some limit, let a tool like Lighthouse tell you... instead of guessing
& worrying about it. For reference, today https://ux.symfony.com
serves 7 uncombined CSS files and 50+ uncombined JS files on page load! The
Lighthouse performance score? 100 ⚡️.

## Long-Term Caching ✅

Forgetting about HTTP/2 & requests, there's one thing that *hasn't* changed:
the importance of **long-term caching**. When a user comes to my site, I want
them to download my `/assets/alien-attack.js` file once and then **never** again.
Heck, I don't even want their browser to take the time to *ask* if it's changed:
just use the cached version.

Doing this is simple and not new. On your web server, set an `Expires` response header
to a super-distant time in the future for every asset (CSS, JS, images, etc.).
Then, after your browser requests `GET /assets/alien-attack.js`, it
will cache that file (nearly) *forever*.

But... what happens when we *change* the file? To bust the cache, we need to
change the URL. That's where **asset versioning** comes in. Instead of naming
the file `/assets/alien-attack.js`, we call it `/assets/alien-attack-10030cb02068eb.js`.
The `10030cb02068eb` is known as a "digest": it's a hash of the file's contents.
When we change the file, the digest changes, the URL changes, and the browser
downloads the new file.

Since manually renaming the file ourselves would be... insane... this is
the *main* task that we still need *some* tool to help us with. Webpack did this
for us. And so does [AssetMapper](https://symfony.com/doc/current/frontend/asset_mapper.html).
In fact, AssetMapper is *built* for this.

For example, if we have an `assets/images/alien-attack.png` file, we can reference
it in a template like this:

```twig
<img src="{{ asset('assets/images/alien-attack.png') }}" />

Output:
<img src="/assets/images/alien-attack-10030cb02068eb.png" />
```

Free versioning! And this works for *any* file you put in a mapped assets directory.

Oh, and fun-fact: now that we're no longer combining files, if update change
just *one* file, only *that* file's name will change... and our users will
only re-download that one file... instead of a combined file that contains
the code from a bunch of files that did *not* change.

## Compression? Minification?

Status check!

* Number of requests: ✅ don't care
* Long-term caching: ✅ AssetMapper

But what about compression... or minification? These are still important, right?
If we can squeeze a 25KB JavaScript file down to 5KB, shouldn't we do that?

Yup! This is something else that Webpack did for us. But, it's also something
that web servers can do! Like long-term caching, you can configure your web
server to compress your assets on-the-fly. Or, even easier, put a service like
Cloudflare in front of your site, and it will do this for you... even choosing
from the best compression algorithm for each browser.

***TIP
Compression (e.g. via gzip or brotli) is *not* the same as minification.
Minification *can* reduce your file sizes a bit more. But do you care enough?
Hey.com famously does *not* minify their assets, relying only on compression...
and they're pretty big. Let tools like Lighthouse tell if you *really* need
to minify. And if you do, you can always add a step to your deploy process.
***

So yes! Compression *still* matters!, But it doesn't need to be part
of your frontend tooling. Solve it once (via web server or CloudFlare), then
celebrate that you never need to think about it again. Maybe work on something
more important instead.

In the next post, we'll talk about preloading: a fancy way to hint to the browser
what files it should download *before* it even knows it needs them.
