# Installing PHP and Symfony CLI Locally

The [Symfony CLI](https://symfony.com/doc/current/setup/symfony_cli.html) is awesome for developing Symfony
applications locally! It enables you to run a local web server, integrates with Docker containers for
services like a database, and provides many other features to streamline your development workflow.

Here at SymfonyCasts, we use it for all our Symfony courses so to best follow along, you'll want to
install it as well.

It's *mostly* batteries-included, but it does need PHP available. The official Symfony recommendation is to
install PHP locally and natively on your computer. Let's look at how to install PHP and the Symfony CLI on
different operating systems. We'll also make sure extensions for the most common databases (MySQL, SQLite,
PostgreSQL) are installed.

To get started, follow the instructions below for your operating system:
1. [macOS](#macos-homebrew)
2. [Ubuntu/Debian Linux](#ubuntu-debian-linux-apt)
3. [Windows](#windows-wsl)

Once you have PHP and the Symfony CLI installed, [verify your installation](#verify-php-symfony-cli-installation).

## macOS (Homebrew)

First, ensure you have the [Homebrew](https://brew.sh) package manager installed (if it isn't already).

### Homebrew PHP

Now, install PHP:

```terminal
brew install php
```

***TIP
This will install the latest stable version of PHP. If you want a specific version, check
[the Homebrew PHP formula](https://formulae.brew.sh/formula/php). Under "Other versions"
you'll find available alternate versions. For example, to install PHP 8.3, run:

```terminal
brew install php@8.3
```
***

### Homebrew PHP Extensions

Homebrew's PHP formula includes many extensions out of the box (including common database extensions).
If you need additional extensions, use [PECL](https://pecl.php.net/) (PHP Extension Community
Library), which was installed with PHP. For instance, to install the Redis extension, run:

```terminal
pecl install redis
```

### Homebrew Symfony CLI

There are several methods to [install the Symfony CLI](https://symfony.com/download), since we already
have Homebrew installed, let's use it to install the Symfony CLI:

```terminal
brew install symfony-cli/tap/symfony-cli
```

You should be ready to go on your Mac! [Verify the installation below](#verify-php-symfony-cli-installation).

## Ubuntu/Debian Linux (apt)

In Ubuntu and Debian, you can install PHP using the `apt` package manager but the default package repository probably
won't have the latest PHP version available. The 3rd-party [`ondrej/php` repository](https://launchpad.net/~ondrej/+archive/ubuntu/php)
is a popular choice that provides the latest PHP versions and extensions. 

### APT PHP

First, add the `ondrej/php` repository to your system by running:

```terminal
sudo add-apt-repository ppa:ondrej/php
sudo apt update
```

To install PHP 8.4 and some common extensions, run:

```terminal
sudo apt install php8.4 php8.4-cli php8.4-common php8.4-curl php8.4-mbstring php8.4-xml php8.4-zip php8.4-gd php8.4-intl php8.4-bcmath 
```

To install the common database extensions, run:

```terminal
sudo apt install php8.4-mysql php8.4-pgsql php8.4-sqlite3
```

***NOTE
Replace `8.4` with the version you want to install. For example: `php8.3 php8.3-cli ...` for PHP 8.3.
***

### APT PHP Extensions

For additional PHP extensions, these are also available via `apt`. For example, to install
the Redis extension, run:

```terminal
sudo apt install php8.4-redis
```

### APT Symfony CLI

There are several methods to [install the Symfony CLI](https://symfony.com/download), since we used
`apt` to install PHP, I'd suggest using that method in the link above to install the Symfony CLI as well.

You should be ready to go! [Verify the installation below](#verify-php-symfony-cli-installation).

## Windows (WSL)

Running PHP natively on Windows (as an `exe`) is *possible*, but can be tricky. I recommend using the
"Windows Subsystem for Linux" (WSL) to run Ubuntu. It isn't in a container or other virtualization tool,
it runs *directly* on Windows! Follow the
[WSL installation guide](https://learn.microsoft.com/en-us/windows/wsl/install)
to get setup with WSL and Ubuntu.

Now, follow the [Ubuntu/Debian Linux (apt)](#ubuntu-debian-linux-apt) section above to get PHP installed!

## Verify PHP/Symfony CLI Installation

Check your PHP version by running:

```terminal
php -v
```

Check your Symfony CLI version by running:

```terminal
symfony version
```

You can run PHP *through* the Symfony CLI. Ensure the following matches your PHP version above:

```terminal
symfony php -v
```

For the common database extensions, ensure they are all installed by running:

```terminal
symfony php -m | grep pdo
```

You should see the following output:

```
pdo_mysql # MySQL extension
pdo_pgsql # PostgreSQL extension
pdo_sqlite # SQLite extension
```

Finally, to ensure that you're ready to go with Symfony, run:

```terminal
symfony check:requirements
```

***TIP
When this command is run *outside* a Symfony project directory, it will check the *general* requirements
for Symfony applications. This includes PHP version, required extensions, and other system checks.
If run *inside* a Symfony project directory, it will perform additional, project-specific checks.
***

Ok, you should be all set up!

Happy Installing! 🚀

---

We'd like to keep this as a *live document* so if you have any issues or suggestions, please comment below or
click the *Edit* button above to propose changes.
