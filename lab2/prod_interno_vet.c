/* Disciplina: Programacao Concorrente */
/* Profa.: Silvana Rossetto */
/* Aluna: Thalita de Freitas Silva */
/* DRE: 120048493 */
/* Laboratório: 2 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// variaveis globais
//tamanho do vetor
long int n;
//vetor de elementos
float *vet1, *vet2;
//numero de threads
int nthreads;

//funcao criada para calcular a variacao relativa
void variacaoRelativa (float valor_seq, float valor_conc){
    float vr = (valor_seq-valor_conc)/valor_seq;
    fprintf(stdout, "\nVariacao Relativa                  =  %.26f\n", vr);
}


//funcao executada pelas threads
void *produtoInternoVetor(void *tid){
    long int id = (long int) tid; //identificador da thread
    int inicio, fim, bloco; //auxiliares para divisao do vetor em blocos
    float soma_local, *ret; //somatorio local

    bloco = n/nthreads; //tamanho do bloco de dados de cada thread
    inicio = id*bloco; //posicao inicial do vetor
    fim = inicio + bloco; //posicao final do vetor
    if (id == (nthreads-1)){
        //a ultima thread trata os elementos restantes caso a divisao n/nthreads nao seja exata
        fim = n;
    }

    //faz o produto interno
    for (int i = inicio; i < fim; i++){
        soma_local += (vet1[i]*vet2[i]);
    }

    //retorna o resultado do produto interno
    ret = malloc(sizeof(float));
    if (ret!=NULL){
        *ret = soma_local;
    }else {
        printf("--ERRO: malloc() thread\n");
    }
    pthread_exit((void*) ret);

}


//funcao principal
int main(int argc, char *argv[]){
    FILE *arquivo; //arquivo de entrada
    size_t ret; //retorno da funcao de leitura no arquivo de entrada

    double resultado_seq; //resultado do produto interno sequencial registrada no arquivo
    float resultado_conc; //resultado do produto interno concorrente
    float *soma_retorno_threads; //auxiliar para retorno das threads

    pthread_t *tid_sistema; //identificador das threads

    //valida e recebe os valores de entrada
    if (argc < 3){
        printf("Use: %s <arquivo de entrada> <numero de threads> \n", argv[0]); 
        exit(-1);
    }

    //abre o arquivo de entrada
    arquivo = fopen(argv[1], "rb");
    if (arquivo == NULL){
        printf("--ERRO: fopen()\n"); 
        exit(-1);
    }

    //lê o tamanho do vetor
    ret = fread(&n, sizeof(long int), 1, arquivo);
    if(!ret){
        fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
        return 3;
    }

    //aloca espaco de memoria para os vetores
    vet1 = malloc(sizeof(float)*n);
    vet2 = malloc(sizeof(float)*n);
    if (vet1 == NULL || vet2 == NULL){
        printf("--ERRO: malloc()\n");
        exit(-1);
    }
    //carrega os vetores de entrada
    ret = fread(vet1, sizeof(float), n, arquivo);
    ret = fread(vet2, sizeof(float), n, arquivo);
    if (ret < n){
        fprintf(stderr, "Erro de leitura dos elementos do vetor\n");
        return 4;
    }
    
    //le o numero de threads da entrada do usuario
    nthreads = atoi(argv[2]);
    //limita o numero de threads ao tamanho do vetor
    if (nthreads>n){
        nthreads=n;
    }
    
    //aloca espaco para o vetor de identificadores das threads
    tid_sistema = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
    if (tid_sistema==NULL){
        printf("--ERRO: malloc()\n");
        exit(-1);
    }
    
    //cria as threads
    for (long int i = 0; i < nthreads; i++){
        if (pthread_create(&tid_sistema[i], NULL, produtoInternoVetor, (void*) i)){
            printf("--ERRO: pthread_create()\n");
            exit(-1);
        }
    }
    
    //espera todas as threads terminarem e calcula a soma total das threads
    resultado_conc = 0;
    for (int i = 0; i < nthreads; i++){
        if (pthread_join(tid_sistema[i], (void*) &soma_retorno_threads)){
            printf("--ERRO: pthread_join()\n");
            exit(-1);
        }
        resultado_conc += *soma_retorno_threads;
        free(soma_retorno_threads);        
    }

    // imprime os resultados
    printf("\n");
    printf("Produto interno concorrente               = %.26f\n", resultado_conc);
    
    //lê o resultado registrado no arquivo
    ret = fread(&resultado_seq, sizeof(double), 1, arquivo);
    printf("\nProduto interno original                 = %.26lf\n", resultado_seq);

    //imprime resultado da variacao relaiva
    variacaoRelativa(resultado_seq, resultado_conc);


    //desaloca os espacos de memoria
    free(vet1);
    free(vet2);
    free(tid_sistema);

    //fecha o arquivo
    fclose(arquivo);

    return 0;

}