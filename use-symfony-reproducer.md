Now that you've created a Symfony reproducer, let's use it to fix a bug in Symfony
or verify a fix proposed by someone else. We'll fork the Symfony repository,
link our reproducer to it, and make the necessary changes. Let's get started!

# Using a Symfony Reproducer

***TIP
This is part 2 of a 2-part _Reproducer_ series. If you haven't already, check
out [Part 1](https://symfonycasts.com/blog/symfony-reproducer) first.
***

Ok, so you've created a reproducer app for a bug you've found in Symfony. For
this article, we'll assume the reproducer you've created in part 1 is located
at `~/my-reproducer`.

The first thing we'll do is _fork_ the [`symfony/symfony`](https://github.com/symfony/symfony)
repository on GitHub. This will give us our own copy of the Symfony repository
where we can work.

1. Visit https://github.com/symfony/symfony and click the _Fork_
   button at the top of the page.
2. Choose your personal GitHub account as the owner and keep the default name
   of `symfony`.
3. Leave all the other options as is and click _Create Fork_. You should now be on https://github.com/your-username/symfony.
4. Wait a few seconds for the fork to be created.
5. Click the green _Code_ dropdown and under the _SSH_ tab,
   copy the URL to your clipboard.

Let's clone the repository to our local _workspace_ (`~`).

```bash
cd ~
git clone <your-fork-url-from-clipboard>
cd symfony
```

Now, let's add the source Symfony repository as an _upstream_ remote so we can
easily pull in the latest changes from Symfony.

```bash
git remote add upstream git@github.com:symfony/symfony.git
```

Next, let's fetch the latest changes from the Symfony repository (repeat
this step whenever you want to update your fork with the latest changes from
Symfony).

```bash
git fetch upstream
```

## Use your Reproducer to Fix the Bug

In part 1, you had to choose the Symfony version to use in your reproducer.
We'll _checkout_ this version in our forked Symfony repository.

```bash
git checkout 7.1
```

Now, we'll ensure we have the latest changes from Symfony in our branch.

```bash
git merge upstream/7.1
```

Since we want to make changes, we'll create our own branch to work in.

```bash
git checkout -b my-fix
```

***TIP
The branch name (`my-fix`) isn't important, but it should be descriptive and
unique to your fork.
***

We're now ready to _link_ our reproducer to this forked Symfony repository. Go
back to your reproducer:

```bash
cd ~/my-reproducer
```

The Symfony repository has a special PHP script to _link_ itself to a Symfony
project. This script will symlink all your app's `symfony/*` to the local fork
versions. Pretty handy, right?

```bash
php ../symfony/link .
```

You're now ready to work on the bug fix. Open your reproducer in your favorite
IDE. Normally, you'd never edit files in the `vendor/` directory, but in this
case, for files in `vendor/symfony/*`, you can! Changes you make to these files
are actually being made in your forked Symfony repository.

After you feel the bug has been fixed, you can go back to your local Symfony
fork and see the changes:

```bash
cd ~/symfony
git status
```

Follow the Symfony documentation on [Creating a Pull Request](https://symfony.com/doc/current/contributing/code/pull_requests.html#step-4-submit-your-pull-request)
to submit your fix to the Symfony repository!

## Use your Reproducer to Verify a Fix

If you aren't quite sure how to fix the bug, but created a Symfony issue and
linked to the reproducer you created in part 1, another community member might
have proposed a fix. You can use your reproducer to verify this fix. This is
helpful to confirm that the fix works and to provide feedback on the PR.

First, find the PR that proposes the fix. Next, you'll need to _checkout_ the
branch that contains the fix in your forked Symfony repository. This branch will
exist on someone else's fork of Symfony.

To do this, you'll need to add the other person's fork as a remote to your local
Symfony repository. On the PR page, notice some text neat the top that looks
something like:

> {user} wants to merge X commit(s) into symfony:{version} from {user}:{branch-name}

Click `{user}:{branch-name}` to jump to the `{user}`'s fork. Like we did with
our own fork, click the green _Code_ dropdown and copy the URL to your clipboard.

In your local Symfony repository, add the other person's fork as a remote:

```bash
cd ~/symfony
git remote add {user} <fork-url-from-clipboard>
git fetch {user}
```

Now, checkout the branch from the user's fork:

```bash
git switch -c {user}-{branch-name} {user}/{branch-name}
```

***TIP
I like to prefix the branch name with the user's username to make it clear
where the branch is coming from.
***

You're now ready to _link_ your reproducer to this branch:

```bash
cd ~/my-reproducer
php ../symfony/link .
```

You can now use your reproducer to verify the fix and provide feedback on the PR!

***TIP
[GitHub Desktop](https://github.com/apps/desktop) and the
[GitHub CLI](https://cli.github.com/) have tools that can make the process of
forking, cloning, and creating PRs easier. The method shown here is the manual,
_raw way_ to do it.
***

I hope these articles can be valuable to both help you fix bugs and help others
identify and fix bugs in Symfony. Symfony is all about community, together we
can make it better!

Any questions or feedback, we're here to help below in the comments!

Happy coding!
