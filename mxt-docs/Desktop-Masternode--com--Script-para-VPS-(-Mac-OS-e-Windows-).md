#### Passo a passo simplificado

#### Encontre a pasta com arquivos de configuração, de acordo com seu sistema operacional:

https://github.com/martexcoin/martexcoin/wiki#MarteX-Data-Folder

#### 1. Siga os passos na carteira desktop:
1. Abra o console de depuração - **Ferramentas > Console**
2. Crie um novo ENDEREÇO, executando: `getaccountaddress MN1`
3. Após confirmado, volte no console de depuração, execute: `masternode genkey`
4. Envie 5000 mxt para o endereço criado, executando: `sendtoaddress ENDEREÇO 5000`
5. Aguarda 16 confirmações.
6. Também execute: `masternode outputs`

- Copie os dados do passo 2, 3 e 6.

7. Feche a carteira.

8. Abra o arquivo **masternode.conf** (pasta encontrada anterior ao passo 1), e insira as seguintes informações:

```
nomeQualquer IP:DO:SEU:VPS:51315 PASSO_3 PASSO_6(colocar os dois retornos com um espaço entre eles)
```

Exemplo: 
```
mn1 127.0.0.2:51315 93HaYBVUCYjEMeeH1Y4sBGLALQZE1Yc1K64xiqgX37tGBDQL8Xg 2bcd3c84c84f87eaa86e4e56834c92927a07f9e18718810b92e0d0324456a 0
```

* Salve o arquivo.

#### 10. Siga os passos no terminal da VPS, inicie o script executando:
```
sudo curl -o- https://raw.githubusercontent.com/martexcoin/mxt-mn/master/MXT_deploy_mn.sh | bash
```

* Siga as instruções do script.
* No final do comando será mostrado todos detalhes do Masternode, é de muita importância que você os guarde
para eventual uso.

11. Inicie sua carteira Desktop.

12. No console digite: masternode status.

13. Acesse o block explorer e espere ele ser validado pela rede.

#### NOTA

Inicialmente ficará PRE-ENABLED.

Posteriormente mudará para ENABLED. Leva cerca de 1-2 horas.

Dependendo da condição da VPS, ela pode vir a ter o status EXPIRED,

Feche a carteira e inicie novamente e caso ocorra o mesmo erro, feche a carteira e apague o arquivo mncache.dat na pasta de configuração.

Tente novamente fechando a carteira e iniciando. Se o problema persistir, refaça os procedimentos de criação.

