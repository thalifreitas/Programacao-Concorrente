#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<pthread.h>
#include "timer.h" 

//descomentar o define abaixo caso deseje imprimir uma versao truncada da matriz gerada no formato texto
//#define TEXTO


//verifica se o número de colunas da matriz-um é igual ao número de linhas da matriz-dois
bool verificaMarizes(int colunas, int linhas){
   if(linhas != colunas){
      return false;
   }
   return true;
}

int main(int argc, char*argv[]) {

   float *matrizUm, *matrizDois, *matrizResultante; //matriz que será carregada do arquivo
   int linhasMatrizUm, colunasMatrizUm, linhasMatrizDois, colunasMatrizDois; //dimensoes da matriz
   long long int tamMatrizUm, tamMatrizDois, tamMatrizResultante; //qtde de elementos na matriz
   FILE * descritorArquivoMatrizUm; //descritor do arquivo de entrada da matriz 1 
   FILE * descritorArquivoMatrizDois; //descritor do arquivo de entrada da matriz 2 
   FILE * descritorArquivoMatrizResultante; 
   double inicio, fim, delta;

   size_t ret; //retorno da funcao de leitura no arquivo de entrada
   
   // Nome do arquivo CSV para armazenar os resultados
    const char *csvFilename = "resultados.csv";
    FILE *csvFile;


   GET_TIME(inicio);

   //recebe os argumentos de entrada
   if(argc < 4) {
      fprintf(stderr, "Digite: %s <arquivo primeira matriz> <arquivo segunda matriz> <nome para o arquivo resultante>\n", argv[0]);
      return 1;
   }
   
   //abre o arquivo da primeira matriz para leitura binaria
   descritorArquivoMatrizUm = fopen(argv[1], "rb");
   if(!descritorArquivoMatrizUm) {
      fprintf(stderr, "Erro de abertura do arquivo da primeira matriz.\n");
      return 2;
   }

   //abre o arquivo da segunda matriz para leitura binaria
   descritorArquivoMatrizDois = fopen(argv[2], "rb");
   if(!descritorArquivoMatrizDois) {
      fprintf(stderr, "Erro de abertura do arquivo da segunda matriz.\n");
      return 2;
   }

   //le as dimensoes da primeira matriz
   ret = fread(&linhasMatrizUm, sizeof(int), 1, descritorArquivoMatrizUm);
   if(!ret) {
      fprintf(stderr, "Erro de leitura das linhas da primeira matriz arquivo.\n");
      return 3;
   }
   ret = fread(&colunasMatrizUm, sizeof(int), 1, descritorArquivoMatrizUm);
   if(!ret) {
      fprintf(stderr, "Erro de leitura das colunas da primeira matriz arquivo.\n");
      return 3;
   }
   tamMatrizUm = linhasMatrizUm * colunasMatrizUm; //calcula a qtde de elementos da primeira matriz


    //le as dimensoes da segunda matriz
   ret = fread(&linhasMatrizDois, sizeof(int), 1, descritorArquivoMatrizDois);
   if(!ret) {
      fprintf(stderr, "Erro de leitura das colunas da segunda matriz arquivo.\n");
      return 3;
   }
   ret = fread(&colunasMatrizDois, sizeof(int), 1, descritorArquivoMatrizDois);
   if(!ret) {
      fprintf(stderr, "Erro de leitura das colunas da segunda matriz arquivo.\n");
      return 3;
   }
   tamMatrizDois = linhasMatrizDois * colunasMatrizDois; //calcula a qtde de elementos da segunda matriz

   //aloca memoria para a primeira matriz
   matrizUm = (float*) malloc(sizeof(float) * tamMatrizUm);
   if(!matrizUm) {
      fprintf(stderr, "Erro de alocao da memoria da primeira matriz.\n");
      return 3;
   }

    //aloca memoria para a segunda matriz
   matrizDois = (float*) malloc(sizeof(float) * tamMatrizDois);
   if(!matrizDois) {
      fprintf(stderr, "Erro de alocao da memoria da segunda matriz.\n");
      return 3;
   }


   //carrega a matriz de elementos do tipo float do arquivo
   ret = fread(matrizUm, sizeof(float), tamMatrizUm, descritorArquivoMatrizUm);
   if(ret < tamMatrizUm) {
      fprintf(stderr, "Erro de leitura dos elementos da primeira matriz.\n");
      return 4;
   }

   //carrega a matriz de elementos do tipo float do arquivo
   ret = fread(matrizDois, sizeof(float), tamMatrizDois, descritorArquivoMatrizDois);
   if(ret < tamMatrizDois) {
      fprintf(stderr, "Erro de leitura dos elementos da segunda matriz.\n");
      return 4;
   }


   if(!verificaMarizes(colunasMatrizUm, linhasMatrizDois)){
      printf("O número de colunas da primeira matriz deve ser igual ao número de linhas da segunda matriz.\n"); 
      return 0;
   }


   tamMatrizResultante = linhasMatrizUm * colunasMatrizDois; //calcula a qtde de elementos da matriz resultante

   //aloca memoria para a matriz resultante
   matrizResultante = (float*) malloc(sizeof(float) * tamMatrizResultante);
   if(!matrizResultante) {
      fprintf(stderr, "Erro de alocao da memoria da matriz resultante.\n");
      return 3;
   }


   //calcula a multiplicacao das duas matrizes de entrada
   for(int i=0; i<linhasMatrizUm; i++){
      for(int j=0; j<colunasMatrizDois; j++){
         for (int k = 0; k < colunasMatrizUm; k++) {
                matrizResultante[i * colunasMatrizDois + j] += matrizUm[i * colunasMatrizUm + k] * matrizDois[k * colunasMatrizDois + j];
         }
      }
   }

   //imprimir na saida padrao a matriz gerada
   #ifdef TEXTO
   for(int i=0; i<linhasMatrizUm; i++) {
      for(int j=0; j<colunasMatrizDois; j++){
         fprintf(stdout, "%.6f ", matrizResultante[i*colunasMatrizDois+j]);
      }
      fprintf(stdout, "\n");
   }
   #endif

   
   //abre arquivo para escrita binária
   descritorArquivoMatrizResultante = fopen(argv[3], "wb");
   if (!descritorArquivoMatrizResultante){
      fprintf(stderr, "Erro de abertura do arquivo da matriz resultante.\n");
      return 3;
   }

   //escreve numero de linhas da matriz
   ret = fwrite(&linhasMatrizUm, sizeof(int), 1, descritorArquivoMatrizResultante);
   //escreve numero de colunas da matriz
   ret = fwrite(&colunasMatrizDois, sizeof(int), 1, descritorArquivoMatrizResultante);
   //escreve os elementos da matriz
   ret = fwrite(matrizResultante, sizeof(float), tamMatrizResultante, descritorArquivoMatrizResultante);

   if (ret < tamMatrizResultante){
      fprintf(stderr, "Erro de escrita no arquivo da matriz resultante.\n");
      return 4;
   }

   //finaliza o uso das variaveis
   fclose(descritorArquivoMatrizUm);
   free(matrizUm);

   //finaliza o uso das variaveis
   fclose(descritorArquivoMatrizDois);
   free(matrizDois);

   //finaliza o uso das variaveis
   fclose(descritorArquivoMatrizResultante);
   free(matrizResultante);

   GET_TIME(fim)   
   delta = fim - inicio;
   printf("Tempo total sequencial:%lf\n", delta);

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
    fprintf(csvFile, "%s, %d,%d,%d,%lf\n", "Sequencial", linhasMatrizUm, colunasMatrizUm, 0, delta);

    fclose(csvFile);

   return 0;
}