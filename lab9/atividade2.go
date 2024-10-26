/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Aluna: Thalita de Freitas Silva */
/* DRE: 120048493 */
/* Laboratório: 9 - Atividade 2 */
/* Uso de canais para comunicação entre goroutines */

package main

import (
	"fmt" 
)

func tarefa(str chan string) { 
	var msg = "" //string vazia

	msg = <- str
	fmt.Println("Goroutine imprime: ",msg)

	str <- "Oi Main, bom dia, tudo bem?" //escreve no canal. 

	msg = <- str
	fmt.Println("Goroutine imprime: ",msg)

	str <- "Certo, entendido."

	msg =<- str
	fmt.Println("Goroutine imprime: ", msg)

	str <- "finalizando..." 
}

func main() {
	var msg = "" //string vazia
	
	str := make(chan string) 

	//cria um goroutine que executará a função 'tarefa'
	go tarefa(str)

	str <- "Olá, Goroutine, bom dia!"

	//lê do canal
	msg = <-str
	fmt.Println("Main imprime: ",msg)

	str <- "Tudo bem! Vou terminar tá?"

	msg =<- str
	fmt.Println("Main imprime: ",msg)

	str <- "finalizando..."

	msg =<- str
	fmt.Println("Main imprime: ", msg)

}
