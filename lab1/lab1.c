/* Disciplina: Programacao Concorrente */
/* Profa.: Silvana Rossetto */
/* Aluna: Thalita de Freitas Silva */
/* DRE: 120048493 */
/* Laboratório: 1 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct{
   int *array, start, end;
} thread_args;


/* função executada pelas threads */
void *incrementArray(void *arg){
   // typecasting do argumento
   thread_args args = *(thread_args *)arg;

   for (int i = args.start; i < args.end; i++){
      args.array[i]++;
   }

   free(arg); // libera a memória que foi alocada na main

   pthread_exit(NULL);
}

/* função que realiza a operação de incremento dos elementos do vetor de forma sequencial */
void incrementArraySequencial(int array[], int n){
   for (int i = 0; i < n; i++){
      array[i]++;
   }
}

/* função de inicialização do vetor */
void createArray(int array[], int n){
   for (int i = 0; i < n; i++){
      array[i] = i;
   }
}

/* função para imprimir o vetor */
void printArray(int array[], int n){
   for (int i = 0; i < n; i++){
      printf("%d \n", array[i]);
   }
}

/* função que verifica se o resultado está certo */
void compareArray(int array1[], int array2[], int n){
   int count_equal = 0;

   for (int i = 0; i < n; i++){
      if (array1[i] == array2[i]){
         count_equal++;
      }
   }

   if (count_equal == n){
      printf("Resultado final correto!\n");
   }
   else{
      printf("Resultado final incorreto!\n");
   }
}

/* função principal do programa */
int main(int argc, char *argv[]){
   thread_args *args; // irá receber os argumentos para a thread
   int M, N;          // quantidade de threads que serão criadas e de elementos do vetor, respectivamente

   // verifica se foram passados os dois argumentos necessários
   if (argc != 3){
      printf("--ERRO: informe a quantidade de threads e quantidade de elementos do vetor.\n");
      return 1;
   }

   // converte para inteiro os argumentos recebidos
   M = atoi(argv[1]);
   N = atoi(argv[2]);

   int array2[N];
   int array3[N];

   // chama a função de inicialização do vetor
   createArray(array2, N);
   createArray(array3, N);
   incrementArraySequencial(array3, N);

   // identificadores das threads no sistema
   pthread_t tid_sistema[M];

   // cria as threads e atribui a tarefa de cada thread
   for (int i = 1; i <= M; i++){
      args = malloc(sizeof(thread_args));

      if (args == NULL){
         printf("--ERRO: malloc()\n");
         return 2;
      }

      // dividindo a quantidade de elementos para cada thread de forma balanceada
      args->array = array2;
      args->start = (i-1) * (N / M);
      args->end = i * (N / M);
      
      // garantindo que a última thread ficará com a "sobra" dos elementos caso haja
      if (i == M){
         args->end += N%M;
      }
      

      if (pthread_create(&tid_sistema[i-1], NULL, incrementArray, (void *)args)){
         printf("--ERRO: pthread_create()\n");
         return 2;
      }
   }

   // aguardar o término da execução das threads criadas
   for (int i = 0; i < M; i++){
      if (pthread_join(tid_sistema[i], NULL)){
         printf("--ERRO: pthread_join() da thread %d\n", i);
      }
   }

   printf("--Thread principal terminou!!!\n");

   // chamar a função de verificação do resultado final
   compareArray(array3, array2, N);

   /* printf("Array concorrente:\n");
   printArray(array2, N);
   printf("\nArray sequencial:\n");
   printArray(array3, N); */

   
}
