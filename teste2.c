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
  float valor_ing;
  int num_clientes;
  int pagRecusado;
  int ingIndispo;
} Evento;

  FILE *arq;
  FILE *arqout;
  int idCliente = 0;
  sem_t mutex;

bool aprova_venda(void *input) {
  Evento *evento2 = input;
  int aux = rand()%2;
  sleep(rand()%5);
  if (aux == 1) {
    sem_post(&mutex);
    return true;
  } else {
    evento2->pagRecusado++;
    sem_post(&mutex);
    return false;
  }
}

void escolhendoLugar (void *input) {
  Evento *evento2 = input;

  if((arqout = fopen("output.txt", "a")) == NULL)
    printf("*Erro ao abrir o arquivo.*\n");

  fprintf(arqout, "> O cliente %d está escolhendo lugar no espetáculo '%s'\n", idCliente, evento2->nome_evento);
  printf("\n> O cliente %d está escolhendo lugar no espetáculo '%s'\n", idCliente, evento2->nome_evento);

  fclose(arqout);

  return;
}

void escolheuLugar (int aux, void *input) {
  Evento *evento2 = input;

  if((arqout = fopen("output.txt", "a")) == NULL)
    printf("*Erro ao abrir o arquivo.*\n");

  fprintf(arqout, "\n> O cliente %d escolheu o lugar %d no espetáculo '%s' e está aguardando a autorização do pagamento!\n", idCliente, aux, evento2->nome_evento);
  printf("\n> O cliente %d escolheu o lugar %d no espetáculo '%s' e está aguardando a autorização do pagamento!\n", idCliente, aux, evento2->nome_evento);

  fclose(arqout);

  return;
}

void pagamentoAprovado (int aux, void *input) {
  Evento *evento2 = input;

  if((arqout = fopen("output.txt", "a")) == NULL)
    printf("*Erro ao abrir o arquivo.*\n");

  fprintf(arqout, "\n< O cliente %d teve seu pagamento aprovado e fez a compra do lugar %d do espetáculo '%s'\n", idCliente, aux, evento2->nome_evento);
  printf("\n< O cliente %d teve seu pagamento aprovado e fez a compra do lugar %d do espetáculo '%s'\n", idCliente, aux, evento2->nome_evento);
  
  fclose(arqout);
  pthread_exit(evento2);
  
  return;
}

void pagamentoNaoAprovado (void *input) {
  Evento *evento2 = input;

  if((arqout = fopen("output.txt", "a")) == NULL)
    printf("*Erro ao abrir o arquivo.*\n");

  fprintf(arqout, "\n< O pagamento do cliente %d no espetáculo '%s' não foi autorizado\n", idCliente, evento2->nome_evento);
  printf("\n< O pagamento do cliente %d no espetáculo '%s' não foi autorizado\n", idCliente, evento2->nome_evento);

  fclose(arqout);
  pthread_exit(evento2);

  return;
}

int ingDispo (void *input) {
  Evento *evento2 = input;
  int MAX = evento2->max_ing;
  int cont = 0;

  for (int i = 0; i < MAX; i++) {
    if (evento2->ingressos[i] == 0)
      cont++;
  }

  if (cont == 0)
    evento2->ingIndispo++;

  return cont;
}

void* function(void *input) {

  sem_wait(&mutex);
  Evento *evento2 = input;
  int aux = 0;
  int MAX = evento2->max_ing;
  idCliente++;

  do {
    escolhendoLugar(evento2);
    sleep(rand()%5);
    aux = rand()%MAX;
    escolheuLugar(aux, evento2);

    if (evento2->ingressos[aux] != 0) {
      if (ingDispo(evento2) > 0){
        printf("-> Lugar já está ocupado, escolha de novo! %d ingressos disponíveis.\n", ingDispo(evento2));
      } else {
          printf("-> Sem mais ingressos disponíveis!\n");
          pthread_exit(evento2);
          sem_post(&mutex);
        }
    }
  } while (evento2->ingressos[aux] != 0);

  if (aprova_venda(evento2)) {
    evento2->ingressos[aux] = 1;
    pagamentoAprovado(aux, evento2);
  } else {
    pagamentoNaoAprovado(evento2);
    pthread_exit(evento2);
  }

  sem_post(&mutex);

  return 0;
}

int main(int argc, char const *argv[]) {

  const char s[2] = "|";
  char result[90];
  int aux = 0;
  srand(time(NULL));
  Evento evento[10];
  int ingVendido = 0;

  sem_init (&mutex, 0, 1);

  if((arq = fopen("input.txt", "r")) == NULL)
    printf("*Erro ao abrir o arquivo.*\n");

  if((arqout = fopen("output.txt", "a")) == NULL)
    printf("*Erro ao abrir o arquivo.*\n");

  while (fgets(result, 90, arq)) {
    strtok(result, s);
    
    strcpy(evento[aux].nome_evento, result);
    evento[aux].max_ing = atoi(strtok(NULL, s));
    evento[aux].valor_ing = atoi(strtok(NULL, s));  
    evento[aux].num_clientes = atoi(strtok(NULL, s));

    for (int i = 0; i < evento[aux].max_ing; i++)
      evento[aux].ingressos[i] = 0;

    evento[aux].ingIndispo = 0;
    evento[aux].pagRecusado = 0;

    aux++;
  }

  int max[10], max2 = 0;

  for (int i = 0; i < aux; ++i) {
    max[i] = evento[i].num_clientes;
    max2 = max2 + evento[i].num_clientes;
  }
  
  pthread_t* threads = (pthread_t*) malloc(max2 * sizeof(pthread_t));

  for (int j = 0; j < aux; ++j) {
    for (int i = 0; i < max[j]; ++i){
      pthread_create(&threads[i], NULL, function, (void*)&evento[j]);
    }
  }

  for (int i = 0; i < max2; ++i)
    pthread_join(threads[i], NULL);

  for (int j = 0; j < aux; ++j) {
    for (int i = 0; i < evento[j].max_ing; i++) {
      if (evento[j].ingressos[i] == 1)
        ingVendido++;
    }
  
  printf("\n***Espetáculo '%s': %d/%d ingressos vendidos (R$ %.2f faturados); ", evento[j].nome_evento, ingVendido, evento[j].max_ing, ingVendido*evento[j].valor_ing);
  ingVendido = 0;
  if (evento[j].ingIndispo != 0)
    printf("%d/%d clientes sem ingresso: %d por falha no pagamento e %d por indisponibilidade de ingressos;\n", evento[j].num_clientes-ingVendido, evento[j].num_clientes, evento[j].pagRecusado, evento[j].ingIndispo); 
  else
    printf("%d/%d clientes sem ingresso: %d por falha no pagamento;\n", evento[j].num_clientes-ingVendido, evento[j].num_clientes, evento[j].pagRecusado); 
  
  }

  sem_destroy(&mutex);
  free(threads);
  fclose(arq);
  
  return 0;
}