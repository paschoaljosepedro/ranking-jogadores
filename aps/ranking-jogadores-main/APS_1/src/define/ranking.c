#include "../header/ranking.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define FILENAME "dados.txt"

bool checkIfScoreExists(const char *filePatch);
void saveScoreFile(struct DESCRITOR *d_STRU);
void loadScoreData(struct DESCRITOR *descritor_STRU);
void insertOnScoreList(struct DESCRITOR *descritor_STRU, uint score,
                       const char *nickname);
void showRanking(struct DESCRITOR *descritor_STRU);

void insertOnScoreList(struct DESCRITOR *descritor_STRU, uint score,
                       const char *nickname) {
  struct RANKING *new = malloc(sizeof(struct RANKING));
  if (!new) {
    perror("Erro ao alocar struct de score");
    return;
  }
  new->score = score;
  strncpy(new->nickname, nickname, sizeof(new->nickname) - 1);
  new->nickname[sizeof(new->nickname) - 1] = '\0';
  new->next = NULL;
  new->previous = descritor_STRU->tail;
  if (descritor_STRU->size == 0)
    descritor_STRU->head = new;
  else
    descritor_STRU->tail->next = new;
  descritor_STRU->tail = new;
  descritor_STRU->size++;
}

bool checkIfScoreExists(const char *filePatch) {
  FILE *file = fopen(filePatch, "r");

  if (file) {
    return true;
  }
  return false;
}

void saveScoreFile(struct DESCRITOR *d_STRU) {
  const char *filename = "dados.txt";
  FILE *file = fopen(filename, "w");

  if (file == NULL) {
    perror("Error opening file for writing");
    return;
  }

  struct RANKING *current = d_STRU->head;
  while (current != NULL) {
    if (fprintf(file, "%d,%s\n", current->score, current->nickname) < 0) {
      perror("Error writing to file");
      fclose(file);
      return;
    }
    current = current->next;
  }

  if (fclose(file) != 0) {
    perror("Error closing file");
    return;
  }

  printf("Data successfully saved to %s\n", filename);
}

void loadScoreData(struct DESCRITOR *descritor_STRU) {
  const char *file = "dados.txt";

  if (!checkIfScoreExists(file)) {
    perror("arquivo nao encontrado");
    return;
  }

  FILE *f = fopen(file, "r");
  if (!f) {
    perror("Erro ao abrir arquivo");
    return;
  }

  char linha[100];
  while (fgets(linha, sizeof(linha), f)) {
    // remove o /n do final da linha
    linha[strcspn(linha, "\n")] = 0;

    char *token = strtok(linha, ",");
    int score = atoi(token);
    token = strtok(NULL, ",");
    char nick[25];
    if (token != NULL) {
      strncpy(nick, token, sizeof(nick) - 1);
      nick[sizeof(nick) - 1] = '\0';
      insertOnScoreList(descritor_STRU, score, nick);
    }
  }
  fclose(file);
}

void showRanking(struct DESCRITOR *descritor_STRU) {
  printf("\nRANKING\n");
  struct RANKING *current = descritor_STRU->head;
  int posicao = 1;
  while (current) {
    printf("Posicao : %d |Nick : %-25s | 3Score: %d\n", posicao,
           current->nickname, current->score);
    current = current->next;
    posicao++;
  }
  printf("Total de jogadores: %d\n", descritor_STRU->size);
}