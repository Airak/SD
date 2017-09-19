
# O que resta fazer deste trabalho?

## Coisas a fazer:
* Unit test
* Executar unit test remotamente
* -enviar arquivos pela rede-

## Como fazer:
*Unit Test:* Em C/C++, gera no final um arquivo maquina.i.log em /tmp/. O executável também deve estar nessa pasta

*Executar unit test remotamente & enviar arquivos pela rede:* Nome é bem auto explicativo. Todas as máquinas estarão rodando o programa (que não é o unit test, para deixar claro) ao mesmo tempo. O programa tem que estar apto a qualquer tempo receber requisições (só criar uma thread pra isso)(por enquanto não acho válido lidar com várias requisões ao mesmo tempo). Uma segunda thread pode ser iniciada quando o usuário quiser, para pedir os arquivos de log das outras máquinas
