#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define MENU_ITEMS 5
#define SUBMENU1_ITEMS 3
#define SUBMENU2_ITEMS 3
#define BAR_HEIGHT 50
#define ITEM_HEIGHT 50
#define ITEM_PADDING 20

/* Variáveis globais */
int current_menu = 0;
int current_submenu = 0;
bool adding_score = false;
bool showing_rankings = false;
bool searching_score = false;
char input_nickname[25] = "";
char input_score[10] = "";
char search_score[25] = "";
char search_results[1024] = "";
int input_pos = 0;
int window_width = 900;
int window_height = 500;
int selected_menu_item = 0;  // Moved to global variables section

/* Estruturas de dados */
struct RANKING {
  struct RANKING *previous;
  int score;
  char nickname[25];
  struct RANKING *next;
};

struct DESCRITOR {
  struct RANKING *head;
  struct RANKING *tail;
  int size;
};

struct DESCRITOR score_list = {NULL, NULL, 0};

/* Funções auxiliares */
struct RANKING *get_node_at_index(struct DESCRITOR *list, int index) {
  if (index < 0 || index >= list->size)
    return NULL;

  struct RANKING *current = list->head;
  for (int i = 0; i < index && current != NULL; i++) {
    current = current->next;
  }
  return current;
}

void swapNodes(struct RANKING *a, struct RANKING *b) {
  if (a == b)
    return;

  int temp_score = a->score;
  char temp_nick[25];
  strcpy(temp_nick, a->nickname);

  a->score = b->score;
  strcpy(a->nickname, b->nickname);

  b->score = temp_score;
  strcpy(b->nickname, temp_nick);
}

/* Algoritmos de ordenação */
void selection_sort(struct DESCRITOR *list) {
  if (list->head == NULL || list->head->next == NULL)
    return;

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
      swapNodes(current, max);
    }
    current = current->next;
  }
}

struct RANKING *merge_by_name(struct RANKING *first, struct RANKING *second) {
  if (!first)
    return second;
  if (!second)
    return first;

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

struct RANKING *merge_sort_by_name(struct RANKING *head) {
  if (!head || !head->next)
    return head;

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
  if (list->size < 2)
    return;

  list->head = merge_sort_by_name(list->head);

  struct RANKING *current = list->head;
  struct RANKING *prev = NULL;

