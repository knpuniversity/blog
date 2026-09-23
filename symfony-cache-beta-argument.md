---
published: 2026-09-23
category: tech
preview: |-
  Symfony's Cache Contracts have a mysterious third argument: `$beta`. Here's where its
  name comes from, how it prevents cache stampedes, and how `beta: INF` lets you force a
  cache item to refresh without throwing away the value you already have.
---

# Force Refresh a Symfony Cache Item with beta: INF

Here's some [Symfony Cache Contracts](https://symfony.com/doc/current/cache.html#cache-basic-usage)
code you've probably written a hundred times:

```php
$value = $cache->get('weather', function (ItemInterface $item) {
    $item->expiresAfter(3600);

    return $this->weatherApi->getForecast();
});
```

Return the cached value if it exists, otherwise run the callback and save its result. But
`get()` accepts more than a key and a callback - here's the full signature:

```php
public function get(
    string $key,
    callable $callback,
    ?float $beta = null,
    ?array &$metadata = null,
): mixed;
```

That third argument - `$beta` - is one I ignored for years. It turns out to be a knob
on the algorithm Symfony uses to prevent cache stampedes. And if we turn that knob all
the way up, we get a genuinely useful application-level feature for free.

## Wait... What's `beta`?

Despite the name, this has nothing to do with beta software. The name comes from the
math behind the cache stampede prevention algorithm Symfony uses.

Symfony's implementation is based on an algorithm called **XFetch**, from the paper
[Optimal Probabilistic Cache Stampede Prevention](https://cseweb.ucsd.edu/~avattani/papers/cache_stampede.pdf).
In it, the Greek letter beta - `β` - is the parameter that controls an
exponential probability distribution used to decide how early a cached value might
be recomputed.

So `$beta` isn't just some funky Symfony parameter name. It's basically a little math
knob.

And notice that the default value is `null`:

```php
beta: null
```

That doesn't mean "disable beta". It means: **let the cache adapter choose its default
beta value**. For Symfony's standard cache adapters, that normally means a beta of
`1.0`, giving you the usual probabilistic early-expiration behavior - which is exactly
what you want most of the time, so you never need to think about this argument at all.

## Avoiding a Cache Stampede

Why does any of this exist?

Imagine a popular cache item expires at exactly 2:00 PM. At 1:59:59, everything is
great. Then 2:00 arrives and a bunch of requests hit the site at once.

Without any protection, all of them could notice:

> Hey! This cache item expired!

And suddenly they're all trying to perform the same expensive work. That's a
[**cache stampede**](https://en.wikipedia.org/wiki/Cache_stampede), also known as the
"thundering herd" problem, and Symfony's Cache Contracts have
[two built-in ways](https://symfony.com/doc/current/cache.html#preventing-cache-stampede)
to help prevent it.

Both are on by default - there's nothing to enable, and if you're using the Cache
Contracts you're already getting them.

First, there's locking. If multiple requests need to compute the same missing value,
Symfony makes sure only one does the work at a time. The others wait for it to
finish.

That's already much better than having every request hammer our database or API. But
waiting isn't ideal either, and this is where Symfony's second trick - probabilistic
early expiration - gets interesting.

Instead of waiting until the cached value is completely gone, Symfony can decide a
little early:

> I'm going to refresh this now.

That one request recomputes the value while everyone else can keep happily using the
existing cached value. Once the recomputation finishes, the fresh value replaces the
old one. No herd - and, even better, no requests waiting around for the cache to warm
back up.

The `$beta` argument controls how aggressive that early expiration is: a larger beta
makes Symfony more eager to recompute early.

Turn the knob the other way:

```php
beta: 0.0
```

and early expiration is disabled entirely. The value is then only recomputed once it
has genuinely expired. Locking still applies, so we're not completely unprotected -
but that's the "everybody waits" behavior we just talked about.

## Turning the Knob All the Way Up

So what happens if we take that math knob to infinity?

```php
$value = $cache->get(
    'weather',
    function (ItemInterface $item) {
        $item->expiresAfter(3600);

        return $this->weatherApi->getForecast();
    },
    beta: INF,
);
```

The early-expiration test always wins. In other words:

> Treat this cache item as expired and recompute it now.

The callback runs, its result is stored, and we get the fresh value back. Which makes
`beta: INF` a lovely way to say "force refresh" - though your first instinct for that
is probably to delete the key:

```php
$cache->delete('weather');

$value = $cache->get('weather', $callback);
```

That works, but it creates an actual cache miss, and the cost lands on *everyone else*.
Once we've deleted the key, the old value is gone, so every other request that needs it
while we're rebuilding is stuck waiting on that same expensive callback. Our own request
waits for the callback either way - that part is unavoidable, and usually fine, since
the request doing the refreshing is a deliberate one: an admin action, a console
command, a Messenger message.

With `beta: INF`, we're saying something more precise:

> Get this value, but force *this request* to refresh it.

Nobody else pays for our refresh: normal traffic keeps reading the existing cached
value until the new one is ready to replace it.

## A Nice "Refresh" Operation

Imagine an admin action that can optionally force some cached API data to refresh.
Instead of one code path for the normal cache lookup and another for deleting and
rebuilding, we can keep everything together:

```php
$products = $cache->get(
    'products',
    function (ItemInterface $item) {
        $item->expiresAfter(3600);

        return $this->productApi->fetchProducts();
    },
    beta: $forceRefresh ? INF : null,
);
```

When `$forceRefresh` is `false`, we pass `beta: null` and the cache adapter uses its
normal probabilistic behavior. When it's `true`, we pass `beta: INF` and Symfony
recomputes the value immediately.

`$forceRefresh` can come from wherever fits the app - a `?forceRefresh=1` query
parameter, a `--refresh-cache` console option, a button in an admin panel - just
restrict who's allowed to trigger it.

Normal request? Use the cache normally.

Explicit refresh? Recompute it now.

One argument covers both cases, and at no point does the cached value disappear out
from under everyone else. Not bad for a parameter most of us have scrolled past a
hundred times without ever wondering what it was for.

Happy cache-herd wrangling!
