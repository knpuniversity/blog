# ¡Traducciones Automáticas de Tutoriales al Español!

Nos complace anunciar que a partir de ahora entregaremos traducciones al español de
nuestros nuevos tutoriales (guiones y subtítulos)... prácticamente al instante. ¿Cómo
lo hemos hecho posible? Podrías pensar que hemos contratado a un profesional que se
encarga de traducir el contenido. Por muy encantadores que sean los humanos, ¡no lo
hemos hecho! Como programadores, nos encanta automatizar los procesos, ¡así que eso
es exactamente lo que hicimos! Aprovechamos DeepL: un potente servicio de traducción
basado en un algoritmo de IA. Y... ¡es sorprendentemente bueno! Pero, esta vez no fue 
tan fácil como hacer una llamada a la API. No, tuvimos que personalizar mucho nuestro 
contenido para que las palabras técnicas y los bloques de código no se tradujeran.

## API DeepL

DeepL te permite envolver tu contenido con etiquetas XML para darle un significado
especial, por ejemplo, puedes envolver una palabra con una etiqueta "ignorar" en caso
de que no quieras que se traduzca, o, puedes envolver unas cuantas líneas de texto
con una etiqueta "no dividir" para manejarlas como parte de la misma frase (útil para
no perder el contexto en los subtítulos)

Para entender el reto, veamos un ejemplo de nuestro tutorial de Doctrine:

> Por ejemplo, un comando se llama \`doctrine:database:create\`. Genial, vamos a probarlo:
> \`\`\`terminal
> php bin/console doctrine:database:create
> \`\`\`

Queremos que DeepL traduzca las dos primeras frases, pero no la palabra técnica `doctrine:database:create`.
También queremos que se salte por completo el bloque del terminal... y que sólo lo
devuelva "textualmente" en la posición correcta. Ves, ¡qué difícil!

Así es como quedaría después del proceso:

```html
For example, one command is called <deepl-ignore>doctrine:database:create</deepl-ignore>. Cool, let's try it:

<deepl-code-fence lang="terminal">
php bin/console doctrine:database:create
</deepl-code-fence>
```

Para ello, tuvimos que implementar un analizador Markdown personalizado que convierte
las etiquetas MD en etiquetas XML específicas, e incluso añade atributos. En este
caso, añadió el atributo `lang="terminal"`que indica el idioma de la etiqueta del
bloque de código. Y, después de enviar un archivo de script a DeepL, ejecutamos una
especie de proceso inverso para restaurar las etiquetas XML en Markdown, además de
arreglar algunos fallos que vienen de DeepL.

Si tienes curiosidad, DeepL tiene un [simulador](https://www.deepl.com/es/docs-api/simulator/) de la
API donde puedes ver todas sus funciones. ¡Tuvimos que usarlas todas!

Por cierto, este es el aspecto de la llamada a la API

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

El objeto `deepL` utilizado en el ejemplo anterior no es más que una instancia de
esta práctica biblioteca de terceros [`babymarkt/deepl-php-lib`](https://github.com/Baby-Markt/deepl-php-lib) 
que utilizamos para comunicarnos con la API DeepL.

## DeepL Glosarios

La API también viene con un glosario en el que puedes definir un conjunto de palabras
que quieres traducir de una manera determinada. Esto es muy útil, por ejemplo, para
evitar traducir los nombres de las bibliotecas

Dato curioso: ¡_Twig_ se traduce como _Ramita_!

## Función de hacer clic en el script

Te habrás dado cuenta de que si haces clic en el texto del guión, el vídeo empezará a
reproducirse en ese momento concreto. Lo conseguimos sincronizando el guión y el
subtítulo del vídeo palabra por palabra, lo que nos permite saber cuándo se produce
cada palabra del guión con los subtítulos, por lo que es muy importante mantener
ambos archivos lo más parecidos posible. Para que esta función funcione en los cursos
traducidos, tenemos que traducir también los archivos de subtítulos. Este es el
aspecto de un archivo de subtítulos:

```terminal
WEBVTT

00:00:01.056 --> 00:00:04.996 align:middle
Rendering a template is pretty common, so
there's a shortcut when you're in a controller.

00:00:05.966 --> 00:00:09.976 align:middle
Replace all of this code with a
simple return $this->render:
```

Pero esto resultó ser un problema porque generamos las traducciones al español a
partir de la salida de un algoritmo de IA, que a veces traduce el contenido del guión
y de los subtítulos de forma ligeramente diferente. Estas ligeras diferencias rompen
la función de clicar el guión
Afortunadamente, esto es poco frecuente. Y si la función de pulsar el guión falla en
el contenido traducido, enviamos una alerta a una de las personas de confianza de
SymfonyCasts para que lo arregle.

## El `StofDoctrineExtensionsBundle` entra en juego

Una cosa es traducir con precisión un tutorial, pero ofrecer un sitio multilingüe es
algo muy... diferente. Como habrás notado, hemos traducido parcialmente el sitio al
español: verás el contenido en español sólo en los tutoriales que están traducidos.
Para ello instalamos y configuramos el `StofDoctrineExtensionsBundle` en la
aplicación. Puedes saber más sobre el comportamiento traducible [aquí](https://github.com/doctrine-extensions/DoctrineExtensions/blob/main/doc/translatable.md).

## ¿Quieres Ayudar?

Como se trata de un proceso ejecutado por máquinas, es posible que notes algunos "errores" 
o simplemente... traducciones tontas. Las máquinas aún no se han apoderado
del mundo. Pero no pasa nada, porque también hemos proporcionado una forma en la que
puedes ayudar fácilmente a mejorar o corregir un error enviando una solicitud de
extracción a los repositorios públicos de GitHub. Sólo tienes que hacer clic en el
botón "Editar en GitHub" que se encuentra en la esquina superior derecha del área de
la secuencia de comandos, editar el archivo de la secuencia de comandos (o del
título) y confirmar los cambios.

Ah, y en este momento, nos centramos sólo en la traducción del contenido al español.
Es posible que nos ampliemos a otros idiomas en el futuro, pero no inmediatamente.

¡Diviértete!
