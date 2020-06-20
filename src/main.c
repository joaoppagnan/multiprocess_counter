/* Lab 4 - Multiprocess Prime Counter - Joao P. O. Pagnan - 199727 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int tam_buffer=100;

int prime_check(unsigned int num){
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
  unsigned int num_teste = 0;
  unsigned int contagem_primos = 0;
  char num_str[tam_buffer];
  fgets(num_str, tam_buffer, stdin);
  char* num_ptr = strtok(num_str, " ");
  while (num_ptr != NULL){
    num_teste = atoi(num_ptr);
    contagem_primos += prime_check(num_teste);
    num_ptr = strtok(NULL, " ");
  }
  printf("numero de primos na string: %d\n", contagem_primos);
  return 0;
}
