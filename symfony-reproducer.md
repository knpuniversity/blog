# Creating a Symfony Reproducer

Oh, snap! You've discovered what you suspect is a bug in Symfony. Nooooooooooooo!
Actually, it's all good! Symfony is open source, so we have some good options to keep you moving:

1. Search the [symfony/symfony](https://github.com/symfony/symfony) repository for keywords related to your problem.
   It's possible this bug has already been reported and maybe a fix is already in progress.
2. Do you think you know how to fix the issue? Open a PR! See the
   [Symfony documentation](https://symfony.com/doc/current/contributing/code/pull_requests.html) to help with this.
   Add a test if possible, it helps a *ton* to get the PR merged!
3. Not sure how to fix? Yeah, bug fixes are hard. Are you able to create a failing test? If so, same as the above
   step, create a PR that demonstrates the bug in a failing test. Even if it doesn't include a fix, it's highly
   likely it will get reviewed and confirmed. You'll also likely get tips on how to fix it!
4. Still no luck? No worries: creating a fix or even a failing test can be tricky!

At this point, you should open a [bug report issue](https://github.com/symfony/symfony/issues/new/choose).

But wait! HOW you open the issue will make a HUGE difference in whether and how quickly the bug is fixed.
Another contributor can probably fix the issue, IF, they can reproduce it. And *that* is the key.
If we can create a simple Symfony app that shows the bug, then it's 10x, no 50x more likely that your
friendly neighborhood Symfony contributor will swing in to save the day.

***TIP
In general, pull requests (like those described in steps 2 and 3 above) are more likely to catch the eye
of maintainers than issues (even a PR that only includes a failing test).
***

## What is a Reproducer

A reproducer is a standard, fresh, Symfony application that, aside from the initial Symfony app skeleton code,
includes only the code to reproduce the bug you are seeing. Maintainers love these as it makes it super easy
to see the same bug as you (it's a fast way to make friends with the core team!). They can clone it and test
it locally to confirm and test possible fixes.

***TIP
Occasionally, even we Symfony devs make mistakes (gasp!).
Creating a reproducer is a great learning experience, and helps narrow down the
bug and (yes) occasionally helps us realize that it wasn't a bug at all, but a
a misconfiguration or mistake in our own app.
***

## Create a Reproducer

To create a reproducer, use the [Symfony CLI](https://symfony.com/download).
Be sure to replace `--version=7.1` with the version of Symfony your app is using.

```bash
symfony new --webapp --version=7.1 my-reproducer
```

***TIP
The `--webapp` flag is optional and may not be required depending on the reproducer you are creating.
It does give you a database and a bunch of other features that are ready to go out of the box.
Also, the maker bundle is included which can be helpful for generating the code you'll need.
***

Ok, we're ready! Open the app in your IDE and add whatever code is needed to see the issue.
This could be entities, services, messages, controllers, tests, or cryptocurrency keys (kidding!).
Remember, it's important to keep the code as simple as possible to make it easy for maintainers
to see the exact code that causes the problem. Add all this as a single commit so you can reference
both the repository and this commit in your Symfony issue.

***TIP
Instructions on how to use and see the issue can be added as a `README.md` at the root of the
repository. Here's a [good example](https://github.com/weaverryan/api_platform_null_object_security_reproducer?tab=readme-ov-file#api-platform-bug-reproducer-object-is-null-during-patch-security)
of this.
***

Next, create a (public) [new repository](https://github.com/new) on GitHub and follow the instructions
to push to it.

You're done! Hero status achieved. Add a link to the reproducer (ideally to the exact commit where you
added the code), then celebrate with a delicious and healthy 🥗. You deserve it!

## Example

Let's look at an example. A while back, in Symfony 4.2, I discovered a bug in the
`ServiceSubscriberTrait` so I opened this [bug report](https://github.com/symfony/symfony/issues/42217).
The exact bug isn't super important for this post but you can review the issue to see
the problem.

As you can see, the issue includes [a reproducer](https://github.com/kbond/subscriber-trait-bug-reproducer),
and instructions on how to experience the problem within the reproducer. If you look further down the issue,
you can see a maintainer suggesting a solution.

Interested in fixing the bug yourself? 10 ⭐s for you! In my next post, we'll see how you can leverage
your own reproducer to do that.

Happy reproducing 😉!
