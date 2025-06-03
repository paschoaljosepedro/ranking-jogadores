#include "header/ranking.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Função auxiliar para obter o nó em uma posição específica
struct RANKING* get_node_at_index(struct DESCRITOR *list, int index) {
    if (index < 0 || index >= list->size) return NULL;
    
    struct RANKING *current = list->head;
    for (int i = 0; i < index && current != NULL; i++) {
        current = current->next;
    }
    return current;
}

// Função para trocar dois nós completamente
void swapNodes(struct DESCRITOR *score_list, struct RANKING *a, struct RANKING *b) {
    if (a == b) return;
    
    // Troca os ponteiros previous
    struct RANKING *temp_prev = a->previous;
    a->previous = b->previous;
    b->previous = temp_prev;
    
    if (a->previous) a->previous->next = a;
    if (b->previous) b->previous->next = b;
    
    // Troca os ponteiros next
    struct RANKING *temp_next = a->next;
    a->next = b->next;
    b->next = temp_next;
    
    if (a->next) a->next->previous = a;
    if (b->next) b->next->previous = b;
    
    // Atualiza head e tail se necessário
    if (score_list->head == a) {
        score_list->head = b;
    } else if (score_list->head == b) {
        score_list->head = a;
    }
    
    if (score_list->tail == a) {
        score_list->tail = b;
    } else if (score_list->tail == b) {
        score_list->tail = a;
    }
}

void printScoreList(struct DESCRITOR *descritor_STRU) {
    printf("\n--- Ranking Atual ---\n");
    struct RANKING *current = descritor_STRU->head;
    int position = 1;
    while (current != NULL) {
        printf("%d. %s - %d pontos\n", position, current->nickname, current->score);
        current = current->next;
        position++;
    }
    printf("---------------------\n");
}

void insertOnScoreList(struct DESCRITOR *descritor_STRU, uint score,
                      const char *nickname) {
    struct RANKING *new_node = (struct RANKING *)malloc(sizeof(struct RANKING));
    new_node->score = score;
    strncpy(new_node->nickname, nickname, 24);
    new_node->nickname[24] = '\0';
    new_node->next = NULL;
    new_node->previous = NULL;

    if (descritor_STRU->head == NULL) {
        descritor_STRU->head = new_node;
        descritor_STRU->tail = new_node;
    } else {
        // Sempre insere no final (não mantém ordenado)
        new_node->previous = descritor_STRU->tail;
        descritor_STRU->tail->next = new_node;
        descritor_STRU->tail = new_node;
    }
    descritor_STRU->size++;
    printScoreList(descritor_STRU);
}