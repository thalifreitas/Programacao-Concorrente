#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include "timer.h"

//descomentar o define abaixo caso deseje imprimir uma versao truncada da matriz gerada no formato texto
//#define TEXTO

float *matrizUm, *matrizDois; //matrizes de entrada
float *matrizResultante; //matriz de saida
int nthreads; //numero de threads

//verifica se o número de colunas da matriz-um é igual ao número de linhas da matriz-dois
bool verificaMarizes(int colunas, int linhas){
   if(linhas != colunas){
      return false;
   }
   return true;
}


// struct
typedef struct{
    int id; //identificador do elemento que a thread irá processar
    int linhasMatrizUm, colunasMatrizUm, linhasMatrizDois, colunasMatrizDois; //dimensao das estruturas de entrada
} tArgs;


// funcao executada pelas threads
void * tarefa(void *arg){
    tArgs *args = (tArgs*) arg;

    //calcula a multiplicacao das duas matrizes de entrada
    for(int i = args->id; i < args->linhasMatrizUm; i+=nthreads){
        for(int j = 0; j < args->colunasMatrizDois; j++){
            for (int k = 0; k < args->colunasMatrizUm; k++) {
                matrizResultante[i * args->colunasMatrizDois + j] += matrizUm[i * args->colunasMatrizUm + k] * matrizDois[k * args->colunasMatrizDois + j];
            }
        }
    }
    pthread_exit(NULL);
}


