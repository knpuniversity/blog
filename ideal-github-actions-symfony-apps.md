---
published: 2026-09-16
category: tech
preview: |-
  A practical, modern GitHub Actions setup for Symfony apps, covering tests, linting,
  static analysis, database checks, Symfony-aware diagnostics, and scheduled dependency
  security audits.
---

# Ideal GitHub Actions for Symfony Apps

# The Ideal GitHub Actions Workflow for Symfony

GitHub Actions workflows have a habit of growing into a pile of copied snippets, old workarounds and checks
that may or may not still be useful.

So, let's start fresh.

This is the GitHub Actions setup I'd use today for a typical Symfony application: a practical baseline for
testing, linting, static analysis and ongoing dependency security.

This is also a living document. As GitHub Actions, PHP, Composer and Symfony evolve, we'll keep it updated.

One important distinction: this is for a **Symfony application**, not a reusable bundle or library. For an
application, our goal is simple:

> Prove that the exact code and dependencies we're about to ship actually work in a production-like
> environment.

Let's build that.

## The Main CI Workflow

Let's start with the CI workflow I'd use today for a typical Symfony app:

```yaml
# .github/workflows/ci.yml

name: CI

# Global environment variables.
env:
  APP_ENV: test
  # For an application, test the PHP version we actually use in production.
  PHP_VERSION: '8.5'

  # Point Doctrine at the PostgreSQL service below.
  DATABASE_URL: 'postgresql://app:password@127.0.0.1:5432/app?serverVersion=16&charset=utf8'

  # Using MySQL instead? Comment out PostgreSQL above and use this.
  # DATABASE_URL: 'mysql://app:password@127.0.0.1:3306/app?serverVersion=8.0.32&charset=utf8mb4'

# Run for every pull request, plus the final commit that lands on main.
on:
  push:
    branches:
      - main
  pull_request:

# CI only needs to read the repository.
permissions:
  contents: read

# If we push again, cancel the now-outdated run.
concurrency:
  group: ${{ github.workflow }}-${{ github.ref }}
  cancel-in-progress: true

jobs:
  tests:
    name: Tests
    runs-on: ubuntu-latest

    # Run PostgreSQL for this job.
    services:
      postgres:
        image: postgres:16-alpine
        env:
          POSTGRES_DB: app
          POSTGRES_USER: app
          POSTGRES_PASSWORD: password
        ports:
          - 5432:5432
        options: >-
          --health-cmd "pg_isready -d app -U app"
          --health-interval 10s
          --health-timeout 5s
          --health-retries 5

      # Using MySQL instead? Comment out the PostgreSQL service above and use this.
      # mysql:
      #   image: mysql:8.0
      #   env:
      #     MYSQL_DATABASE: app
      #     MYSQL_USER: app
      #     MYSQL_PASSWORD: password
      #     MYSQL_ROOT_PASSWORD: password
      #   ports:
      #     - 3306:3306
      #   options: >-
      #     --health-cmd "mysqladmin ping -h 127.0.0.1 -uapp -ppassword"
      #     --health-interval 10s
      #     --health-timeout 5s
      #     --health-retries 5

    steps:
      # Check out the application.
      - name: Checkout
        uses: actions/checkout@v7

      # Install PHP.
      - name: Setup PHP
        uses: shivammathur/setup-php@v2
        with:
          php-version: ${{ env.PHP_VERSION }}
          coverage: none

      # Install the exact locked dependencies and automatically cache Composer downloads.
      - name: Install dependencies
        uses: ramsey/composer-install@v4
        with:
          require-lock-file: true

      # Create the test database and bring its schema up to date.
      - name: Setup database
        run: |
          php bin/console doctrine:database:create --if-not-exists
          php bin/console doctrine:migrations:migrate --no-interaction

      # Verify that the migrations and Doctrine mapping agree. If not, show the missing SQL.
      - name: Validate database schema
        run: |
          php bin/console doctrine:schema:validate || {
            echo "::group::Schema changes"
            php bin/console doctrine:schema:update --dump-sql
            echo "::endgroup::"
            exit 1
          }

      # Run the test suite.
      - name: Run tests
        run: vendor/bin/phpunit

  lint:
    name: Lint
    runs-on: ubuntu-latest

    steps:
      # Check out the application.
      - name: Checkout
        uses: actions/checkout@v7

      # Install PHP and the Symfony CLI.
      - name: Setup PHP
        uses: shivammathur/setup-php@v2
        with:
          php-version: ${{ env.PHP_VERSION }}
          tools: symfony-cli
          coverage: none

      # Make sure composer.json is valid and composer.lock is in sync.
      - name: Validate Composer files
        run: composer validate --strict --no-check-publish

      # Install the exact locked dependencies and automatically cache Composer downloads.
      - name: Install dependencies
        uses: ramsey/composer-install@v4
        with:
          require-lock-file: true

      # Make sure the production service container compiles correctly.
      - name: Lint container
        run: php bin/console lint:container --env=prod

      # Catch invalid Twig before it reaches production.
      - name: Lint Twig
        run: php bin/console lint:twig --env=prod

      # Catch invalid Symfony YAML configuration.
      - name: Lint YAML
        run: php bin/console lint:yaml config --parse-tags

      #! Requires symfony/translation.
      # Validate the contents of all translation catalogs.
      - name: Lint translations
        run: php bin/console lint:translations

      # Catch Symfony-specific problems like unknown routes, templates and services.
      - name: Symfony diagnostics
        run: symfony lsp:check --format=github

      #! Requires symfony/asset-mapper.
      # Make sure production assets can be compiled.
      - name: Compile assets
        run: php bin/console asset-map:compile --env=prod

      # Exercise the production cache warmers before deployment.
      - name: Warm production cache
        run: APP_DEBUG=0 php bin/console cache:warmup --env=prod

  #! Requires php-cs-fixer/shim as a dev dependency.
  php-cs-fixer:
    name: PHP CS Fixer
    runs-on: ubuntu-latest

    steps:
      # Check out the application.
      - name: Checkout
        uses: actions/checkout@v7

      # Use the same PHP version as the application.
      - name: Setup PHP
        uses: shivammathur/setup-php@v2
        with:
          php-version: ${{ env.PHP_VERSION }}
          coverage: none

      # Install the project-local PHP CS Fixer version and its dependencies.
      - name: Install dependencies
        uses: ramsey/composer-install@v4
        with:
          require-lock-file: true

      # Check code style without changing files, and show the diff when it fails.
      - name: Check code style
        run: vendor/bin/php-cs-fixer check --diff --using-cache=no

  #! Requires phpstan/phpstan as a dev dependency.
  phpstan:
    name: PHPStan
    runs-on: ubuntu-latest

    steps:
      # Check out the application.
      - name: Checkout
        uses: actions/checkout@v7

      # Use the same PHP version as the application.
      - name: Setup PHP
        uses: shivammathur/setup-php@v2
        with:
          php-version: ${{ env.PHP_VERSION }}
          coverage: none

      # Install the project-local PHPStan version and its extensions.
      - name: Install dependencies
        uses: ramsey/composer-install@v4
        with:
          require-lock-file: true

      # Use a warm PHPStan result cache when possible to speedup analysis
      - name: "Restore PHPStan result cache"
        uses: actions/cache/restore@55cc8345863c7cc4c66a329aec7e433d2d1c52a9 # v6.1.0
        with:
          path: "phpstan/tmp"
          key: "result-cache-v1-${{ env.PHP_VERSION }}-${{ github.run_id }}"
          restore-keys: |
            result-cache-v1-${{ env.PHP_VERSION }}-

      # Run static analysis using the project's PHPStan configuration.
      - name: Run PHPStan
        run: vendor/bin/phpstan analyse --no-progress

      # Persist the PHPStan cache for re-use on the next run
      - name: "Save result cache"
        uses: actions/cache/save@55cc8345863c7cc4c66a329aec7e433d2d1c52a9 # v6.1.0
        if: ${{ !cancelled() }}
        with:
          path: "phpstan/tmp"
          key: "result-cache-v1-${{ env.PHP_VERSION }}-${{ github.run_id }}"
```

