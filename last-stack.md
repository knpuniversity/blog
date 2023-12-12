# Your LAST Stack

> Read the entire series about LAST Stack:
> * Your LAST Stack
> * [Myth: JS imports need a Build System](/blog/myth-imports-need-build)
> * [Stop Combining CSS & JS](/blog/stop-combining-files)
> * [Preloading Assets](/blog/preloading)
>
> And look out for the [30 Days with Last Stack](https://symfonycasts.com/screencast/30-days-last)
> tutorial.

The mechanics behind the internet - browsers & web servers - have, for most
of history, been terrible. We rounded corners using images, did transitions using
jQuery, and combined all our JS into a single file, amongst other unsavory things.

Fortunately, times have changed. Browsers let us do amazing things out-of-the-box.
HTTP/2+ allows us to serve individual files without a material performance impact
and these can be compressed on-the-fly by the web server. This means that we can build
apps in a new way, using tools that are simpler *precisely* because they
*don't* do many things that we no longer need.

And since cool tools deserve cool names, say hello to **LAST Stack**:

* **L** [Live Components](https://symfony.com/bundles/ux-live-component/current/index.html)
* **A** [AssetMapper](https://symfony.com/doc/current/frontend/asset_mapper.html)
* **S** [Stimulus](https://stimulus.hotwired.dev/) (or Symfony)
* **T** [Turbo](https://turbo.hotwired.dev/) (or Twig)

LAST stack is about **web standards**. AssetMapper (stable in Symfony
6.4) leverages [importmaps](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/script/type/importmap):
a standard that's now supported on all major browsers (and has a polyfill for
old browsers). This is built on top of another standard - [ES modules](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Guide/Modules)
and is backed up by the power of HTTP/2 & HTTP/3: allowing us to *not* worry
about bundling our assets. Add in [preloading](https://developer.mozilla.org/en-US/docs/Web/HTML/Attributes/rel/modulepreload)
and we have a fast, standards-based way to load our assets!

Next, LAST stack is **no build**. Yup, it's a **zero** build time **Node-free**
setup. Thanks to the fact that browsers understand `import` statements (+ modern JS)
and HTTP/2+, there's... nothing left to build! Yes, we're back to opening up a JavaScript
file, writing code & refreshing the page. I've been doing this for 6
months and it puts the joy back into my code.

I hope you've noticed that, despite the trendy name, LAST stack is **not** new
nor unique. And that's the third pillar: **shared tools**. Stimulus
and Turbo are open source JavaScript projects used by Ruby on Rails and apps
everywhere. When they move forward ([Turbo 8 looks huge!](https://dev.37signals.com/a-happier-happy-path-in-turbo-with-morphing/)
and [Strada](https://strada.hotwired.dev/) empowers mobile apps),
*we* move forward. The nature of JavaScript modules also means that we can use
*any* JavaScript package we want to solve a problem: nothing is specific to Symfony. 

LASTly, LAST stack is about **simplicity** and **productivity**. No build, no
complexity, and a low cognitive load: an entire system that can be understood
by a single developer. Use Twig to render *all* your HTML, Turbo to transform
into a single-page app, Stimulus for an extra sprinkle of JavaScript & Live
Components for highly interactive parts of your page (yes, still using Twig!).

The future is here and what's old is new. Welcome to LAST stack.
