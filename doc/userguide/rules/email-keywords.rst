Email Keywords
==============

.. role:: example-rule-emphasis

email.from
----------

Matches the MIME ``From`` field of an email.

Comparison is case-sensitive.

Syntax::

 email.from; content:"<content to match against>";

``email.from`` is a 'sticky buffer' and can be used as a ``fast_pattern``.

This keyword maps to the EVE field ``email.from``

Example
^^^^^^^

Example of a signature that would alert if a packet contains the MIME field ``from`` with the value ``toto <toto@gmail.com>``

.. container:: example-rule

  alert smtp any any -> any any (msg:"Test mime email from"; :example-rule-emphasis:`email.from; content:"toto <toto@gmail.com>";` sid:1;)

email.subject
-------------

Matches the MIME ``Subject`` field of an email.

Comparison is case-sensitive.

Syntax::

 email.subject; content:"<content to match against>";

``email.subject`` is a 'sticky buffer' and can be used as a ``fast_pattern``.

This keyword maps to the EVE field ``email.subject``

Example
^^^^^^^

Example of a signature that would alert if a packet contains the MIME field ``subject`` with the value ``This is a test email``

.. container:: example-rule

  alert smtp any any -> any any (msg:"Test mime email subject"; :example-rule-emphasis:`email.subject; content:"This is a test email";` sid:1;)

email.to
--------

Matches the MIME ``To`` field of an email.

Comparison is case-sensitive.

Syntax::

 email.to; content:"<content to match against>";

``email.to`` is a 'sticky buffer' and can be used as a ``fast_pattern``.

This keyword maps to the EVE field ``email.to``

Example
^^^^^^^

Example of a signature that would alert if a packet contains the MIME field ``to`` with the value ``172.16.92.2@linuxbox``

.. container:: example-rule

  alert smtp any any -> any any (msg:"Test mime email to"; :example-rule-emphasis:`email.to; content:"172.16.92.2@linuxbox";` sid:1;)
