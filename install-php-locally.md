# Installing PHP Locally (for Symfony CLI)

The [Symfony CLI](https://symfony.com/doc/current/setup/symfony_cli.html) is awesome for developing Symfony
applications locally! It enables you to run a local web server, integrates with Docker containers for
services like a database, and provides many other features to streamline your development workflow.
It's mostly batteries included, but it does require PHP to be installed locally.

We use it here at SymfonyCasts for almost all our courses. Let's look at how to install PHP on different
operating systems. We'll also make sure extensions for the most common databases (MySQL, SQLite, PostgreSQL)
are installed.

Make sure you have the [Symfony CLI installed](https://symfony.com/download) before getting started!

## macOS (Homebrew)

First, ensure you have [Homebrew](https://brew.sh) installed.

Next, install PHP:

```terminal
brew install php
```

This will install the latest stable version. Ensure it's working by running:

```terminal
symfony check:requirements
```

PHP should be ready to go with common extensions enabled. Also, the MySQL, SQLite, and PostgreSQL extensions
should be installed by default. Double check by running:

```terminal
php -m | grep pdo
```

### Homebrew PHP Extensions

If you need additional extensions, not included, use [pecl](https://pecl.php.net/), which was installed with PHP.
For instance, to install the Redis extension, run:

```terminal
pecl install redis
```

## Ubuntu/Debian Linux (apt)

In Ubuntu and Debian, you can install PHP using the `apt` package manager. Your default package repository probably
won't have the latest PHP version. The 3rd-party `ondrej/php` repository is a popular choice that provides
the latest PHP versions and extensions. Add it to your system by running:

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
Replace `8.4` with the version you want to install. Example: `php8.5 php8.5-cli ...` for PHP 8.5.
***

Ensure all looks good by running:

```terminal
symfony check:requirements
```

### APT PHP Extensions

For additional PHP extensions, you don't need to use `pecl`, these are also available via `apt`. For example, to install
the Redis extension, run:

```terminal
sudo apt install php8.4-redis
```

## Windows (WSL)

Running PHP natively on Windows is possible, but can be tricky. I recommend using the "Windows Subsystem for Linux
(WSL)" to run Ubuntu. It isn't in a container or anything, it runs *directly* on Windows! Follow the
[WSL installation guide](https://learn.microsoft.com/en-us/windows/wsl/install) to get setup with WSL and Ubuntu.

Now, follow the [Ubuntu/Debian Linux (apt)](#ubuntu-debian-linux-apt) section above to get PHP installed!

---

We'd like to keep this as a *live document* so if you have any issues or suggestions, please comment below or
click the *Edit* button above to propose changes.

Happy Installing! 🚀
