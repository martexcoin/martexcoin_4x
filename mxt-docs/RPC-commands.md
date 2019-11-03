## Follow all rpc commands.

### For specific usage, download wallet and run: help "command"
### In order to use zcore-cli (RPC Client), you need to configure zcore.conf:

https://github.com/martexcoin/martexcoin/wiki#MarteX-Data-Folder

#### Add/Change these lines:
```
rpcuser=ruser
rpcpassword=rpassword
rpcport=51314
rpcconnect=ip que queres conectar
rpcallowip=ip que queres permitir conectar
listen=1
daemon=1
server=1
```

### Linux - RPC Client: 
`./MarteXd COMMAND`

### Curl 
`curl --user 'User' --data-binary '{"jsonrpc":"1.0","id":"curltext","method":"getinfo","params":[]}' -H 'content-type:text/plain;' http://127.0.0.1:RPCPORT`

*User is the same **rpcuser **in MarteX.conf
*Password is the same **rpcpassword **in MarteX.conf
*PORT is the same **rpcport **in MarteX.conf

### Desktop Wallet:
Go to **Information / Console / Network Traffic** and run the command.


 Command          | Description                        | Params         | Sector
 -----------------|------------------------------------|----------------|-----------
anonsend <MarteX> <amount> | Inicia o processo de envio de moedas de maneira anônima. (Observação, uma documentação mais detalhada será feita sobre esta função). | | | anonsen
getbestblockhash | Exibe o último hash processado por sua carteira. (Returns the hash of the best (tip) block in the longest block chain.) | | block
getblock <hash> [txinfo] | Mostra todas as informações do blockhash inserido. | | block
getblockbynumber <number> [txinfo] | Mostra todas as informações do número do bloco inserido. | | block
getblockhash <index> | Exibe o bloco do hash informado. | | block
getblocktemplate [params] | Returns data needed to construct a block to work on. | | block
getcheckpoint | Expõe em qual block hash (string and interger) a carteira está sincronizada com os masternode; exibe horário UTC,e o tipo de política. | | block
getdifficulty | Exibe a dificuldade de mineração da moeda na modalidade POS. | | block
getmininginfo | Expõe informações sobre o estado de mineração da moeda. (Returns an object containing mining-related information: * blocks * currentblocksize * currentblocktx * difficulty * errors * generate * genproclimit * hashespersec * pooledtx * testnet) | | block
getrawtransaction <txid> [verbose=0] | Returns raw transaction representation for given transaction id. | | block
gettransaction "txid" ( includeWatchonly ) | Returns an object about the given transaction containing: * "amount" : total amount of the transaction * "confirmations" :  number of confirmations of the transaction * "txid" : the transaction ID * "time" : time associated with the transaction<ref>From block timestamp, unless transaction was already in memory pool then the local time when the client added the transaction to its memory pool</ref>. * "details" - An array of objects containing: ** "account" ** "address" ** "category" ** "amount" ** "fee" | | block
keypoolrefill ( newsize ) | Fills the keypool, requires wallet passphrase to be set. | | block
submitblock <hex data> [optional-params-obj] | Attempts to submit new block to network. | | block
addnode <node> <add|remove|onetry> | Attempts add or remove <node> from the addnode list or try a connection to <node> once. | | connection
getaddednodeinfo <dns> [node] | Returns information about the given added node, or all added nodes (note that onetry addnodes are not listed here) If dns is false, only a list of added nodes will be provided, otherwise connected information will also be available. | | connection
getblockcount | Exibe a quantidade de blocos existentes na blockchain. | | connection
getconnectioncount | Exibe a quantidade de conexões peer-to-peer sua carteira possui. | | connection
getnettotals | Exibe a quantidade de bytes enviados e recebidos e o tempo demorado para isso em millisegundos. | | connection
getpeerinfo | Exibe informações relevantes de conecção para cada peer conectado. | | connection
getrawmempool | Comando que não retorna nada. (Returns all transaction ids in memory pool) | | connection
getstakereport | Exibe informações que não sou capaz de saber o que é. | | connection
getstakesubsidy <hex string> | ????? | | connection
getstakinginfo | Exibe informações que não sou capaz de saber o que é. | | connection
getsubsidy [nTarget] | ????? | | connection
getvelocityinfo | Exibe informações sobre ???? | | connection
getwork [data] |  If [data] is not specified, returns formatted hash data to work on:* "midstate" : precomputed hash state after hashing the first half of the data * "data" : block data * "hash1" : formatted hash buffer for second hash * "target" : little endian hash target If [data] is specified, tries to solve the block and returns true if it was successful. | | connection
getworkex [data, coinbase] | ????? | | connection
listaddressgroupings | Returns all addresses in the wallet and info used for coincontrol. | | connection
ping | Exibe o ping sobre os peers que estás conectado. | | connection
scanforalltxns [fromHeight] | ????? | | connection
scanforstealthtxns [fromHeight] | ????? | | connection
  | ????? | | fasttransaction
