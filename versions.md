# Now Exposed: composer.json & package.json for every Tutorial

We're lucky to get a *lot* of user feedback at SymfonyCasts. One of the
most common ones sounds a bit like this:

> Hey! Could you post the date that a tutorial was recorded? I want to
> make sure it's up to date.

I *totally* get this: one of the first things I do when I look at a blog
post or Stack Overflow is: how old is this?

But for tutorials, the date doesn't tell the full story: some topics are
virtually timeless (e.g. the entire object-oriented series) while other
technologies change quickly.

A *better* question is:

> What versions of the technology does this tutorial teach?

Or:

> What version of Symfony is this built on?

To answer these, starting today, every tutorial (except for a few ancient ones)
now publishes their full `composer.json` and `package.json` (if it has one) file
contents right on the main tutorial page under a button that highlights a main
library that's being taught. If you open the contents, you can see the exact,
*locked* version of each library from the final code in the tutorial.

<img src="https://d399irh3pgqnz3.cloudfront.net/prod/uploads/blog/versions/versions.gif" alt="Animation of clicking the new versions button" />

I already *love* this feature. But we also realized that even *it* can sometimes
be misleading. For example, our
[Doctrine Queries Tutorial](https://symfonycasts.com/screencast/doctrine-queries) is built
on an old version of Symfony (version 2!), but the logic of making queries in Doctrine
hasn't changed. In this case, we give you some extra context:

> This course is built on Symfony 2, but most of the concepts apply just fine
> to newer versions of Symfony. If you have questions, let us know :).

This gives *you* the information that this uses an old version of Symfony, but
the confidence to know that what we want you to learn from this course is still relevant.

Let us know how you like the feature and if we can offer any other info to make
the tutorials more useful! Huge thanks to
[Victor](https://twitter.com/bocharsky_bw) for his work, re-work and re-work again
to make this feature *really* shine.

❤️ Ryan

