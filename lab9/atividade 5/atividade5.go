/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Aluna: Thalita de Freitas Silva */
/* DRE: 120048493 */
/* Laboratório: 9 - Atividade 5 */
/* Uso de canais para comunicação entre goroutines */

/* OBS.: Esse codigo foi feito com base no arquivo channels1.go disponibilizado pela professora no classroom. */

package main

import (
	"fmt"
	"math"
)

/* Funcao para verificar a primalidade de um numero */
func ehPrimo(n int64) int {
	if n <= 1 {
		return 0
	}
	if n == 2 {
		return 1
	}
	if n%2 == 0 {
		return 0
	}
	for i := int64(3); i < int64(math.Sqrt(float64(n)))+1; i += 2 {
		if n%i == 0 {
			return 0
		}
	}
	return 1
}

func main() {

	N := 100 // tamanho da sequencia de numeros gerada
	M := 4   // quantidade de goroutines

	/* Canal valores: guarda os valores gerados pela main e transfere esses valores para a goroutine que ira processar e conferir sua primalidade. */
	valores := make(chan int, N)

	/* Canal done: sera usado para sinalizar o fim do processamento dos valores. */
	done := make(chan bool, M)

	/* Canal primos: guarda a quantidade de primos processada por cada goroutine. */
	primos := make(chan int, M)

	primos_total := 0 //para somar a quantidade de primos processada por cada goroutine individualmente

	for g := 0; g < M; g++ { //gera M goroutines
		go func() {
			cont_primos := 0 //cada goroutine tera sua propria variavel para contador os primos
			for i := range valores {
				if ehPrimo(int64(i)) == 1 {
					cont_primos++ //se o numero for primo, incrementa o contador
				}
			}
			primos <- cont_primos //cada goroutine envia a quantidade de primos processada pelo canal
			//fmt.Println("Todos os valores foram vistos")
			done <- true
		}()
	}

	// gera sequencia de numeros e os envia para o canal 'valores'
	for i := 1; i <= N; i++ {
		valores <- i
		//fmt.Println("Valor enviado", i)
	}

	close(valores) // fecha o canal 'valores', sinalizando que todos os valores gerados ja foram enviados
	//fmt.Println("Todos os valores gerados foram enviados")

	// recebe os resultados de cada goroutine e soma no contador total de primos
	for g := 0; g < M; g++ {
		<-done
		primos_total += <-primos
	}

	close(primos)

	fmt.Println("----- Quantidade de primos: ", primos_total)

	//fmt.Println("Fim")
}