// funcao principal
int main(int argc, char *argv[])
{
    int linhasUm, colunasUm, linhasDois, colunasDois; //linhas e colunas da primeira e segunda matriz 
    long long int tamMatrizUm, tamMatrizDois, tamMatrizResultante; //qtde de elementos das matrizes
    FILE *descritorArquivoMatrizUm, *descritorArquivoMatrizDois, *descritorArquivoMatrizResultante;
    tArgs *args; //identificadores locais das threads e dimensoes das matrizes de entrada
    size_t ret;              // retorno da funcao de leitura no arquivo de entrada
    pthread_t *tid; //identificadores das threads
    double inicio, fim, delta;
    
    // Nome do arquivo CSV para armazenar os resultados
    const char *csvFilename = "resultados.csv";
    FILE *csvFile;

    GET_TIME(inicio);

    // recebe os argumentos de entrada
    if (argc < 5){
        fprintf(stderr, "Digite: %s <arquivo primeira matriz> <arquivo segunda matriz> <nome para o arquivo resultante> <qtd de threads>\n", argv[0]);
        return 1;
    }

    nthreads = atoi(argv[4]);

    // abre o arquivo da primeira matriz para leitura binaria
    descritorArquivoMatrizUm = fopen(argv[1], "rb");
    if (!descritorArquivoMatrizUm){
        fprintf(stderr, "Erro de abertura do arquivo da primeira matriz.\n");
        return 2;
    }

    // abre o arquivo da segunda matriz para leitura binaria
    descritorArquivoMatrizDois = fopen(argv[2], "rb");
    if (!descritorArquivoMatrizDois){
        fprintf(stderr, "Erro de abertura do arquivo da segunda matriz.\n");
        return 2;
    }

    // le as dimensoes da primeira matriz
    ret = fread(&linhasUm, sizeof(int), 1, descritorArquivoMatrizUm);
    if (!ret){
        fprintf(stderr, "Erro de leitura das linhas da primeira matriz arquivo.\n");
        return 3;
    }

    ret = fread(&colunasUm, sizeof(int), 1, descritorArquivoMatrizUm);
    if (!ret){
        fprintf(stderr, "Erro de leitura das colunas da primeira matriz arquivo.\n");
        return 3;
    }

    tamMatrizUm = linhasUm * colunasUm; // calcula a qtde de elementos da primeira matriz

    // le as dimensoes da segunda matriz
    ret = fread(&linhasDois, sizeof(int), 1, descritorArquivoMatrizDois);
    if (!ret){
        fprintf(stderr, "Erro de leitura das linhas da segunda matriz arquivo.\n");
        return 3;
    }

    ret = fread(&colunasDois, sizeof(int), 1, descritorArquivoMatrizDois);
    if (!ret){
        fprintf(stderr, "Erro de leitura das colunas da segunda matriz arquivo.\n");
        return 3;
    }

    tamMatrizDois = linhasDois * colunasDois; // calcula a qtde de elementos da segunda matriz

    // aloca memoria para a primeira matriz
    matrizUm = (float *)malloc(sizeof(float) * tamMatrizUm);
    if (!matrizUm){
        fprintf(stderr, "Erro de alocao da memoria da primeira matriz.\n");
        return 3;
    }

    // aloca memoria para a segunda matriz
    matrizDois = (float *)malloc(sizeof(float) * tamMatrizDois);
    if (!matrizDois){
        fprintf(stderr, "Erro de alocao da memoria da segunda matriz.\n");
        return 3;
    }

    if(!verificaMarizes(colunasUm, linhasDois)){
        printf("O número de colunas da primeira matriz deve ser igual ao número de linhas da segunda matriz.\n");
        return 0;
    }

    // carrega a matriz de elementos do tipo float do arquivo da primeira matriz
    ret = fread(matrizUm, sizeof(float), tamMatrizUm, descritorArquivoMatrizUm);
    if (ret < tamMatrizUm){
        fprintf(stderr, "Erro de leitura dos elementos da primeira matriz.\n");
        return 4;
    }

    // carrega a matriz de elementos do tipo float do arquivo da segunda matriz
    ret = fread(matrizDois, sizeof(float), tamMatrizDois, descritorArquivoMatrizDois);
    if (ret < tamMatrizDois){
        fprintf(stderr, "Erro de leitura dos elementos da segunda matriz.\n");
        return 4;
    }

    tamMatrizResultante = linhasUm * colunasDois; 

    //aloca memoria para a matriz resultante
    matrizResultante = (float*) malloc(sizeof(float) * tamMatrizResultante);
    if(!matrizResultante) {
        fprintf(stderr, "Erro de alocao da memoria da matriz resultante.\n");
        return 3;
    }

    //limita o numero de threads ao tamanho da matriz resultante
    if(nthreads > tamMatrizResultante){
        nthreads = tamMatrizResultante;
    }

    // multiplicacao das duas matrizes
    tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
    if (tid==NULL){
        puts("Erro malloc");
        return 2;
    }

    args = (tArgs*) malloc(sizeof(tArgs)*nthreads);
    if (args==NULL){
        puts("Erro malloc");
        return 2;
    }

    //criando as threads
    for (int i=0; i<nthreads; i++){
        (args+i)->id = i;
        (args+i)->linhasMatrizUm = linhasUm;
        (args+i)->colunasMatrizUm = colunasUm;
        (args+i)->linhasMatrizDois = linhasDois;
        (args+i)->colunasMatrizDois = colunasDois;

        if(pthread_create(tid+i, NULL, tarefa, (void*)(args+i))){
            puts("ERRO pthread_create");
            return 3;
        }
    }


    //espera pelo termino das threads
    for (int i=0; i<nthreads; i++){
        pthread_join(*(tid+i), NULL);
    }


    //imprimir na saida padrao a matriz gerada
    #ifdef TEXTO
    for(int i=0; i<linhasUm; i++) {
        for(int j=0; j<colunasDois; j++){
            fprintf(stdout, "%.6f ", matrizResultante[i*colunasDois+j]);
        }
        fprintf(stdout, "\n");
    }
    #endif


    //abre arquivo para escrita binária da matriz resultante
    descritorArquivoMatrizResultante = fopen(argv[3], "wb");
    if (!descritorArquivoMatrizResultante){
        fprintf(stderr, "Erro de abertura do arquivo da matriz resultante.\n");
        return 3;
    }

    //escreve numero de linhas da matriz
    ret = fwrite(&linhasUm, sizeof(int), 1, descritorArquivoMatrizResultante);
    //escreve numero de colunas da matriz
    ret = fwrite(&colunasDois, sizeof(int), 1, descritorArquivoMatrizResultante);
    //escreve os elementos da matriz
    ret = fwrite(matrizResultante, sizeof(float), tamMatrizResultante, descritorArquivoMatrizResultante);

    if (ret < tamMatrizResultante){
        fprintf(stderr, "Erro de escrita no arquivo da matriz resultante.\n");
        return 4;
    }

    //libera a memoria
    free(matrizUm);
    free(matrizDois);
    free(matrizResultante);
    free(args);
    free(tid);

    fclose(descritorArquivoMatrizUm);
    fclose(descritorArquivoMatrizDois);
    fclose(descritorArquivoMatrizResultante);


    GET_TIME(fim);
    delta = fim - inicio;
    printf("Tempo total concorrente:%lf\n", delta);

    // abre o arquivo CSV para adicionar resultados
    csvFile = fopen(csvFilename, "a");
    if (csvFile == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo CSV.\n");
        return 3;
    }

    //Se o arquivo CSV estiver vazio, escreve o cabeçalho
    if (ftell(csvFile) == 0) {
        fprintf(csvFile, "Forma, LinhasUm,ColunasUm,NumThreads,TempoTotal\n");
    }

    // grava os resultados no arquivo CSV
    fprintf(csvFile, "%s, %d,%d,%d,%lf\n", "Concorrente", linhasUm, colunasUm, nthreads, delta);

    fclose(csvFile);

    return 0;

}