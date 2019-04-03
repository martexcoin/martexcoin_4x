MarteX [MXT] integration/staging tree
=====================================

![MarteXcoin](https://raw.githubusercontent.com/martexcoin/martexcoin/master/src/qt/res/images/splash.png)

### Change log
2.7.4.1
- Fix

2.7.4.0
- New reward to 2.5 MXT after block 1593000/350000 main/testnet
- New Supply to 11600000
- New CLIENT_NAME to "MarteX Core"

2.7.3.0
- New coinbase maturity, disconect old versions

2.7.2.1
- Wait for full synchronization to use getblocktemplate.
- Corrected getblocktemplate coinbasevalue output

2.7.2.0
- Foundation rewards after unixtimestamp 1534291200(15 Ago 2018 00:00:00 GMT)
- Remove reference CBitcoinAddress
- Fix compile error: /usr/bin/ld: cannot find -lz on debian 8 or 9
- Fix ports
- Add raw transaction hex to gettransaction wallet RPC call and return Object blockToJSON
- Fix compile error on debian 8 or 9 in libssl-dev
- Fix macOS 10.12 build warning
- Retarget using DarkGravityWave v3 (testnet)
- RPC: repairwallet removes orphan related records from the wallet

2.7.1.0
- fix reward pow masternode
- fix reward pow masternode, version

2.7.0.0
- Implemented soft lock for preventing masternode funds to be used in transactions (-masternodesoftlock)
- Implement CHECKLOCKTIMEVERIFY/CHECKSEQUENCEVERIFY
- Re-encoding the PoW blocks
- Generate masternode.conf on initial wallet start
- Add RpcCall getstakereport
- Remove unused ThreadSafeAskFee

2.6.2.6
- Disable temporarily velocity system

2.6.2.5
- Fix velocity time from block 32000
- Fix Corrected Velocity timestamp logic handling

2.6.2.4
- Fix the block space from block 22500
- Rehabilitate velocity from block 22500
- Fix rpc masternode status

Bootstrap
---------
- https://martexcoin.org/releases/bootstrap.zip

Blockchain
---------
- https://martexcoin.org/releases/blockchain-latest.zip (updated daily)


Official Site https://www.martexcoin.org

What is the MarteX [MXT] Blockchain?
---------------------------

The MarteX [MXT] Blockchain is an experimental smart contract platform protocol that enables 
instant payments to anyone, anywhere in the world in a private, secure manner. 
MarteX [MXT] uses peer-to-peer blockchain technology developed by Bitcoin to operate
with no central authority: managing transactions, execution of contracts, and 
issuing money are carried out collectively by the network. MarteX [MXT] is the name of 
open source software which enables the use of this protocol.

Specifications and General info
------------------
MarteX uses
	libsecp256k1,
	libgmp,
	Boost1.63,
	OR Boost1.57,  
	Openssl1.02k,
	Berkeley DB 6.2.23,
	QT5.8 to compile


Block Spacing: 1 Minutes
Stake Minimum Age: 15 Confirmations (PoS-v3)

Port: 51315
RPC Port: 51314


BUILD LINUX
-----------
1) git clone https://github.com/martexcoin/martexcoin

2) cd martexcoin/src

3) sudo make -f makefile.unix USE_UPNP=-           # Headless

(optional)

4) strip MarteXd

5) sudo cp MarteXd /usr/local/bin

License
-------

MarteX [MXT] is released under the terms of the MIT license. See [COPYING](COPYING) for more
information or see https://opensource.org/licenses/MIT.

Development Process
-------------------

The `master` branch is regularly built and tested, but is not guaranteed to be
completely stable. [Tags](https://github.com/martexcoin/martexcoin/tags) are created
regularly to indicate new official, stable release versions of MarteX [MXT].

The contribution workflow is described in [CONTRIBUTING.md](CONTRIBUTING.md).

The developer [mailing list](https://lists.linuxfoundation.org/mailman/listinfo/bitcoin-dev)
should be used to discuss complicated or controversial changes before working
on a patch set.

Developer Slack can be found at http://martexcoin.slack.com.

Testing
-------

Testing and code review is the bottleneck for development; we get more pull
requests than we can review and test on short notice. Please be patient and help out by testing
other people's pull requests, and remember this is a security-critical project where any mistake might cost people
lots of money.

### Automated Testing

Developers are strongly encouraged to write [unit tests](/doc/unit-tests.md) for new code, and to
submit new unit tests for old code. Unit tests can be compiled and run
(assuming they weren't disabled in configure) with: `make check`

There are also [regression and integration tests](/qa) of the RPC interface, written
in Python, that are run automatically on the build server.

### Manual Quality Assurance (QA) Testing

Changes should be tested by somebody other than the developer who wrote the
code. This is especially important for large or high-risk changes. It is useful
to add a test plan to the pull request description if testing the changes is
not straightforward.
