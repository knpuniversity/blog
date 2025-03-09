# Advanced `mailto:` with Symfony Mailer

Consider the following scenario: you manage the website for company that
builds and sells dinosaur enclosures: "Prehistoric Pens".

There's a section for the sales team where they can review pending
quotes. They want a new feature: when they view a quote, they want to
quickly send a follow-up email to the customer.

Ok, so what about a "Send Follow-up" button that auto sends a generated email with
Symfony Mailer. We could make the `Reply-To` the currently logged in sales person
so they get replies.

They want to be able to customize the message... sure, we can add a form to fill out.

But... they want to send the email from their own email client so they can find it later
in their *sent items*.

Sweet! That's super easy! We can just create a "Send Follow-up" button that's a mailto link.
Something like:

```twig
<a href="mailto:{{ quote.customer.email }}?subject=Following up on Quote #{{ quote.id }}">
    Send Follow-up
</a>
```

But... they want the email to include the quote PDF as an attachment... I... don't think you
can do this with a `mailto:` link.

We can do this with Symfony Mailer (well actually, the underlying Mime package that Mailer uses)
and a controller:

```php
use Symfony\Component\Mime\DraftEmail;

#[Route('/quotes/{id}/followup-email', name: 'quote_followup_email')]
public function downloadFollowupEmail(Quote $quote): Response
{
    $pdf = $quote->getFile();

    $email = (new DraftEmail())
        ->to($quote->getCustomer()->getEmail())
        ->subject('Following up on Quote #'.$quote->getId())
        ->html('Hi there! Just following up on the quote we sent you.')
        ->attach($pdf->read(), $pdf->path()->name(), $pdf->mimeType())
    ;

    $response = new Response($email->toString());
    $contentDisposition = $response->headers->makeDisposition(ResponseHeaderBag::DISPOSITION_ATTACHMENT, $quote->getId().'.eml');
    
    $response->headers->set('Content-Type', 'message/rfc822');
    $response->headers->set('Content-Disposition', $contentDisposition);
    
    return $response;
}
```

Let's unpack this:
1. We create an email like normal... but using a `DraftEmail` object. This
   is a special kind of email that cannot be sent with Mailer directly.
2. Add the Quote PDF attachment also like normal.
3. We then send an *attachment* response. The content is the *stringified*
   email. The content type is `message/rfc822` which is the MIME type for an email
   (`.eml` file).

Now, on our quote page, instead of a `mailto:` link, we can link to this controller:

```twig
<a href="{{ path('quote_followup_email', {id: quote.id}) }}">
    Send Follow-up
</a>
```

When the sales person clicks this link, their browser will download an `.eml` file.
Opening this will open their email client with the email pre-filled and the PDF attached.
They can adjust the message, maybe add their signature, and send it off!

See the Symfony Documentation for more information about
[Draft Emails](https://symfony.com/doc/current/mailer.html#draft-emails).

Check out our full and free Symfony
[Mailer and Webhook with Mailtrap](https://symfonycasts.com/screencast/mailtrap)
course for more email fun!

Happy emailing!
