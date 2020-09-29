Release Notes
=============

MarteXcoin Core version  is now available from:

  <>

This minor release includes various bug fixes and performance
improvements, as well as updated translations.

Please report bugs using the issue tracker at GitHub:

  <https://github.com/martexcoin/martexcoin/issues>

To receive security and update notifications, please subscribe to:

  <>

How to Upgrade
==============

If you are running an older version, shut it down. Wait until it has completely
shut down (which might take a few minutes in some cases), then run the
installer (on Windows) or just copy over `/Applications/coin-Qt` (on Mac)
or `coind`/`coin-qt` (on Linux).

Upgrading directly from a version of coin Core that has reached its EOL is
possible, but it might take some time if the data directory needs to be migrated. Old
wallet versions of coin Core are generally supported.

Compatibility
==============

MarteXcoin Core is supported and extensively tested on operating systems
using the Linux kernel, macOS 10.12+, and Windows 7 and newer.  Bitcoin
Core should also work on most other Unix-like systems but is not as
frequently tested on them.  It is not recommended to use Bitcoin Core on
unsupported systems.

From MarteXcoin Core 0.20.0 onwards, macOS versions earlier than 10.12 are no
longer supported. Additionally, MarteXcoin Core does not yet change appearance
when macOS "dark mode" is activated.

Known Bugs
==========

The process for generating the source code release ("tarball") has changed in an
effort to make it more complete, however, there are a few regressions in
this release:

- The generated `configure` script is currently missing, and you will need to
  install autotools and run `./autogen.sh` before you can run
  `./configure`. This is the same as when checking out from git.

- Instead of running `make` simply, you should instead run
  `COIN_GENBUILD_NO_GIT=1 make`.

Notable changes
===============


version change log
==================

Credits
=======

Thanks to everyone who directly contributed to this release:

Mention names of people or github profle

As well as to everyone that helped with translations on
[Transifex](https://www.transifex.com).
