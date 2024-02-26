# Live Components + Turbo Streams: Navigating a Turbo Frame

Recently, we received a fascinating [question](https://symfonycasts.com/screencast/last-stack/live-components#comment-31545),
summarized as:

> Suppose I submit a Live component to a [LiveAction](https://symfony.com/bundles/ux-live-component/current/index.html#actions)
> and redirect from there. How can I make this navigate a `<turbo-frame>` instead
> of the full page?

Let's learn a bit about how Live Components handles redirects, then see how we can
use Turbo Streams to navigate a frame instead of the full page. This is team work
at the max!

## How Live Components Handles Redirects

To answer this, let's create an imaginary Live component that sends a message:

```php
class SendMessage extends AbstractController
{
    #[LiveProp(writable: true)]
    public string $message = '';

    #[LiveAction]
    public function send()
    {
        // ... send the message

        return $this->redirectToRoute('my-route');
    }
}
```

The template for this component might look like:

```twig
<div {{ attributes }}>
    <input data-model="message" />

    <button data-action="live#action" data-action-name="send">Send</button>
```

***TIP
In the upcoming 2.16.0 version of LiveComponents, `data-action-name` will become
`data-live-action-param`.
***

With this setup, when the user clicks the "Send" button, the `send()` method is
called, which returns a redirect. Redirects are tricky in JavaScript. In fact,
it's impossible for JavaScript to detect when a redirect happens (your browser
automatically makes a second request to the new URL and returns that, all without
JavaScript knowing this happened).

To work around this limitation, the LiveComponents PHP code intercepts the redirect
and changes it to a non-redirect response that *includes* the new URL. The LiveComponents
JavaScript detects that, then redirects the page using JavaScript. Or, if `Turbo` is
available, it uses `Turbo.visit()` to navigate to the new page. Sweet!

## turbo-frame & turbo-stream Secret Powers

So then, what if we want to navigate a single `<turbo-frame>` on the page instead
of the full page?

Before we get there, let me tell you about two *super* cool things about Turbo:

1. `<turbo-frame>` is a [custom HTML element](https://developer.mozilla.org/en-US/docs/Web/API/Web_components/Using_custom_elements).
   As part of this, if you set the `src` attribute of a `<turbo-frame>` - whether
   via JavaScript or even manually in your browser's debugger - the frame will make
   an Ajax request to that URL and navigate. Or, if a new `<turbo-frame>` pops
   onto the page *and* it has a `src` attribute, the same thing will happen.

2. `<turbo-stream>` is *also* a custom HTML element. The important thing about this
   is that we can put a `<turbo-stream>` onto the page - in *any* way we want - and
   it will be processed by Turbo.

## Navigating a Frame with Turbo Streams

So, to navigate a frame instead of the full page, first, instead of redirecting
from the `LiveAction`, set a flag that indicates that you're in a "success" situation:

```php
class SendMessage extends AbstractController
{
    #[LiveProp(writable: true)]
    public string $message = '';

    // not a LiveProp so it will automatically revert
    // back to false on the next render
    public bool $isSuccessful = false;

    #[LiveAction]
    public function send()
    {
        // ... send the message

        $this->isSuccessful = true;
    }
}
```

Then, in the template, anywhere inside your root component element, add something like this:

```twig
{% if isSuccessful %}
    <turbo-stream action="replace" target="my-turbo-frame-id">
        <template>
            <turbo-frame
                id="my-turbo-frame-id"
                src="{{ path('my-route') }}"
            ></turbo-frame>
        </template>
    </turbo-stream>
{% endif %}
```

That's it! When this `<turbo-stream>` pops onto the page, it'll replace your existing
`<turbo-frame>` with this new one. That new element will instantly activate,
notice its `src` attribute, and make an Ajax request to fetch that page. It'll
then use its normal behavior of only loading the matching frame from that page
into the frame.

That's it! No new feature needed from Live Components: just leverage the power of
Turbo :).
