Why I Switched from Assetic to Gulp
===================================

    **tl;dr** Assetic was created when there were no real frontend tools for processing
    and combining CSS/JS. But now there are, and unless you feel really uncomfortable
    with Node.js, I recommend using `Gulp`_ instead of Assetic.

`Assetic`_ (PHP library) does one simple job: processes and combines assets.
If you've used the Symfony framework, you've probably used it: it's still
the recommended tool for taking CSS and JS files, putting them through filters
(e.g. Sass, Uglify) and then combining them into one file for production.

But today, I don't recommend using Assetic. Assetic was created back when
there *were* no industry-standard solutions for processing and combining
assets. So each language (framework) created their own: Assetic is based
off of `webassets`_ from Python. And it works really well.

Node.js - and the explosion of packages available via ``npm`` - has changed
all that. There now *are* asset-processing tools that can be used by PHP
nerds, Python dorks, Java Dude(ttes) and frontend geeks. In this new world,
the tool I like most is `Gulp`_, because it's powerful, and just fun. Using
it over Assetic has some real-world advantages:

**Gulp versus Assetic: Pros**

A. More people are working on Gulp than Assetic: i.e. you have more tools;
B. More people are using Gulp than Assetic: i.e. you find more help on StackOverflow;
C. You already know JavaScript: i.e. it's not so scary (and I'll show you).

What about disadvantages? Well, there is one, and it's legit:

**Gulp versus Assetic: Cons**

A) Gulp adds a new layer to your dev stack (Node.js) and your learning curve;

This last disadvantage is real: if you're low on time, already understand
Assetic, and need to do some basic frontend processing: keep using it and
stop reading. It's totally ok - Assetic is still a great tool for your case.

For everyone else, let's look at Gulp really quickly in 3 steps:

1) Use Gulp to create Command-Line Tasks
----------------------------------------

In the simplest sense, Gulp let's you create command-line tasks (like Symfony's
`app/console`_). After installing gulp (``npm install gulp``), you create
a single file called ``gulpfile.js``. This example creates a single task,
which prints a message:

.. code-block:: javascript

    var gulp = require('gulp');

    gulp.task('default', function() {
        console.log('GULP THIS!');
    });

To run it, just do:

.. code-block:: bash

    gulp default

Got the idea?

2) Read and Process Files
-------------------------

Gulp could be used for anything, but usually you're doing the same thing:
reading some files (e.g. .scss files), processing them (e.g. through Sass)
then writing out the new files:

.. code-block:: javascript

    var gulp = require('gulp');
    var sass = require('gulp-sass');

    gulp.task('default', function() {
        gulp.src('app/Resources/assets/sass/**/*.scss')
            .pipe(sass())
            .pipe(gulp.dest('web/css'));
    });

Now, any ``.scss`` files in the ``sass`` directory (recursive - that's the
extra ``/**`` part)  will have a processed ``.css`` version in ``web/css``.

3) Combine Files (and a lot more)
---------------------------------

Now that our ``.scss`` files have been processed, why not combine them all
into 1 CSS file? That's just a couple extra lines:

.. code-block:: javascript

    var gulp = require('gulp');
    var sass = require('gulp-sass');
    var concat = require('gulp-concat');

    gulp.task('default', function() {
        gulp.src('app/Resources/assets/sass/**/*.scss')
            .pipe(sass())
            .pipe(concat('main.css'))
            .pipe(gulp.dest('web/css'));
    });

Run ``gulp default`` again (or just ``gulp``, which triggers the ``default``
automatically) to process and concatenate all your Sass files into one, ``main.css``.

Feel good? If you want to get a whole working setup, you can find that in
our `Gulp!`_ tutorial, which includes things like: `sourcemaps`_, `minification`_, `uglification`_
and `cache busting/versioning`_.

If you've worked with Gulp and have any tips or warnings for others, I'd
love it if you shared.

Cheers!

.. _`Gulp`: http://knpuniversity.com/screencast/gulp
.. _`Gulp!`: http://knpuniversity.com/screencast/gulp
.. _`Assetic`: https://github.com/kriswallsmith/assetic
.. _`webassets`: http://webassets.readthedocs.org/en/latest/
.. _`app/console`: http://knpuniversity.com/screencast/symfony2-ep1/bundles#the-console
.. _`sourcemaps`: http://knpuniversity.com/screencast/gulp/sourcemaps
.. _`minification`: http://knpuniversity.com/screencast/gulp/minify
.. _`uglification`: http://knpuniversity.com/screencast/gulp/javascript
.. _`cache busting/versioning`: http://knpuniversity.com/screencast/gulp/version-cache-busting
