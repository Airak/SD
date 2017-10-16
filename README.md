# Trabalho Prático de Sistemas Distribuídos

[TODO LIST](https://github.com/gustavohsborba/SD/blob/master/doc/afazer.md "Coisas a Fazer")


---- GROUP ----

Gustavo Borba <borba@cefetmg.br>
Marco Antonio <marcochel@gmail.com>
Matheus Martins <matheus.mmag@gmail.com>
Rafael Batista <rafael.cefetmg@outlook.com>

---- PARA EXECUTAR ----

Para compilar, basta entrar em src/ e executar o comando:
  make
Executar o servidor:
  ./server_rpc
Executar comando store:
  ./store K V
  K: Número inteiro referente a qual node o valor desejado deve ser guardado. 
  V: Valor desejado a se guardar para o valor de key igual a K.
Executar comando find
  ./find K
  K: Número inteiro referente a qual node o valor desejado deve ser guardado. 

É necessário adicionar todos os IPs dos servidores em peers.txt