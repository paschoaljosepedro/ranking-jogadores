#include "../header/ranking.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct RANKING *binarySearchRanking(const struct DESCRITOR *desc_stru,
                                    int targetScore);

struct RANKING *binarySearchRanking(const struct DESCRITOR *desc_stru,
                                    int targetScore) {
  int left = 0;
  int right = desc_stru->size - 1;
  struct RANKING *result = NULL;

  if (desc_stru == NULL || desc_stru->head == NULL) {
    fprintf(stderr, "Erro: Descritor vazio ou nulo\n");
    return NULL;
  }

    if (desc_stru->head->score == targetScore) {
    return desc_stru->head;
  }

  while (left <= right) {
    int mid = left + (right - left) / 2;

    struct RANKING *current = desc_stru->head;
    for (int i = 0; i < mid && current != NULL; i++) {
      current = current->next;
    }

    if (current == NULL) {
      break;
    }

    if (current->score == targetScore) {
      result = current;
      right = mid - 1;
    } else if (current->score < targetScore) {
      right = mid - 1;
    } else {
      left = mid + 1;
    }
  }

  return result;
}