***NOTE
The `ramsey/composer-install` action handles Composer caching for us, so there's no separate
`actions/cache` step.
***

There's nothing especially clever here - and that's kind of the point.

We have four jobs running in parallel: **Tests**, **Lint**, **PHPStan**, and **PHP CS Fixer**. Together,
they check that the app works, Symfony can successfully build it, static analysis is happy, and the code
style is consistent.

## A Scheduled Composer Security Audit

Security advisories can appear long after a dependency was added, so running an audit only when code changes
isn't enough. Instead, I like a small, separate workflow that runs once a week:

```yaml
# .github/workflows/composer-audit.yml

name: Composer Security Audit

on:
  # Run automatically every Monday at noon UTC.
  schedule:
    - cron: '0 12 * * 1'

  # Also allow us to run the audit manually.
  workflow_dispatch:

permissions:
  contents: read

jobs:
  composer-audit:
    name: Composer Audit
    runs-on: ubuntu-latest

    steps:
      # Check out composer.lock.
      - name: Checkout
        uses: actions/checkout@v7

      # Install Composer. No project dependencies need to be installed.
      - name: Setup Composer
        uses: shivammathur/setup-php@v2
        with:
          tools: composer
          coverage: none

      # Fail on security advisories while still reporting abandoned packages.
      - name: Run Composer audit
        run: composer audit --locked --abandoned=report

      # Optional: notify Slack when the audit fails.
      # - name: Notify Slack on failure
      #   if: failure()
      #   uses: slackapi/slack-github-action@v4
      #   with:
      #     method: chat.postMessage
      #     token: ${{ secrets.SLACK_TOKEN }}
      #     payload: |
      #       channel: <CHANNEL_ID>
      #       username: "Composer Audit Failed"
      #       icon_emoji: ":rotating_light:"
      #       text: |
      #         Repo: ${{ github.repository }}
      #         Branch: ${{ github.ref_name }}
      #
      #         <${{ github.server_url }}/${{ github.repository }}/actions/runs/${{ github.run_id }}|View details>
```

***NOTE
Scheduled GitHub Actions workflows only run from the repository's default branch. Make sure
`composer-audit.yml` is committed there, or the schedule won't trigger.
***

The key is `composer audit --locked`: Composer can check the exact dependencies in `composer.lock` without
installing the app or its dependencies.

We don't even care which PHP version this workflow uses. It doesn't need to match production - we just need
PHP to run Composer.

I also use `--abandoned=report` so abandoned packages are reported without failing a workflow that's
specifically about security vulnerabilities.

And since scheduled workflow failures are easy to miss, I've included an optional Slack notification that
fires when the audit fails.

## Wrapping Up

A good CI setup should give you confidence without becoming its own project.

The setup above is intentionally pretty boring: test the app, lint it, analyze it, check the code style, and keep an eye on dependency security.

And since all of this evolves, we'll keep this post updated as GitHub Actions, PHP, Composer, and Symfony best practices change.

Got a check you always add, or one you think doesn't earn its place? Let us know in the comments below!

Happy Continuous Integrating!
