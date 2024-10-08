Disciplina: Programação Concorrente
Profa.: Silvana Rossetto
Aluna: Thalita de Freitas Silva
DRE: 120048493
Laboratório: 6 - Avaliação dos resultados

Foi usado como base para a construção do código 'my_main_rwlock.c' o código 'main_rwlock.c' disponilizado pela professora.
Algumas alterações foram necessárias, incluindo definir QTDE_OPS sendo igual a 100 e alterar a porcentagem de operações 
(operacoes de consulta com 60%, insercao com 20% e remocao com 20%) para facilitar os testes e a visualização das mensagens de log. Além disso, foram incluídas 4 novas funções (2 para leitura e 2 para escrita), e foram usadas
duas variaveis de condicao para fazer o controle dos leitores e escritores do programa.

Após realizar alguns testes, variando também a quantidade de threads, percebemos que a condição de prioridade para operações de escrita
está sendo atendida: sempre que vemos uma mensagem de uma operação de escrita sendo iniciada, a próxima mensagem é dessa mesma operação de escrita sendo encerrada, 
não havendo, por exemplo, operações de leitura sendo iniciadas enquanto há uma operação de escrita em espera.