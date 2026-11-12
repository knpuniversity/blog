---
published: YYYY-MM-DD
category: tech
preview: |-
    Building our courses means researching features, questioning workarounds,
    and collaborating on better solutions. Here's how the work behind the
    lessons helps improve Symfony itself.
---

# How Teaching Symfony Helps Improve It

While researching and building our recent [Symfony Security: The
Basics](https://symfonycasts.com/screencast/symfony8-security) and [Symfony
Security: Going
Further](https://symfonycasts.com/screencast/symfony8-security-extra) courses I
found a few things that could make the developer experience better,
opportunities for clearer explanations and a few things that led to discussions
and pull requests.

The process of making a course includes researching a feature, building an
example, trying the awkward cases, and explaining every step. A workaround we
might accept in our own application deserves another look when we're about to
teach it to someone else.

Several improvements from creating those two courses are now a part of Symfony
8.2, let's take a deeper look at those.

## Disabling users who are already logged in

Disabling an account sounds straightforward: add an `enabled` flag and have a
user checker reject disabled users during login.

But what happens if the user is already logged in?

Using `EquatableInterface` for this meant taking over Symfony's user comparison
logic. Keeping the existing checks for password, role and identifier changes
meant reproducing that logic in the application. [That's quite a detour for a
boolean flag.](https://github.com/symfony/symfony/issues/66052)

This led to
[`CheckRefreshedUserEvent`](https://github.com/symfony/symfony/pull/66117) and
the [`user_checker_on_refresh`
option](https://github.com/symfony/symfony/pull/66136). Symfony can keep its
normal checks and run your configured user checker when it reloads the user from
the session. The [official SecurityBundle recipe for Symfony
8.2](https://github.com/symfony/recipes/pull/1578) now enables this by default,
so new applications that add a checker for disabled accounts will also log those
users out on their next request.

## Safer impersonation and simpler buttons

Impersonation is useful when you need to see what another user sees. The
familiar `_switch_user` query parameter makes it easy to try, but it also means
changing the active user through a GET request.

For the course, we wanted a straightforward way to use a dedicated POST endpoint
with CSRF protection. That became the [new `path` and CSRF options for
`switch_user`](https://github.com/symfony/symfony/pull/64879).

During review, [Nicolas Grekas added `impersonation_form()` and
`impersonation_exit_form()`](https://github.com/symfony/symfony/pull/64879#issuecomment-5164279380).
These provide the form's action URL and hidden fields from the firewall
configuration, including the CSRF token when enabled. The template still renders
the form.

It made sense to do the same for logout, and Nicolas beat me to it by adding
[`logout_form()`](https://github.com/symfony/symfony/pull/65129).

Nicolas then [took the idea further with button
helpers](https://github.com/symfony/symfony/pull/66786). They render the
complete POST form, including its hidden fields and button, saving us from
writing the same Twig markup each time:

```twig
{{ logout_button('Log out') }}
{{ impersonation_button('Impersonate', user.userIdentifier) }}
{{ impersonation_exit_button('Exit impersonation') }}
```

We can customize the labels and HTML attributes.

There were smaller fixes along the way, too, including [making the impersonation
helpers respect a configured
`target_route`](https://github.com/symfony/symfony/pull/67667) when no explicit
destination is supplied.

## Connecting sudo mode to the login form

Asking someone to confirm their password before a sensitive action is another
feature we cover in the courses.

Nicolas Grekas added the [recent-authentication checks and re-authentication
infrastructure](https://symfony.com/blog/new-in-symfony-8-2-sudo-mode) for
Symfony 8.2. The next step was [making `form_login` participate in the new
flow](https://github.com/symfony/symfony/pull/66495).

Symfony can send the user back to the login form and provide their existing
username for it to fill in. The login page can also distinguish a password
confirmation from an ordinary login, which matters if it normally redirects
anyone who's already authenticated.

That last detail is easy to miss until you put the whole flow together.

## Giving users useful error messages

The same kind of friction shows up when a rate limit is reached or a signed link
has expired. An attribute makes the check convenient, but an HTML application
often needs a helpful message and a redirect.

Another contribution [lets controllers receive specific failures from
attributes](https://github.com/symfony/symfony/pull/67563) such as
`#[RateLimit]` and `#[IsSignatureValid]`, so the controller can handle the
problem while keeping the attribute. It removes a reason to go back to
performing the check by hand.

## Signed links that stop working when they should

A password-reset link should stop working after the password changes. [Versioned
signed URLs](https://github.com/symfony/symfony/pull/64408) let a link's
validity depend on a value such as the user's password hash. Once that value
changes, the old link no longer validates.

A related change [added support for a default expiration and deprecated signing
URLs without an expiry](https://github.com/symfony/symfony/pull/64455). These
are useful building blocks for links that grant temporary access or perform a
specific action.

## Configuration that is easier to explain

The stateless CSRF defaults for login and logout were installed by the Form
component's recipe. An application using Security without Forms missed those
defaults. The [configuration now lives in the appropriate
recipes](https://github.com/symfony/recipes/pull/1554), so installing the
security components supplies their own defaults.

## A misleading `remember_me` option

Researching remember-me behavior for the courses turned up a setting that
Symfony accepted but no longer used: the `remember_me` option under `form_login`
and other authenticators. Changing its value had no effect.

That [option is deprecated in Symfony
8.2](https://github.com/symfony/symfony/pull/64875). It's a small cleanup, but
an option that does nothing is confusing whether you're writing a course or
configuring your own application.

## Creating rate limiters where you need them

The rate limiting research also led to
[`RateLimiterBuilder`](https://github.com/symfony/symfony/pull/60084). In
Symfony 8.2, a service can inject one builder and create limiter factories as
needed, without a separate configuration entry and injection for every limiter.

That's useful for a limiter needed in just one place, or when its settings are
only known at runtime. It lets us configure those limiters directly in the code
that uses them.

That work is continuing. [Another
PR](https://github.com/symfony/symfony/pull/67406) would let us define a rate
limiting policy directly on a controller action or its injected limiter
argument. With the argument approach, the controller would decide when to
consume the limit, which is useful when only failed attempts should count. This
one didn't make Symfony 8.2, but we're hoping it lands in 8.3.

## The work carries beyond the course

These changes were shaped by discussion and review with other Symfony
contributors. Finding the problem is one piece of the open source puzzle;
arriving at something that fits the framework takes collaboration.

In [Symfony 8.2 Security
Upgrades](https://symfonycasts.com/screencast/symfony-82-security), we return to
the same application and replace several of our earlier workarounds with the new
tools. We simplify logout, protect impersonation with POST and CSRF, revisit
disabled-user sessions, and apply the new recent-authentication checks.

This is a less visible part of our tutorial creation process. The time we spend
building examples and questioning awkward steps leads to improvements in the
Symfony developer experience. That's part of the hidden value of your
SymfonyCasts subscription: helping support the research and collaboration that
make improvements like these possible.

And this time, part of the lesson is deleting code, a very satisfying outcome.
