# Composer 2.9 Automatic Security Blocking: Fireside Chat

In November 2025 we got Composer 2.9 which introduced an important shift in how
to manage security dependencies in PHP projects, specifically automatic security blocking.

In the before times, Composer helped you audit your dependencies — but that
audit was passive. It would warn you when installed packages matched known
vulnerabilities, but it didn’t stop you from installing them. Yikes!

Lucky for us, Composer now automatically blocks updates to packages with known security
advisories — and that behavior is enabled by default. Boom!

## The Problem: Failing Installs and Updates

If you’re already using a vulnerable library in your project, upgrading or
requiring a package can now fail outright. While in the past, you would have 
received a quiet warning you about a security advisory, Composer now refuses to 
resolve constraints if a package version is known to be insecure.

Here’s what that experience looks like in practice:

- You run `composer update` or `composer require vendor/package`.
- Composer analyzes the dependency graph and security advisories from Packagist.
- If one or more selected versions have known vulnerabilities, the resolver blocks them.
- The operation fails — blocking install/update until you take decisive action.

As you can imagine, this can cause problems. Especially if you’re upgrading multiple packages or
maintaining legacy branches where secure versions aren’t yet available. There have been reports that an 
otherwise safe update fails because Composer refuses to pull in a version flagged as insecure. So, let's
take a deep breath and keep going.

## Why is this Change Important to you?

You might be thinking: “Why should I care, I already use composer audit in my CI.” That’s great,
but the difference with Composer 2.9 is:

- Blocking happens during dependency resolution, not just at the end.
- You aren't allowed to accidentally deploy or run code with known security issues.
- You no longer need 3rd-party meta-packages (like `roave/security-advisories`) for
this purpose — Composer has your back.

## Controlling or Fixing Security Blocking

Automatic security blocking is powerful, but as with all defaults there are
times when you need to take back control. You get several levers in composer.json
to manage this behavior.

1. Disable Blocking Entirely

If you’re in a situation where you know a package is safe (or you’re stuck
waiting for a patch), you can turn off security blocking:

```json
{
    "config": {
        "audit": {
            "block-insecure": false
        }
    }
}
```

This restores the old behavior where vulnerabilities are reported, but no
install is blocked.

2. Ignore Specific Advisories

Instead of disabling blocking globally, you can target specific advisory IDs:

```json
{
    "config": {
        "audit": {
            "ignore": {
                "GHSA-xxxx-xxxx": "We have a workaround",
                "CVE-2025-12345": "Not used in our context"
            }
        }
    }
}
```

Ignored advisories will still appear in reports, but won’t block resolution.
That lets you keep the protections in place while making informed exceptions.

3. Configure Severity Levels

Composer lets you control blocking based on severity:

```json
{
    "config": {
        "audit": {
            "ignore-severity": {
                "low": { "apply": "all" },
                "medium": { "apply": "block" }
            }
        }
    }
}
```

You can choose to only block high severity issues, while allowing low or
informational ones during install/update.

## Best Practices With Automatic Security Blocking

Here’s how to approach this feature in real projects:

✅ Run `composer audit` in the CI

Blocking is a great tool and coupled with `composer audit` your CI can 
really enforce clean dependency baselines.

✅ Keep your lock file up to date

Since security blocking runs during dependency resolution, make sure you regularly
refresh your `composer.lock` file. Easy peasy!

✅ Use advisory ignores thoughtfully

Even though it seems like a fun relaxing hobby, try not to ignore vulnerabilities. 
And if you have to, document your choices for both your colleagues and your future self.

❌ Don’t disable blocking globally (if you can avoid it)

It defeats the purpose of automated supply-chain security.

## Wrapping it up

Composer 2.9 automatic security blocking is a very cool moment for dependency
security in PHP. It stops installs and updates when vulnerabilities
are known, forcing developers to take security seriously at the root of the
dependency graph.

Yes, this can be disruptive — but the benefits are clear: fewer vulnerable
deployments, stronger supply-chain hygiene, and tighter feedback loops in CI/CD.

And if you need flexibility, Composer gives you fine-grained configuration via
`audit.block-insecure`, `audit.ignore`, and severity controls.

## Tutorial Tip

This will be helpful when working with dependencies on some of our older courses.

Happy secure installs!
