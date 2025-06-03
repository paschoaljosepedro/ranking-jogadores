#include "header/x11.h"
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
int input_pos = 0;
char search_results[1024] = "";

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

  // Troca apenas os dados
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

  // Atualiza tail e ponteiros previous
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

      // Verifica ocorrências adjacentes
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

  // Verifica primeiro elemento
  if (list->head->score == score) {
    char buffer[256];
    sprintf(buffer, "Pontuacao %d encontrada na posicao 1: %s\n", score,
            list->head->nickname);
    strcat(search_results, buffer);
    return;
  }

  // Encontra intervalo
  int i = 1;
  while (i < list->size) {
    struct RANKING *node = get_node_at_index(list, i);
    if (!node)
      break;

    if (node->score <= score)
      break;
    i *= 2;
  }

  // Busca binária no intervalo
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

      // Verifica ocorrências adjacentes
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

void insertOnScoreList(struct DESCRITOR *list, uint score,
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

/* Funções de interface gráfica */
void drawSearchResults(Display *display, Window w, int width, int height,
                       GC gc) {
  XSetForeground(display, gc, 0xC0C0C0);
  XFillRectangle(display, w, gc, 0, 0, width, height);

  XSetForeground(display, gc, 0x808080);
  XFillRectangle(display, w, gc, 0, 0, width, BAR_HEIGHT);
  XFillRectangle(display, w, gc, 0, height - BAR_HEIGHT, width, BAR_HEIGHT);

  XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
  const char *title = current_submenu == 1
                          ? "Resultados da Pesquisa Binaria"
                          : "Resultados da Pesquisa Exponencial";
  XDrawString(display, w, gc, 20, BAR_HEIGHT / 2 + 5, title, strlen(title));

  XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
  int start_y = BAR_HEIGHT + 30;
  int line_height = 20;

  char *line = strtok(search_results, "\n");
  while (line != NULL && start_y < height - BAR_HEIGHT) {
    XDrawString(display, w, gc, 20, start_y, line, strlen(line));
    start_y += line_height;
    line = strtok(NULL, "\n");
  }

  XDrawString(display, w, gc, width / 4, height - BAR_HEIGHT - 30,
              "Pressione Esc para voltar", 23);
}

void drawSearchForm(Display *display, Window w, int width, int height, GC gc) {
  XSetForeground(display, gc, 0xC0C0C0);
  XFillRectangle(display, w, gc, 0, 0, width, height);

  XSetForeground(display, gc, 0x808080);
  XFillRectangle(display, w, gc, 0, 0, width, BAR_HEIGHT);
  XFillRectangle(display, w, gc, 0, height - BAR_HEIGHT, width, BAR_HEIGHT);

  XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
  const char *title = current_submenu == 1
                          ? "Pesquisa Binaria (por nome)"
                          : "Pesquisa Exponencial (por pontuacao)";
  XDrawString(display, w, gc, 20, BAR_HEIGHT / 2 + 5, title, strlen(title));

  XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
  XDrawString(display, w, gc, width / 4, height / 3,
              current_submenu == 1 ? "Nome do Jogador:" : "Pontuacao:",
              current_submenu == 1 ? 16 : 10);
  XDrawRectangle(display, w, gc, width / 4 + 150, height / 3 - 15, 200, 30);
  XDrawString(display, w, gc, width / 4 + 155, height / 3 + 5, search_score,
              strlen(search_score));

  XDrawString(display, w, gc, width / 4, height - BAR_HEIGHT - 30,
              "Pressione Enter para pesquisar ou Esc para cancelar", 50);
}

void drawRankings(Display *display, Window w, int width, int height, GC gc) {
  XSetForeground(display, gc, 0xC0C0C0);
  XFillRectangle(display, w, gc, 0, 0, width, height);

  XSetForeground(display, gc, 0x808080);
  XFillRectangle(display, w, gc, 0, 0, width, BAR_HEIGHT);
  XFillRectangle(display, w, gc, 0, height - BAR_HEIGHT, width, BAR_HEIGHT);

  XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
  XDrawString(display, w, gc, 20, BAR_HEIGHT / 2 + 5, "Rankings", 8);

  XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
  int start_y = BAR_HEIGHT + 30;
  int line_height = 20;

  struct RANKING *current = score_list.head;
  int position = 1;

  XDrawString(display, w, gc, width / 4, start_y, "Posição", 7);
  XDrawString(display, w, gc, width / 4 + 100, start_y, "Nickname", 8);
  XDrawString(display, w, gc, width / 4 + 300, start_y, "Pontuação", 9);
  start_y += line_height;

  while (current != NULL && position <= 10) {
    char pos_str[10], score_str[10];
    sprintf(pos_str, "%d", position);
    sprintf(score_str, "%d", current->score);

    XDrawString(display, w, gc, width / 4, start_y, pos_str, strlen(pos_str));
    XDrawString(display, w, gc, width / 4 + 100, start_y, current->nickname,
                strlen(current->nickname));
    XDrawString(display, w, gc, width / 4 + 300, start_y, score_str,
                strlen(score_str));

    current = current->next;
    position++;
    start_y += line_height;
  }

  XDrawString(display, w, gc, width / 4, height - BAR_HEIGHT - 30,
              "Pressione Esc para voltar", 23);
}

void drawAddScoreForm(Display *display, Window w, int width, int height,
                      GC gc) {
  XSetForeground(display, gc, 0xC0C0C0);
  XFillRectangle(display, w, gc, 0, 0, width, height);

  XSetForeground(display, gc, 0x808080);
  XFillRectangle(display, w, gc, 0, 0, width, BAR_HEIGHT);
  XFillRectangle(display, w, gc, 0, height - BAR_HEIGHT, width, BAR_HEIGHT);

  XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
  XDrawString(display, w, gc, 20, BAR_HEIGHT / 2 + 5, "Adicionar Pontuacao",
              19);

  XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
  XDrawString(display, w, gc, width / 4, height / 3, "Nickname:", 9);
  XDrawRectangle(display, w, gc, width / 4 + 100, height / 3 - 15, 200, 30);
  XDrawString(display, w, gc, width / 4 + 105, height / 3 + 5, input_nickname,
              strlen(input_nickname));

  XDrawString(display, w, gc, width / 4, height / 3 + 50, "Pontuacao:", 10);
  XDrawRectangle(display, w, gc, width / 4 + 100, height / 3 + 35, 200, 30);
  XDrawString(display, w, gc, width / 4 + 105, height / 3 + 55, input_score,
              strlen(input_score));

  XDrawString(display, w, gc, width / 4, height - BAR_HEIGHT - 30,
              "Pressione Enter para confirmar ou Esc para cancelar", 50);
}

void drawMenu(Display *display, Window w, int width, int height,
              MenuItem *items, int items_count, GC gc, GC selected_gc) {
  XSetForeground(display, gc, 0xC0C0C0);
  XFillRectangle(display, w, gc, 0, 0, width, height);

  XSetForeground(display, gc, 0x808080);
  XFillRectangle(display, w, gc, 0, 0, width, BAR_HEIGHT);
  XFillRectangle(display, w, gc, 0, height - BAR_HEIGHT, width, BAR_HEIGHT);

  XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
  const char *title =
      current_menu == 0
          ? "Menu Principal"
          : (current_submenu == 1 ? "Submenu Pesquisa" : "Submenu Ordenacao");
  XDrawString(display, w, gc, 20, BAR_HEIGHT / 2 + 5, title, strlen(title));

  int start_y = BAR_HEIGHT + 50;
  for (int i = 0; i < items_count; i++) {
    if (items[i].selected) {
      XSetForeground(display, selected_gc, 0x606060);
      XFillRectangle(display, w, selected_gc, width / 4,
                     start_y + i * ITEM_HEIGHT, width / 2,
                     ITEM_HEIGHT - ITEM_PADDING);
      XSetForeground(display, gc, WhitePixel(display, DefaultScreen(display)));
    } else {
      XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
    }

    XDrawString(display, w, gc,
                width / 2 - XTextWidth(XQueryFont(display, XGContextFromGC(gc)),
                                       items[i].label, strlen(items[i].label)) /
                                2,
                start_y + i * ITEM_HEIGHT + (ITEM_HEIGHT - ITEM_PADDING) / 2 +
                    5,
                items[i].label, strlen(items[i].label));
  }
}

void handleSubmenuSelection(int selected_index) {
  if (selected_index == SUBMENU1_ITEMS - 1 ||
      selected_index == SUBMENU2_ITEMS - 1) {
    current_menu = 0;
  } else if (current_submenu == 1) { // Submenu Pesquisa
    if (selected_index == 0 || selected_index == 1) {
      searching_score = true;
      search_score[0] = '\0';
      search_results[0] = '\0';
      if (selected_index == 0) { // Binária - ordena por nome
        merge_sort_wrapper(&score_list);
      } else { // Exponencial - ordena por score
        selection_sort(&score_list);
      }
    }
  } else if (current_submenu == 2) { // Submenu Ordenação
    if (selected_index == 0) {       // Selection Sort
      selection_sort(&score_list);
      showing_rankings = true;        // Mostra o resultado
    } else if (selected_index == 1) { // Merge Sort
      merge_sort_wrapper(&score_list);
      showing_rankings = true; // Mostra o resultado
    }
  }
}

void salvarDados(struct DESCRITOR *lista) {
  FILE *arquivo = fopen("dados.txt", "w");
  if (arquivo == NULL) {
    printf("Erro ao abrir arquivo para escrita\n");
    return;
  }
  struct RANKING *current = lista->head;
  while (current != NULL) {
    fprintf(arquivo, "%d,%s\n", current->score, current->nickname);
    current = current->next;
  }
  fclose(arquivo);
  printf("Dados salvos");
}

void carregarDados(struct DESCRITOR *lista) {
  FILE *arquivo = fopen("dados.txt", "r");
  if (arquivo == NULL) {
    printf("Arquivo nao encontrado. Criando novo...\n");
  }
  char linha[100];

  while (fgets(linha, sizeof(linha), arquivo)) {
    // remove o /n do final da linha
    linha[strcspn(linha, "\n")] = 0;

    char *token = strtok(linha, ",");
    int score = atoi(token);
    token = strtok(NULL, ",");
    char nick[25];
    if (token != NULL) {
      strncpy(nick, token, sizeof(nick) - 1);
      nick[sizeof(nick) - 1] = '\0';
      insertOnScoreList(lista, score, nick);
    }
  }
  fclose(arquivo);
}

void initGui() {
  XEvent event;
  Display *display = XOpenDisplay(NULL);
  if (display == NULL) {
    fprintf(stderr, "Não foi possível abrir o display X\n");
    exit(1);
  }

  int screen = DefaultScreen(display);
  int width = 900, height = 500;
  Window w = XCreateSimpleWindow(display, DefaultRootWindow(display), 500, 500,
                                 width, height, 1, BlackPixel(display, screen),
                                 WhitePixel(display, screen));

  XSizeHints size_hints;
  size_hints.flags = PMinSize;
  size_hints.min_width = 600;
  size_hints.min_height = 300;
  XSetWMNormalHints(display, w, &size_hints);

  GC gc = XCreateGC(display, w, 0, NULL);
  GC selected_gc = XCreateGC(display, w, 0, NULL);

  XSelectInput(display, w, ExposureMask | StructureNotifyMask | KeyPressMask);
  XMapWindow(display, w);

  MenuItem main_items[MENU_ITEMS] = {{"Pesquisar", 0},
                                     {"Ordenar", 0},
                                     {"Adicionar", 0},
                                     {"Exibir Rankings", 0},
                                     {"Sair", 0}};
  MenuItem submenu1_items[SUBMENU1_ITEMS] = {
      {"Binaria", 0}, {"Exponencial", 0}, {"Voltar", 0}};
  MenuItem submenu2_items[SUBMENU2_ITEMS] = {
      {"Selection", 0}, {"Merge", 0}, {"Voltar", 0}};

  int selected_index = 0;
  int submenu_selected = 0;
  main_items[selected_index].selected = 1;

  for (;;) {
    XNextEvent(display, &event);
    switch (event.type) {
    case Expose:
      if (showing_rankings) {
        drawRankings(display, w, width, height, gc);
      } else if (adding_score) {
        drawAddScoreForm(display, w, width, height, gc);
      } else if (searching_score && strlen(search_results) > 0) {
        drawSearchResults(display, w, width, height, gc);
      } else if (searching_score) {
        drawSearchForm(display, w, width, height, gc);
      } else if (current_menu == 0) {
        drawMenu(display, w, width, height, main_items, MENU_ITEMS, gc,
                 selected_gc);
      } else if (current_submenu == 1) {
        drawMenu(display, w, width, height, submenu1_items, SUBMENU1_ITEMS, gc,
                 selected_gc);
      } else {
        drawMenu(display, w, width, height, submenu2_items, SUBMENU2_ITEMS, gc,
                 selected_gc);
      }
      break;

    case ConfigureNotify:
      width = event.xconfigure.width;
      height = event.xconfigure.height;
      break;

    case KeyPress:
      if (showing_rankings) {
        KeySym key = XLookupKeysym(&event.xkey, 0);
        if (key == XK_Escape) {
          showing_rankings = false;
        }
      } else if (adding_score) {
        KeySym key = XLookupKeysym(&event.xkey, 0);
        if (key == XK_Return) {
          if (strlen(input_nickname) > 0 && strlen(input_score) > 0) {
            int score = atoi(input_score);
            insertOnScoreList(&score_list, score, input_nickname);
          }
          adding_score = false;
          input_nickname[0] = '\0';
          input_score[0] = '\0';
          input_pos = 0;
        } else if (key == XK_Escape) {
          adding_score = false;
          input_nickname[0] = '\0';
          input_score[0] = '\0';
          input_pos = 0;
        } else if (key == XK_BackSpace) {
          if (input_pos > 0) {
            if (input_pos <= 25) {
              input_nickname[input_pos - 1] = '\0';
            } else {
              input_score[input_pos - 26] = '\0';
            }
            input_pos--;
          }
        } else if (key == XK_Tab) {
          input_pos = 26;
        } else if (input_pos < 36) {
          char ch = (char)key;
          if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
              (input_pos >= 26 && ch >= '0' && ch <= '9')) {
            if (input_pos < 25) {
              input_nickname[input_pos] = ch;
              input_nickname[input_pos + 1] = '\0';
            } else if (input_pos >= 26 && input_pos < 36) {
              input_score[input_pos - 26] = ch;
              input_score[input_pos - 25] = '\0';
            }
            input_pos++;
          }
        }
        drawAddScoreForm(display, w, width, height, gc);
      } else if (searching_score && strlen(search_results) > 0) {
        KeySym key = XLookupKeysym(&event.xkey, 0);
        if (key == XK_Escape) {
          search_results[0] = '\0';
          searching_score = false;
        }
      } else if (searching_score) {
        KeySym key = XLookupKeysym(&event.xkey, 0);
        if (key == XK_Return) {
          if (strlen(search_score) > 0) {
            if (current_submenu == 1 && submenu_selected == 0) {
              binary_search(&score_list, search_score);
            } else {
              exponential_search(&score_list, atoi(search_score));
            }
          }
        } else if (key == XK_Escape) {
          searching_score = false;
          search_score[0] = '\0';
        } else if (key == XK_BackSpace) {
          if (strlen(search_score) > 0) {
            search_score[strlen(search_score) - 1] = '\0';
          }
        } else if ((key >= XK_a && key <= XK_z) ||
                   (key >= XK_A && key <= XK_Z) ||
                   (key >= XK_0 && key <= XK_9) || key == XK_space) {
          if (strlen(search_score) < 24) {
            char ch = (char)key;
            search_score[strlen(search_score)] = ch;
            search_score[strlen(search_score) + 1] = '\0';
          }
        }
        drawSearchForm(display, w, width, height, gc);
      } else if (current_menu == 0) {
        main_items[selected_index].selected = 0;

        KeySym key = XLookupKeysym(&event.xkey, 0);
        switch (key) {
        case XK_Up:
          selected_index = (selected_index - 1 + MENU_ITEMS) % MENU_ITEMS;
          break;
        case XK_Down:
          selected_index = (selected_index + 1) % MENU_ITEMS;
          break;
        case XK_Return:
          if (selected_index == MENU_ITEMS - 1) {
            XCloseDisplay(display);
            exit(0);
          } else if (selected_index == 0) {
            current_menu = 1;
            current_submenu = 1;
            submenu_selected = 0;
            submenu1_items[submenu_selected].selected = 1;
          } else if (selected_index == 1) {
            current_menu = 1;
            current_submenu = 2;
            submenu_selected = 0;
            submenu2_items[submenu_selected].selected = 1;
          } else if (selected_index == 2) {
            adding_score = true;
            input_pos = 0;
          } else if (selected_index == 3) {
            showing_rankings = true;
          }
          break;
        }

        main_items[selected_index].selected = 1;
      } else {
        if (current_submenu == 1) {
          submenu1_items[submenu_selected].selected = 0;
        } else {
          submenu2_items[submenu_selected].selected = 0;
        }

        KeySym key = XLookupKeysym(&event.xkey, 0);
        switch (key) {
        case XK_Up:
          submenu_selected =
              (submenu_selected - 1 +
               (current_submenu == 1 ? SUBMENU1_ITEMS : SUBMENU2_ITEMS)) %
              (current_submenu == 1 ? SUBMENU1_ITEMS : SUBMENU2_ITEMS);
          break;
        case XK_Down:
          submenu_selected =
              (submenu_selected + 1) %
              (current_submenu == 1 ? SUBMENU1_ITEMS : SUBMENU2_ITEMS);
          break;
        case XK_Return:
          handleSubmenuSelection(submenu_selected);
          break;
        case XK_Escape:
          current_menu = 0;
          break;
        }

        if (current_submenu == 1) {
          submenu1_items[submenu_selected].selected = 1;
        } else {
          submenu2_items[submenu_selected].selected = 1;
        }
      }

      if (showing_rankings) {
        drawRankings(display, w, width, height, gc);
      } else if (adding_score) {
        drawAddScoreForm(display, w, width, height, gc);
      } else if (searching_score && strlen(search_results) > 0) {
        drawSearchResults(display, w, width, height, gc);
      } else if (searching_score) {
        drawSearchForm(display, w, width, height, gc);
      } else if (current_menu == 0) {
        drawMenu(display, w, width, height, main_items, MENU_ITEMS, gc,
                 selected_gc);
      } else if (current_submenu == 1) {
        drawMenu(display, w, width, height, submenu1_items, SUBMENU1_ITEMS, gc,
                 selected_gc);
      } else {
        drawMenu(display, w, width, height, submenu2_items, SUBMENU2_ITEMS, gc,
                 selected_gc);
      }
      break;
    }
  }
}

int main() {

    struct DESCRITOR *score = &score_list;
    carregarDados(score);
  // Dados de teste
//   insertOnScoreList(&score_list, 150, "Carlos");
//   insertOnScoreList(&score_list, 200, "Ana");
//   insertOnScoreList(&score_list, 100, "Beatriz");
//   insertOnScoreList(&score_list, 300, "Daniel");
//   insertOnScoreList(&score_list, 250, "Eduardo");

  printf("Lista original:\n");
  printScoreList(&score_list);

  // Teste Selection Sort
  selection_sort(&score_list);
  printf("\nApós Selection Sort (por pontuação):\n");
  printScoreList(&score_list);

  // Teste Merge Sort
  merge_sort_wrapper(&score_list);
  printf("\nApós Merge Sort (por nome):\n");
  printScoreList(&score_list);

  initGui();
  salvarDados(score);
  return 0;
}