/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Aluna: Thalita de Freitas Silva */
/* DRE: 120048493 */
/* Laboratório: 6 */
/* Programa concorrente que cria e faz operacoes sobre uma lista de inteiros */

#include <stdio.h>
#include <stdlib.h>
#include "list_int.h"
#include <pthread.h>
#include "timer.h"

#define QTDE_OPS 100 //quantidade de operacoes sobre a lista (insercao, remocao, consulta)
#define QTDE_INI 100 //quantidade de insercoes iniciais na lista
#define MAX_VALUE 100 //valor maximo a ser inserido

//lista compartilhada iniciada 
struct list_node_s* head_p = NULL; 
//qtde de threads no programa
int nthreads;

pthread_mutex_t mutex;
pthread_cond_t cond_escrita, cond_leitura;
int leitores = 0, escritores = 0, escritores_fila = 0;

// Inicia leitura
void entraLeitura(){
    pthread_mutex_lock(&mutex);
    while (escritores>0 || escritores_fila>0) {
        pthread_cond_wait(&cond_leitura, &mutex);
    }
    leitores++;
    pthread_mutex_unlock(&mutex);
}

// Encerra leitura
void saiLeitura(){
    pthread_mutex_lock(&mutex);
    leitores--;
    if (leitores==0){
        pthread_cond_signal(&cond_escrita);
    }
    pthread_mutex_unlock(&mutex);    
}

// Inicia escrita
void entraEscrita(){
    pthread_mutex_lock(&mutex);
    escritores_fila++;
    while (leitores>0 || escritores>0){
        pthread_cond_wait(&cond_escrita, &mutex);
    }
    escritores_fila--;
    escritores++;
    pthread_mutex_unlock(&mutex);
    
}

// Encerra escrita
void saiEscrita(){
    pthread_mutex_lock(&mutex);
    escritores--;
    // prioridade para operações de escrita
    if (escritores_fila > 0) {
        // prioriza a proxima operacao de escrita, caso haja escritores em espera
        pthread_cond_signal(&cond_escrita);
    } else {
        // libera os leitores, caso nao haja escritores esperando
        pthread_cond_broadcast(&cond_leitura);
    }
    pthread_mutex_unlock(&mutex);
}



//tarefa das threads
void* tarefa(void* arg) {
   long int id = (long int) arg;
   int op;
   int in, out, read; 
   in=out=read = 0;

   // realiza operacoes de consulta (60%), insercao (20%) e remocao (20%)
   for (long int i = id; i < QTDE_OPS; i += nthreads){
      op = rand() % 100;
      if (op < 60){
        entraLeitura(); // Inicia leitura
        printf("Leitor de consulta %ld esta lendo.\n", id);
        Member(i % MAX_VALUE, head_p);  /* Ignore return value */
        saiLeitura();
        printf("Leitor %ld encerrou leitura.\n", id);
        read++;
      }
      else if (60 <= op && op < 80){
        entraEscrita(); // Inicia escrita
        printf("Escritor de insercao %ld esta escrevendo.\n", id);
        Insert(i % MAX_VALUE, &head_p); /* Ignore return value */
        saiEscrita();
        printf("Escritor %ld encerrou escrita.\n", id);;
        in++;
      }
      else if (op >= 80){
        entraEscrita(); // Inicia escrita
        printf("Escritor de remocao %ld esta escrevendo.\n", id);
        Delete(i % MAX_VALUE, &head_p); /* Ignore return value */
        saiEscrita();
        printf("Escritor %ld encerrou escrita.\n", id);;
        out++;
      }
   }
   //registra a qtde de operacoes realizadas por tipo
   printf("\nThread %ld: in=%d out=%d read=%d\n", id, in, out, read);
   printf("\n");
   pthread_exit(NULL);
}

/*-----------------------------------------------------------------*/
int main(int argc, char* argv[]) {
   pthread_t *tid;
   double ini, fim, delta;
   
   //verifica se o numero de threads foi passado na linha de comando
   if(argc<2) {
      printf("Digite: %s <numero de threads>\n", argv[0]); return 1;
   }
   nthreads = atoi(argv[1]);

   //insere os primeiros elementos na lista
   for(int i=0; i<QTDE_INI; i++)
      Insert(i%MAX_VALUE, &head_p);  /* Ignore return value */
   

   //aloca espaco de memoria para o vetor de identificadores de threads no sistema
   tid = malloc(sizeof(pthread_t)*nthreads);
   if(tid==NULL) {  
      printf("--ERRO: malloc()\n"); return 2;
   }

   //tomada de tempo inicial
   GET_TIME(ini);
   //inicializa mutex e variaveis de condicao
   pthread_mutex_init(&mutex, NULL);
   pthread_cond_init(&cond_escrita, NULL);
   pthread_cond_init(&cond_leitura, NULL);
   
   //cria as threads
   for(long int i=0; i<nthreads; i++) {
      if(pthread_create(tid+i, NULL, tarefa, (void*) i)) {
         printf("--ERRO: pthread_create()\n"); return 3;
      }
   }
   
   //aguarda as threads terminarem
   for(int i=0; i<nthreads; i++) {
      if(pthread_join(*(tid+i), NULL)) {
         printf("--ERRO: pthread_join()\n"); return 4;
      }
   }

   //tomada de tempo final
   GET_TIME(fim);
   delta = fim-ini;
   printf("Tempo: %lf\n", delta);

   //libera mutex e variaveis de condicao
   pthread_mutex_destroy(&mutex);
   pthread_cond_destroy(&cond_escrita);
   pthread_cond_destroy(&cond_leitura);

   //libera o espaco de memoria do vetor de threads
   free(tid);

   //libera o espaco de memoria da lista
   Free_list(&head_p);

   return 0;
}  /* main */

