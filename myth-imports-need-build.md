# Myth: JS imports need a Build System

> Read the entire series about LAST Stack:
> * [Your LAST Stack](/blog/last-stack)
> * Myth: JS imports need a Build System
> * [Stop Combining CSS & JS](/blog/stop-combining-files)
> * [Preloading Assets](/blog/preloading)
>
> And look out for the [30 Days with Last Stack](https://symfonycasts.com/screencast/30-days-last)
> tutorial.

When I was a youngster (back when we kids had to walk uphill both ways to
school through snow) JavaScript thrived on global variables. We added a
`script` tag to jQuery, then, bam! We could run around and use `$` as
recklessly as our heart desired. It was the best of times... it was the
weirdest of times.

```html
<script src="https://code.jquery.com/jquery-3.6.0.min.js"></script>
<script>
    $(document).ready(function() {
        #('body').innerHTML('A simpler time...');
    });
</script>
```

Then Node invented `require` and `module.exports`. This gave
us the concept of "modules": instead of setting & relying on global
variables, we could explicitly export & import code from one file to
another. Nice!

Eventually (way back in 2015!), the JavaScript community standardized
modules with the `import` and `export` keywords. And suddenly,
JavaScript started to look like a real programming language.

## The Rise of Bundlers

There was just one problem: browsers didn't understand `import`! This ushered
in the era of bundlers: tools like Webpack that would crawl through your
JavaScript files, find the `import` statements and combine everything into a
final set of files that browsers *could* understand - i.e. files that did *not*
have `import` & `export`.

## The Rise of Browsers

But, unless you're building an SPA with a frontend framework, the era of
bundlers is over! Browsers *do* now understand `import` and `export` natively,
and have for a while.

Let's try it right now! Seriously! Find a project, open `base.html.twig` or *any* file,
paste anywhere... then refresh. I'll wait:

```html
<script type="module">
    import { cowthink } from 'https://cdn.jsdelivr.net/npm/cowsayjs@2.0.0/+esm';

    console.log(cowthink('I\'d rather eat grass than keep bundling... though I do ❤️ grass...'));
</script>
```

That's right, that cow *did* just give you its opinion on bundlers... and grass. But the
point is: this "just works". If importing from a URL looks weird to you - that's
ok. We don't do this in practice thanks to "importmaps"... but that's a topic
for another day (and something that AssetMapper handles for you).

## The Rise of AssetMapper

So then... do we still need bundlers? Let's find out with this handy questionnaire:

1) **Do you need to support IE 11?** If yes -> then you need a bundler.
And, you should chat with your client about this - even
Microsoft dropped IE 11 support more than 2 years ago and `0.37%`
of users are still using it as of Nov 2023). Don't be a browser "hoarder".
It's ok to let go.

2) **Are you building an SPA with a frontend framework?** If yes -> then you need
a bundler and you should use the tools that come with that framework.

3) **Else** -> you **don't** need a bundler! 🚀

This means we can serve our unbundled, source JavaScript files directly.
And that's what [AssetMapper](https://symfony.com/doc/current/frontend/asset_mapper.html)
is all about: it lets you code like "normal" and
serves up your raw, individual files (with versioned filenames for
caching & a few other goodies).

In the next post, I'll talk about performance, why we don't combine files,
HTTP/2 and caching.
