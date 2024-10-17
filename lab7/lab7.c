/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Aluna: Thalita de Freitas Silva */
/* DRE: 120048493 */
/* Laboratório: 7 */
/* Programa concorrente para ler, manipular e imprimir na tela os caracteres de um arquivo texto, usando semaforos para tratar as demandas de sincronizacao. */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <semaphore.h>

#define NTHREADS 3 // Número de threads
#define N_BUFFER 10 // Tamanho dos buffers

//variáveis globais
sem_t semaf1_vazio, semaf1_cheio, semaf2_vazio, semaf2_cheio;
char buffer1[N_BUFFER], buffer2[N_BUFFER];
int buffer1_inseridos = 0, buffer1_remove = 0, buffer2_inseridos = 0, buffer2_remove = 0;


// lê os caracteres do arquivo de entrada
void *thread1 (void *arg){
    int c;

    FILE *arquivo = fopen("entrada.txt", "r");
    if (arquivo == NULL) {
        printf("--ERRO: fopen()\n");
        pthread_exit(NULL);
    }
    
    while ((c = fgetc(arquivo)) != EOF) {
        sem_wait(&semaf1_vazio); //aguarda espaço vazio no buffer1

        buffer1[buffer1_inseridos] = (char)c;
        buffer1_inseridos = (buffer1_inseridos + 1) % N_BUFFER; //índice circular

        sem_post(&semaf1_cheio); //sinaliza que há um novo caractere no buffer1
    }

    sem_wait(&semaf1_vazio);
    buffer1[buffer1_inseridos] = '\0';
    buffer1_inseridos = (buffer1_inseridos + 1) % N_BUFFER;
    sem_post(&semaf1_cheio);

    fclose(arquivo);
    pthread_exit(NULL);
}

// processa os caracteres do buffer1 e os deposita no buffer2
void *thread2 (void *arg) {
    int n = 0;
    int count = 0;
    char c;
    
    while (1) {
        sem_wait(&semaf1_cheio); //aguarda caracteres disponíveis no buffer1

        c = buffer1[buffer1_remove];
        buffer1_remove = (buffer1_remove + 1) % N_BUFFER; //índice circular
        sem_post(&semaf1_vazio); //sinaliza que há espaço vazio no buffer1

        if (c == '\0') {
            sem_wait(&semaf2_vazio);
            buffer2[buffer2_inseridos] = '\0'; 
            buffer2_inseridos = (buffer2_inseridos + 1) % N_BUFFER;
            sem_post(&semaf2_cheio);
            break;
        }

        //verifica se é hora de inserir uma quebra de linha
        if (count == (2 * n + 1)) { 
            sem_wait(&semaf2_vazio);
            buffer2[buffer2_inseridos] = '\n';
            buffer2_inseridos = (buffer2_inseridos + 1) % N_BUFFER;
            sem_post(&semaf2_cheio);

            count = 0;
            n++;
        } else if (count==10 && n>10) {
            sem_wait(&semaf2_vazio);
            buffer2[buffer2_inseridos] = '\n';
            buffer2_inseridos = (buffer2_inseridos + 1) % N_BUFFER;
            sem_post(&semaf2_cheio);

            count = 0;
        } else {
            if(c == '\0'){
                break;
            }
            //inserir o caractere processado no buffer2
            sem_wait(&semaf2_vazio);  //aguarda espaço vazio no buffer2
            buffer2[buffer2_inseridos] = c; 
            buffer2_inseridos = (buffer2_inseridos + 1) % N_BUFFER;
            sem_post(&semaf2_cheio);  //sinaliza que há um novo caractere no buffer2

            count++;
        }        
    }
    pthread_exit(NULL);
}

// imprime os caracteres do buffer 2
void *thread3 (void *arg){
    char c;
    
    while (1) {
        sem_wait(&semaf2_cheio); //aguarda caracteres disponíveis no buffer2

        c = buffer2[buffer2_remove];
        buffer2_remove = (buffer2_remove + 1) % N_BUFFER; //índice circular
        sem_post(&semaf2_vazio); //sinaliza que há espaço vazio no buffer2

        if (c == '\0') {
            break;
        }

        //imprime o caractere lido
        printf("%c", c);
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]){
    pthread_t tid[NTHREADS];

    //inicializa os semáforos
    sem_init(&semaf1_cheio, 0, 0);
    sem_init(&semaf1_vazio, 0, N_BUFFER);
    sem_init(&semaf2_cheio, 0, 0);
    sem_init(&semaf2_vazio, 0, N_BUFFER);

    //cria as threads
    if (pthread_create(&tid[0], NULL, thread1, NULL)) {
        printf("--ERRO: pthread_create() para thread1\n");
        exit(-1);
    }

    if (pthread_create(&tid[1], NULL, thread2, NULL)) {
        printf("--ERRO: pthread_create() para thread2\n");
        exit(-1);
    }

    if (pthread_create(&tid[2], NULL, thread3, NULL)) {
        printf("--ERRO: pthread_create() para thread3\n");
        exit(-1);
    }

    //espera todas as threads terminarem
    for (int t=0; t<NTHREADS; t++) {
    if (pthread_join(tid[t], NULL)) {
         printf("--ERRO: pthread_join() \n"); exit(-1); 
    } 
  } 

    // Finaliza os semáforos
    sem_destroy(&semaf1_cheio);
    sem_destroy(&semaf1_vazio);
    sem_destroy(&semaf2_cheio);
    sem_destroy(&semaf2_vazio);

    return 0;
}
