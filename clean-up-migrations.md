# Clean up your migrations!

Database migrations are a great way to safely update your database schema,
and so it's very useful for production where you do not want to lose the data
you have. If you want to know more about migrations - we have a
[screencast](https://symfonycasts.com/screencast/symfony-doctrine/migrations)
about it, check it out!

But here, I want to give you a tip on how to clean up your migrations...
if you find your `migrations/` directory growing and growing... like we did!
First of all, this is mostly a problem for local development: on production,
we don't really care how many migration files were executed in the past.

But while developing locally, to set things up, we recommend creating your database and
then running the `doctrine:migrations:migrate` command so that you have exactly what's
on production, including a `migration_versions` table. This is better than running the
`doctrine:schema:update` command, which then makes it hard to test new migrations.

But, if you have a lot of old migration files, this can start to get kinda slow!
On SymfonyCasts, we had 49. And so, while it's TOTALLY optional, if a giant `migrations/`
folder bothers you, you CAN clean it up! Also, congratulations on having a successful
site that has lived long enough to need this.

## Generating a Complete Single Migration

***TIP
Now you can achieve this with built-in features in a much easier way than described
in the beginning of this blog post!

First of all, delete all your migrations:

```terminal
rm -rf migrations/*
```

Now, dump the current schema into a new single migration:

```terminal
symfony console doctrine:migrations:dump-schema
```

***TIP
 The `doctrine:migrations:dump-schema` command doesn't dump foreign key constraints. Instead,
 better use:
 
 ```terminal
symfony console doctrine:migrations:diff --from-empty-schema
```
***

The bundle provides one more command that helps to "roll up" the migrations by deleting
all tracked versions and insert the one version that exists:

```terminal
symfony console doctrine:migrations:rollup
```

You just need to deploy the code to production and run this command manually. But
if you have automated deployment that runs migrations - it might not fit you.
For automated deploy, continue reading this blog post from
[Skipping the Re-generated Migration on Production](https://symfonycasts.com/blog/clean-up-migrations#skipping-the-re-generated-migration-on-production).

P.S. Thanks to [Christophe Coevoet](http://github.com/stof) for pointing it in the comments.
***

Of course, we're going to do this locally first, so, run:

```terminal
rm -rf migrations/*
```

to drop all the migrations. Then, make sure you don't have important data
you care about in your local database, then drop its schema completely with:

```terminal
symfony console doctrine:schema:drop --force
```

***TIP
The `symfony console` command is the same as `bin/console` but it allows environment
variables to be injected if you're using the Docker integration.
***

Now let's generate a new migration with
[MakerBundle](https://symfonycasts.com/screencast/symfony-fundamentals/maker-command):

```terminal
symfony console make:migration
```

Double-check the queries inside the new file to make sure the new migration looks good.

But we can't just commit these changes and deploy to production... because the database server
will try to run this command and recreate the tables we already have on production!
There's almost nothing *less* fun than migrations failing during a deploy!
How can we work around it without manually messing with the production database?

### Skipping the Re-generated Migration on Production

With a trick: rename the migration you just created to one that was already executed on
production. You can check the latest migration name you dropped by running:

```terminal
git status
```

For example, if you had a migration file called `Version20220415102030.php`
before, rename the new migration file that was just generated to this exact name,
and don't forget to change the class name inside the file accordingly. This will
allow you to deploy to production, but this migration won't be executed
(it will be skipped) because it was already executed in the past.

## Removing the Previously Executed Migrations from Production

But we can improve this a bit more. Because if you executed this migration right
now - it would say something like:

> [WARNING] You have X previously executed migrations in the database that are not
> registered migrations.

Rude! This is because the `migration_versions` table on production will now show that it
executed a bunch of migrations in the past... but these migration files don't exist
anymore! We could remove them manually from the production database but... yikes!
Manually running a `DELETE` query on the production database is less fun than taking
your cat to the vet.

Generate a blank migration instead:

```terminal
symfony console doctrine:migration:generate
```

Then, open it, and add a new SQL statement at the end:

```php
final class Version20220415102031 extends AbstractMigration
{
    public function up(Schema $schema): void
    {
        // this up() migration is auto-generated, please modify it to your needs

        $this->addSql('DELETE FROM migration_versions WHERE version NOT LIKE "%20220415102030" AND version NOT LIKE "%20220415102031"');
    }

    // ...
}
```

Where `20220415102031` is the number of the blank migration we just generated,
and `20220415102030` is the number of the other complete migration we have.
Make sure you replace those numbers with yours!

And we're ready! Try it locally first:

```terminal
symfony console doctrine:migration:migrate
```

It will still show the warning, but only once. To double-check, you may want
to look at `migration_versions` table in your local database to make sure
we have only those 2 migrations there.

Now, it's safe to deploy it to production and celebrate with a clean `migrations/`
directory!
