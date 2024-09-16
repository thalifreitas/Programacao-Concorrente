## Como executar os programas
1. gcc -o gera geraMatriz.c
2. ./gera \<linhas\> \<colunas\> \<nome arquivo primeira matriz\>
3. ./gera \<linhas\> \<colunas\> \<nome arquivo segunda matriz\>

4. gcc -o \<sequencial\> multiplicacaoSequencial.c
5. ./sequencial \<arquivo primeira matriz\> \<arquivo segunda matriz\> \<nome para o arquivo resultante sequencial\>
6. gcc -o \<concorrente\> multiplicacaoConcorrente.c
7. ./concorrente \<arquivo primeira matriz\> \<arquivo segunda matriz\> \<nome para o arquivo resultante concorrente\> \<qtd de threads\>

Usando o comando diff, é possivel confirmar se o resultado sequencial e concorrente estao iguais:
diff \<nome do arquivo resultante sequencial\> \<nome do arquivo resultante concorrente\>
Caso retorne vazio significa que os resultados são iguais.
