# Fun with Symfony's Console Component

One of the best parts of using Symfony's Console component is all the output
control you have to the CLI: colors, tables, progress bars etc. Usually, you create
a command to do this. But what you may *not* know is that you can get to all
this goodness in a single, flat PHP file

## Using ConsoleOutput in a Flat File

Suppose you've started a new project and want to write a CLI script with some added
flare: colors, tables and a progress part. First, just grab the `symfony/console`
component:

```bash
composer require symfony/console
```

Next, open up a flat php file, require the autoloader and create a new
`ConsoleOutput` object:

```php
// print_pretty.php

require __DIR__.'/vendor/autoload.php';

use Symfony\Component\Console\Output\ConsoleOutput;
use Symfony\Component\Console\Formatter\OutputFormatter;

$output = new ConsoleOutput();
$output->setFormatter(new OutputFormatter(true));

$output->writeln(
    'The <comment>console</comment> component is'
    .' <bg=magenta;fg=cyan;option=blink>sweet!</bg=magenta;fg=cyan;option=blink>'
);
```

The `ConsoleOutput` class is the heart of all the styling control. And when used
with the `OutputFormatter`, you're able to print text with built-in styles for
`<comment>`, `<info>`, `<error>` and `<question>`. Or, you can control the background
and foreground colors manually.

Ok, let's run this from the command line!

```bash
php print_pretty.php
```

<img style="width: 100%;" src="/images/console/print_color.gif" alt="Print with Color" />

That's right, it's blinking. You can also wrap up your custom colorings and options
(like our blinking cyan section) into a new style (e.g. `<annoying_blinking>`).
See [OutputFormatter](https://github.com/symfony/symfony/blob/2.8/src/Symfony/Component/Console/Formatter/OutputFormatter.php)
and [OutputFormatterStyle](https://github.com/symfony/symfony/blob/2.8/src/Symfony/Component/Console/Formatter/OutputFormatterStyle.php)
for details.

## How about Table and a Progress Bar?

With the `ConsoleOutput` object, using any of the other
[Console Helpers](http://symfony.com/doc/current/components/console/helpers/index.html)
is easy! Let's build a table with a progress bar that shows how things are going:

```php
// print_pretty.php

// ... all the other code from before

$rows = 10;
$progressBar = new ProgressBar($output, $rows);
$progressBar->setBarCharacter('<fg=magenta>=</fg=magenta>');
$progressBar->setProgressCharacter("\xF0\x9F\x8D\xBA");

$table = new Table($output);
for ($i = 0; $i<$rows; $i++) {
    $table->addRow([
        sprintf('Row <info># %s</info>', $i),
        rand(0, 1000)
    ]);
    usleep(300000);

    $progressBar->advance();
}

$progressBar->finish();
$output->writeln('');
$table->render();
```

Moment of truth:

```bash
php print_pretty.php
```

Check out the animated gif below!

***TIP
Yes, that is a beer icon :). You can find more icons [here](http://www.fileformat.info/info/unicode/block/miscellaneous_symbols_and_pictographs/list.htm).
Just use the UTF-8 value - `e.g. 0xF0 0x9F 0x8D 0xBA` or `f09f8dba` and put it into
the `\xF0\x9F\x8D\xBA` format.
***

That's just another trick hidden inside Symfony. If you want to learn more, check
out our entire [Symfony track](http://knpuniversity.com/tracks/symfony)!

Have fun!

<img style="width: 100%;" src="/images/console/full.gif" alt="With table and progress bar" />
