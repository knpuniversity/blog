# Using a Symfony Reproducer

***TIP
This is part 2 of a 2-part _Reproducer_ series. If you haven't already, check
out [Part 1](https://symfonycasts.com/blog/symfony-reproducer) first.
***

Ok, so you've created a reproducer app for a bug you've found in Symfony. For
this article, we'll assume the reproducer you've created in part 1 is located
at `~/my-reproducer`.

## Forking the Symfony Repository

The first thing we'll do is _fork_ the [`symfony/symfony`](https://github.com/symfony/symfony)
repository on GitHub. This will give us our own copy of the Symfony repository
where we can work.

1. Visit https://github.com/symfony/symfony and click the _Fork_
   button at the top of the page.
2. Choose your personal GitHub account as the owner and keep the default name
   of `symfony`.
3. Leave all the other options as is and click _Create Fork_.
4. You should now be on https://github.com/your-username/symfony.
5. Wait a few seconds for the fork to be created.
6. Click the green _Code_ dropdown and under the _SSH_ tab,
   copy the URL to your clipboard.

## Cloning your Fork

Let’s get the repo down to your local workspace:

```bash
cd ~
git clone <your-fork-url-from-clipboard>
cd symfony
```

Now we’re going to add the original Symfony repo as an _upstream_ remote. Why?
So we can easily pull in the latest Symfony updates and stay fresh!

```bash
git remote add upstream git@github.com:symfony/symfony.git
```

Let’s grab the latest changes from Symfony (you’ll want to run this command
whenever Symfony gets an update, so it’s good to get used to it).

```bash
git fetch upstream
```

## Use your Reproducer to Fix the Bug

In Part 1, you picked the Symfony version for your reproducer. Now it’s time
to match that version in our forked Symfony repo. Let’s check it out:

```bash
git checkout 7.1 # or any other version you picked for your reproducer
```

Boom! You’ve got the right version. Now, let’s make sure we’re up-to-date
by merging in any new changes from the _upstream_ repository:

```bash
git merge upstream/7.1
```

Great! Now, we’re going to create a new branch for our bug fix:

```bash
git checkout -b my-fix
```

***TIP
The branch name (`my-fix`) isn't important, but it should be descriptive and
unique to your fork.
***

## Linking your Reproducer to your Fork

Let’s link that reproducer you created to your freshly forked Symfony repo.
Head back to your reproducer directory:

```bash
cd ~/my-reproducer
```

Symfony comes with a handy PHP script to link itself to a project. This will
symlink all your app’s `symfony/*` packages to your local fork. Super neat, right?

```bash
php ../symfony/link .
```

## Fixing the Bug

Open your reproducer in your favorite IDE. Normally, messing with files in the 
`vendor/` directory is a no-no, but in this case, for files in `vendor/symfony/*`,
you can! Changes you make to these files are actually being made in your forked
Symfony repository because of the symlinks.

Once you’ve squashed that bug, jump back into your local Symfony fork and take a
look at your changes:

```bash
cd ~/symfony
git status
```

If everything looks good, follow the Symfony documentation on
[Creating a Pull Request](https://symfony.com/doc/current/contributing/code/pull_requests.html#step-4-submit-your-pull-request)
to submit your fix to the Symfony repository!

## Verifying a Proposed Fix

But if you can't fix it yourself - don't worry! Someone from the awesome Symfony community
may propose a fix. If this happens, you can help with the review and confirm with your
reproducer if the fix works. PR authors _love_ seeing "this fixes my issue" comments!

First, track down the Pull Request (PR) that has the fix. You’ll see something like this
at the top of the PR:

> {user} wants to merge X commit(s) into symfony:{version} from {user}:{branch-name}

Click `{user}:{branch-name}` to jump to the `{user}`'s fork. Like we did with
our own fork, click the green _Code_ dropdown and copy the URL to your clipboard.

We’re going to add their fork as a remote:

```bash
cd ~/symfony
git remote add {user} <fork-url-from-clipboard>
git fetch {user}
```

Now, checkout the branch from the user's fork:

```bash
git switch -c {branch-name} {user}/{branch-name}
```

***TIP
I like to prefix the branch name with the user’s GitHub handle
so I don’t get lost in branch-land!

```bash
git switch -c fabpot-fix-something fabpot/fix-something
```
***

Back to your reproducer! Let’s link it to this new branch:

```bash
cd ~/my-reproducer
php ../symfony/link .
```

Now, use your reproducer to verify that fix and show some love by
giving feedback on the PR!

## Bonus Tools

If you want to make your life a little easier (and why wouldn’t you?), check out
[GitHub Desktop](https://github.com/apps/desktop) or the
[GitHub CLI](https://cli.github.com). They’re awesome for forking, cloning,
and submitting PRs without all the manual fuss.

And there you have it! Whether you’re fixing a bug or helping the community
to squash one, you’re making Symfony even better. Got questions or feedback?
Drop them in the comments below!

Happy coding!
