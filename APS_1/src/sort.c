#include "header/ranking.h"
#include <string.h>

// Selection Sort corrigido (ordena por pontuação)
void selection_sort(struct DESCRITOR *list) {
    if (list->head == NULL || list->head->next == NULL) {
        return;
    }

    struct RANKING *current = list->head;
    
    while (current != NULL) {
        struct RANKING *max = current;
        struct RANKING *r = current->next;
        
        while (r != NULL) {
            if (r->score > max->score) {
                max = r;
            }
            r = r->next;
        }
        
        if (max != current) {
            swapNodes(list, current, max);
            // Se trocamos o current, precisamos atualizá-lo
            if (current == list->head) list->head = max;
            if (max == list->tail) list->tail = current;
            current = max;
        }
        
        current = current->next;
    }
}

// Funções para Merge Sort por nome
struct RANKING* merge_by_name(struct RANKING *first, struct RANKING *second) {
    if (!first) return second;
    if (!second) return first;

    if (strcasecmp(first->nickname, second->nickname) <= 0) {
        first->next = merge_by_name(first->next, second);
        first->next->previous = first;
        first->previous = NULL;
        return first;
    } else {
        second->next = merge_by_name(first, second->next);
        second->next->previous = second;
        second->previous = NULL;
        return second;
    }
}

struct RANKING* merge_sort_by_name(struct RANKING *head) {
    if (!head || !head->next) return head;
    
    struct RANKING *slow = head;
    struct RANKING *fast = head->next;
    
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    
    struct RANKING *mid = slow->next;
    slow->next = NULL;
    
    struct RANKING *left = merge_sort_by_name(head);
    struct RANKING *right = merge_sort_by_name(mid);
    
    return merge_by_name(left, right);
}

void merge_sort_wrapper(struct DESCRITOR *list) {
    if (list->size < 2) return;
    
    list->head = merge_sort_by_name(list->head);
    
    // Atualiza o tail
    struct RANKING *current = list->head;
    while (current->next) {
        current = current->next;
    }
    list->tail = current;
}