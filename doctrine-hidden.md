# Doctrine's HIDDEN gem

We had a great question from a user recently where the answer involved a
little-known Doctrine feature called `HIDDEN`. You won't need it often,
but it deserves its 5 in the spotlight!

`HIDDEN` allows you to select a field so that you can sort by it... but *without*
include it in the results:

```php
/** @return Voyage[] */
public function findAllOrderedByPastFirst(): array
{
    return $this->createQueryBuilder('voyage')
        ->addSelect('CASE WHEN voyage.leaveAt < :now THEN 1 ELSE 0 END AS HIDDEN isPast')
        ->orderBy('isPast', 'DESC')
        ->setParameter('now', new \DateTimeImmutable())
        ->getQuery()
        ->getResult();
}
```

The beauty of `HIDDEN` is that, to `orderBy()` something, that "something" needs
to be selected. But *without* the `HIDDEN`, instead of returning an
array of `Voyage` objects, Doctrine would suddenly return an array of arrays,
where each array has a `0` key containing the `Voyage` object and an `isPast` key.
Yikes!

Here are a few other examples shared by users:

## Ordering based on State

Ordering based on the "state" of an entity thanks to [@ker0x](https://twitter.com/ker0x/status/1744405031560871967):

```php
$this->createQueryBuilder('voyage')
    ->addSelect(
        'CASE
            WHEN voyage.state = :confirmed THEN 0
            WHEN voyage.state = :canceled THEN 1
            WHEN voyage.state = :accepted THEN 2
            WHEN voyage.state = :pending THEN 3
            ELSE 4
        END AS HIDDEN state_order'
        )
    ->setParameters([
        'confirmed' => Voyage::STATE_CONFIRMED,
        'canceled' => Voyage::STATE_CANCELED,
        'accepted' => Voyage::STATE_ACCEPTED,
        'pending' => Voyage::STATE_PENDING,
    ])
    ->orderBy('state_order', 'ASC');
```

## Ordering Null Values Last

Ordering `null` values first thanks to [@Thibault_1635](https://twitter.com/Thibault_1635/status/1744867305786384855):

```php
$this->createQueryBuilder('voyage')
    ->addSelect('CASE WHEN voyage.endAt IS NULL THEN 1 ELSE 0 END AS HIDDEN control_null_first')
    ->orderBy('control_null_first', 'DESC');
```

Have fun!
