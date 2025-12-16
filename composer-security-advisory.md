# Composer 2.9 Automatic Security Blocking

Composer 2.9 (released in November 2025) introduced an important shift in how
to manage security dependencies in PHP projects: automatic security blocking.

Prior to this release, Composer helped you audit your dependencies — but that
audit was passive. It warned you when installed packages matched known
vulnerabilities, but it didn’t stop you from installing them.

Composer now automatically blocks updates to packages with known security
advisories — and that behavior is enabled by default.

## The Problem It Creates: Failing Installs and Updates

If you’re already using a vulnerable library in your project, upgrading or
requiring a package can now fail outright. Instead of quietly
warning you about a security advisory, Composer refuses to resolve constraints
if a package version is known to be insecure.

Here’s what that experience looks like in practice:

- You run composer update or composer require vendor/package.
- Composer analyzes the dependency graph and security advisories from Packagist.
- If one or more selected versions have known vulnerabilities, the resolver blocks them.
- The operation fails — blocking install/update until you take action.

This can cause problems, especially if you’re upgrading multiple packages or
maintaining legacy branches where secure versions aren’t yet available. In some cases,
teams report that an otherwise safe update fails because
Composer refuses to pull in a version flagged as insecure.

## Why This Change Matters

You might be thinking: “I already use composer audit in my CI.” That’s great —
but the difference with Composer 2.9 is:

- Blocking happens during dependency resolution, not just afterward.
- You can’t accidentally deploy or run code with known security issues.
- You no longer need 3rd-party meta-packages (like `roave/security-advisories`) for
this purpose — Composer has you covered.

In other words: Composer now stops the ship from sailing into vulnerable waters
instead of just waving red flags when it’s too late.

## How to Control or Fix Security Blocking

Automatic security blocking is powerful, but as with all defaults, there are
times when you need control. Composer gives you several levers in composer.json
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

Rather than disabling blocking globally, you can target specific advisory IDs:

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
Composer

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
Composer

Best Practices With Automatic Security Blocking

Here’s how to approach this feature in real projects:

✅ Add composer audit to CI

Blocking is great, but coupled with audit reports your CI can enforce clean
dependency baselines.

✅ Keep your lock file up to date

Security blocking runs during dependency resolution — so make sure you regularly
refresh your composer.lock.

✅ Use advisory ignores thoughtfully

Don’t ignore vulnerabilities without understanding impact — add comments
explaining why.

❌ Don’t disable blocking globally unless necessary

It defeats the purpose of automated supply-chain security.

## Conclusion

Composer 2.9 automatic security blocking is a watershed moment for dependency
security in PHP. By default it stops installs and updates when vulnerabilities
are known, forcing developers to take security seriously at the root of the
dependency graph.

Yes, this can be disruptive — but the benefits are clear: fewer vulnerable
deployments, stronger supply-chain hygiene, and tighter feedback loops in CI/CD.

And if you need flexibility, Composer gives you fine-grained configuration via
audit.block-insecure, audit.ignore, and severity controls.
Composer
