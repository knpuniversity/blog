# Automated Tutorial Spanish Translations!

We're happy to announce that from now on we'll deliver Spanish translations to our new
tutorials (scripts and subtitles)... pretty much instantly.  How did we make this possible? You might think that
we hired a professional who takes care of translating the content. As lovely as humans are, 
we did *not* do this! As programmers, we love to automate processes, so, that's
exactly what we did! We leverage *DeepL*: a powerful translation service based on an AI algorithm.
a translation service based on an AI algorithm. And... it's surprisingly good! But, 
this time it wasn't as easy as making an API call. Nope, we had to heavily customize our 
content so that technical words and code blocks are *not* translated. We also need to help Deepl
understand some of our technical language.
the translation as accurate as possible. 

## DeepL API

DeepL allows you to wrap your content with XML tags to give it a special meaning, for example, 
you can wrap a word with an "ignore" tag in case you don't want it to be translated, or, you can 
wrap a few text lines with a "non-splitting" tag to handle them as part of the 
same sentence (useful for not losing context on the subtitles). For this purpose, we had to 
implement a custom Markdown parser that converts MD tags into specific XML tags. For example,
an inlined code block `$blocks = []` is parsed into `<dp-ignore>$blocks = []</dp-ignore>`. If you're curious,
curious, DeepL has an API [simulator](https://www.deepl.com/es/docs-api/simulator/) where you can see all of its features. 
We got to use them all!

By the way, this is how the API call to translate a script file looks like:
```php
public function translate(string $text, string $glossaryId = null): string
{
    $response = $this->deepL->translate(
        $text,
        Locale::LOCALE_DEFAULT, // source language
        Locale::LOCALE_SPANISH, // target language
        'xml', // tag handling
        static::getIgnoredTags(),
        'less', // formality
        'nonewlines', // split sentences
        1, // preserver formatting
        static::getNonSplittingTags(),
        static::getSplittingTags(),
        $glossaryId
    );

    return $response[0]['text'];
}
```

After translating the content we run a sort of inverse process for restoring the 
XML tags into Markdown, besides fixing a few flaws that comes from DeepL.

## DeepL Glossaries

The API also comes with a glossary where you can define a set of words that you want to translate
in a particular way. This is very useful, for example, to avoid translating library names. 

Fun fact: _Twig_ is translated into _Ramita_!

## Script-clicking Feature

You may have noticed that if you click on the script text, the video will start playing
at that specific moment. We manage to do that by synchronizing the script and video subtitle word-by-word, giving us the ability to know *when* each word in the script occurs.
with the subtitles, so it's very important to keep both files as similar as possible. This
turned out to be a problem because we generate the Spanish subtitles from the output
of an AI algorithm, which sometimes translates the script and identical subtitles content *slightly* differently. These slight differences break the script script-clicking feature! Fortunately, this is rare. And if the script-clicking feature fails on the translated subtitles, we send an alert to one of the trusty folks at Symfonycasts to fix it. Unfortunately, this is still a small, manual process.
on the translated subtitles, and in case of an error, we send an alert so someone from the staff
can fix it, unfortunately this latter step is still a manual process.

## The `StofDoctrineExtensionsBundle` comes in to play

It's one thing to accurately translate a tutorial, but delivering a multi-language site is quite...
it's quite a different thing to do. As you may have noticed, we partially translated the site
into Spanish: you will see Spanish content only on tutorials that are translated. For this
purpose we installed and configured the `StofDoctrineExtensionsBundle` in the application. 
You can learn more about the _translatable_ behavior [here](https://github.com/doctrine-extensions/DoctrineExtensions/blob/main/doc/translatable.md)

## Want to Help?

Since this is a process run by machines, you may notice some "misprints" or just... silly translations.
Machines haven't quite taken over the world yet. But that's ok because we've also delivered a way 
where you can easily help improve or fix an error by sending a pull request to public GitHub repositories.
Simply click the "Edit on GitHub" button which is at the right top corner of the script area, 
edit the script (or caption) file and commit the changes.

Oh, and right now, we're focused only on translating content into Spanish. We may expand to other languages in the future, but not immediately.
to other languages in the future.

Thank you for reading! And I hope you will find this helpful.
