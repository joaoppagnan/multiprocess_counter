/* Lab 4 - Multiprocess Prime Counter - Joao P. O. Pagnan - 199727 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

const int tam_buffer=100;

unsigned int prime_check(unsigned int num){
  unsigned int primo = 0;
  if (num == 0){
    return primo;
  }
  else if (num == 1){
    return primo;
  }
  else {
    for (float divisor = 2; divisor < num; ++divisor){
      float quociente_flt;
      int quociente_int;
      quociente_flt = num/divisor;
      quociente_int = (int)quociente_flt;
      if (quociente_int == quociente_flt){
	return primo;
      }
    }
    primo = 1;
    return primo;
  }
}

int main(){
  int num_count = 0;
  int index_mem = 0; /* variavel para mapear as paginas da memoria compartilhada */
  int process_count = 0; /* variavel para contar o numero de processos */

  /* parametros da memoria compartilhada */
  int protection = PROT_READ | PROT_WRITE;
  int visibility = MAP_SHARED | MAP_ANON;

  /* contagem compartilhada de quantos primos nos temos na string */
  int *shared_count_ptr;
  shared_count_ptr = (int*) mmap(NULL, sizeof(int), protection, visibility, 0, 0);

  /* memoria mapeada para o processo pai enviar os numeros para os filhos */
  int *shared_num;
  shared_num = (int*) mmap(NULL, sizeof(int*), protection, visibility, 0, 0);

  /* instrucoes para receber uma string de entrada, dividir nos espacos e enviar cada numero para uma pagina da memoria compartilhada */
  char num_str[tam_buffer];
  fgets(num_str, tam_buffer, stdin);
  char* num_ptr = strtok(num_str, " ");
  while (num_ptr != NULL){
    shared_num[index_mem] = atoi(num_ptr);
    printf("coloquei o numero %d na posicao %d de memoria\n", atoi(num_ptr), index_mem++);
    num_ptr = strtok(NULL, " ");
    num_count++;
  }

  pid_t filho[num_count];

  for (int i = 0; i < num_count; i++){
    filho[i] = fork();
    if (filho[i] == 0){
      printf("eu estou no filho %d e estou testando o numero %d\n", i, shared_num[i]);
      (*shared_count_ptr) += prime_check(shared_num[i]);
      printf("o filho %d atualizou a contagem para %d\n", i, (*shared_count_ptr));
      exit(0);
    }
  }

  printf("todo os numeros foram enviados para teste, vou esperar todos os filhos terminarem\n");
  for (int i = 0; i < num_count; i++){
    waitpid(filho[i], NULL, 0);
  }

  printf("todos os filhos terminaram, vamos ver o resultado\n");
  printf("%d\n", (*shared_count_ptr));
  return 0;
}