masternode "command"... ( "passphrase" ) | ????? | | masternode
masternodelist ( "mode" "filter" ) | ????? | | masternode
signmessage "MarteX" "message" | Sign a message with the private key of an address. | | sms
signrawtransaction <hex string> [{"txid":txid,"vout":n,"scriptPubKey":hex,"redeemScript":hex},...] [<privatekey1>,...] [sighashtype="ALL"] | Adds signatures to a raw transaction and returns the resulting raw transaction. | | sms
smsgaddkey <address> <pubkey> | ?????? | | sms
smsgbuckets [stats or dump] | ????? | | sms
smsgdisable | ?????? | | sms
smsgenable | ?????? | | sms
smsggetpubkey <address> | ?????? | | sms
smsginbox [all or unread or clear] | ?????? | | sms
smsglocalkeys [whitelist or all or wallet or recv <+/-> <address (endereço)> or anon <+/-> <address (endereço)>] | ?????? | | sms
smsgoptions [list or set <optname> <value>] | ?????? | | sms
smsgoutbox [all or clear] | ?????? | | sms
smsgscanbuckets | ?????? | | sms
smsgscanchain | ?????? | | sms
smsgsend <addrFrom> <addrTo> <message> | ?????? | | sms
smsgsendanon <addrTo> <message> | ?????? | | sms
addmultisigaddress nrequired ["key",...] ( "account" ) | Add a nrequired-to-sign multisignature address to the wallet. Each key is a bitcoin address or hex-encoded public key. If [account] is specified, assign address to [account]. Returns a string containing the address. | | wallet
addredeemscript <redeemScript> [account] | ????? | | wallet
backupwallet "destination" | Faz o backup (cópia) da carteira em um arquivo. Você deve digitar o diretório onde deverá ser gravado o arquivo. | | wallet
checkwallet | Verifica a integridade da carteira. | | wallet
createmultisig nrequired ["key",...] | Creates a multi-signature address and returns a json object. | | wallet
createrawtransaction [{"txid":txid,"vout":n},...] {address:amount,...} ( locktime ) | Creates a raw transaction spending given inputs. | | wallet
decoderawtransaction <hex string> | Produces a human-readable JSON object for a raw transaction. | | wallet
decodescript <hex string> | ????? | | wallet
dumpprivkey <MarteX> | Mostra a chave privada do endereço público Martex inserido. | | wallet
dumpwallet <filename> | Exports all wallet private keys to file. | | wallet
encryptwallet "passphrase" | Adiciona uma senha para aumentar a segurança da carteira. (Encrypts the wallet with <passphrase>.) | | wallet
getaccount "MarteX" | Returns the account associated with the given address. | | wallet
getaccountaddress "account" | Exibe um endereço endereço Martex público de determinada conta. (Returns the current bitcoin address for receiving payments to this account. If <account> does not exist, it will be created along with an associated new address that will be returned.) | | wallet
getaddressesbyaccount "account" | Returns the list of addresses for the given account. | | wallet
getbalance ( "account" minconf includeWatchonly ) | Exibe o balanço geral de todos os enderoços adicionados na carteira. (If [account] is not specified, returns the server's total available balance.<br/>If [account] is specified, returns the balance in the account.) | | wallet
getinfo | Expõe informações todas informações relevantes do Node. | | wallet
getnewaddress ( "account" ) | Cria um novo endereço público para uma determinada conta. (Returns a new bitcoin address for receiving payments. If [account] is specified payments received with the address will be credited to [account].) | | wallet
getnewpubkey [account] | Gera uma nova chave pública para uma determinada conta?????????? Se for um endereço HD, você pode gerar diversas pubkey para uma mesma chave privada. | | wallet
getnewstealthaddress [label] | ????? | | wallet
getreceivedbyaccount "account" ( minconf ) | Returns the total amount received by addresses with [account] in transactions with at least [minconf] confirmations. If [account] not provided return will include all transactions to all accounts. | | wallet
getreceivedbyaddress "MarteX" ( minconf ) | Returns the amount received by <bitcoinaddress> in transactions with at least [minconf] confirmations. It correctly handles the case where someone has sent to the address in multiple transactions. Keep in mind that addresses are only ever used for receiving transactions. Works only for addresses in the local wallet, external addresses will always show 0. | | wallet
importaddress "address" ( "label" rescan ) | Importa um endereço público sem importar a chave privada. | | wallet
importprivkey <MarteXprivkey> [label] [rescan=true] | Importa uma chave privada. (Adds a private key (as returned by dumpprivkey) to your wallet. This may take a while, as a rescan is done, looking for existing transactions. Optional [rescan] parameter added in . Note: There's no need to import public key, as in ECDSA (unlike RSA) this can be computed from private key.) | | wallet
importstealthaddress <scan_secret> <spend_secret> [label] | ????? | | wallet
importwallet <filename> | Importa uma carteira a partir de um arquivo. | | wallet
listaccounts ( minconf includeWatchonly) | Returns Object that has account names as keys, account balances as values. | | wallet
listreceivedbyaccount ( minconf includeempty includeWatchonly) | Returns an array of objects containing: * "account" : the account of the receiving addresses * "amount" : total amount received by addresses with this account * "confirmations" : number of confirmations of the most recent transaction included | | wallet
listreceivedbyaddress ( minconf includeempty includeWatchonly) |  Returns an array of objects containing: * "address" : receiving address * "account" : the account of the receiving address * "amount" : total amount received by the address * "confirmations" : number of confirmations of the most recent transaction included. To get a list of accounts on the system, execute listreceivedbyaddress 0 true. | | wallet
listsinceblock ( "blockhash" target-confirmations includeWatchonly) | Get all transactions in blocks since block [blockhash], or all transactions if omitted. [target-confirmations] intentionally does not affect the list of returned transactions, but only affects the returned "lastblock" value. | | wallet
liststealthaddresses [show_secrets=0] | ????? | | wallet
listtransactions ( "account" count from includeWatchonly) | Returns up to [count] most recent transactions skipping the first [from] transactions for account [account]. If [account] not provided it'll return recent transactions from all accounts. | | wallet
listunspent [minconf=1] [maxconf=9999999]  ["address",...] | Returns array of unspent transaction inputs in the wallet. | | wallet
makekeypair [prefix] | ????? | | wallet
move "fromaccount" "toaccount" amount ( minconf "comment" ) | Move from one account in your wallet to another. | | wallet
repairwallet | Repara a wallet e retorna o valor em boolean (verdadeiro ou falso) | | wallet
resendtx | ????? | | wallet
reservebalance [<reserve> [amount]] | ????? | | wallet
searchrawtransactions <address> [verbose=1] [skip=0] [count=100] | ????? | | wallet
sendalert <message> <privatekey> <minver> <maxver> <priority> <id> [cancelupto] | ????? | | wallet
sendfrom "fromaccount" "toMarteX" amount ( minconf "comment" "comment-to" ) | <amount> is a real and is rounded to 8 decimal places. Will send the given amount to the given address, ensuring the account has a valid balance using [minconf] confirmations. Returns the transaction ID if successful (not in JSON object). | | wallet
sendmany "fromaccount" {"address":amount,...} ( minconf "comment" ) | Amounts are double-precision floating point numbers. | | wallet
sendrawtransaction <hex string> | Submits raw transaction (serialized, hex-encoded) to local node and network. | | wallet
sendtoaddress "MarteX" amount ( "comment" "comment-to" ) | <amount> is a real and is rounded to 8 decimal places. Returns the transaction ID <txid> if successful. | | wallet
sendtostealthaddress <stealth_address> <amount> [comment] [comment-to] [narration] | ????? | | wallet
setaccount "MarteX" "account" | Sets the account associated with the given address. Assigning address that is already assigned to the same account will create a new address associated with that account. | | wallet
settxfee amount | <amount> is a real and is rounded to the nearest ?????? | | wallet
spork <name> [<value>] | ????? | | wallet
stop | Desligar o MarteX wallet. | | wallet
validateaddress <MarteX> | Valida um endereço MarteX e retorna em boolean (verdadeiro ou falso). | | wallet
validatepubkey <MarteXpubkey> | Valida um endereço MarteX público e retorna em boolean (verdadeiro ou falso). | | wallet
verifymessage <MarteX> <signature> <message> | Verify a signed message. | | wallet
help | Show all commands list. (List commands, or get help for a command.) | | wallet
cclistcoins | ?????? | | WhatIsThis
checkkernel [{"txid":txid,"vout":n},...] [createblocktemplate=false] | ????? | | WhatIsThis
