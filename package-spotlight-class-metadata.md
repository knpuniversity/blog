# Package Spotlight: `zenstruck/class-metadata`

Consider an *auditing* feature in your app. You want to track activity including
changes to all your entities. Your `activity` table includes a reference to the
entity (the entity class name) and the entity's ID. This enables you to later
retrieve and display:

1. All activity
2. Activity for a specific entity class
3. Activity for a specific entity

Storing the class name in the database has a few downsides:

1. Renaming the class requires a database migration to update potentially 1000's of rows.
2. This name isn't human-readable. When displaying activity, it'd be nice to display a more
   human-friendly name.

Ok, what about showing the short name of the class? `App\Entity\ProductCategory` could be
`ProductCategory` or even converted to kebab-case: `product-category`. The problem here is you
can't go the other way: from `product-category` back to `App\Entity\ProductCategory` to be able
to query the database for *all* `product-category` activity or *all* `product-category`
with an ID of `27` activity.

## Enter `zenstruck/class-metadata`

This package enables you to add an `#[Alias]` attribute to any class in your app.
