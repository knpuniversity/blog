# Clean up your migrations!

Database migrations are a great way to safely update your database schema,
and so it's very useful for production where you do not want to lose the data
you have. If. you want to know more about migrations - SymfonyCasts has a
[screencasts](https://symfonycasts.com/screencast/symfony-doctrine/migrations)
about it, check it out!

But here, I will give you a tip on how to clean up your migrations correctly.
After a while, you may have a lot of migrations that start running long,
especially if you're running them on a huge set of data. In this case,
you can drop all the migrations you have so far and create a single migration
that will recreate the whole database schema from scratch.

Of course, we're going to do this locally first, so, run:
```terminal
rm -rf migrations/*
```

to drop all the migrations. Then, make sure you don't have important data
you care about in your local database and drop its schema completely with:
```terminal
symfony console doctrine:schema:drop --force
```

Now let's generate a new migration with
[MakerBundle](https://symfonycasts.com/screencast/symfony-fundamentals/maker-command):
```terminal
symfony console make:migration
```

***TIP
If you don't have MakerBundle installed - you can easily add it with:
```terminal
symfony composer req maker
```

Or you can do this with DoctrineMigrationsBundle as well:
```terminal
symfony console doctrine:migration:diff
```
***

Double-check the queries inside the new file to make sure the new migration looks good.

But we cannot just commit changes and deploy to production because the database server
will try to recreate the tables we already have on production and migration will fail.
How can we work around it without messing up manually with the production database?

Rename the migration you just created to one that was already executed on
the production - you can check the latest migration name you dropped by running:
```terminal
git status
```

For example, if you had a migration file called `Version20220415102030.php`
before - rename the new migration file that was just generated to this exact name,
and don't forget to change its class name inside the file accordingly. This will
allow you to deploy to production but this migration won't be executed (will be skipped)
because it was already executed in the past.

Basically, that's it. The trick is to *modify* the existing migration our production
database knows about so that this migration would be skipped because we already have
a valid schema there.

But we can improve this a little bit more. Well, if you executed this migration
right now - it would say something like:

> [WARNING] You have X previously executed migrations in the database that are not
> registered migrations.

every time you execute migrations unless you clean up those deleted migrations from
the `migration_versions` table. But who wants to remove them manually on production?

Generate a blank migration instead:
```terminal
symfony console doctrine:migration:generate
```

Then, open it and add a new SQL statement at the end:
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
and `20220415102030` is the number of another complete migration we have.
Make sure you replaced those numbers with yours!

And we're ready! Try it locally first:
```terminal
symfony console doctrine:migration:migrate
```

to make sure it does not fail. It will still show the warning, but only once.
All the further runs will not show it anymore. To double-check,  you may want
to look at `migration_versions` table in your local database to make sure we
have only those 2 migrations there.

Now, it's safe to deploy it to production and celebrate with the low total number
of migrations that you have now!
