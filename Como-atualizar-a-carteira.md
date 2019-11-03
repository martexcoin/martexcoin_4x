Os procedimentos são simples, mas requer atenção.

## Procedimento de backup

https://github.com/martexcoin/martexcoin/wiki/Como-fazer-backup


## VPS de Masternode

Se você for atualizar a VPS do Masternode, depois de ter usado o script:

https://github.com/martexcoin/martexcoin/masternode-auto

### Siga os passos:

1. Inicialmente logue no usuário root: 

`su root`

2. Pare a carteira:

`MarteXd stop`

3. Execute o script de update:

`sudo curl -o- https://raw.githubusercontent.com/martexcoin/martexcoin/masternode-auto/master/update.sh | bash`

Não é necessário seguir os passos abaixo para Linux.

***



## Windows 

1. Feche a carteira

2. Instale a carteira através do site ou github.

3. Inicie a carteira nova. ( atualizado )

4. Faça um atalho a partir do arquivo 'MarteXd.exe',
 em uma das seguintes pastas:
 
Local de instalação para sistema operacional x64

`C:\Program Files\MXT\`

Local de instalação para sistema operacional x86

`C:\Program Files (x86)\MXT\`

***


## Linux

1. Feche a carteira, execute:

  `MarteXd stop`

2. Instale o novo cliente, execute no terminal:

   `git clone https://github.com/martexcoin/martexcoin`
  
   `cd ~martexcoin/src`

   `make -f makefile.unix USE_UPNP=-`

   `strip MarteXd`

   `cd ~martexcoin`
   
   `qmake martex.pro "USE_UPNP=-" "USE_QRCODE=1"`

   `make -f Makefile`

	Caso der algum erro, seguir os passos nestes links: https://forum.martexcoin.org/d/7-compilar-a-wallet-no-linux ; https://github.com/martexcoin/martexcoin/blob/master/doc/build-unix.md 

3. Abra a carteira. ( atualizado )

   `MarteXd`

   `MarteXd getinfo`

4. Se for uma carteira de um Masternode, não esqueça de inicia-lo:

   `MarteXd`


***



### FAQ

1. Devo fazer backup 'apenas' antes de atualizar ?
```
R: Não! é de suma importância sempre manter o backup atualizado. Com o tempo sua carteira acrescenta muitas novas informações, então o backup antigo já não pode conter essas novas informações, o que pode levar a perda de saldo no momento de usar o backup.
 
A: Sempre use o backup mais recente.
```

2. Preciso fazer backup de quais arquivos?

```
R: É necessário fazer backup do arquivo wallet.dat, esse arquivo é o único que mantém seus saldos e apenas usando ele que pode controlar todo seu MarteXd.
   Caso sua carteira local conter configuração para masternodes, faça backup do arquivo masternode.conf.
 
A: Guarde backup do arquivo wallet.dat, e se houver masternodes, também guarde o arquivo masternode.conf.
```

3. Após atualizar a carteira é preciso substituir usando o backup?
```
R: Não há necessidade, mas aconselhamos separar uma pasta nova para o backup da nova atualização.
```

4. Iniciei o masternode e ficou como EXPIRED, o que fazer?
```
R: Esse status ocorre por diversos motivos, mas é normal quando há atualização, poís você precisa fechar a carteira do seu masternode levando ao status citado.
   Você pode tentar iniciá-lo, caso o mesmo continue, é preciso recriar o masternode. 
   Portanto, se houver a continuação, apague o arquivo 'mncache.dat' ( encontrado na pasta MXT em AppData no windows ou em .MXT no linux ) e crie novamente o masternode.
```
