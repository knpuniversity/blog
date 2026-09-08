---
title: 'Mailtrap Tutorial (Free) = Mailer + Webhook + Contributing back to Symfony'
published: 2025-03-01
category: tech
preview: |-
    The free Mailtrap tutorial is here! It's all about the Mailer & Webhook
    components with the joy of previewing emails in a sandbox. As a bonus,
    this tutorial triggered several contributions back to Symfony!
---

# Mailtrap Tutorial = (Free) Mailer + Webhook + Contributing back to Symfony  

Good news Symfony peeps: we now have an updated Mailer tutorial that also covers the Webhook component *and* - thanks to a sponsorship from your friendly neighborhood [Mailtrap](https://mailtrap.io/) friends - is free!

We use Mailtrap (which is amazing btw) to get a total refresher on using Mailer: it’s like the original tutorial, but updated and with extra goodies.

## Why Mailtrap?  

With Mailtrap, sending emails can be both fun in development and simple & reliable on production.

Testing emails **shouldn't suck**. Real SMTP servers? Clunky. Sending test emails? Messy. Spamming real users? Disaster.  

Mailtrap gives you a **sandboxed email testing environment** — send, preview, inspect, and even check spam scores in one place.  

## Mailer & Webhook Superpowers  

We’re going deep on Symfony’s **Mailer**: queueing, Twig templates, and more. But this time we go further: **Webhooks.**  

Webhooks let us react in real-time when emails are delivered, opened, or bounced, which is important because if you keep trying to send emails to a bouncing address, it hurts your domain’s SPAM reputation. Boo!

## Contributing Back to Symfony  

While integrating Mailtrap, we hit a few limitations in Symfony’s **Webhook component** — so we **fixed them**! In fact there were several improvements:

- Mailtrap Bridge PR: https://github.com/symfony/symfony/pull/58252
- Mailtrap Bridge Webhook PR: https://github.com/symfony/symfony/pull/58403
- Bulk webhook event parsing PR: https://github.com/symfony/symfony/pull/58248
- SendGrid's bridge bulk webhook event parsing PR: https://github.com/symfony/symfony/pull/58401

Those improvements are now part of Symfony core.  

Big win for the community. Big win for you.  

## Get Started Right Now

The free tutorial is fully released and ready for you: https://symfonycasts.com/screencast/mailtrap

Happy coding! 🚀
