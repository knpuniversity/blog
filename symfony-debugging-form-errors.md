# Accessing and Debugging Symfony Form Errors

I recently saw an old post on StackOverflow called
[How to get form validation errors after binding the request to the form](http://stackoverflow.com/questions/6978723/symfony2-how-to-get-form-validation-errors-after-binding-the-request-to-the-fo).
It has a lot of answers, most of them are only partially right and a lot
are outdated. So, I wanted to look at the *right* answer, and why it's that
way :).

***TIP
To see real examples of using forms and customizing form rendering, start
off with our Symfony2 Series ([Episode 2](http://knpuniversity.com/screencast/symfony2-ep2/registration-form) and [Episode 4](http://knpuniversity.com/screencast/symfony2-ep4/form-customizationsespecially).
***

## Debugging

First, if you're trying to figure out what errors you have and which field
each is attached to, you should use the
`Form::getErrorsAsString()`
method that was introduced in Symfony 2.1 (so a long time ago!). Use it temporarily
in a controller to see what's going on::

    public function formAction(Request $request)
    {
        $form = $this->createForm(new ProductType());

        $form->handleRequest($request);
        if ($form->isValid()) {
            // ... form handling
        }

        var_dump($form->getErrorsAsString());die;

        $this->render(
            'DemoBundle:Product:form.html.twig',
            array('form' => $form->createView())
        );
    }

That's it. To make things even simpler, you also have the [Form panel](http://symfony.com/blog/new-in-symfony-2-4-great-form-panel-in-the-web-profiler) of
the web debug toolbar in Symfony 2.4. So, debugging form errors

## Why $form->getErrors() doesn't Work

***TIP
As of Symfony 2.5, we have a new tool! `$form->getErrors(true)` *will*
return *all* of the errors from *all* of the fields on your form.
***

Raise your hand virtually if you've tried doing this to debug a form::

    public function formAction(Request $request)
    {
        $form = $this->createForm(new ProductType());

        $form->handleRequest($request);
        if ($form->isValid()) {
            // ... form handling
        }

        var_dump($form->getErrors());die;

        $this->render(
            'DemoBundle:Product:form.html.twig',
            array('form' => $form->createView())
        );
    }

What do you get? Almost definitely an empty array, even when the form has
lots of errors. Yea, I've been there too.

The reason is that a form is actually more than just one `Form` object:
it's a tree of `Form` objects. Each field is represented by its *own* `Form`
object and the errors for that field are stored on it.

Assuming the form has `name` and `price` fields, how can we get the errors
for each field?

```php
$globalErrors = $form->getErrors()
$nameErrors = $form['name']->getErrors();
$priceErrors = $form['price']->getErrors();
```

By saying `$form['name']`, we get the `Form` object that represents *just*
the `name` field, which gives us access to *just* the errors attached to
that field. This means that there's no *one* spot to get *all* of the errors
on your entire form. Saying `$form->getErrors()` gives you only "global"
errors, i.e. errors that aren't attached to any specific field (e.g. a CSRF token
failure).

## Render all the Errors at the Top of your Form

***TIP
As of Symfony 2.5, you can use `$form->getErrors(true)` to get an array
of *all* the errors in your form. Yay!
***

One common question is how you might render all of your errors in one big
list at the top of your form. Again, there's no *one* spot where you can
get a big array of *all* of the errors, so you'd need to build it yourself::

    // a simple, but incomplete (see below) way to collect all errors
    $errors = array();
    foreach ($form as $fieldName => $formField) {
        // each field has an array of errors
        $errors[$fieldName] = $formField->getErrors();
    }

We can iterate over `$form` (a `Form` object) to get all of its fields.
And again, remember that each field (`$formField` here), is also a `Form`
object, which is why we can call
`Form::getErrors()`
on each.

In reality, since a form can be many-levels deep, this solution isn't good
enough. Fortunately, someone already posted a more complete one on
[Stack Overflow](http://stackoverflow.com/a/8216192/805814) (see the 2.1 version).

From here, you can pass these into your template and render each. Of course,
you'll need to make sure that you don't call `{{ form_errors() }}` on any
of your fields, since you're printing the errors manually (and remember that
`form_row` calls `form_errors` automatically).

***TIP
Each field also has an [error_bubbling](http://symfony.com/doc/current/reference/forms/types/text.html#error-bubbling) option. If this is set to `true`
(it defaults to `false` for most fields), then the error will "bubble"
and attach itself to the parent form. This means that if you set this
option to `true` for *every* field, all errors would be attached to
the top-level Form object and could be rendered by calling `{{ form_errors(form) }}`
in Twig.
***

## Accessing Errors Inside Twig

We can also do some magic in Twig with errors using magical things called
*form variables*. These guys are *absolutely fundamental* to customizing
how your forms render.

***TIP
If you're new to form theming and variables and need to master them,
check out [Episode 4](http://knpuniversity.com/screencast/symfony2-ep4/form-customizations) of our Symfony series.
***

Normally, field errors are rendered in Twig by calling `form_errors` on
each individual field:

```html+jinja
{{ form_errors(form) }}

{{ form_label(form.name) }}
{{ form_widget(form.name) }}
{{ form_errors(form.name) }}
```
***TIP
The `form_row` function calls `form_errors` internally.
***

Just like in the controller, the errors are attached to the individual fields
themselves. Hopefully it make sense now why `form_errors(form)` renders *global*
errors and `form_errors(form.name)` renders the errors attached to the
name field.

***TIP
Once you're in Twig, each field (e.g. `form`, `form.name`) is an instance
of :symfonyclass:`Symfony\\Component\\Form\\FormView`.
***

If you need to customize how the errors are rendered, you can always use
[form theming](http://knpuniversity.com/screencast/symfony2-ep4/form-customizations). But you can also do it by leverage form variables:

```html+jinja
{{ form_errors(form) }}

{{ form_label(form.name) }}
{{ form_widget(form.name) }}

{% if form.name.vars.errors|length > 0 %}
<ul class="form-errors name">
    {% for error in form.name.vars.errors %}
        {{ error }}
    {% endfor %}
</ul>
{% endif %}
```

The key here is `form.name.vars`: a strange array of "variables" that you
have access to on *every* field. One of the variables you have access to
is `errors`, but there are many others, like `label` and `required`.
Each variable is normally used internally to render the field, but you can
use them manually if you need to:

```html+jinja
<label for="form.name.vars.id">
    {{ form.name.vars.label }} {{ form.name.vars.required ? '*' : '' }}
</label>
```

To find out what variables a field has, just dump them:

```html+jinja
{{ dump(form.price.vars) }}
```

It is also possible to compile a list of all form errors for the current form and display them inside a paragraph tah (or any other formatting you wish to use). This requires some additional looping:

.. code-block:: html+jinja

    {% set messages = [] %}
    {% for child in form.children %}
        {% if child.vars.errors|length > 0 %}
            {% for error in child.vars.errors %}
                {% set messages = messages|merge([error.message]) %}
            {% endfor %}
        {% endif %}
    {% endfor %}
    {% if messages|length > 0 %}
        <p>
            {% for message in messages %}
                {{ message }}<br />
            {% endfor %}
        </p>
    {% endif %}


***TIP
When you are form theming, these variables become accessible in your
form theme template as local variables inside the form blocks (e.g.
simply `label` or `id`).
***

## Takeaways

The key lesson is this: **each form is a big tree**. The top level `Form` has
children, each which is also a `Form` object (or a `FormView` object
when you're in Twig). If you want to access information about a field (is
it required? what are its errors?), you need to first get access to the *child*
form and go from there.

## Learn More

Stuck on other Symfony topics or want to learn Symfony from the context of
an actual project? Check out our Getting Started with [Symfony Series](http://knpuniversity.com/screencast/tag/symfony) and
cut down on your learning curve!
