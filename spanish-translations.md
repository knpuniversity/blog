# Automated Tutorial Spanish Translations!

We're happy to announce that from now on we'll deliver Spanish translations to our new
tutorials (scripts and subtitles)... pretty much instantly.  How did we make this possible? You might think that
we hired a professional who takes care of translating the content. As lovely as humans are, 
we did *not* do this! As programmers, we love to automate processes, so, that's
exactly what we did! We leverage *DeepL*: a powerful translation service based on an AI algorithm. 
And... it's surprisingly good! But, this time it wasn't as easy as making an API call. 
Nope, we had to heavily customize our content so that technical words and code blocks 
are *not* translated.

## DeepL API

DeepL allows you to wrap your content with XML tags to give it a special meaning, for example, 
you can wrap a word with an "ignore" tag in case you don't want it to be translated, or, you can 
wrap a few text lines with a "non-splitting" tag to handle them as part of the 
same sentence (useful for not losing context on the subtitles). 

To understand the challenge, let's look at an example from our Doctrine tutorial:

> For example, one command is called `doctrine:database:create`. Cool, let's try it:
> ```terminal
> php bin/console doctrine:database:create
> ```

We want DeepL to translate the first two sentences, but not the technical word `doctrine:database:create`. 
We also want it to skip the terminal block entirely... and to just return it "verbatim" 
in the correct position. See, tricky!

This is how it would look like after the process:

```html
For example, one command is called <deepl-ignore>doctrine:database:create</deepl-ignore>. Cool, let's try it:

<deepl-code-fence lang="terminal">
php bin/console doctrine:database:create
</deepl-code-fence>
```

For this purpose, we had to implement a custom Markdown parser that converts MD tags into
specific XML tags, and it even adds attributes. In this case, it added the `lang="terminal"`
attribute that tells the language of the code block tag. And, after sending a script file to DeepL,
we run a sort of inverse process for restoring the XML tags into Markdown, besides 
fixing a few flaws that comes from DeepL.

If you're curious, DeepL has an API [simulator](https://www.deepl.com/es/docs-api/simulator/) 
where you can see all of its features. We got to use them all!

By the way, this is how the API call looks like:

```php
public function translate(string $text, string $glossaryId = null): string
{
    $ignoredTags = ['deepl-ignore', 'deepl-code-fence'];
    $nonSplittingTags = ['deepl-strike', 'deepl-subtitles-cue'];
    $splittingTags = ['deepl-split', 'deepl-list'];
    $response = $this->deepL->translate(
        $text,
        Locale::LOCALE_DEFAULT, // source language
        Locale::LOCALE_SPANISH, // target language
        'xml', // tag handling
        $ignoredTags,
        'less', // formality
        'nonewlines', // split sentences
        1, // preserver formatting
        $nonSplittingTags,
        $splittingTags,
        $glossaryId
    );

    return $response[0]['text'];
}
```

The `deepL` object used in the previous example is just an instance of this handy
third party library [`babymarkt/deepl-php-lib`](https://github.com/Baby-Markt/deepl-php-lib) that
we use to communicate with the DeepL API.

## DeepL Glossaries

The API also comes with a glossary where you can define a set of words that you want to translate
in a particular way. This is very useful, for example, to avoid translating library names. 

Fun fact: _Twig_ is translated into _Ramita_!

## Script-clicking Feature

You may have noticed that if you click on the script text, the video will start playing
at that specific moment. We manage to do that by synchronizing the script and video subtitle word-by-word, 
giving us the ability to know *when* each word in the script occurs with the subtitles, so it's 
very important to keep both files as similar as possible. So, for making this feature to work
on translated courses we need to translate the subtitles files as well. This is how a
subtitles file look like:

```
WEBVTT

00:00:01.056 --> 00:00:04.996 align:middle
Rendering a template is pretty common, so
there's a shortcut when you're in a controller.

00:00:05.966 --> 00:00:09.976 align:middle
Replace all of this code with a
simple return $this->render:
```

But this turned out to be a problem because we generate the Spanish translations from the 
output of an AI algorithm, which sometimes translates the script and subtitles 
content *slightly* differently. These slight differences break the script script-clicking feature! 
Fortunately, this is rare. And if the script-clicking feature fails on the translated content, 
we send an alert to one of the trusty folks at SymfonyCasts to fix it.

## The `StofDoctrineExtensionsBundle` comes in to play

It's one thing to accurately translate a tutorial, but delivering a multi-language site is quite...
a different thing to do. As you may have noticed, we partially translated the site
into Spanish: you will see Spanish content only on tutorials that are translated. For this
purpose we installed and configured the `StofDoctrineExtensionsBundle` in the application. 
You can learn more about the _translatable_ behavior [here](https://github.com/doctrine-extensions/DoctrineExtensions/blob/main/doc/translatable.md)

## Want to Help?

Since this is a process run by machines, you may notice some "misprints" or just... silly translations.
Machines haven't quite taken over the world yet. But that's ok because we've also delivered a way 
where you can easily help improve or fix an error by sending a pull request to public GitHub repositories.
Simply click the "Edit on GitHub" button which is at the right top corner of the script area, 
edit the script (or caption) file and commit the changes.

Oh, and right now, we're focused only on translating content into Spanish. We may expand to other 
languages in the future, but not immediately.

Have fun!
