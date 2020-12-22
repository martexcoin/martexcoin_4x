MarteX Core version 3.0.0
==========================

Release is now available from:

  <https://www.martexcoin.org/downloads/#wallets>

This is an optional release and only contains changes for testnet. It is not required to update masternodes on mainnet.

Please report bugs using the issue tracker at github:

  <https://github.com/martexcoin/martexcoin/issues>


Upgrading and downgrading
=========================

How to Upgrade
--------------

If you are running an older version, shut it down. Wait until it has completely
shut down (which might take a few minutes for older versions), then run the
installer (on Windows) or just copy over /Applications/MarteX-Qt (on Mac) or
martexd/martex-qt (on Linux).

Notable changes
===============

Fork/Reset testnet at block 4001
--------------------------------

This release is NOT required on mainnet. It is intended to be deployed on testnet and will cause a fork at block 4001.
The plan is to restart all testing for the v3.0.0 upgrade process.

When deployed on testnet, it is required to start with a fresh data directory or call MarteX Core with `-reindex-chainstate`.

Credits
=======

Thanks to everyone who directly contributed to this release,
as well as everyone who submitted issues and reviewed pull requests.


Older releases
==============

MarteX Core tree 3.0.0.x was a fork of Bitcoin Core tree 0.12.
