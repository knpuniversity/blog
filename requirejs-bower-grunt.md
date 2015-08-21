# Evolving RequireJS, Bower and Grunt

***TIP
Instead of Grunt, you might want to check out Gulp! It's at least as
powerful and a lot easier. Checkout our series [Gulp! Refreshment for Your Frontend Assets](https://knpuniversity.com/screencast/gulp).
***

A few weeks ago, Leanna and I were one of the lucky 600+ that attended SymfonyCon
in Warsaw - one of the best conferences we've been to! We hung out with some
of our best tech friends, watched [Leanna](http://twitter.com/leannapelham) win tech Jeopardy,
and had the pleasure to meet a lot of new friends!

I also gave a Christmas-themed talk on a *really* neat subject:
"[Cool like a Frontend] Developer(http://www.slideshare.net/weaverryan/cool-like-frontend-developer-grunt-requirejs-bower-and-other-tools-29177248)", renamed to "Deck the Halls with Grunt, RequireJS & Bower".
And because examples are best, an example project from the presentation
lives on GitHub: [knpuniversity/symfonycon-frontend](https://github.com/knpuniversity/symfonycon-frontend)

If you're curious about this stuff and couldn't be there for my talk, go
read those slides. Then come back. We have a new piece to talk about.

## An Evolving Best Practice

Like most new tech, what makes this stuff tricky is the lack of real projects
and best practices when you're learning it. That was the point of my talk:
to give you something real to build off of.

But I also knew that my solution wouldn't end up being the best. In fact,
a tip from [Gediminas](http://www.slideshare.net/weaverryan/cool-like-frontend-developer-grunt-requirejs-bower-and-other-tools-29177248) (of [DoctrineExtensions](https://github.com/l3pp4rd/DoctrineExtensions) fame) and some others have
already led me to one big change.

## Keeping the assets Directory at the Root of your Project

In my talk, I propose having a `web/assets/` directory where you put all
of your JS, CSS/SASS, fonts, etc. When you run Grunt (which runs the RequireJS
optimizer), this is copied to `web/assets-built`, and then some changes
are made to it. In the end, the only change we need to make to our Symfony
project is to point all of our assets to `/web/assets-built` instead of
`/web/assets` when we're in the `prod` environment.

But a better solution may be to put the `assets` directory at the *root*
of your project. This has a few advantages:

#. Any source files (like SASS files) aren't exposed to the web;
#. You no longer need to worry about changing between pointing to `/assets`
   and `/assets-built` in your Symfony project.

The second point is very nice. By the end of my presentation, I defined two
important Grunt tasks:

1. `grunt` - operates on `web/assets` and does some basic things like
   SASS compilation';
2. `grunt production` - copies `web/assets` to `web/assets-built` and
   then does several things to it.

With this new setup, we would change this slightly:

1. `grunt` - copies `assets/` to `web/assets` and does some basic things
   like SASS compilation';
2. `grunt production` - copies `assets` to `web/assets` and then does
   several things to it.

The difference is that - whether we're developing or deploying - our assets
*always* live in `web/assets`. This means that you don't need *any* logic
in your Symfony application to change paths from `/assets/` to `/assets-built`.
Developing? Just use `grunt` (or, more usefully `grunt watch`). Want
to use the assets as they'll be built for production? Just run `grunt production`.

### Changes to Gruntfile.js

If you want to try this, let's talk about the exact changes we need. It's
a simple 3-step process. If you want to skip and see the end result, check
out the [assets-in-root](https://github.com/knpuniversity/symfonycon-frontend/tree/assets-in-root) branch on GitHub.

1) First, the easy part: move `web/assets` to `assets`. Awesome.

2) Next, update your Twig templates to simply point at the `assets` directory,
replacing the "smarter" variable used before.

Before:

```html+jinja
<script src="{{ asset(assetsPath~'/vendor/requirejs/require.js') }}"></script>
<script>
    requirejs.config({
        baseUrl: '{{ asset(assetsPath~'/js') }}'
    });
    // ...
</script>
```

After:

```html+jinja
<script src="{{ asset('assets/vendor/requirejs/require.js') }}"></script>
<script>
    requirejs.config({
        baseUrl: '{{ asset('assets/js') }}'
    });
    // ...
</script>
```

3) Modify the `Gruntfile.js` to copy `assets/` to `web/assets` and
   then operate entirely on the `web/assets` directory.

Ok, this part isn't *so* simple. First, you'll need a new Grunt plugin:
[grunt-contrib-copy](https://github.com/gruntjs/grunt-contrib-copy) by adding it to `package.json`, and importing its
tasks in `Gruntfile.js`:

```javascript
// Gruntfile.js
// ...
module.exports = function (grunt) {
    // ...

    grunt.loadNpmTasks('grunt-contrib-copy');
    // ...
};
```

With some configuration, this will copy one directory (e.g. `assets`) to
another directory (`web/assets`). We've been relying on RequireJS to do
this until now, but I now want something that will copy these files, even
if I'm not using the RequireJS optimizer:

```javascript
// Gruntfile.js
// ...

copy: {
    main: {
        files: [
            {
                expand: true,
                src: ['assets/**'], dest: 'web'}
        ]
    }
},
// ...
```

With this, we now have a new `grunt copy` command, which will copy `assets/`
to `web/assets`. That's not very useful on its own, but we can now point
all the other tasks in `Gruntfile.js` to operate on the `web/assets` directory,
including Compass, JSHint and RequireJS.

We also have two "watch" sub-commands that guarantee that JSHint is run whenever
JavaScript files change and Compass whenever `.scss` files change. We'll
continue to have the watch sub-task look for file changes in the `assets/`
directory at the root of our project, since that's where we edit files. But
before running `jshint` or `compass`, each will call `copy` first, to
copy things into `web/assets`:

```javascript
// Gruntfile.js
// ...

watch: {
    scripts: {
        files: ['assets/js/**'],
        tasks: ['copy', 'jshint']
    },
    // watch all .scss files and run compass
    compass: {
        files: 'assets/sass/*.scss',
        tasks: ['copy', 'compass:dev'],
        options: {
            spawn: false
        }
    }
}
```

The setup probably still has a few imperfections, but to see it all put together,
see the [grunt-contrib-copy](https://github.com/gruntjs/grunt-contrib-copy) branch on GitHub. This setup adds a small amount
of complexity, since you must copy files every time any change is made, even
while developing. But since this is all handled in Grunt and `grunt watch`,
we only feel that complexity when we're first getting things configured.

## Cleaning up SASS and old Files

I've also been talking with a [Matt Davis](https://twitter.com/mdavis1982), we brought up some more potential
improvements/problems:

1. The SASS files no longer live in `web/`, but are still copied to `web/`
when Grunt runs. If you really want to hide these files, you'll need to omit
them from the `copy` task, or remove them afterwards.

2. If you delete a file from `assets/`, it will still live in `web/assets/`,
because the `copy` task copies new files, but nothing ever removes the
old files.

The answer to both of these is the [grunt-contrib-clean](https://github.com/gruntjs/grunt-contrib-clean) plugin.

***TIP
The solution to this has been even *further* evolved to never copy the
sass files at all. Just check out the [assets-in-root](https://github.com/knpuniversity/symfonycon-frontend/tree/assets-in-root) branch on GitHub
or [pull request #7](https://github.com/knpuniversity/symfonycon-frontend/pull/7) for more details. Thanks to [Daniel Paschke](https://github.com/paschdan) for
the tips.
***

First, install it like any Grunt plugins:

```text
$ npm install grunt-contrib-clean --save-dev
```

Then activate its tasks in `Gruntfile.js`:

```javascript
// Gruntfile.js
module.exports = function (grunt) {
    // ...
    grunt.loadNpmTasks('grunt-contrib-clean');
    // ...
};
```

We'll create 2 subtasks: one for cleaning out `web/assets` before copying
and another for cleaning out the `web/assets/sass` directory *after* copying:

```javascript
// Gruntfile.js
// ...

grunt.initConfig({
    clean: {
        build: {
            src: ['<%= targetDir %>/**']
        },
        sass: {
            src: ['<%= targetDir %>/sass']
        }
    },
});

// ...
// sub-task that copies assets to web/assets, and also cleans some things
grunt.registerTask('copy:assets', ['clean:build', 'copy', 'clean:sass']);

// the "default" task (e.g. simply "Grunt") runs tasks for development
grunt.registerTask('default', ['copy:assets', 'jshint', 'compass:dev']);

// register a "production" task that sets everything up before deployment
grunt.registerTask('production', ['copy:assets', 'jshint', 'requirejs', 'uglify', 'compass:dist']);
```

We've also created a new convenience task: `copy:assets`, which cleans
`web/assets`, copies `assets/` to `web/assets/`, then removes `web/assets/sass`.
Phew! Just make sure that this new `copy:assets` is the first step
in our `default` and `production` tasks. Now, when we run `grunt` or
`grunt production`, all the copying and cleaning will happen first.

## Other Improvements?

This was the first big change that I've come across, but if you see other
improvements, I'd love to hear them!

Have fun!
