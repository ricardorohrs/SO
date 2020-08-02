#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct {
  char nome_evento[15];
  int max_ing;
  int ingressos[15];
  int valor_ing;
  int num_clientes;
} Evento;

  FILE *arq;
  FILE *arqout;
  int idCliente = 0;

  sem_t semaf;

bool aprova_venda() {
  int aux = rand()%2;
  sleep(rand()%5); //tempo de espera para aprovar a compra
  if (aux == 1)
    return true;
  else 
    return false;
}

void escolhendoLugar (void *input) {
  Evento *evento2 = input;

  if((arqout = fopen("output.txt", "a")) == NULL)
    printf("Erro ao abrir o arquivo.\n");

  fprintf(arqout, "O cliente %d está escolhendo lugar no espetáculo '%s'\n", idCliente, evento2->nome_evento);
  printf("\nO cliente %d está escolhendo lugar no espetáculo '%s'\n", idCliente, evento2->nome_evento);

  fclose(arqout);

  return;
}

void escolheuLugar (int aux, void *input) {
  Evento *evento2 = input;

  if((arqout = fopen("output.txt", "a")) == NULL)
    printf("Erro ao abrir o arquivo.\n");

  fprintf(arqout, "\nO cliente %d escolheu o lugar %d no espetáculo '%s' e está aguardando a autorização do pagamento!\n", idCliente, aux, evento2->nome_evento);
  printf("\nO cliente %d escolheu o lugar %d no espetáculo '%s' e está aguardando a autorização do pagamento!\n", idCliente, aux, evento2->nome_evento);

  fclose(arqout);

  return;
}

void pagamentoAprovado (int aux, void *input) {
  Evento *evento2 = input;

  if((arqout = fopen("output.txt", "a")) == NULL)
    printf("Erro ao abrir o arquivo.\n");

  fprintf(arqout, "\nO cliente %d teve seu pagamento aprovado e fez a compra do lugar %d do espetáculo '%s'\n", idCliente, aux, evento2->nome_evento);
  printf("\nO cliente %d teve seu pagamento aprovado e fez a compra do lugar %d do espetáculo '%s'\n", idCliente, aux, evento2->nome_evento);
  
  fclose(arqout);
  pthread_exit(evento2);
  
  return;
}

void pagamentoNaoAprovado (void *input) {
  Evento *evento2 = input;

  if((arqout = fopen("output.txt", "a")) == NULL)
    printf("Erro ao abrir o arquivo.\n");

  fprintf(arqout, "\nO pagamento do cliente %d no espetáculo '%s' não foi autorizado\n", idCliente, evento2->nome_evento);
  printf("\nO pagamento do cliente %d no espetáculo '%s' não foi autorizado\n", idCliente, evento2->nome_evento);

  fclose(arqout);
  pthread_exit(evento2);

  return;
}

void* function(void *input) {
  Evento *evento2 = input;

  int MAX = evento2->max_ing;

  escolhendoLugar(evento2);

  sleep(rand()%5); //tempo de espera pra escolher
  int aux = rand()%MAX; //escolhendo o lugar

  escolheuLugar(aux, evento2);
  if (aprova_venda()) {
    if (evento2->ingressos[aux] == 0) {
      evento2->ingressos[aux] = 1;
      pagamentoAprovado(aux, evento2);
/*    for (int i = 0; i < MAX; i++)
      printf("%d\n", evento2->ingressos[i]);*/
    } else {
      printf("Lugar já está ocupado!\n");
      pthread_exit(evento2);
    } 
  } else {
      pagamentoNaoAprovado(evento2);
      pthread_exit(evento2);
    }
}

