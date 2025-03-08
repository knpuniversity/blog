# Package Spotlight: `zenstruck/class-metadata`

Ever found yourself trying to explain how or why something happened in your app?
Then it's time to implement an *auditing* feature! This will allow you to track activity
including changes to all your entities. An `Activity` entity includes a reference to the
modified entity (the entity class name) and the entity's ID. This enables you to later
retrieve and display:

1. All activity
2. Activity for a specific entity class
3. Activity for a specific entity instance

Now, storing the class name in the database does have a few downsides:

1. Renaming the class requires a database migration to update potentially 1000's of rows.
2. This name isn't human-readable. Being human myself, it would be nice if this activity
   was displayed in a way that was easy for me to read.

Ok, what about showing the short name of the class? `App\Entity\ProductCategory` could be
`ProductCategory` or even converted to kebab-case: `product-category`. The problem here is
that you can't go the other way: from `product-category` back to `App\Entity\ProductCategory`
to query the database for `product-category` activity.

## Enter `zenstruck/class-metadata`

First, let's install it with:

```terminal
composer require zenstruck/class-metadata
```

***NOTE
You'll be prompted to enable the Composer plugin - be sure to do this.
***

This package enables you to add an `#[Alias]` attribute to any class in your app:

```php
use Zenstruck\Alias;

#[Alias('product-category')]
class ProductCategory
{
    // ...
}
```

***IMPORTANT
Whenever you add or change an attribute, be sure to run:

```terminal
composer dump-autoload
```

Behind the scenes, the Composer plugin hooks into the autoload dump process. It scans
your codebase for any `#[Alias]` attributes and generates a mapping file that enables
superfast lookups. The mapping is pure PHP arrays, so no reflection or scanning is
needed at runtime. But, this does mean you need to run `composer dump-autoload`
whenever you *add or change an alias*.
***

## Aliases at Runtime

At runtime, you can get the alias for a class or object like so:

```php
use Zenstruck\Alias;

Alias::for(ProductCategory::class); // "product-category"
// or even
$category = new ProductCategory();
Alias::for($category); // "product-category"
```

In our auditing example, you'd use this when saving activity to the database:

```php
class ActivityManager
{
    // ...
    
    function logActivity(object $entity, int $entityId, string $action): void
    {
        if (!$alias = Alias::for($entity)) {
            return; // no alias 
        }
    
        $this->saveToDatabase($alias, $entityId, $action);
    }
}
```

***NOTE
`Alias::for()` will return `null` if no alias is found for the given class or object.
***

To go *the other way* - from alias to class name - you can do:

```php
Alias::classFor('product-category'); // "App\Entity\ProductCategory"
```

After querying our database for activity, we can use this to get the class name
from the stored alias:

```php
class Activity
{
    // ...

    public function getEntityClass(): ?string
    {
        return Alias::classFor($this->entityAlias);
    }

    // ...
}
```

***NOTE
`Alias::classFor()` will return `null` if no class is found for the given alias.
***

## Alternative Alias Configuration

What about classes you don't control, like vendor classes? No problem! You can configure
these in your app's `composer.json` file:

```json
{
    "extra": {
        "class-metadata": {
            "map": {
                "Galactic\\Fleet\\Hyperdrive": "hyperdrive"
            }
        }
    }
}
```

## Additional Metadata

Say we don't want *all* entities to be auditable - easy, they just need to be *marked* as such.
We have a couple options:

1. Use a marker interface (an interface without methods), e.g. `AuditableInterface`, and
   have auditable entities implement it.
2. Create a custom class attribute, e.g. `#[Auditable]`, and add it to auditable entities.

These both work, but this package provides a third option: the `#[Metadata]` attribute:

```php
use Zenstruck\Metadata;

#[Metadata('auditable', true)]
class ProductCategory
{
    // ...
}
```

***IMPORTANT
Metadata is also picked up by the Composer plugin during the autoload dump process. Be sure to run:

```terminal
composer dump-autoload
```

whenever you add or change metadata.
***

Now, before logging activity, we first check if the entity is auditable:

```php
use Zenstruck\Metadata;

class ActivityManager
{
    // ...
    
    function onEntityChange(object $entity, int $entityId, string $action): void
    {
        if (!Metadata::get($entity, 'auditable')) {
            return; // not auditable
        }
    
        $this->logActivity($entity, $entityId, $action);
    }
}
```

***NOTE
`Metadata::get()` will return `null` if the given key is not found for the class or object.
***

Check out the [Metadata Lookup documentation](https://github.com/zenstruck/class-metadata/blob/1.x/README.md#metadata-lookup)
for more exciting usages!

## Listing all Aliases and Metadata

Don't have all of your configured aliases and metadata memorized? No problem, the
package's Composer plugin provides a command to list them all:

```terminal
composer list-class-metadata
```

The output looks something like this:

```
 ---------------------------- ------------------ --------------------
  Class                        Alias              Metadata
 ---------------------------- ------------------ --------------------
  App\Entity\ProductCategory   product-category   {"auditable":true}
 ---------------------------- ------------------ --------------------
```

---

Whether you need simple aliases for storage, or metadata to drive business logic, this package
keeps things fast, organized, and clean.

👉 Check out the [GitHub repo](https://github.com/zenstruck/class-metadata) to get aliasing!
