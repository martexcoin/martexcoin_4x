All procedures are simple, but requires attention.

## Backup procedure

https://github.com/martexcoin/martexcoin/wiki/How-to-make-an-Backup

## Wallet version check

#### To check if your wallet is running in lastest version run:

#### Linux:
 
`MarteXd getinfo`

#### Windows / MacOS:

Go to Options > Debug Console, and execute:
getinfo

In Wallet v1.6.0 should be:

...
  "version": 1060000,
...

## Masternode Update 

#### Only continue if you follow backup procedure.
#### Only for those who used auto-install script;
#### If you didnt, please skip to Linux sector.

1. Login in root user.

`su root`

2. Stop script service:

`systemctl stop worker01.service`

3. Get latest wallet:
```
   git clone https://github.com/martexcoin/martexcoin
  
   cd ~martexcoin/src

   make -f makefile.unix USE_UPNP=-

   strip MarteXd

   cd ~martexcoin
   
   qmake martex.pro "USE_UPNP=-" "USE_QRCODE=1"

   make -f Makefile

	Caso der algum erro, seguir os passos nestes links: https://forum.martexcoin.org/d/7-compilar-a-wallet-no-linux ; https://github.com/martexcoin/martexcoin/blob/master/doc/build-unix.md
```

4. Start script service.

`systemctl start worker01.service`

#### Now check if your wallet is running in lastest version running.

***

## Windows 

#### Only continue if you follow backup procedure.

1. Close your wallet

2. Get latest wallet: https://martexcoin.org/getstarted.html

3. Already in recent version, start your wallet. 

4. In order to create a shortcut 'MarteXd.exe',
 you may find in one of this folders:
 
Windows x64

`C:\Program Files\MXT\`

Windows x86

`C:\Program Files (x86)\MXT\`


#### Now check if your wallet is running in lastest version running.

***

## Linux

#### Only continue if you follow backup procedure.

1. Close your wallet running

  `MarteXd stop`

2. To install new wallet, run:
`git clone https://github.com/martexcoin/martexcoin`
  
   `cd ~martexcoin/src`

   `make -f makefile.unix USE_UPNP=-`

   `strip MarteXd`

   `cd ~martexcoin`
   
   `qmake martex.pro "USE_UPNP=-" "USE_QRCODE=1"`

   `make -f Makefile`

	Caso der algum erro, seguir os passos nestes links: https://forum.martexcoin.org/d/7-compilar-a-wallet-no-linux ; https://github.com/martexcoin/martexcoin/blob/master/doc/build-unix.md 

3. Start your wallet ( latest version )
   `MarteXd`

   `MarteXd getinfo`

4. If this is a Masternode, do not forget to start it:

   `MarteXd`

#### Now check if your wallet is running in lastest version running.

***

### FAQ

1. Should i backup ONLY before update?
```
R: No! backup is extreme important and should be done always according with your scheudle. This happens because your wallet receives many new informations, your last backup becames obsolete compared to your actual wallet.

* Always use your last backup in order to recover.

```

2. Should i backup any other file?

```
R: Besides wallet.dat, if you have a masternode you may need to backup masternodes.conf so you can restore masternodes when you need to.
```

3. "After update my wallet, should i replace with backup" ?
```
R: That's not necessary. 
   We advice to separate your backups.
```

4. "I started my wallet and it gives me EXPIRED status" ?
```
R: This status occurs for many reasons, but it's normal when you have an fork update.
   You can try to start him, if it continues EXPIRED, try to remove 'mncache.dat' file.
   If it persist EXPIRED status, you need to recreate your masternode.
```