  while (current != NULL) {
    current->previous = prev;
    prev = current;
    if (current->next == NULL) {
      list->tail = current;
    }
    current = current->next;
  }
}

/* Funções de pesquisa */
void binary_search(struct DESCRITOR *list, const char *nickname) {
  search_results[0] = '\0';
  if (list->head == NULL) {
    strcat(search_results, "Lista vazia!\n");
    return;
  }

  int left = 0, right = list->size - 1, found = 0;

  while (left <= right) {
    int mid = left + (right - left) / 2;
    struct RANKING *mid_node = get_node_at_index(list, mid);

    if (!mid_node)
      break;

    int cmp = strcasecmp(mid_node->nickname, nickname);
    if (cmp == 0) {
      char buffer[256];
      sprintf(buffer, "Jogador '%s' encontrado na posicao %d: Pontuacao %d\n",
              nickname, mid + 1, mid_node->score);
      strcat(search_results, buffer);
      found = 1;

      int i = mid - 1;
      struct RANKING *prev = get_node_at_index(list, i);
      while (i >= 0 && prev && strcasecmp(prev->nickname, nickname) == 0) {
        sprintf(buffer, "Jogador '%s' encontrado na posicao %d: Pontuacao %d\n",
                nickname, i + 1, prev->score);
        strcat(search_results, buffer);
        i--;
        prev = get_node_at_index(list, i);
      }

      i = mid + 1;
      struct RANKING *next = get_node_at_index(list, i);
      while (i < list->size && next &&
             strcasecmp(next->nickname, nickname) == 0) {
        sprintf(buffer, "Jogador '%s' encontrado na posicao %d: Pontuacao %d\n",
                nickname, i + 1, next->score);
        strcat(search_results, buffer);
        i++;
        next = get_node_at_index(list, i);
      }
      break;
    }

    if (cmp < 0)
      left = mid + 1;
    else
      right = mid - 1;
  }

  if (!found) {
    sprintf(search_results, "Jogador '%s' nao encontrado na lista.\n",
            nickname);
  }
}

void exponential_search(struct DESCRITOR *list, int score) {
  search_results[0] = '\0';
  if (list->head == NULL) {
    strcat(search_results, "Lista vazia!\n");
    return;
  }

  if (list->head->score == score) {
    char buffer[256];
    sprintf(buffer, "Pontuacao %d encontrada na posicao 1: %s\n", score,
            list->head->nickname);
    strcat(search_results, buffer);
    return;
  }

  int i = 1;
  while (i < list->size) {
    struct RANKING *node = get_node_at_index(list, i);
    if (!node)
      break;

    if (node->score <= score)
      break;
    i *= 2;
  }

  int left = i / 2, right = (i < list->size) ? i : list->size - 1, found = 0;

  while (left <= right) {
    int mid = left + (right - left) / 2;
    struct RANKING *mid_node = get_node_at_index(list, mid);

    if (!mid_node)
      break;

    if (mid_node->score == score) {
      char buffer[256];
      sprintf(buffer, "Pontuacao %d encontrada na posicao %d: %s\n", score,
              mid + 1, mid_node->nickname);
      strcat(search_results, buffer);
      found = 1;

      int i = mid - 1;
      struct RANKING *prev = get_node_at_index(list, i);
      while (i >= 0 && prev && prev->score == score) {
        sprintf(buffer, "Pontuacao %d encontrada na posicao %d: %s\n", score,
                i + 1, prev->nickname);
        strcat(search_results, buffer);
        i--;
        prev = get_node_at_index(list, i);
      }

      i = mid + 1;
      struct RANKING *next = get_node_at_index(list, i);
      while (i < list->size && next && next->score == score) {
        sprintf(buffer, "Pontuacao %d encontrada na posicao %d: %s\n", score,
                i + 1, next->nickname);
        strcat(search_results, buffer);
        i++;
        next = get_node_at_index(list, i);
      }
      break;
    }

    if (mid_node->score > score)
      left = mid + 1;
    else
      right = mid - 1;
  }

  if (!found) {
    sprintf(search_results, "Pontuacao %d nao encontrada na lista.\n", score);
  }
}

/* Funções de manipulação da lista */
void printScoreList(struct DESCRITOR *list) {
  printf("\n--- Ranking Atual ---\n");
  struct RANKING *current = list->head;
  int position = 1;
  while (current != NULL) {
    printf("%d. %s - %d pontos\n", position, current->nickname, current->score);
    current = current->next;
    position++;
  }
  printf("---------------------\n");
}

void insertOnScoreList(struct DESCRITOR *list, int score,
                       const char *nickname) {
  struct RANKING *new_node = (struct RANKING *)malloc(sizeof(struct RANKING));
  new_node->score = score;
  strncpy(new_node->nickname, nickname, 24);
  new_node->nickname[24] = '\0';
  new_node->next = NULL;
  new_node->previous = NULL;

  if (list->head == NULL) {
    list->head = new_node;
    list->tail = new_node;
  } else {
    new_node->previous = list->tail;
    list->tail->next = new_node;
    list->tail = new_node;
  }
  list->size++;
  printScoreList(list);
}

/* Funções de persistência de dados */
void salvarDados(struct DESCRITOR *lista) {
    FILE *arquivo = fopen("ranking.txt", "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo para escrita\n");
        return;
    }
    
    struct RANKING *current = lista->head;
    while (current != NULL) {
        fprintf(arquivo, "%s,%d\n", current->nickname, current->score);
        current = current->next;
    }
    
    fclose(arquivo);
    printf("Dados salvos com sucesso!\n");
}

