# Symfony Deprecations Explained (Upgrade Without Breaking Things)

Upgrading Symfony does not have to feel like walking through a minefield. In fact, if you are using Symfony the way it is designed,
even major upgrades can be predictable, safe, and even a little boring - in the best way.

At the heart of this is Symfony's deprecation system.

When a feature is going to change or be removed, Symfony does not just rip it out and hope for the best. Instead, it marks that
feature as *deprecated*. Your app keeps working exactly as before - but you will start seeing warnings that point you to the new,
recommended approach. It's like getting a heads-up from the future version of your app.

This is what makes the upgrade path so smooth: **deprecations are your roadmap**.

If you are on Symfony 7.4, every breaking change coming in Symfony 8 is already flagged as a deprecation. That means you can fix
everything *before* you upgrade. And once your app runs with zero deprecations, you have essentially done the hard work already.

_**No deprecations = safe major upgrade**_

If you have been putting off an upgrade because you are worried about breaking things, try flipping your mindset: do not think about upgrading
first - think about eliminating deprecations. Once those are gone, the upgrade becomes the easy part.

We've got a quick video explanation for you on how this all fits together and why it is such a powerful part of the framework:

<div style="position: relative;padding-bottom: 56.25%; padding-top: 25px;">
<iframe width="560" height="315" src="https://www.youtube.com/embed/m2m2CRs4yMA?si=_8JLWx96Xc3jOUcn" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" referrerpolicy="strict-origin-when-cross-origin" allowfullscreen></iframe>
</div>

To dig deeper into how to find and fix deprecations, check out the
[Tracking & Fixing Deprecations](https://symfonycasts.com/screencast/symfony8-upgrade/deprecations)
chapter of our [Symfony 8 Upgrade course](https://symfonycasts.com/screencast/symfony8-upgrade)!
