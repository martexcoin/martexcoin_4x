## FastSend Technical Information

FastSend has been integrated into the Core Daemon in two ways:
* "push" notifications (ZMQ and `-fastsendnotify` cmd-line/config option);
* RPC commands.

#### ZMQ

When a "Transaction Lock" occurs the hash of the related transaction is broadcasted through ZMQ using both the `zmqpubrawtxlock` and `zmqpubhashtxlock` channels.

* `zmqpubrawtxlock`: publishes the raw transaction when locked via FastSend
* `zmqpubhashtxlock`: publishes the transaction hash when locked via FastSend

This mechanism has been integrated into Bitcore-Node-MarteX which allows for notification to be broadcast through Insight API in one of two ways:
* WebSocket: [https://github.com/martexcoin/insight-api-martex#web-socket-api](https://github.com/martexcoin/insight-api-martex#web-socket-api)
* API: [https://github.com/martexcoin/insight-api-martex#fastsend-transactions](https://github.com/martexcoin/insight-api-martex#fastsend-transactions)

#### Command line option

When a wallet FastSend transaction is successfully locked a shell command provided in this option is executed (`%s` in `<cmd>` is replaced by TxID):

```
-fastsendnotify=<cmd>
```

#### RPC

Details pertaining to an observed "Transaction Lock" can also be retrieved through RPC, it’s important however to understand the underlying mechanism.

By default, the MarteX Core daemon will launch using the following constant:

```
static const int DEFAULT_FASTSEND_DEPTH = 5;
```

This value can be overridden by passing the following argument to the MarteX Core daemon:

```
-fastsenddepth=<n>
```

The key thing to understand is that this value indicates the number of "confirmations" a successful Transaction Lock represents. When Wallet RPC commands which support `minconf` and `addlockconf` parameters (such as `listreceivedbyaddress`) are performed and `addlockconf` is `true`, then `fastsenddepth` attribute is taken into account when returning information about the transaction. In this case the value in `confirmations` field you see through RPC is showing the number of `"Blockchain Confirmations" + "FastSend Depth"` (assuming the funds were sent via FastSend).

There is also a field named `instantlock` (that is present in commands such as `listsinceblock`). The value in this field indicates whether a given transaction is locked via FastSend.

**Examples**

1. `listreceivedbyaddress 0 true`
   * FastSend transaction just occurred:
        * confirmations: 5
   * FastSend transaction received one confirmation from blockchain:
        * confirmations: 6
   * non-FastSend transaction just occurred:
        * confirmations: 0
   * non-FastSend transaction received one confirmation from blockchain:
        * confirmations: 1

2. `listreceivedbyaddress 0`
   * FastSend transaction just occurred:
        * confirmations: 0
   * FastSend transaction received one confirmation from blockchain:
        * confirmations: 1
   * non-FastSend transaction just occurred:
        * confirmations: 0
   * non-FastSend transaction received one confirmation from blockchain:
        * confirmations: 1

3. `listsinceblock`
    * FastSend transaction just occurred:
        * confirmations: 0
        * instantlock: true
    * FastSend transaction received one confirmation from blockchain:
        * confirmations: 1
        * instantlock: true
    * non-FastSend transaction just occurred:
        * confirmations: 0
        * instantlock: false
    * non-FastSend transaction received one confirmation from blockchain:
        * confirmations: 1
        * instantlock: false