void carregarDados(struct DESCRITOR *lista) {
    FILE *arquivo = fopen("ranking.txt", "r");
    if (arquivo == NULL) {
        printf("Arquivo não encontrado. Criando novo arquivo...\n");
        return;
    }
    
    char linha[100];
    while (fgets(linha, sizeof(linha), arquivo) {
        // Remove a quebra de linha
        linha[strcspn(linha, "\n")] = 0;
        
        // Divide a linha em nickname e score
        char *token = strtok(linha, ",");
        if (token == NULL) continue;
        
        char nickname[25];
        strncpy(nickname, token, sizeof(nickname) - 1);
        nickname[sizeof(nickname) - 1] = '\0';
        
        token = strtok(NULL, ",");
        if (token == NULL) continue;
        
        int score = atoi(token);
        
        // Insere na lista
        insertOnScoreList(lista, score, nickname);
    }
    
    fclose(arquivo);
    printf("Dados carregados com sucesso!\n");
}

/* Funções de desenho OpenGL */
void drawText(float x, float y, const char *text) {
    glRasterPos2f(x, y);
    for (const char *c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
}

void drawSearchResults() {
    glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Barra superior
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(0, window_height);
    glVertex2f(window_width, window_height);
    glVertex2f(window_width, window_height - BAR_HEIGHT);
    glVertex2f(0, window_height - BAR_HEIGHT);
    glEnd();
    
    // Barra inferior
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(window_width, 0);
    glVertex2f(window_width, BAR_HEIGHT);
    glVertex2f(0, BAR_HEIGHT);
    glEnd();
    
    // Título
    glColor3f(0.0f, 0.0f, 0.0f);
    const char *title = current_submenu == 1
                          ? "Resultados da Pesquisa Binaria"
                          : "Resultados da Pesquisa Exponencial";
    drawText(20, window_height - BAR_HEIGHT/2 + 5, title);
    
    // Resultados
    int start_y = window_height - BAR_HEIGHT - 30;
    int line_height = 20;
    
    char *line = strtok(search_results, "\n");
    while (line != NULL && start_y > BAR_HEIGHT) {
        drawText(20, start_y, line);
        start_y -= line_height;
        line = strtok(NULL, "\n");
    }
    
    drawText(window_width/4, BAR_HEIGHT + 30, "Pressione Esc para voltar");
    
    glutSwapBuffers();
}

void drawSearchForm() {
    glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Barra superior
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(0, window_height);
    glVertex2f(window_width, window_height);
    glVertex2f(window_width, window_height - BAR_HEIGHT);
    glVertex2f(0, window_height - BAR_HEIGHT);
    glEnd();
    
    // Barra inferior
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(window_width, 0);
    glVertex2f(window_width, BAR_HEIGHT);
    glVertex2f(0, BAR_HEIGHT);
    glEnd();
    
    // Título
    glColor3f(0.0f, 0.0f, 0.0f);
    const char *title = current_submenu == 1
                          ? "Pesquisa Binaria (por nome)"
                          : "Pesquisa Exponencial (por pontuacao)";
    drawText(20, window_height - BAR_HEIGHT/2 + 5, title);
    
    // Campo de pesquisa
    drawText(window_width/4, window_height/3,
             current_submenu == 1 ? "Nome do Jogador:" : "Pontuacao:");
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(window_width/4 + 150, window_height/3 - 15);
    glVertex2f(window_width/4 + 350, window_height/3 - 15);
    glVertex2f(window_width/4 + 350, window_height/3 + 15);
    glVertex2f(window_width/4 + 150, window_height/3 + 15);
    glEnd();
    
    glColor3f(0.0f, 0.0f, 0.0f);
    drawText(window_width/4 + 155, window_height/3 + 5, search_score);
    
    drawText(window_width/4, BAR_HEIGHT + 30,
             "Pressione Enter para pesquisar ou Esc para cancelar");
    
    glutSwapBuffers();
}

void drawRankings() {
    glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Barra superior
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(0, window_height);
    glVertex2f(window_width, window_height);
    glVertex2f(window_width, window_height - BAR_HEIGHT);
    glVertex2f(0, window_height - BAR_HEIGHT);
    glEnd();
    
    // Barra inferior
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(window_width, 0);
    glVertex2f(window_width, BAR_HEIGHT);
    glVertex2f(0, BAR_HEIGHT);
    glEnd();
    
    // Título
    glColor3f(0.0f, 0.0f, 0.0f);
    drawText(20, window_height - BAR_HEIGHT/2 + 5, "Rankings");
    
    // Cabeçalho
    int start_y = window_height - BAR_HEIGHT - 30;
    drawText(window_width/4, start_y, "Posição");
    drawText(window_width/4 + 100, start_y, "Nickname");
    drawText(window_width/4 + 300, start_y, "Pontuação");
    start_y -= 20;
    
    // Itens
    struct RANKING *current = score_list.head;
    int position = 1;
    
    while (current != NULL && position <= 10) {
        char pos_str[10], score_str[10];
        sprintf(pos_str, "%d", position);
        sprintf(score_str, "%d", current->score);
        
        drawText(window_width/4, start_y, pos_str);
        drawText(window_width/4 + 100, start_y, current->nickname);
        drawText(window_width/4 + 300, start_y, score_str);
        
        current = current->next;
        position++;
        start_y -= 20;
    }
    
    drawText(window_width/4, BAR_HEIGHT + 30, "Pressione Esc para voltar");
    
    glutSwapBuffers();
}

void drawAddScoreForm() {
    glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Barra superior
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(0, window_height);
    glVertex2f(window_width, window_height);
    glVertex2f(window_width, window_height - BAR_HEIGHT);
    glVertex2f(0, window_height - BAR_HEIGHT);
    glEnd();
    
    // Barra inferior
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(window_width, 0);
    glVertex2f(window_width, BAR_HEIGHT);
    glVertex2f(0, BAR_HEIGHT);
    glEnd();
    
    // Título
    glColor3f(0.0f, 0.0f, 0.0f);
    drawText(20, window_height - BAR_HEIGHT/2 + 5, "Adicionar Pontuacao");
    
    // Campos
    drawText(window_width/4, window_height/3, "Nickname:");
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(window_width/4 + 100, window_height/3 - 15);
    glVertex2f(window_width/4 + 300, window_height/3 - 15);
    glVertex2f(window_width/4 + 300, window_height/3 + 15);
    glVertex2f(window_width/4 + 100, window_height/3 + 15);
    glEnd();
    
    glColor3f(0.0f, 0.0f, 0.0f);
    drawText(window_width/4 + 105, window_height/3 + 5, input_nickname);
    
    drawText(window_width/4, window_height/3 - 50, "Pontuacao:");
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(window_width/4 + 100, window_height/3 - 65);
    glVertex2f(window_width/4 + 300, window_height/3 - 65);
    glVertex2f(window_width/4 + 300, window_height/3 - 35);
    glVertex2f(window_width/4 + 100, window_height/3 - 35);
    glEnd();
    
    glColor3f(0.0f, 0.0f, 0.0f);
    drawText(window_width/4 + 105, window_height/3 - 45, input_score);
    
    drawText(window_width/4, BAR_HEIGHT + 30,
             "Pressione Enter para confirmar ou Esc para cancelar");
    
    glutSwapBuffers();
}

void drawMenu() {
    glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Barra superior
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(0, window_height);
    glVertex2f(window_width, window_height);
    glVertex2f(window_width, window_height - BAR_HEIGHT);
    glVertex2f(0, window_height - BAR_HEIGHT);
    glEnd();
    
    // Barra inferior
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(window_width, 0);
    glVertex2f(window_width, BAR_HEIGHT);
    glVertex2f(0, BAR_HEIGHT);
    glEnd();
    
    // Título
    glColor3f(0.0f, 0.0f, 0.0f);
    const char *title =
        current_menu == 0
            ? "Menu Principal"
            : (current_submenu == 1 ? "Submenu Pesquisa" : "Submenu Ordenacao");
    drawText(20, window_height - BAR_HEIGHT/2 + 5, title);
    
    // Itens do menu
    int start_y = window_height - BAR_HEIGHT - 50;
    const char *items[MENU_ITEMS] = {"Pesquisar", "Ordenar", "Adicionar", "Exibir Rankings", "Sair"};
    
    for (int i = 0; i < MENU_ITEMS; i++) {
        if (i == selected_menu_item) {
            glColor3f(0.4f, 0.4f, 0.4f);
            glBegin(GL_QUADS);
            glVertex2f(window_width/4, start_y - i*ITEM_HEIGHT);
            glVertex2f(3*window_width/4, start_y - i*ITEM_HEIGHT);
            glVertex2f(3*window_width/4, start_y - i*ITEM_HEIGHT + ITEM_HEIGHT - ITEM_PADDING);
            glVertex2f(window_width/4, start_y - i*ITEM_HEIGHT + ITEM_HEIGHT - ITEM_PADDING);
            glEnd();
            glColor3f(1.0f, 1.0f, 1.0f);
        } else {
            glColor3f(0.0f, 0.0f, 0.0f);
        }
        
        int text_width = glutBitmapLength(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)items[i]);
        drawText(window_width/2 - text_width/2, 
                start_y - i*ITEM_HEIGHT + (ITEM_HEIGHT - ITEM_PADDING)/2 + 5, 
                items[i]);
    }
    
    glutSwapBuffers();
}

/* Funções de manipulação de menu */
void handleSubmenuSelection(int selected_index) {
    if (selected_index == SUBMENU1_ITEMS - 1 ||
        selected_index == SUBMENU2_ITEMS - 1) {
        current_menu = 0;
    } else if (current_submenu == 1) {
        if (selected_index == 0 || selected_index == 1) {
            searching_score = true;
            search_score[0] = '\0';
            search_results[0] = '\0';
            if (selected_index == 0) {
                merge_sort_wrapper(&score_list);
            } else {
                selection_sort(&score_list);
            }
        }
    } else if (current_submenu == 2) {
        if (selected_index == 0) {
            selection_sort(&score_list);
            showing_rankings = true;
        } else if (selected_index == 1) {
            merge_sort_wrapper(&score_list);
            showing_rankings = true;
        }
    }
}

/* Funções GLUT */
void display() {
    if (showing_rankings) {
        drawRankings();
    } else if (adding_score) {
        drawAddScoreForm();
    } else if (searching_score && strlen(search_results) > 0) {
        drawSearchResults();
    } else if (searching_score) {
        drawSearchForm();
    } else if (current_menu == 0) {
        drawMenu();
    } else if (current_submenu == 1) {
        drawMenu();
    } else {
        drawMenu();
    }
}

void keyboard(unsigned char key, int x, int y) {
    if (showing_rankings) {
        if (key == 27) { // ESC
            showing_rankings = false;
        }
    } else if (adding_score) {
        if (key == 13) { // Enter
            if (strlen(input_nickname) > 0 && strlen(input_score) > 0) {
                int score = atoi(input_score);
                insertOnScoreList(&score_list, score, input_nickname);
                salvarDados(&score_list); // Salva os dados após adicionar
            }
            adding_score = false;
            input_nickname[0] = '\0';
            input_score[0] = '\0';
            input_pos = 0;
        } else if (key == 27) { // ESC
            adding_score = false;
            input_nickname[0] = '\0';
            input_score[0] = '\0';
            input_pos = 0;
        } else if (key == 8) { // Backspace
            if (input_pos > 0) {
                if (input_pos <= 25) {
                    input_nickname[input_pos - 1] = '\0';
                } else {
                    input_score[input_pos - 26] = '\0';
                }
                input_pos--;
            }
        } else if (key == 9) { // Tab
            input_pos = 26;
        } else if (input_pos < 36) {
            if ((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z') ||
                (input_pos >= 26 && key >= '0' && key <= '9')) {
                if (input_pos < 25) {
                    input_nickname[input_pos] = key;
                    input_nickname[input_pos + 1] = '\0';
                } else if (input_pos >= 26 && input_pos < 36) {
                    input_score[input_pos - 26] = key;
                    input_score[input_pos - 25] = '\0';
                }
                input_pos++;
            }
        }
    } else if (searching_score && strlen(search_results) > 0) {
        if (key == 27) { // ESC
            search_results[0] = '\0';
            searching_score = false;
        }
    } else if (searching_score) {
        if (key == 13) { // Enter
            if (strlen(search_score) > 0) {
                if (current_submenu == 1 && selected_menu_item == 0) {
                    binary_search(&score_list, search_score);
                } else {
                    exponential_search(&score_list, atoi(search_score));
                }
            }
        } else if (key == 27) { // ESC
            searching_score = false;
            search_score[0] = '\0';
        } else if (key == 8) { // Backspace
            if (strlen(search_score) > 0) {
                search_score[strlen(search_score) - 1] = '\0';
            }
        } else if ((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z') ||
                   (key >= '0' && key <= '9') || key == ' ') {
            if (strlen(search_score) < 24) {
                search_score[strlen(search_score)] = key;
                search_score[strlen(search_score) + 1] = '\0';
            }
        }
    } else if (current_menu == 0) {
        if (key == 13) { // Enter
            if (selected_menu_item == MENU_ITEMS - 1) {
                salvarDados(&score_list);
                exit(0);
            } else if (selected_menu_item == 0) {
                current_menu = 1;
                current_submenu = 1;
                selected_menu_item = 0;
            } else if (selected_menu_item == 1) {
                current_menu = 1;
                current_submenu = 2;
                selected_menu_item = 0;
            } else if (selected_menu_item == 2) {
                adding_score = true;
                input_pos = 0;
            } else if (selected_menu_item == 3) {
                showing_rankings = true;
            }
        } else if (key == 'w' || key == 'W') {
            selected_menu_item = (selected_menu_item - 1 + MENU_ITEMS) % MENU_ITEMS;
        } else if (key == 's' || key == 'S') {
            selected_menu_item = (selected_menu_item + 1) % MENU_ITEMS;
        }
    } else {
        if (key == 13) { // Enter
            handleSubmenuSelection(selected_menu_item);
        } else if (key == 27) { // ESC
            current_menu = 0;
        } else if (key == 'w' || key == 'W') {
            int max_items = current_submenu == 1 ? SUBMENU1_ITEMS : SUBMENU2_ITEMS;
            selected_menu_item = (selected_menu_item - 1 + max_items) % max_items;
        } else if (key == 's' || key == 'S') {
            int max_items = current_submenu == 1 ? SUBMENU1_ITEMS : SUBMENU2_ITEMS;
            selected_menu_item = (selected_menu_item + 1) % max_items;
        }
    }
    
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    if (current_menu == 0) {
        if (key == GLUT_KEY_UP) {
            selected_menu_item = (selected_menu_item - 1 + MENU_ITEMS) % MENU_ITEMS;
        } else if (key == GLUT_KEY_DOWN) {
            selected_menu_item = (selected_menu_item + 1) % MENU_ITEMS;
        }
    } else {
        if (key == GLUT_KEY_UP) {
            int max_items = current_submenu == 1 ? SUBMENU1_ITEMS : SUBMENU2_ITEMS;
            selected_menu_item = (selected_menu_item - 1 + max_items) % max_items;
        } else if (key == GLUT_KEY_DOWN) {
            int max_items = current_submenu == 1 ? SUBMENU1_ITEMS : SUBMENU2_ITEMS;
            selected_menu_item = (selected_menu_item + 1) % max_items;
        }
    }
    
    glutPostRedisplay();
}

void reshape(int w, int h) {
    window_width = w;
    window_height = h;
    
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(window_width, window_height);
    glutCreateWindow("Sistema de Ranking");
    
    glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
    
    // Carrega os dados do arquivo ao iniciar
    carregarDados(&score_list);
    
    printf("Lista original:\n");
    printScoreList(&score_list);
    
    selection_sort(&score_list);
    printf("\nApós Selection Sort (por pontuação):\n");
    printScoreList(&score_list);
    
    merge_sort_wrapper(&score_list);
    printf("\nApós Merge Sort (por nome):\n");
    printScoreList(&score_list);
    
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutReshapeFunc(reshape);
    
    glutMainLoop();
    
    // Salva os dados ao sair
    salvarDados(&score_list);
    return 0;
}