> No programa original 'soma-lock-atom.c', altere a implementacão da thread extra de modo a garantir que ela imprima todos os 20 primeiros valores de soma que sao múltiplos de 10. O objetivo é fazer a thread ExecutaTarefa pausar sua execução quando um múltiplo de 10 é alcançado e somente continuar depois que o seu valor for impresso.

## Como executar o programa 
1. gcc -o \<soma-lock-atom\> soma-lock-atom.c 
2. ./soma-lock-atom \<qtd de threads\>