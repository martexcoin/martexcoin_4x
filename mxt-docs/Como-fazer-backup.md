## Antes de ler, esteja atento ao seguinte:

Você é responsável pela sua carteira.

Se você esquecer sua senha, não há como recuperar, então mantenha sempre um backup com a senha mais recente.

Não indicaremos nenhum serviço de terceiro para manter seu backup.

Sempre use o backup mais recente.

No caso de possuir masternodes e estiver migrando de computador, etc. Faça cópia do arquivo masternodes.conf no processo.

## Arquivos de backup

Arquivo | Descrição | Obrigatório | Prioridade
--------|-----------|--------- |-----------
wallet.dat | Arquivo que contém todos seus endereços e o saldo referente ao endereço. | Sim | Alta
masternode.conf | Arquivo que contém configurações de Masternodes, usado apenas por carteiras 'cold-wallet'. | Apenas se utilizado. | Média
MarteX.conf | Arquivo com configurações da carteira, e se usado por masternode, pode conter a genkey referente ao mesmo. | Apenas se utilizado. | Baixa

## Windows / MacOS 
1. Encontre a pasta de acordo com seu sistema operacional.

https://github.com/martexcoin/martexcoin/wiki#MarteX-Data-Folder

2. Faça backup dos arquivos acima para sua pasta final de backup.

## Linux

Para copiar os arquivos via terminal, execute:

`cp ~/.MXT/arquivo /pasta/de/backup`

#### Lembre-se de estar logado no usuário onde a carteira esta instalada.

