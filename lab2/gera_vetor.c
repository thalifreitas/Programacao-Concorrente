/* Disciplina: Programacao Concorrente */
/* Profa.: Silvana Rossetto */
/* Aluna: Thalita de Freitas Silva */
/* DRE: 120048493 */
/* Laboratório: 2 */


/*
Programa auxiliar para gerar um vetor de floats 
*/


#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TEXTO
#define MAXIMO 1000 //valor maximo de um elemento do vetor

int main(int argc, char *argv[]){
    float *vetor1, *vetor2; //vetores que serão gerados
    long int n; //qtde de elementos do vetor que serão gerados
    float elem1, elem2; //valor gerado para incluir no vetor
    double resultado=0; //resultado do produto interno entre os vetores
    int fator=1; //fator multiplicador para gerar números negativos
    FILE * descritorArquivo; //descritor do arquivo saída
    size_t ret; //retorno da função de escrita no arquivo de saída

    // recebe os argumentos de entrada
    if(argc < 3){
        fprintf(stderr, "Digite: %s <dimensao> <arquivo saida>\n", argv[0]);
        return 1;
    }
    n = atoi(argv[1]);

    //aloca memoria para os vetores
    vetor1 = (float*) malloc(sizeof(float)*n);
    vetor2 = (float*) malloc(sizeof(float)*n);
    if(!vetor1 || !vetor2){
        fprintf(stderr, "Erro de alocao da memoria do vetor\n");
        return 2;
    }


    //preenche os vetores com valores float aleatorios
    srand(time(NULL));
    for (long int i = 0; i < n; i++){
        elem1 = (rand() % MAXIMO)/3.0 * fator;
        elem2 = (rand() % MAXIMO)/3.0 * fator;

        vetor1[i] = elem1;
        vetor2[i] = elem2;

        resultado += (elem1*elem2); //guardando o produto de dois elementos dos vetores
        fator*=-1;
    }

    //imprime na saida padrao o vetor gerado
    #ifdef TEXTO
    fprintf(stdout, "%ld\n", n);
    for (long int i = 0; i < n; i++){
        fprintf(stdout, "%f", vetor1[i]);
    }
    printf("\n");
    for (long int i = 0; i < n; i++){
        fprintf(stdout, "%f", vetor2[i]);
    }
    fprintf(stdout, "\n");
    fprintf(stdout, "soma=%lf\n", resultado);    
    #endif


    // escreve os vetores no arquivo binário
    //abre o arquivo para escrita binária
    descritorArquivo = fopen(argv[2], "wb");
    if (!descritorArquivo){
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return 3;
    }

    //escreve a dimensao
    ret = fwrite(&n, sizeof(long int), 1, descritorArquivo);

    //escreve os elementos dos vetores
    ret = fwrite(vetor1, sizeof(float), n, descritorArquivo);
    ret = fwrite(vetor2, sizeof(float), n, descritorArquivo);

    if (ret < n){
        fprintf(stderr, "Erro de escrita no  arquivo\n");
        return 4;
    }

    //escreve o resultado
    ret = fwrite(&resultado, sizeof(double), 1, descritorArquivo);

    //finaliza o uso das variaveis
    fclose(descritorArquivo);
    free(vetor1);
    free(vetor2);
    
    return 0;

}