int main(int argc, char const *argv[]) {

  const char s[2] = "|";
  char *token;
  char result[90];
  int aux = 0;
  srand(time(NULL));
  Evento evento[10];

  sem_init (&semaf, 0, 1);

  if((arq = fopen("input.txt", "r")) == NULL)
    printf("Erro ao abrir o arquivo.\n");

  if((arqout = fopen("output.txt", "a")) == NULL)
    printf("Erro ao abrir o arquivo.\n");

  while (fgets(result, 90, arq)) {
    token = strtok(result, s);
    
    strcpy(evento[aux].nome_evento, result);
    evento[aux].max_ing = atoi(strtok(NULL, s));
    evento[aux].valor_ing = atoi(strtok(NULL, s));  
    evento[aux].num_clientes = atoi(strtok(NULL, s));

/*    printf("%s\n", evento[aux].nome_evento);
    printf("max_ing %d\n", evento[aux].max_ing);
    printf("valor_ing %d\n", evento[aux].valor_ing);
    printf("num_clientes %d\n\n", evento[aux].num_clientes);*/

    for (int i = 0; i <= evento[aux].max_ing; i++) {
      evento[aux].ingressos[i] = 0;
    }

    aux++;
  }

  int max[5], max2 = 0;

  for (int i = 0; i < aux; ++i) {
    max[i] = evento[i].num_clientes;
    max2 = max2 + evento[i].num_clientes;
  }
  
  pthread_t* threads = (pthread_t*) malloc(max2 * sizeof(pthread_t));

  for (int j = 0; j < aux; ++j) {
    for (int i = 0; i < max[j]; ++i) {
      idCliente++;
      pthread_create(&threads[i], NULL, function, (void*)&evento[j]);
      pthread_join(threads[j], NULL);
    }
  }
    
/*    for (int i = 0; i < max[0]; ++i) {
      idCliente++;
      pthread_create(&threads[i], NULL, function, (void*)&evento[0]);
      pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < max[1]; ++i) {
      idCliente++;
      pthread_create(&threads[i], NULL, function, (void*)&evento[1]);
      pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < max[2]; ++i) {
      idCliente++;
      pthread_create(&threads[i], NULL, function, (void*)&evento[2]);
      pthread_join(threads[i], NULL);
    }*/

  for (int j = 0; j < aux; ++j) {
    for (int i = 0; i < max[j]; ++i) {
      pthread_join(threads[i], NULL);
    }
  }

  fclose(arq);
  
  return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct {
  char nome_evento[15];
  int max_ing;
  int ingressos[15];
  int valor_ing;
  int num_clientes;
} Evento;

  FILE *arq;
  FILE *arqout;
  int idCliente = 0;

  sem_t semaf;

bool aprova_venda() {
  int aux = rand()%2;
  sleep(rand()%5); //tempo de espera para aprovar a compra
  if (aux == 1)
    return true;
  else 
    return false;
}

void escolhendoLugar (void *input) {
  Evento *evento2 = input;

  if((arqout = fopen("output.txt", "a")) == NULL)
    printf("Erro ao abrir o arquivo.\n");

  fprintf(arqout, "O cliente %d está escolhendo lugar no espetáculo '%s'\n", idCliente, evento2->nome_evento);
  printf("\nO cliente %d está escolhendo lugar no espetáculo '%s'\n", idCliente, evento2->nome_evento);

  fclose(arqout);

  return;
}

void escolheuLugar (int aux, void *input) {
  Evento *evento2 = input;

  if((arqout = fopen("output.txt", "a")) == NULL)
    printf("Erro ao abrir o arquivo.\n");

  fprintf(arqout, "\nO cliente %d escolheu o lugar %d no espetáculo '%s' e está aguardando a autorização do pagamento!\n", idCliente, aux, evento2->nome_evento);
  printf("\nO cliente %d escolheu o lugar %d no espetáculo '%s' e está aguardando a autorização do pagamento!\n", idCliente, aux, evento2->nome_evento);

  fclose(arqout);

  return;
}

void pagamentoAprovado (int aux, void *input) {
  Evento *evento2 = input;

  if((arqout = fopen("output.txt", "a")) == NULL)
    printf("Erro ao abrir o arquivo.\n");

  fprintf(arqout, "\nO cliente %d teve seu pagamento aprovado e fez a compra do lugar %d do espetáculo '%s'\n", idCliente, aux, evento2->nome_evento);
  printf("\nO cliente %d teve seu pagamento aprovado e fez a compra do lugar %d do espetáculo '%s'\n", idCliente, aux, evento2->nome_evento);
  
  fclose(arqout);
  pthread_exit(evento2);
  
  return;
}

void pagamentoNaoAprovado (void *input) {
  Evento *evento2 = input;

  if((arqout = fopen("output.txt", "a")) == NULL)
    printf("Erro ao abrir o arquivo.\n");

  fprintf(arqout, "\nO pagamento do cliente %d no espetáculo '%s' não foi autorizado\n", idCliente, evento2->nome_evento);
  printf("\nO pagamento do cliente %d no espetáculo '%s' não foi autorizado\n", idCliente, evento2->nome_evento);

  fclose(arqout);
  pthread_exit(evento2);

  return;
}

void* function(void *input) {
  Evento *evento2 = input;
  int aux = 0;
  int MAX = evento2->max_ing;

  do {
    escolhendoLugar(evento2);

    sleep(rand()%5); //tempo de espera pra escolher
    aux = rand()%MAX; //escolhendo o lugar

    escolheuLugar(aux, evento2);
    if (evento2->ingressos[aux] != 0) {
      printf("Lugar já está ocupado, escolha de novo!\n");
    }
  } while (evento2->ingressos[aux] != 0);

  if (aprova_venda()) {
    evento2->ingressos[aux] = 1;
    pagamentoAprovado(aux, evento2);
  } else {
    pagamentoNaoAprovado(evento2);
    pthread_exit(evento2);
  }

}

int main(int argc, char const *argv[]) {

  const char s[2] = "|";
  char *token;
  char result[90];
  int aux = 0;
  srand(time(NULL));
  Evento evento[10];

  sem_init (&semaf, 0, 1);

  if((arq = fopen("input.txt", "r")) == NULL)
    printf("Erro ao abrir o arquivo.\n");

  if((arqout = fopen("output.txt", "a")) == NULL)
    printf("Erro ao abrir o arquivo.\n");

  while (fgets(result, 90, arq)) {
    token = strtok(result, s);
    
    strcpy(evento[aux].nome_evento, result);
    evento[aux].max_ing = atoi(strtok(NULL, s));
    evento[aux].valor_ing = atoi(strtok(NULL, s));  
    evento[aux].num_clientes = atoi(strtok(NULL, s));

/*    printf("%s\n", evento[aux].nome_evento);
    printf("max_ing %d\n", evento[aux].max_ing);
    printf("valor_ing %d\n", evento[aux].valor_ing);
    printf("num_clientes %d\n\n", evento[aux].num_clientes);*/

    for (int i = 0; i <= evento[aux].max_ing; i++) {
      evento[aux].ingressos[i] = 0;
    }

    aux++;
  }

  int max[5], max2 = 0;

  for (int i = 0; i < aux; ++i) {
    max[i] = evento[i].num_clientes;
    max2 = max2 + evento[i].num_clientes;
  }
  
  pthread_t* threads = (pthread_t*) malloc(max2 * sizeof(pthread_t));

  for (int j = 0; j < aux; ++j) {
    for (int i = 0; i < max[j]; ++i) {
      idCliente++;
      pthread_create(&threads[i], NULL, function, (void*)&evento[j]);
      pthread_join(threads[i], NULL);
    }
  }


    
/*    for (int i = 0; i < max[0]; ++i) {
      idCliente++;
      pthread_create(&threads[i], NULL, function, (void*)&evento[0]);
      pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < max[1]; ++i) {
      idCliente++;
      pthread_create(&threads[i], NULL, function, (void*)&evento[1]);
      pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < max[2]; ++i) {
      idCliente++;
      pthread_create(&threads[i], NULL, function, (void*)&evento[2]);
      pthread_join(threads[i], NULL);
    }*/

  for (int j = 0; j < aux; ++j) {
    for (int i = 0; i < max[j]; ++i) {
      pthread_join(threads[i], NULL);
    }
  }

  fclose(arq);
  
  return 0;
}