*After branching off for a major version release of MarteXcoin Core, use this
template to create the initial release notes draft.*

*The release notes draft is a temporary file that can be added to by anyone. See
[/doc/developer-notes.md#release-notes](/doc/developer-notes.md#release-notes)
for the process.*

*Create the draft, named* "*version* Release Notes Draft"
*(e.g. "4.0.4 Release Notes Draft")*

*Before the final release, move the notes back to this git repository.*

*version* Release Notes Draft
===============================

MarteXcoin Core version *version* is now available from:

  <https://github.com/martexcoin/martexcoin/releases>

This release includes new features, various bug fixes and performance
improvements, as well as updated translations.

Please report bugs using the issue tracker at GitHub:

  <https://github.com/martexcoin/martexcoin/issues>

To receive security and update notifications, please subscribe to:

  <>

How to Upgrade
==============

If you are running an older version, shut it down. Wait until it has completely
shut down (which might take a few minutes for older versions), then run the
installer (on Windows) or just copy over /Applications/MarteX-Qt (on Mac) or
martexd/martex-qt (on Linux).

Upgrading directly from a version of MarteXcoin Core that has reached its EOL is
possible, but it might take some time if the data directory needs to be migrated. Old
wallet versions of MarteXcoin Core are generally supported.

Compatibility
==============

MarteXcoin Core is supported and extensively tested on operating systems
using the Linux kernel, macOS 10.14+, and Windows 7 and newer.  MarteXcoin
Core should also work on most other Unix-like systems but is not as
frequently tested on them.  It is not recommended to use MarteXcoin Core on
unsupported systems.

From MarteXcoin Core 3.0.0 onwards, macOS versions earlier than 10.14 are no
longer supported. Additionally, MarteXcoin Core does not yet change appearance
when macOS "dark mode" is activated.

Notable changes
===============

P2P and network changes
-----------------------

Updated RPCs
------------

Changes to Wallet or GUI related RPCs can be found in the GUI or Wallet section below.

New RPCs
--------

Build System
------------

New settings
------------

Updated settings
----------------

Changes to Wallet or GUI related settings can be found in the GUI or Wallet section below.

- Steps to follow in how to document the source code.  (#45)

Tools and Utilities
-------------------

Wallet
------

GUI changes
-----------

Low-level changes
=================

RPC
---

Tests
-----

Credits
=======

Thanks to everyone who directly contributed to this release:


As well as to everyone that helped with translations on
[Transifex](https://www.transifex.com/martexcoin/martexcoin/).
