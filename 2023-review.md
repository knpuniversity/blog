# 2023: A Year in Tutorials & Open Source

For SymfonyCon 2022, I wore a Mickey Mouse costume in front of over 1000
Symfony developers. This year, I brought my 7-year-old son Beckett. 
You all treated him like a star, made him balloon animals & helped him
build Legos. The rest of the year, I got to play with Symfony, create
tutorials and work with some of the most gifted people I know to build
open source tools that I'm extremely proud of.

I have an odd, wonderful job, entirely crowd-sourced by you lovely people.

So in the interest of openness, I wanted share what we did in 2023, my hopes
and dreams for 2024 and... some stats!

### 2023 Open Source

In 2023, I spent `636 hours` on open source, entirely sponsored
by Symfonycasts - i.e. you! The truth is that open source takes time, and time
requires money. You make this economically possible. [Victor](https://github.com/bocharsky-bw)
also worked hard on supporting our libraries, which was huge. 

My time breaks down into a few major buckets:

* [AssetMapper](https://symfonycasts.com/screencast/asset-mapper) `235 hours`
  * Bootstrapped the new component from scratch
  * Ushered in the "no build" era for Symfony!
* [Symfony UX](https://ux.symfony.com/) `143 hours`
  * 7 minor releases (`2.7` - `2.13`) with hundreds of new features
  * The `<twig:Component />` HTML syntax!
  * 3 new components: `ux-translator` and `ux-svelte`, `ux-toggle-password`
  * Added StimulusBundle
* [Live Components](https://ux.symfony.com/live-component) `135 hours`
  * Smart rendering system added
  * Much more robust hydration system & support for DTOs
  * Stabilization: the package will soon *not* be experimental
  * Deferred / lazy loading

We also created new packages!

* [symfonycasts/tailwind-bundle](https://github.com/symfonycasts/tailwind-bundle)
* [symfonycasts/sass-bundle](https://github.com/symfonycasts/sass-bundle)
* [symfonycasts/dynamic-forms](https://github.com/symfonycasts/dynamic-forms)
* [symfonycasts/micro-mapper](https://github.com/symfonycasts/micro-mapper)

None of this mentions the *real* work: maintenance, reviewing pull requests,
fixing tests, etc. I work on this, but the true stars are people from the
community & the Symfony core team. For example, check out the work from
[nicolas-grekas](https://github.com/nicolas-grekas)! Thanks to our users,
Symfonycasts is able to financially support Symfony itself in a significant
way.

### 2023 Tutorials

In 2023, we published `19.2` hours of content, which is slightly down from 2022
(`20.7` hours). That's frustrating. There is *so* much content I'd like to cover,
and we have big goals this year to streamline our processes. Personally, I'd love
to hit 30 or 40 hours. Symfony deserves that. And, fortunately, the Symfonycasts
team helps so I can focus my time.

How many tutorials were watched? Over `3100 DAYS` of content was watch last year.
That's nearly 10 years of tutorials in 1 year! The top tutorials were:

| Screencast                                                                       | Days Watched |
|----------------------------------------------------------------------------------|--------------|
| [Symfony](https://symfonycasts.com/screencast/symfony)                           | 538          |
| [ApiPlatform](https://symfonycasts.com/screencast/api-platform)                  | 239          |
| [Symfony Fundamentals](https://symfonycasts.com/screencast/symfony-fundamentals) | 185          |
| [Symfony & Doctrine](https://symfonycasts.com/screencast/symfony-doctrine)       | 155          |
| [EasyAdminBundle](https://symfonycasts.com/screencast/easyadminbundle)           | 147          |
| [Symfony Security](https://symfonycasts.com/screencast/symfony-security)         | 121          |
| [ApiPlatform Ep2](https://symfonycasts.com/screencast/api-platform-security)     | 102          |
| [Stimulus](https://symfonycasts.com/screencast/stimulus)                         | 94           |
| [Messenger](https://symfonycasts.com/screencast/messenger)                       | 88           |
| [Symfony 5](https://symfonycasts.com/screencast/symfony5)                        | 73           |

If you zoom into the final 3 months, not surprisingly LAST Stack and 
API Platform episode 3 are 6th and 8th.

How long does it take to make a tutorial? Great question! The biggest variance
depends on research. Yup, sometimes I have a lot to learn before hitting record!

***TIP
* `wh` - "work hours" - the number of hours of work spent
* `vh` - "video hour" - the number of hours of video produced

The times below are *my* hours: they don't take into account editing and
other important things I have help with.
***

* [30 Days of LAST Stack](https://symfonycasts.com/screencast/last-stack)
  `126 hours` spent / `4.1 hour` tutorial = `30.7` wh/vh
* [API Platform Ep3](https://symfonycasts.com/screencast/api-platform-extending)
  `97 hours` spent / `3.7 hour` tutorial = `26.2` wh/vh
* [API Platform Ep2](https://symfonycasts.com/screencast/api-platform-security)
  `83 hours` spent / `3.7 hour` tutorial = `22.4` wh/vh
* [API Platform Ep1](https://symfonycasts.com/screencast/api-platform)
  `58 hours` spent / `2.9 hour` tutorial = `20.4` wh/vh
* [AssetMapper](https://symfonycasts.com/screencast/asset-mapper)
  `37 hours` spent / `2.1 hour` tutorial = `17.6` wh/vh
* [Integration Testing](https://symfonycasts.com/screencast/phpunit-integration)
  `26.5 hours` spent / `1.0 hour` tutorial = `26.5` wh/vh
* [Doctrine Queries](https://symfonycasts.com/screencast/doctrine-queries)
  `25 hours` spent / `1.4 hour` tutorial = `17.8` wh/vh

The key is ``wh/vh``: how much work goes into each hour of video produced.
I have a few ideas to improve this:

* **New tutorial authoring tool**. We use a fantastic, internal tool called
  TutsHero to help us build the code for each tutorial. But it's time to
  modernize that and better integrate it with the recording process.

* **AI in Script Editing**. Fun fact: I first record rough audio while recording
  the video. We use AI to transcribe that. Then we manually clean up the
  script, reformatting code (`foo arow bar` becomes `$foo->bar()`), adding
  `ticks` around code, and rephrasing. Finally, I record this. The script
  editing this takes a significant amount of time and I think AI can help
  automate this.

What about other authors? Sure! though finding consistent, high-quality authors is
hard. If you have a passion for teaching, definitely [reach out](https://symfonycasts.com/contact)!

### 2023 Symfonycasts.com

Our team - Diego, Vladimir, Victor & Leanna - work a lot on Symfonycasts.com so
that we can have nice things... and I can focus on tutorials (Diego & Victor also
help with creating tutorials).

Out of the million different things from 2023, one stands our: our new Tailwind-powered
design. We're rolling this out slowly - but you can see it on this blog section
or any course pages. And this is more than a re-skinning: it's a chance for us
to embrace the [LAST Stack](https://symfonycasts.com/blog/last-stack) principles:
leverage Turbo, delete a lot of custom JavaScript and use Stimulus everywhere.
It's *fantastically* fun to work with.

## Comments & Questions Answered

At the end of each tutorial, I like to say:

> If you have any questions, we're here for you down in the comments.

And we mean it. In 2023, 2572 comments were posted to the site, out of which
1144 were replies from **us**. The best part? All of those questions and
answers live in public: they help everyone.

### What about 2024?

So what's next? I have a few goals for 2024:

* **More languages**. Each new tutorial's script and subtitles are already
  translated into Spanish in a high quality way thanks to AI. We should add
  more languages to make the content accessible to more people.

* **More tutorials**. We have a lot of ideas for new tutorials. We're also
  working on a new authoring tool to help us create them faster.

* **Live-streaming**. I started live-streaming in 2023 and it was fun!
  Should we keep doing it? Channel at - https://www.youtube.com/@weaverryan.

* **Videos of Ryan**. That's me! In 2024, I'd like to actually get on camera
  for the videos. We'll see how that goes :).

Got something else on your mind? Let me know in the comments!

And thank you ❤️.
