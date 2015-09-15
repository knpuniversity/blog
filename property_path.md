# Symfony's Under-Used property_path option

Though not required, we typically, bind Symfony form objects to a class (often
an entity). This means that the field names must correspond with the property names
on the class (well actually, the getter/setter methods, via the
[PropertyAccess](http://symfony.com/doc/current/components/property_access/introduction.html)
component):

```php
// ...

class ProductFormType extends AbstractType
{
    public function buildForm(FormBuilderInterface $builder, array $options)
    {
        $builder
            ->add('name', 'text')
            ->add('price', 'number');
    }

    public function configureOptions(OptionsResolver $resolver)
    {
        $resolver->setDefault('data_class', 'AppBundle\Model\Product');
    }

    // ...
}
```

Since this has `name` and `price` fields, the `Product` class *must* also have the
methods `getName()`, `setName()`, `getPrice()` and `setPrice()`:

```php
// ...

class Product
{
    private $name;
    private $price;

    public function getName()
    {
        return $this->name;
    }
    public function setName($name)
    {
        $this->name = $name;
    }
    public function getPrice()
    {
        return $this->price;
    }
    public function setPrice($price)
    {
        $this->price = $price;
    }
}

```

## The Magic: property_path Option

But the mapping *doesn't* actually use the field's name (e.g. `price`) to find the
getters/setters. In the background, each field has a [property_path](http://symfony.com/doc/current/reference/forms/types/form.html#property-path)
option, which defaults to the field name. This means that the field names could
be something entirely different than the property name, as long as the `property_path`
is set:

```
// ProductFormType.php
// ...

public function buildForm(FormBuilderInterface $builder, array $options)
{
    $builder
        ->add('productName', 'text', [
            'property_path' => 'name'
        ])
        ->add('mainPrice', 'number', [
            'property_path' => 'price'
        ]);
}
```

When would this be useful? Suppose the `Product` object has a related `Category`
object:

```
class Product
{
    // ...

    /**
     * @var Category
     */
    private $category;

    public function getCategory()
    {
        return $this->category;
    }
    public function setCategory(Category $category)
    {
        $this->category = $category;
    }
}
```

What if you want the user to be able to edit the category's `title` property, right
inside the Product form? That can be done by using [embedded forms](http://symfony.com/doc/current/book/forms.html#embedded-forms).
Or, you can leverage `property_path`:

```php
// ProductFormType.php
// ...

public function buildForm(FormBuilderInterface $builder, array $options)
{
    $builder
        ->add('name', 'text')
        ->add('price', 'number')
        ->add('categoryTitle', 'text', [
            'property_path' => 'category.title'
        ]);
}
```

The key is `category.title`, which leverages the PropertyAccess component and means
that `$product->getCategory()->getTitle()` and `$product->getCategory()->setTitle()`
are called in the background when the form is loaded and saved.

Now, note that this won't *change* the Category from one to another: you're actually
modifying the title of the same object. You also need to make sure that the Product
has a Category, or else the PropertyAccess component can't call down the path. But, depending
on your situation, this might be the perfect (and simplest) solution.

Cheers!
