# Bundles, No Bundles and AppBundle in 10 Steps

Ah, the AppBundle: my **favorite** part of the Symfony best practices.

    "RYAN, What are you an idiot!? That's a terrible idea!"

Ok, not everyone agrees - it's cool :). But here's what's interesting: if you
*don't* like the AppBundle, I bet I actually *agree* with your reasons.

    "What!?"

Let's figure this out in 10 Steps.

## 1) Keep Calm: Because I don't Care

I mean this with <3. We're using Symfony! This means you can do whatever
you want. Nothing has changed in Symfony to prevent this, and nothing will.
That's an awesome start.

## 2) Sorry, Your Bundles aren't Bundles: They're Directories

<img class="pull-right" style="width: 300px;" src="/images/appbundle/coupled-bundles.png">

A traditional Symfony project is made up of bundles that are coupled together.
Ok, maybe you have some standalone bundles, but somewhere, there's a group
that are really coupled. And that's great! We're building an app here, not
an open-source library. These coupled bundles *are* your app.

But in this case, they **aren't** really bundles: **they're just directories**.
There's no technical advantage to having 1 or 10: we're just trying to organize
things to our subjective liking.

A true bundle is a standalone, reusable entity. These are just directories.



## 3) AppBundle: Just a Different Directory Structure

![App Bundle](/images/appbundle/app-bundle.png)
   :align: right
   :width: 150px

Now, if we decide to move everything into one bundle, it's nothing more than
a different directory structure. You can even keep the same amount of organization
by putting sub-directories in `Controller` or anywhere else.

You might like this, or you might not. The point is: it's subjective, there's
no technical benefit of having multiple bundles.



## 4) AppBundle and AppKernel are Best Friends!

Your application *is* AppKernel, and it lives in `app/`. It builds our
1 container, 1 compiled list of routes, 1 collection of translations, 1 group
of mapped entities and everything else. Even if you have these things spread
out across bundles, it's all loaded right here.

So why not move more things into `app/`? Remember, we're not planning on
re-using this stuff somewhere else.

### a) Move service configuration to app/

![Move Config](/images/appbundle/move-config.png)
   :align: right
   :width: 300px

Since each kernel has only *one* container, it's logical to move service
config out of the bundle and into `app/`.

    "But if I move my service configuration out of my bundle it's coupled
    to my app"

That's right! But it probably already *was* coupled. And if you *do* need
to re-use something, great! Put it in a *true*, standalone bundle. Here,
I'm talking about moving pieces out of bundles that are *truly* a part of
your app.

***TIP
Like with everything, if you have *a lot* of services, feel free to create
an `app/config/services` directory with multiple files.
***


### b) Moving templates to app/

![Move Templates](/images/appbundle/move-templates.png)
   :align: right
   :width: 300px

Next, let's move the templates into `app/`. I know many people *hate* this,
because it puts the templates in a different directory than the controllers.
That's subjective, but fair - and :ref:`I talk about that later <app-bundle-templates-decoupled>`.

This is a subjective change, but it has one hidden improvement: you no longer
need to use the weird three-part colon syntax. As a Symfony expert, *you*
know this syntax. But I give a lot of trainings, and these Symfony-isms give
beginners a lot of trouble.

Instead, you just render the filename. The only rule you need to know is
that templates live in `app/Resources/views`. This reduces complexity,
and that's huge.



## 5) No Bundles!?

![No Bundle](/images/appbundle/no-bundle.png)
   :align: right
   :width: 300px

You can keep doing this until AppBundle holds *only* PHP classes.

But wait, why do we need a bundle at all then? You don't! A bundle is just
a directory! And we can rename directories! We could rename this to `Ryan`
and even delete the bundle file.

***TIP
You can start thinking of AppKernel as your one "bundle". It can do
anything a bundle can, including complex stuff like registering compiler
passes.
***

So why isn't *this* the Symfony best practice instead of AppBundle? A few
reasons:

* Without a bundle, you lose a few shortcuts, like the `_controller` shortcut
  and the automatic bundle aliasing for entities (e.g. `AcmeDemoBundle:Post`).
  All of these are optional, but it's more work without them.

* It would be a *big* philosophical leap, and change needs to be done carefully.
  Having only *one* bundle was a big enough change.

But philosophically, I *do* hope you'll think of your `AppBundle` as just
a directory for PHP classes. And for Symfony 3.0, maybe we'll get there!



<a name="app-bundle-templates-decoupled"></a>

## 6) I hate having my Templates in app/, Controllers in src/

![All in App](/images/appbundle/all-in-app.png)
   :align: right
   :width: 300px

The biggest complaint I've heard about the AppBundle is this: I don't like
that my controllers would live in `src/`, but the templates they render
would live in `app/`.

That's subjective, but totally fair (it hasn't bothered me).

To solve this, we could move our `Ryan` directory (or `AppBundle`, before
my rename) into `app/`.

This works with no code changes except for a new autoload entry:

```json
{
    "autoload": {
        "psr-4": { "Ryan\\": "app/src" }
    }
}
```

I'm not recommending that everyone runs and does this, but logically, everything
is coupled to `app/`, so it makes perfect sense. I hope it at least gets
you thinking!

***TIP
Still want the templates closer to the controllers? No problem, keep
them in `AppBundle` :).
***



## 7) But I want to create a Decoupled Library!

![Decoupled Library](/images/appbundle/decoupled-library.png)
   :align: right
   :width: 150px

Sweet! Just create a directory in `src/` and put your decoupled library
right there. It's ready to be re-used!

## 8) But I want to re-use a Bundle between projects or kernels!

![Decoupled Bundle](/images/appbundle/decoupled-bundle.png)
   :align: right
   :width: 150px

Nice! Just create the bundle in `src/` (or `vendor/`, etc) and treat
it like *true*, decoupled bundle.

## 9) I don't know, I *still* want multiple Bundles

Still feel like you need more bundles? No worries - create as many as you
want. But don't be afraid to choose *one* bundle that you *really* couple
to your `app/` directory - it might just make your life simpler.

## 10) What if I have multiple Kernels?

Multiple kernels? Sounds like a neat project :). You should have one super-coupled
bundle per kernel. For example, `WebKernel` & `WebBundle`, `ApiKernel`
and `ApiBundle`. If you need to share things between kernels, put this
into proper, de-coupled bundles that are booted by each kernel.

## Do We Agree Now?

One main argument against the AppBundle is that you should make your code
modular. I agree! But having 1 directory or 10 doesn't make a difference.
But these things do:

* creating service classes, with minimal dependencies (+ skinny controllers);

* (if applicable) identifying parts of your code that you *truly* need to
  re-use between projects/kernels and writing them as proper bundles or libraries;

* potentially creating multiple, focused apps (e.g. backend API, frontend
  app, separate app for handling jobs, etc).

So even if you don't like the AppBundle, I hope you'll see that it has nothing
to do with writing more or less modular code. That's still up to you :).

<3 Ryan
