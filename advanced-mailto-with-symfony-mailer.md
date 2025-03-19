# Ditch mailto Links: Pre-Fill & even Attach Files with Symfony Mailer (.eml Magic!)

Congratulations on your new gig working on the site for "Prehistoric Pens": the leader
in dinosaur enclosures!

Each morning (after getting a soda), our sales person Dennis reviews customer requests
asking for a quote on a specific pen. To be personal (and see it in his sent messages),
Dennis copies the customer email address, composes an email in his own email client and
types out a personal message. He also finds and attaches the "specs" PDF of the pen the customer wants.

This all works fine. But darn it! Dennis is even lazier than the programmers! He wants to be
able to click a link that opens a new email in his mail client, prefilled with `to` set to the
customer's email, a subject *and* the pen's PDF attachment. All of this can be done with
`mailto:`, *except* for the attachment part.

But good news Dennis! This *is* possible! Programmers rule!

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

When Dennis clicks this link, his browser will download a `.eml` file.
Opening this will open his email client with the `to` email pre-filled and the PDF attached.
All he needs to do is add the message (written with the famous Dennis Nedry charm),
hit send and finish that tasty soda. Another successful day selling dino pens Dennis!

See the Symfony Documentation for more information about
[Draft Emails](https://symfony.com/doc/current/mailer.html#draft-emails).

And check out our full and free Symfony
[Mailer and Webhook with Mailtrap](https://symfonycasts.com/screencast/mailtrap)
course for more email goodies!

Happy emailing!
