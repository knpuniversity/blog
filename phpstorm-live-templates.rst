Our favorite PhpStorm Live Templates: Share yours
=================================================

.. tip::

    **tl;dr** We've shared our PhpStorm settings (starting with live templates) on
    a new repository: https://github.com/knpuniversity/phpstorm-settings. What can
    you add to it?

Live templates? To see one in action, open a file in PhpStorm that has some HTML.
Now type just ``a`` and hit tab. Boom! This quickly becomes ``<a href=""></a>`` with
your cursor already waiting inside the quotes for you to type the URL. Hit tab again,
and your cursor moves between the tags so you can add text.

.. tip::

    Check out our tutorial - `PhpStorm LiveTemplates`_ - to see how to create
    live templates and use variables.

Most editors have a feature like this, and if you're not leveraging them, you're
slowing yourself down. Seriously: taking a few minutes to get into these now could
add up to a *lot* of hours saved in the future. The vim users at KnpLabs *love* this
kind of stuff, and have published their own snippets (`docteurklein`_, `PedroTroller`_, `Einenlum`_).

In our tutorial about live templates, we turn ``formhandle`` into a snippet that
types about 10 *lines* of form-handling boilerplate code from 10 *characters* of
text. So I started wondering: what are some other *awesome* live templates we should
all be using? On Facebook, `someone asked`_ if these were shareable... and of course
they are - just like all PhpStorm configuration.

A Repository for Live Templates (and other Settings)
----------------------------------------------------

So we created a repository to share useful PhpStorm settings (including live templates
of course):

https://github.com/knpuniversity/phpstorm-settings

Feel free to copy from this, or use it all - instructions are on the repository.

The starting list includes:

* ``404Unless`` (controller): Adds an if statement with a 404
* ``action`` (controller): Builds an action method
* ``formhandle`` (controller): Adds form-handling boiler-plate code
* ``formrow`` (twig): Renders ``form_row(form.???)``
* ``formrowfull`` (twig): Renders the widget, label and errors for a field
* ``include`` (twig): Adds ``{{ include('???') }}``
* ``method`` (controller): Adds ``@Method`` annotation
* ``path`` (twig): Adds ``{{ path('???') }}``
* ``render`` (twig): Adds ``{{ render(controller('???')) }}``
* ``rendertwig`` (controller) Adds ``$this->render('???')`` 
* ``repofind`` (controller) Adds ``$this->getDoctrine()->getRepository('???')->???()``
* ``route`` (controller) Adds ``@Route`` annotation
* ``service`` (yaml) Adds the basic YAML service definition structure
* ``tags`` (yaml) Adds a ``tags`` line to a service

I'm not the first person to do this, but I *am() hoping that you'll suggest some more
live templates, make these better, and even start adding other things - like code
stylings, file templates and colors.

Happy (fast) coding!

.. _`PhpStorm LiveTemplates`: http://knpuniversity.com/screencast/phpstorm/live-templates
.. _`docteurklein`: https://github.com/docteurklein/dot-files/tree/master/vim/UltiSnips/php
.. _`PedroTroller`: https://github.com/PedroTroller/DotFiles/tree/master/Symlink/vim/UltiSnips
.. _`Einenlum`: https://gitlab.com/Einenlum/dotfiles/tree/master/symlinks/.vim/UltiSnips/php
.. _`someone asked`: https://www.facebook.com/KnpLabs/photos/a.192365440813366.44246.191948140855096/922724781110758/?type=1&comment_id=922850677764835&offset=0&total_comments=3
