# Creating a Symfony Reproducer

Oh snap! You've discovered what you suspect is a bug in Symfony. Don't despair, Symfony is open source so
there many ways you can help get it resolved. The following is a general list of steps you can take:

1. Search the [symfony/symfony](https://github.com/symfony/symfony) repository for keywords related to your problem.
   If you're seeing an unexpected exception, this message could be a good thing to search. It's possible this bug has
   already been reported and maybe a fix is already in progress. Be sure to search issues, pull requests and discussions.
2. Ask about your issue in the `#support` channel on the [Symfony Slack](https://symfony.com/slack). This can be a good
   way to confirm the bug or perhaps get ideas for a workaround. Keep in mind that users who respond in this channel are
   volunteers. Don't be discouraged if you don't get an answer.
3. Do you think you know how to fix the issue? Open a PR! See the
   [Symfony documentation](https://symfony.com/doc/current/contributing/code/pull_requests.html) to help with this.
   Be sure to add a test if possible as this will likely be asked of you during the review.
4. Not sure how to fix? Are you able to create a failing test? Same as the above step, create a PR that demonstrates
   the bug in a failing test. Even if it doesn't include a fix, it's highly likely it will get reviewed and confirmed.
   You'll also likely get tips on how to fix!
5. Open a [bug report issue](https://github.com/symfony/symfony/issues/new/choose) and clearly explain the issue
   you are facing. Initially including a reproducer (as we'll build below) is a great way to make your issue stand out!

> [!TIP]
> In general, pull requests (like described in steps 3 and 4 above) are more likely to catch the eye
> of maintainers than issues (even a PR that only includes a failing test).

After you've created an issue (step 5 above), a maintainer (or even another contributor) will hopefully start
a conversation to discuss the bug further. It's possible the bug is hard to describe or hard for maintainers to
understand. A common thing they may ask is to _create a reproducer_. What's this? How do you do this? In my
experience as a maintainer, when asking for a reproducer, this usually ends the discussion as it perhaps
seems too hard or time-consuming for the contributor. This is not the case! Let's look at how to do this!

## What is a Reproducer

A reproducer is a standard, fresh, Symfony application that, aside from the initial Symfony app skeleton code,
includes only the code to reproduce the bug you are seeing. Maintainers love these as it makes it super easy
to see the same bug as you. They can clone it and test it locally to confirm and test possible fixes.

> [!TIP]
> Just the process of creating a reproducer is a great learning experience and helps narrow down the actual
> bug. Many times, working on a reproducer will resolve your issue as you may find it's not a bug at all but
> just a misconfiguration or misuse of a feature.

## Create a Reproducer

The easiest way to create a reproducer is using the [Symfony CLI](https://symfony.com/download):

```bash
symfony new --webapp my-reproducer
```

This new app will be using the latest stable version of Symfony. If you need to target
a specific Symfony version, use the `--version` option:

```bash
symfony new --webapp --version=7.0 my-reproducer
```

If you are living on the edge and discovered a bug in the latest development version
of Symfony, great! You can use `--version=next` to create the app.

```bash
symfony new --webapp --version=next my-reproducer
```

Now, open the new app in your IDE. You're now ready to add the code to recreate the the problem.
This could be entities, services, messages, controllers, tests, etc. Remember, it's important
to keep the code as simple as possible to make it easy for maintainers to see the exact code
that causes the problem. Add all this as a single commit so you can reference both the repository
and this commit in your Symfony issue.

> [!TIP]
> Instructions on how to use and see the issue can be added as a `README.md` at the root of the
> repository.

Once ready, create a [new repository](https://github.com/new) on GitHub and follow the instructions
to push your local repository to it. The GitHub repository _must_ be public in order for
maintainers to access it.

Now, in your new or existing Symfony issue, link to this repository. As described above, it's nice
to include the link to the commit in your reproducer also. This helps maintainers to see exactly
the code changed from the standard app. Instructions on how to re-create the issue in your
reproducer should be described also.

## Example

Let's look at an example. A while back, in Symfony 4.2, I discovered a bug in the
`ServiceSubscriberTrait` so I opened this [bug report](https://github.com/symfony/symfony/issues/42217).
The exact bug isn't super important for this post but you can review the issue to see
the problem.

As you can see, the issue includes [a reproducer](https://github.com/kbond/subscriber-trait-bug-reproducer),
and instructions on how to experience the problem within the reproducer.

If you look further down the issue, you can see a maintainer suggesting a solution. Based on this feedback,
I ended up creating [a PR](https://github.com/symfony/symfony/pull/42238) to fix this bug.

In the next post, we'll look at how you can work on a such a fix within the context of your reproducer.
This allows you to quickly verify your solution and create a Symfony PR with the fix.

We'll also look at how to verify a PR created by another contributor with a proposed fix for your bug.
You'll be able to ensure the PR fixes the bug within your reproducer and the application where you
initially discovered the bug.

## Final Thoughts

While this post has been about creating a reproducer for a Symfony core bug, you can create
reproducers for bugs you find in 3rd party bundles or packages. You'll just need to require
and configure these packages in the reproducer.

Any questions, comments, or feedback, let us know below in the comments.

Happy reproducing :wink:!
