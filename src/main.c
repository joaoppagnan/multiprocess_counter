/* Lab 4 - Multiprocess Prime Counter - Joao P. O. Pagnan - 199727 */

/* bibliotecas que serao utilizadas no codigo */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

/* define o numero maximo de processos filhos em paralelo */
#define N_MAX_PROCESS 4

/* define o tamanho do buffer de entrada */
const int tam_buffer = 100;

/* definicao de uma funcao para checar se um numero eh primo */
unsigned int prime_check(unsigned int num){
  unsigned int primo = 0; /* a variavel sera 0 se nao for primo e 1 se for primo */
  if (num == 0){ /* definicao do caso base 0 */
    return primo;
  }
  else if (num == 1){ /* definicao do caso base 1 */
    return primo;
  }
  else {
    for (float divisor = 2; divisor < num; ++divisor){ /* loop para testar as divisoes */
      float quociente_flt;
      int quociente_int;
      quociente_flt = num/divisor;
      quociente_int = (int)quociente_flt;
      if (quociente_int == quociente_flt){ /* se o float e o int forem iguais, entao o resultado eh inteiro */
	return primo; /* se ele ver que o resultado eh inteiro, ele retorna o valor 0, nao primo */
      }
    }
    primo = 1; /* caso contrario ele retorna o 1, significando que o numero eh inteiro */
    return primo;
  }
}

/* funcao principal do codigo */
int main(){
  int num_count = 0; /* variavel para contar o numero de numeros a serem checados */
  int index_mem = 0; /* variavel para mapear as paginas da memoria compartilhada */
  int process_count = 0; /* variavel para contar o numero de processos em paralelo */

  /* parametros da memoria compartilhada */
  int protection = PROT_READ | PROT_WRITE;
  int visibility = MAP_SHARED | MAP_ANON;

  /* contagem compartilhada de quantos primos nos temos na string */
  int *shared_count_ptr;
  shared_count_ptr = (int*) mmap(NULL, sizeof(int), protection, visibility, 0, 0);

  /* memoria mapeada por paginas para o processo pai enviar os numeros para os filhos */
  int *shared_num;
  shared_num = (int*) mmap(NULL, sizeof(int*), protection, visibility, 0, 0);

  /* instrucoes para receber uma string de entrada, dividir nos espacos e enviar cada numero para uma pagina da memoria compartilhada */
  char num_str[tam_buffer];
  fgets(num_str, tam_buffer, stdin);
  char* num_ptr = strtok(num_str, " ");
  while (num_ptr != NULL){
    shared_num[index_mem++] = atoi(num_ptr);
    num_ptr = strtok(NULL, " ");
    num_count++;
  }

  /* definicao dos pids dos processos filhos, criando um para cada numero */
  pid_t filho[num_count];

  /* aqui, ele vai fazer um fork() para cada processo, porem, ele so ira realizar o fork() se a variavel que indica o numero de processos em paralelo for menor que o numero maximo de processos definido no comeco do codigo */
  for (int i = 0; i < num_count; i++){
    filho[i] = fork();
    if (filho[i] == 0){ /* cada filho vai acessar a sua pagina de memoria correspondente */
      /*printf("eu estou no filho %d e estou testando o numero %d\n", i, shared_num[i]);*/
      (*shared_count_ptr) += prime_check(shared_num[i]); /* atualiza a variavel de contagem com o resultado da expressao que checa se eh primo*/
      /*printf("o filho %d atualizou a contagem para %d\n", i, (*shared_count_ptr));*/
      exit(0);
    }
    ++process_count;
    if (process_count >= N_MAX_PROCESS){/* aqui ele checa o numero de processos em paralelo */ 
      /*printf("estou com quatro processos em paralelo, vou esperar algum terminar\n");*/
      wait(NULL); /* se o programa ver que atingiu o maximo de processos em paralelo, ele espera algum terminar para criar outro */
      --process_count;
    }
  }

  /* nessa parte, ja enviamos todos os numeros para serem checados, entao esperaremos todos os processos filhos terminarem de checar */
  for (int i = 0; i < num_count; i++){
    waitpid(filho[i], NULL, 0);
  }

  /* terminado os processos filhos, nos exibimos o resultado final*/
  printf("%d\n", (*shared_count_ptr));
  return 0;
}
