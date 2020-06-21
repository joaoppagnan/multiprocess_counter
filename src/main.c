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
int prime_check(unsigned long long num){
  if ((num == 0) || (num == 1)){ /* definicao dos casos base */
    return 0;
  }
  unsigned long long divisor;
  for (divisor = 2; divisor <= num - 1; divisor++){
    if (num%divisor == 0){ /* checa se o numero eh um multiplo do divisor e retorna 0 para o nao-primo*/
      return 0;
    }
  }
  if (divisor == num){ /* se o numero passou por todos os testes de divisor, entao ele eh primo*/
    return 1;
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
  unsigned long long *shared_num;
  shared_num = (unsigned long long*) mmap(NULL, sizeof(unsigned long long*), protection, visibility, 0, 0);

  /* instrucoes para receber uma string de entrada, dividir nos espacos e enviar cada numero para uma pagina da memoria compartilhada */
  char num_str[tam_buffer];
  fgets(num_str, tam_buffer, stdin);
  char* num_ptr = strtok(num_str, " ");
  while (num_ptr != NULL){
    shared_num[index_mem++] = atoll(num_ptr);
    num_ptr = strtok(NULL, " ");
    num_count++;
  }

  /* definicao dos pids dos processos filhos, criando um para cada numero */
  pid_t filho[num_count];

  /* aqui, ele vai fazer um fork() para cada processo, porem, ele so ira realizar o fork() se a variavel que indica o numero de processos em paralelo for menor que o numero maximo de processos definido no comeco do codigo */
  for (int i = 0; i < num_count; i++){
    filho[i] = fork();
    if (filho[i] == 0){ /* cada filho vai acessar a sua pagina de memoria correspondente */
      (*shared_count_ptr) += prime_check(shared_num[i]); /* atualiza a variavel de contagem com o resultado da expressao que checa se eh primo*/
      exit(0);
    }
    ++process_count;
    if (process_count >= N_MAX_PROCESS){/* aqui ele checa o numero de processos em paralelo */ 
      wait(NULL); /* se o programa ver que atingiu o maximo de processos em paralelo, ele espera algum terminar para criar outro */
      --process_count;
    }
  }

  /* nessa parte, ja enviamos todos os numeros para serem checados, entao esperaremos todos os processos filhos terminarem de checar */
  for (int i = 0; i < num_count; i++){
    waitpid(filho[i], NULL, 0);
  }

  /* terminados os processos filhos, nos exibimos o resultado final*/
  printf("%d\n", (*shared_count_ptr));
  return 0;
}
