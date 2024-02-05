# Redirect Turbo Frame

Question from user: Hi, I'm using a Live Component with a Live Action that ends with
return $this->redirectToRoute('my-route');
Is there a way to load this new page (only the changing part) in a turbo-frame ?
Thanks for your answer
-- Cyrill

Ryan's Response:

That's an interesting situation! Internally, when you redirect from LiveAction, LiveComponents sees that and checks to see if Turbo is installed. If it is, it does a Turbo.visit() instead of a real, full site redirect.

So that works great. But you want something different: not a page navigation, but a frame navigation. I'm not sure that it makes sense to add this feature specifically to LiveComponents, but there is a solution:

A) In your LiveAction, instead of redirecting, set some flag (i.e. property on your class) that indicates that you're in this "success" situation.

B) In your template, anywhere inside your root component element, add something like this:

```
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
That should do it! When this turbo-stream pops onto the page, it'll replace your existing turbo-frame with this new one. That new element will instantly activate, see its src attribute, and make an Ajax request to fetch that page. It'll then use its normal behavior of only loading the matching frame from that page into the frame.

Let me know if this works! I love combining these tools :)