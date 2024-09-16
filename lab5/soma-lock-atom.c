/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Aluna: Thalita de Freitas Silva */
/* DRE: 120048493 */
/* Laboratório: 5 */
/* Codigo: Comunicação entre threads usando variável compartilhada e exclusao mutua com bloqueio */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

int multiplos, imprime = 0; //variavel para guardar a quantidade de multiplos impressos; variavel para definir se um multiplo esta sendo impresso
long int soma = 0; //variavel compartilhada entre as threads
pthread_mutex_t mutex; //variavel de lock para exclusao mutua
pthread_cond_t cond; //variavel de condicao

//funcao executada pelas threads
void *ExecutaTarefa (void *arg) {
  long int id = (long int) arg;
  printf("Thread : %ld esta executando...\n", id);

  for (int i=0; i<100000; i++) {
    pthread_mutex_lock(&mutex);

    // mantem sua execução pausada enquanto um multiplo esta sendo impresso, e enquanto não tiverem sido impresso os 20 multiplos
    while (imprime == 1 && multiplos<=20)  {
      pthread_cond_wait(&cond, &mutex);
    }

    soma++;

    // quando um multiplo eh identificado, eh enviado um sinal para a thread extra para que ela possa imprimir o valor
    if (soma%10 == 0) {
      imprime=1;
      multiplos++;
      pthread_cond_signal(&cond);
    }
    
    pthread_mutex_unlock(&mutex);  
  }
  printf("Thread : %ld terminou!\n", id);
  pthread_exit(NULL);
}

//funcao executada pela thread de log
void *extra (void *args) {
  printf("Extra : esta executando...\n");

  // imprime os 20 primeiros multiplos de soma
  for (int i=0; i<20; i++) {
    pthread_mutex_lock(&mutex);

    // mantem sua execução pausada enquanto um multiplo de 10 nao for identificado
    while (imprime == 0) {
      pthread_cond_wait(&cond, &mutex);
     }
     
    printf("soma = %ld \n", soma);
    imprime=0; // redefine a variavel 'sinalizando' a finalizacao da impressao do multiplo
    pthread_cond_broadcast(&cond); // sinaliza que as threads podem continuar
    pthread_mutex_unlock(&mutex);
  }
  printf("Extra : terminou!\n");
  pthread_exit(NULL);
}

//fluxo principal
int main(int argc, char *argv[]) {
   pthread_t *tid; //identificadores das threads no sistema
   int nthreads; //qtde de threads (passada linha de comando)

   //--le e avalia os parametros de entrada
   if(argc<2) {
      printf("Digite: %s <numero de threads>\n", argv[0]);
      return 1;
   }
   nthreads = atoi(argv[1]);

   //--aloca as estruturas
   tid = (pthread_t*) malloc(sizeof(pthread_t)*(nthreads+1));
   if(tid==NULL) {puts("ERRO--malloc"); return 2;}

   //--inicilaiza o mutex (lock de exclusao mutua)
   pthread_mutex_init(&mutex, NULL);
   pthread_cond_init(&cond, NULL);

   //--cria as threads
   for(long int t=0; t<nthreads; t++) {
     if (pthread_create(&tid[t], NULL, ExecutaTarefa, (void *)t)) {
       printf("--ERRO: pthread_create()\n"); exit(-1);
     }
   }

   //--cria thread de log
   if (pthread_create(&tid[nthreads], NULL, extra, NULL)) {
      printf("--ERRO: pthread_create()\n"); exit(-1);
   }

   //--espera todas as threads terminarem
   for (int t=0; t<nthreads+1; t++) {
     if (pthread_join(tid[t], NULL)) {
         printf("--ERRO: pthread_join() \n"); exit(-1); 
     } 
   } 

   //--finaliza o mutex
   pthread_mutex_destroy(&mutex);
   pthread_cond_destroy(&cond);
   
   printf("Valor de 'soma' = %ld\n", soma);

   return 0;
}