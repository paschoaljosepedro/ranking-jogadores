#include "header/ranking.h"
#include <string.h>
#include <stdio.h>

// Pesquisa Binária por nome
void binary_search(char search_results[1024], struct DESCRITOR *list, const char *nickname) {
    search_results[0] = '\0';
    
    if (list->head == NULL) {
        strcat(search_results, "Lista vazia!\n");
        return;
    }
    
    int left = 0;
    int right = list->size - 1;
    int found = 0;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        struct RANKING *mid_node = get_node_at_index(list, mid);
        
        if (mid_node == NULL) break;
        
        int cmp = strcasecmp(mid_node->nickname, nickname);
        if (cmp == 0) {
            char buffer[256];
            sprintf(buffer, "Jogador '%s' encontrado na posicao %d: Pontuacao %d\n", 
                   nickname, mid + 1, mid_node->score);
            strcat(search_results, buffer);
            found = 1;
            
            // Mostra todas as ocorrências (pode haver nomes iguais)
            int i = mid - 1;
            struct RANKING *prev = get_node_at_index(list, i);
            while (i >= 0 && prev != NULL && strcasecmp(prev->nickname, nickname) == 0) {
                sprintf(buffer, "Jogador '%s' encontrado na posicao %d: Pontuacao %d\n", 
                       nickname, i + 1, prev->score);
                strcat(search_results, buffer);
                i--;
                prev = get_node_at_index(list, i);
            }
            
            i = mid + 1;
            struct RANKING *next = get_node_at_index(list, i);
            while (i < list->size && next != NULL && strcasecmp(next->nickname, nickname) == 0) {
                sprintf(buffer, "Jogador '%s' encontrado na posicao %d: Pontuacao %d\n", 
                       nickname, i + 1, next->score);
                strcat(search_results, buffer);
                i++;
                next = get_node_at_index(list, i);
            }
            break;
        }
        
        if (cmp < 0) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    
    if (!found) {
        sprintf(search_results, "Jogador '%s' nao encontrado na lista.\n", nickname);
    }
}

// Pesquisa Exponencial (por pontuação)
void exponential_search(char search_results[1024], struct DESCRITOR *list, int score) {
    search_results[0] = '\0';
    
    if (list->head == NULL) {
        strcat(search_results, "Lista vazia!\n");
        return;
    }
    
    // Verifica se está no primeiro elemento
    if (list->head->score == score) {
        char buffer[256];
        sprintf(buffer, "Pontuacao %d encontrada na posicao 1: %s\n", 
               score, list->head->nickname);
        strcat(search_results, buffer);
        return;
    }
    
    // Encontra o intervalo
    int i = 1;
    while (i < list->size) {
        struct RANKING *node = get_node_at_index(list, i);
        if (node == NULL) break;
        
        if (node->score <= score) {
            break;
        }
        i *= 2;
    }
    
    // Faz busca binária no intervalo encontrado
    int left = i / 2;
    int right = (i < list->size) ? i : list->size - 1;
    int found = 0;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        struct RANKING *mid_node = get_node_at_index(list, mid);
        
        if (mid_node == NULL) break;
        
        if (mid_node->score == score) {
            char buffer[256];
            sprintf(buffer, "Pontuacao %d encontrada na posicao %d: %s\n", 
                   score, mid + 1, mid_node->nickname);
            strcat(search_results, buffer);
            found = 1;
            
            // Mostra todas as ocorrências
            int i = mid - 1;
            struct RANKING *prev = get_node_at_index(list, i);
            while (i >= 0 && prev != NULL && prev->score == score) {
                sprintf(buffer, "Pontuacao %d encontrada na posicao %d: %s\n", 
                       score, i + 1, prev->nickname);
                strcat(search_results, buffer);
                i--;
                prev = get_node_at_index(list, i);
            }
            
            i = mid + 1;
            struct RANKING *next = get_node_at_index(list, i);
            while (i < list->size && next != NULL && next->score == score) {
                sprintf(buffer, "Pontuacao %d encontrada na posicao %d: %s\n", 
                       score, i + 1, next->nickname);
                strcat(search_results, buffer);
                i++;
                next = get_node_at_index(list, i);
            }
            break;
        }
        
        if (mid_node->score > score) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    
    if (!found) {
        sprintf(search_results, "Pontuacao %d nao encontrada na lista.\n", score);
    }
}