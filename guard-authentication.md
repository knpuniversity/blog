# [DX] Guard: Symfony Authentication with a Smile

***TIP

**tl;dr** Guard is a new library that makes Symfony authentication a joy. And
we've written a big tutorial all about it: [KnpUGuard Tutorial](https://knpuniversity.com/screencast/guard).
***

Symfony's authorization system - the stuff related to [voters](https://knpuniversity.com/screencast/symfony-voters) and roles - is
awesome. It's simple, it kicks butt, and it's one of my favorite things, just behind
fresh-baked cookies.

But then there's that other part: authentication. This is how you login: maybe with
a form or via OAuth, like Facebook login. This part is probably the
*single worst part of Symfony*. It's over-engineered, hard to customize and no fun
to work with.

Examples? Creating a form login is easy, but customizing how you load the user
or what happens when authentication fails is hard. Creating an API token is pretty
easy, assuming you understand what a "token" is and you don't need multiple authentication
methods. And how would you handle Facebook login? Fortunately, [HWIOAuthBundle](https://github.com/hwi/HWIOAuthBundle)
exists, but it has [a lot of security classes](https://github.com/hwi/HWIOAuthBundle/tree/master/Security) to make this happen.

This problem was screaming for a solution. If we could make Symfony's authentication
system simple and fun, the whole security system would go from a pain, to a powerful
tool.

## Introducing Guard Authentication (+ Tutorial)

Hello Guard! ([GitHub](https://github.com/knpuniversity/KnpUGuard), [Packagist](https://packagist.org/packages/knpuniversity/guard)): a tiny library (and bundle) that puts
every part of an authentication scheme into one place: [GuardAuthenticatorInterface](https://github.com/knpuniversity/KnpUGuard/blob/master/src/GuardAuthenticatorInterface.php).
To create your custom authentication system, just make one class, implement this
interface, fill in the methods and celebrate with a milk shake.

Need to customize how you query or load your user? You'll do that in `getUser()`.
Have a special way to check passwords or that a token is valid? Do whatever you want
in `createAuthenticatedToken()`. Maybe you need to hook into what happens right after
the user successfully authenticates. Just do that in `onAuthenticationSuccess()`
or `onAuthenticationFailure()` for the opposite.

Read the [KnpUGuard Tutorial](https://knpuniversity.com/screencast/guard) to get started.

This library won't be perfect yet, so if you find any issues or have a use-case
that isn't possible, [open an issue](https://github.com/knpuniversity/KnpUGuard/issues) and let's see if we can improve things.

## Why not put Guard into Symfony Itself?


Yes, there is a secret goal behind all of this: to get Guard merged into Symfony.
There *is* a pull request already [symfony/symfony#14673](https://github.com/symfony/symfony/pull/14673), but thanks to KnpUGuard,
you don't have to wait for the next version of Symfony. Releasing it now also let's
us test and improve things, so that the final version - if it's accepted into Symfony -
will be *truly* great.

So, read the [KnpUGuard Tutorial](https://knpuniversity.com/screencast/guard), try it out, and report back. With any luck, Symfony's
authentication system will be a tale of [The Ugly Duckling](https://en.wikipedia.org/wiki/The_Ugly_Duckling).
