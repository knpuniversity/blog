# Keeping Doctrine Entities Honest

> Your Doctrine entities are lying to you!

For years, the standard way to build Doctrine entities in Symfony has looked
something like this (and it's still what MakerBundle generates today):

```php
#[ORM\Entity]
class ConferenceTalk
{
    #[ORM\Id]
    #[ORM\GeneratedValue]
    #[ORM\Column]
    private ?int $id = null;

    #[Assert\NotBlank]
    #[ORM\Column(length: 255)]
    private ?string $title = null;

    #[ORM\Column(type: Types::TEXT, nullable: true)]
    private ?string $abstract = null;

    public function getId(): ?int
    {
        return $this->id;
    }

    public function getTitle(): ?string
    {
        return $this->title;
    }

    public function setTitle(?string $title): static
    {
        $this->title = $title;

        return $this;
    }

    public function getAbstract(): ?string
    {
        return $this->abstract;
    }

    public function setAbstract(?string $abstract): static
    {
        $this->abstract = $abstract;

        return $this;
    }
}
```

At first glance, this looks perfectly reasonable.

The `title` field is required. We know that because it has a
`NotBlank` constraint and the database column is not nullable.

But look closer.

```php
private ?string $title = null;

public function setTitle(?string $title): static
```

According to the PHP type system, the title is optional. In fact, the public
API of this class explicitly allows us to set it to `null`.

That means this is perfectly valid:

```php
$talk = new ConferenceTalk();
```

And so is this:

```php
$talk = new ConferenceTalk();
$talk->setTitle(null);
```

Both objects represent a conference talk that can never be successfully
persisted.

Eventually, Doctrine catches the problem:

```php
$talk = new ConferenceTalk();

$entityManager->persist($talk);
$entityManager->flush(); // boom!
```

> **SQLSTATE[23000]: Integrity constraint violation: 1048 Column 'title' cannot be null**

The database knows that a conference talk must have a title.

Our entity does not.

So why do we build entities this way?

Historically, this pattern was optimized for simplicity.

A single object could be used everywhere:

- Render a form
- Receive submitted data
- Validate that data
- Persist it to the database

For beginners, this is fantastic. There is very little ceremony, very little
boilerplate, and very little to learn before you can build working CRUD
screens.

The tradeoff is that entities end up serving two purposes.

They represent persisted application data, but they also act as form models.
Because user input is often incomplete or invalid, entities need to be able
to temporarily hold data that should never actually exist in the database.

That's why we end up with a seemingly contradictory combination:

```php
#[Assert\NotBlank]
private ?string $title = null;
```

The validator says the field is *required*.

The PHP type says it's *optional*.

But what if our entities didn't need to act as form models at all?

What if incomplete user input lived somewhere else?

## Giving Incomplete Data Somewhere Else to Live

Validation is not the problem.

The problem is that we're asking our entity to be two different things at the
same time:

- A valid `ConferenceTalk` entity
- A form data model (a temporary container for incomplete user input)

So let's split those responsibilities.

First, create a DTO (Data Transfer Object) that represents the form data:

```php
final class ConferenceTalkDto
{
    #[Assert\NotBlank]
    public ?string $title = null;

    public ?string $abstract = null;
}
```

Unlike our entity, the DTO is allowed to be incomplete.

That's its job. It represents data coming from the outside world before we've
proven that it's valid.

Our entity can finally tell the truth:

```php
#[ORM\Entity]
class ConferenceTalk
{
    #[ORM\Id]
    #[ORM\GeneratedValue]
    #[ORM\Column]
    private ?int $id = null;

    #[ORM\Column(length: 255)]
    private string $title;

    #[ORM\Column(type: Types::TEXT, nullable: true)]
    private ?string $abstract = null;

    public function __construct(string $title)
    {
        $this->title = $title;
    }

    public function getTitle(): string
    {
        return $this->title;
    }

    public function getAbstract(): ?string
    {
        return $this->abstract;
    }

    public function setAbstract(?string $abstract): void
    {
        $this->abstract = $abstract;
    }
}
```

Notice what's changed.

The required `title` is no longer nullable and must be provided when the
entity is created:

```php
public function __construct(string $title)
{
    $this->title = $title;
}
```

The optional `abstract` remains nullable, and does not have a constructor argument,
because it truly is optional.

Now the entity's API matches its actual requirements. If you want a
`ConferenceTalk`, you must provide a title:

```php
$talk = new ConferenceTalk('Keeping Doctrine Entities Honest');
```

At this point, the PHP type system, the entity, and the database all agree on
what is required and what is optional.

We've eliminated an entire category of impossible states.

A `ConferenceTalk` can no longer exist without a title.

The natural objection is: 

> Great, but now I have two classes. Do I need to
> manually copy data between them?

Historically, yes.

And that boilerplate is one of the reasons many Symfony applications have
continued using entities directly with forms and validation.

This is where [Symfony's ObjectMapper component](https://symfony.com/doc/current/object_mapper.html)
([added in 7.3](https://symfony.com/blog/new-in-symfony-7-3-objectmapper-component)) changes the tradeoff.

Instead of manually creating entities from DTOs:

```php
$talk = new ConferenceTalk($dto->title);

$talk->setAbstract($dto->abstract);
```

we can let `ObjectMapper` do the transformation for us:

```php
$talk = $objectMapper->map($dto, ConferenceTalk::class);
```

The DTO remains flexible enough to receive and validate user input. The
entity remains strict enough to represent valid application data.
`ObjectMapper` bridges the gap between the two.

In other words:

- The DTO receives and *validates* user input
- The entity represents *valid* application data
- `ObjectMapper` moves data between them

Each object has a single responsibility, and each object tells the truth
about what it represents.

## What Does This Look Like in Practice?

The nice thing about this approach is that it doesn't require a massive
rewrite. In my example application, only five things changed:

1. Add a DTO
2. Update the form to map to the DTO instead of the entity
3. Make the entity requirements honest
4. Update the create controller
5. Update the edit controller

### 1. Add a DTO

First, I created a new `ConferenceTalkDto` class:

```php
// src/Dto/ConferenceTalkDto.php

final class ConferenceTalkDto
{
    #[Assert\NotBlank]
    public ?string $title = null;

    public ?string $abstract = null;
}
```

This class is now responsible for receiving user input and holding invalid
data during validation.

### 2. Update the Form

The form barely changes.

Before:

```php
// src/Form/ConferenceTalkType.php

public function configureOptions(OptionsResolver $resolver): void
{
    $resolver->setDefaults([
        'data_class' => ConferenceTalk::class,
    ]);
}
```

After:

```php
// src/Form/ConferenceTalkType.php

public function configureOptions(OptionsResolver $resolver): void
{
    $resolver->setDefaults([
        'data_class' => ConferenceTalkDto::class,
    ]);
}
```

That's it.

The form now hydrates the DTO instead of the entity.

### 3. Make the Entity Honest

With validation moved to the DTO, the entity can finally express what is
actually required.

Before:

```php
#[Assert\NotBlank]
#[ORM\Column(length: 255)]
private ?string $title = null;

// ...
public function setTitle(?string $title): void // (nullable property)
{
    $this->title = $title;
}

public function getTitle(): ?string // (nullable return type)
{
    return $this->title;
}
```

After:

```php
#[ORM\Column(length: 255)]
private string $title;

public function __construct(string $title)
{
    $this->title = $title;
}

// ...
public function setTitle(string $title): void // (non-nullable property)
{
    $this->title = $title;
}

public function getTitle(): string // (non-nullable return type)
{
    return $this->title;
}
```

The title is no longer nullable because a conference talk without a title
should never exist.

### 4. Update the Create Controller

This is where ObjectMapper comes in.

Before, the form populated the entity directly:

```php
$talk = new ConferenceTalk();

$form = $this->createForm(
    ConferenceTalkType::class,
    $talk,
);

$form->handleRequest($request);

if ($form->isSubmitted() && $form->isValid()) {
    $entityManager->persist($talk);
    $entityManager->flush();
}
```

Now the form populates the DTO:

```php
$dto = new ConferenceTalkDto();

$form = $this->createForm(
    ConferenceTalkType::class,
    $dto,
);

$form->handleRequest($request);

if ($form->isSubmitted() && $form->isValid()) {
    $talk = $objectMapper->map(
        $dto,
        ConferenceTalk::class,
    );

    $entityManager->persist($talk);
    $entityManager->flush();
}
```

### 5. Update the Edit Controller

Editing existing entities works too.

Before:

```php
$form = $this->createForm(
    ConferenceTalkType::class,
    $talk,
);

$form->handleRequest($request);

if ($form->isSubmitted() && $form->isValid()) {
    $entityManager->flush();
}
```

After:

```php
$dto = $objectMapper->map(
    $talk,
    ConferenceTalkDto::class,
);

$form = $this->createForm(
    ConferenceTalkType::class,
    $dto,
);

$form->handleRequest($request);

if ($form->isSubmitted() && $form->isValid()) {
    $objectMapper->map(
        $dto,
        $talk,
    );

    $entityManager->flush();
}
```

Notice what's happening.

For creation, `ObjectMapper` creates a new entity from the DTO.

For editing, `ObjectMapper` first creates a DTO from the entity so the form
has something to edit. Once validation succeeds, it maps the DTO back onto
the existing entity.

The pattern remains consistent:

- Forms work with DTOs
- Validation runs on DTOs
- Entities remain valid application data

Most importantly, we no longer have a period of time where a
`ConferenceTalk` exists without a title!

## ObjectMapper Changes the Equation

For years, we've accepted a compromise.

We allow entities to exist in invalid states because they also need to act as
form models. We make required properties nullable. We rely on validation to
enforce requirements that our type system cannot express.

The tradeoff made sense when entities doubled as form models.

But ObjectMapper changes the equation.

For the first time, Symfony has a first-class way to separate user input from
persisted entities without introducing a bunch of manual mapping code.

That allows each object to focus on a single responsibility:

- DTOs receive and validate user input
- Entities represent valid application data

The result is a model where required data is actually required, optional data
is actually optional, and entities can no longer be created in impossible
states.

At least to me, that feels like a cleaner approach.

I'm curious what others think.

Should MakerBundle generate DTOs by default? Should `make:crud` offer a
DTO/ObjectMapper workflow? Have you already been using a pattern like this in
your applications?

Maybe it's time for our entities to start telling the truth.

What do you think? Let us know in the comments!
