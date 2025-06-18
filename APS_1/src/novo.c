// #DEFINICOES

// Bibliotecas necessárias para OpenGL, interface gráfica e manipulação de dados
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h> // Para medição de tempo

// Constantes de configuração
#define MENU_ITEMS 5            // Número de itens no menu principal
#define SUBMENU1_ITEMS 3        // Número de itens no submenu de pesquisa
#define SUBMENU2_ITEMS 3        // Número de itens no submenu de ordenação
#define BAR_HEIGHT 50           // Altura da barra de menu
#define ITEM_HEIGHT 50          // Altura de cada item de menu
#define ITEM_PADDING 20         // Espaçamento entre itens
#define MAX_NICKNAME_LENGTH 24  // Tamanho máximo do nickname
#define MAX_SCORE_LENGTH 9      // Tamanho máximo da pontuação (9 dígitos)
#define MAX_SEARCH_RESULTS 1024 // Tamanho máximo para resultados de busca

// Variáveis globais para dimensão da janela
int window_width = 900;
int window_height = 500;

// Enumerações para tipos de campos, menus e estados da aplicação
typedef enum { FIELD_NICKNAME, FIELD_SCORE } FieldType;
typedef enum { MENU_MAIN, MENU_SEARCH, MENU_SORT } MenuType;
typedef enum {
  STATE_MENU,
  STATE_ADDING,
  STATE_EDITING,
  STATE_RANKINGS,
  STATE_SEARCH_FORM,
  STATE_SEARCH_RESULTS
} AppState;

// Estrutura para medição de tempo
typedef struct {
  long seconds;
  long microseconds;
  double elapsed;
} TimerResult;

// Estrutura de nó da lista encadeada para rankings
typedef struct RANKING {
  struct RANKING *previous;               // Ponteiro para o nó anterior
  int score;                              // Pontuação do jogador
  char nickname[MAX_NICKNAME_LENGTH + 1]; // Nome do jogador
  struct RANKING *next;                   // Ponteiro para o próximo nó
} Ranking;

// Estrutura da lista encadeada
typedef struct {
  Ranking *head; // Primeiro elemento da lista
  Ranking *tail; // Último elemento da lista
  int size;      // Tamanho da lista
} RankingList;

// Variáveis globais do estado da aplicação
RankingList score_list = {NULL, NULL, 0};       // Lista de pontuações
int selected_menu_item = 0;                     // Item de menu selecionado
AppState currentState = STATE_MENU;             // Estado atual da aplicação
MenuType currentMenu = MENU_MAIN;               // Menu atual
int currentSubmenu = 0;                         // Submenu atual
char searchInput[MAX_NICKNAME_LENGTH + 1] = ""; // Entrada de pesquisa
char searchResults[MAX_SEARCH_RESULTS] = "";    // Resultados de pesquisa
char timeResults[256] = "";                 // Resultados de tempo de ordenação
bool nicknameFieldActive = true;            // Campo de nickname ativo
bool scoreFieldActive = false;              // Campo de pontuação ativo
char scoreInput[MAX_SCORE_LENGTH + 1] = ""; // Entrada de pontuação

// #UTILS

// Callback para eventos de mouse
void mouse(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    // Converter coordenadas do mouse para coordenadas da janela OpenGL
    int glY = window_height - y;

    if (currentState == STATE_ADDING) {
      // Verificar clique no campo de nickname
      if (x >= window_width / 4 + 150 && x <= window_width / 4 + 350 &&
          glY >= window_height / 2 - 15 && glY <= window_height / 2 + 15) {
        nicknameFieldActive = true;
        scoreFieldActive = false;
      }
      // Verificar clique no campo de pontuação
      else if (x >= window_width / 4 + 150 && x <= window_width / 4 + 350 &&
               glY >= window_height / 2 - 65 && glY <= window_height / 2 - 35) {
        nicknameFieldActive = false;
        scoreFieldActive = true;
      }
    }
    glutPostRedisplay(); // Solicita redesenho da tela
  }
}

// Calcula o tempo decorrido entre dois momentos
TimerResult calculateTimer(struct timeval start, struct timeval end) {
  long seconds = end.tv_sec - start.tv_sec;
  long microseconds = end.tv_usec - start.tv_usec;
  double elapsed = seconds + microseconds * 1e-6;
  return (TimerResult){seconds, microseconds, elapsed};
}

// Desenha texto na tela usando rasterização
void drawText(float x, float y, const char *text) {
  glRasterPos2f(x, y); // Define posição para o texto
  for (const char *c = text; *c != '\0'; c++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c); // Desenha cada caractere
  }
}

// Desenha um campo de texto na interface
void drawTextField(float x, float y, float width, float height, bool isActive,
                   const char *text) {
  // Define cor do campo (azul claro se ativo, branco se inativo)
  glColor3f(isActive ? 0.8f : 1.0f, isActive ? 0.8f : 1.0f,
            isActive ? 1.0f : 1.0f);
  // Desenha retângulo do campo
  glBegin(GL_QUADS);
  glVertex2f(x, y);
  glVertex2f(x + width, y);
  glVertex2f(x + width, y + height);
  glVertex2f(x, y + height);
  glEnd();

  // Desenha o texto dentro do campo
  glColor3f(0.0f, 0.0f, 0.0f);
  drawText(x + 5, y + height / 2 + 5, text);
}

// Desenha a barra de menu superior e inferior
void drawMenuBar(const char *title) {
  // Barra superior
  glColor3f(0.5f, 0.5f, 0.5f); // Cor cinza
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

  // Título na barra superior
  glColor3f(0.0f, 0.0f, 0.0f); // Cor preta
  drawText(20, window_height - BAR_HEIGHT / 2 + 5, title);
}

// #MANIPULACAO DE LISTA

// Obtém um nó da lista pelo índice
Ranking *getNodeAtIndex(RankingList *list, int index) {
  if (index < 0 || index >= list->size)
    return NULL; // Índice inválido

  Ranking *current = list->head;
  for (int i = 0; i < index && current != NULL; i++) {
    current = current->next; // Percorre a lista até o índice desejado
  }
  return current;
}

// Troca os valores de dois nós (não troca os nós em si, apenas os dados)
void swapNodes(Ranking *a, Ranking *b) {
  if (a == b)
    return; // Nada a fazer se forem o mesmo nó

  // Armazena valores temporários
  int temp_score = a->score;
  char temp_nick[MAX_NICKNAME_LENGTH + 1];
  strcpy(temp_nick, a->nickname);

  // Troca os valores
  a->score = b->score;
  strcpy(a->nickname, b->nickname);

  b->score = temp_score;
  strcpy(b->nickname, temp_nick);
}

// Imprime a lista de pontuações no console (para debug)
void printScoreList(RankingList *list) {
  printf("\n--- Ranking Atual ---\n");
  Ranking *current = list->head;
  int position = 1;
  while (current != NULL) {
    printf("%d. %s - %d pontos\n", position, current->nickname, current->score);
    current = current->next;
    position++;
  }
  printf("---------------------\n");
}

// Insere uma nova pontuação na lista
void insertScore(RankingList *list, int score, const char *nickname) {
  // Aloca memória para o novo nó
  Ranking *new_node = (Ranking *)malloc(sizeof(Ranking));
  new_node->score = score;
  strncpy(new_node->nickname, nickname, MAX_NICKNAME_LENGTH);
  new_node->nickname[MAX_NICKNAME_LENGTH] = '\0'; // Garante terminação nula
  new_node->next = NULL;
  new_node->previous = NULL;

  // Insere na lista (no final)
  if (list->head == NULL) {
    // Lista vazia
    list->head = new_node;
    list->tail = new_node;
  } else {
    // Lista não vazia, insere no final
    new_node->previous = list->tail;
    list->tail->next = new_node;
    list->tail = new_node;
  }
  list->size++;
  printScoreList(list); // Imprime a lista atualizada (debug)
}

// #ALGORITMO ORDENACAO

// Ordenação por seleção (ordena por pontuação em ordem decrescente)
void selectionSort(RankingList *list, char *timeResults) {
  if (list->head == NULL || list->head->next == NULL)
    return; // Lista vazia ou com um elemento

  struct timeval start, end;
  gettimeofday(&start, NULL); // Inicia medição de tempo

  Ranking *current = list->head;
  while (current != NULL) {
    Ranking *max = current; // Assume que o atual é o máximo
    Ranking *r = current->next;

    // Encontra o nó com a maior pontuação no restante da lista
    while (r != NULL) {
      if (r->score > max->score) {
        max = r;
      }
      r = r->next;
    }

    // Troca os valores se encontrou um maior
    if (max != current) {
      swapNodes(current, max);
    }
    current = current->next;
  }

  gettimeofday(&end, NULL); // Finaliza medição de tempo
  TimerResult timer = calculateTimer(start, end);
  // Formata string com resultados do tempo
  sprintf(timeResults, "Selection Sort: %.6f segundos | %d elementos",
          timer.elapsed, list->size);
}

// Função auxiliar para merge sort (combina duas listas ordenadas)
Ranking *mergeByName(Ranking *first, Ranking *second) {
  if (!first)
    return second;
  if (!second)
    return first;

  // Compara os nicknames (case insensitive)
  if (strcasecmp(first->nickname, second->nickname) <= 0) {
    first->next = mergeByName(first->next, second);
    first->next->previous = first;
    first->previous = NULL;
    return first;
  } else {
    second->next = mergeByName(first, second->next);
    second->next->previous = second;
    second->previous = NULL;
    return second;
  }
}

// Merge sort recursivo (ordena por nome em ordem alfabética)
Ranking *mergeSortByName(Ranking *head) {
  if (!head || !head->next)
    return head; // Caso base

  // Divide a lista em duas usando a técnica do ponteiro rápido e lento
  Ranking *slow = head;
  Ranking *fast = head->next;

  while (fast && fast->next) {
    slow = slow->next;
    fast = fast->next->next;
  }

  Ranking *mid = slow->next;
  slow->next = NULL; // Divide a lista

  // Ordena recursivamente cada metade
  Ranking *left = mergeSortByName(head);
  Ranking *right = mergeSortByName(mid);

  // Combina as metades ordenadas
  return mergeByName(left, right);
}

// Função principal do merge sort
void mergeSort(RankingList *list, char *timeResults) {
  if (list->size < 2)
    return; // Lista muito pequena para ordenar

  struct timeval start, end;
  gettimeofday(&start, NULL); // Inicia medição de tempo

  // Chama o merge sort recursivo
  list->head = mergeSortByName(list->head);

  // Atualiza os ponteiros previous e tail após a ordenação
  Ranking *current = list->head;
  Ranking *prev = NULL;
  while (current != NULL) {
    current->previous = prev;
    prev = current;
    if (current->next == NULL) {
      list->tail = current;
    }
    current = current->next;
  }

  gettimeofday(&end, NULL); // Finaliza medição de tempo
  TimerResult timer = calculateTimer(start, end);
  // Formata string com resultados do tempo
  sprintf(timeResults, "Merge Sort: %.6f segundos | %d elementos",
          timer.elapsed, list->size);
}

// #ALGORITMO PESQUISA

// Pesquisa binária (requer lista ordenada por nome)
void binarySearch(RankingList *list, const char *nickname,
                  char *searchResults) {
  searchResults[0] = '\0'; // Limpa resultados anteriores
  if (list->head == NULL) {
    strcat(searchResults, "Lista vazia!\n");
    return;
  }

  struct timeval start, end;
  gettimeofday(&start, NULL); // Inicia medição de tempo

  int left = 0, right = list->size - 1, found = 0;
  while (left <= right) {
    int mid = left + (right - left) / 2; // Evita overflow
    Ranking *mid_node = getNodeAtIndex(list, mid);

    if (!mid_node)
      break;

    int cmp =
        strcasecmp(mid_node->nickname, nickname); // Comparação case insensitive
    if (cmp == 0) {
      // Jogador encontrado
      char buffer[256];
      sprintf(buffer, "Jogador '%s' encontrado na posicao %d: Pontuacao %d\n",
              nickname, mid + 1, mid_node->score);
      strcat(searchResults, buffer);
      found = 1;

      // Verifica elementos iguais à esquerda (pode haver nomes iguais)
      int i = mid - 1;
      Ranking *prev = getNodeAtIndex(list, i);
      while (i >= 0 && prev && strcasecmp(prev->nickname, nickname) == 0) {
        sprintf(buffer, "Jogador '%s' encontrado na posicao %d: Pontuacao %d\n",
                nickname, i + 1, prev->score);
        strcat(searchResults, buffer);
        i--;
        prev = getNodeAtIndex(list, i);
      }

      // Verifica elementos iguais à direita
      i = mid + 1;
      Ranking *next = getNodeAtIndex(list, i);
      while (i < list->size && next &&
             strcasecmp(next->nickname, nickname) == 0) {
        sprintf(buffer, "Jogador '%s' encontrado na posicao %d: Pontuacao %d\n",
                nickname, i + 1, next->score);
        strcat(searchResults, buffer);
        i++;
        next = getNodeAtIndex(list, i);
      }
      break;
    }

    if (cmp < 0)
      left = mid + 1; // Procura na metade direita
    else
      right = mid - 1; // Procura na metade esquerda
  }

  if (!found) {
    sprintf(searchResults, "Jogador '%s' nao encontrado na lista.\n", nickname);
  }

  gettimeofday(&end, NULL); // Finaliza medição de tempo
  TimerResult timer = calculateTimer(start, end);
  char time_msg[256];
  sprintf(time_msg, "\n\nTempo de busca binária: %.6f segundos", timer.elapsed);
  strcat(searchResults, time_msg); // Adiciona tempo ao resultado
}

// Pesquisa exponencial (para lista ordenada por pontuação decrescente)
void exponentialSearch(RankingList *list, int score, char *searchResults) {
  searchResults[0] = '\0'; // Limpa resultados anteriores
  if (list->head == NULL) {
    strcat(searchResults, "Lista vazia!\n");
    return;
  }

  struct timeval start, end;
  gettimeofday(&start, NULL); // Inicia medição de tempo

  // Verifica primeiro elemento
  if (list->head->score == score) {
    char buffer[256];
    sprintf(buffer, "Pontuacao %d encontrada na posicao 1: %s\n", score,
            list->head->nickname);
    strcat(searchResults, buffer);
    return;
  }

  // Encontra o intervalo onde o elemento pode estar
  int i = 1;
  while (i < list->size) {
    Ranking *node = getNodeAtIndex(list, i);
    if (!node)
      break;

    if (node->score <= score) // Pontuação menor ou igual à procurada
      break;
    i *= 2; // Dobra o índice a cada iteração
  }

  // Faz busca binária no intervalo encontrado
  int left = i / 2, right = (i < list->size) ? i : list->size - 1, found = 0;
  while (left <= right) {
    int mid = left + (right - left) / 2;
    Ranking *mid_node = getNodeAtIndex(list, mid);

    if (!mid_node)
      break;

    if (mid_node->score == score) {
      char buffer[256];
      sprintf(buffer, "Pontuacao %d encontrada na posicao %d: %s\n", score,
              mid + 1, mid_node->nickname);
      strcat(searchResults, buffer);
      found = 1;

      // Verifica elementos iguais à esquerda
      int i = mid - 1;
      Ranking *prev = getNodeAtIndex(list, i);
      while (i >= 0 && prev && prev->score == score) {
        sprintf(buffer, "Pontuacao %d encontrada na posicao %d: %s\n", score,
                i + 1, prev->nickname);
        strcat(searchResults, buffer);
        i--;
        prev = getNodeAtIndex(list, i);
      }

      // Verifica elementos iguais à direita
      i = mid + 1;
      Ranking *next = getNodeAtIndex(list, i);
      while (i < list->size && next && next->score == score) {
        sprintf(buffer, "Pontuacao %d encontrada na posicao %d: %s\n", score,
                i + 1, next->nickname);
        strcat(searchResults, buffer);
        i++;
        next = getNodeAtIndex(list, i);
      }
      break;
    }

    if (mid_node->score > score)
      left = mid + 1; // Procura na metade direita
    else
      right = mid - 1; // Procura na metade esquerda
  }

  if (!found) {
    sprintf(searchResults, "Pontuacao %d nao encontrada na lista.\n", score);
  }

  gettimeofday(&end, NULL); // Finaliza medição de tempo
  TimerResult timer = calculateTimer(start, end);
  char time_msg[256];
  sprintf(time_msg, "\n\nTempo de busca exponencial: %.6f segundos",
          timer.elapsed);
  strcat(searchResults, time_msg); // Adiciona tempo ao resultado
}

// #FUNCAO PERSISTENCIA

// Salva os dados da lista em arquivo
void saveData(RankingList *list) {
  FILE *file = fopen("ranking.txt", "w"); // Abre arquivo para escrita
  if (file == NULL) {
    printf("Erro ao abrir arquivo para escrita\n");
    return;
  }

  // Escreve cada nó no arquivo no formato "nickname,score"
  Ranking *current = list->head;
  while (current != NULL) {
    fprintf(file, "%s,%d\n", current->nickname, current->score);
    current = current->next;
  }

  fclose(file); // Fecha o arquivo
  printf("Dados salvos com sucesso!\n");
}

// Carrega os dados do arquivo para a lista
void loadData(RankingList *list) {
  FILE *file = fopen("ranking.txt", "r"); // Abre arquivo para leitura
  if (file == NULL) {
    printf("Arquivo não encontrado. Criando novo arquivo...\n");
    return;
  }

  char line[100];
  while (fgets(line, sizeof(line), file)) { // Lê cada linha do arquivo
    line[strcspn(line, "\n")] = 0;          // Remove quebra de linha

    // Divide a linha em nickname e pontuação
    char *token = strtok(line, ",");
    if (token == NULL)
      continue;

    char nickname[MAX_NICKNAME_LENGTH + 1];
    strncpy(nickname, token, MAX_NICKNAME_LENGTH);
    nickname[MAX_NICKNAME_LENGTH] = '\0'; // Garante terminação nula

    token = strtok(NULL, ",");
    if (token == NULL)
      continue;

    int score = atoi(token);            // Converte pontuação para inteiro
    insertScore(list, score, nickname); // Insere na lista
  }

  fclose(file); // Fecha o arquivo
  printf("Dados carregados com sucesso!\n");
}

// #FUNCAO DESENHO

// Desenha tela de resultados de pesquisa
void drawSearchResultsScreen(const char *title, const char *results) {
  glClearColor(0.75f, 0.75f, 0.75f, 1.0f); // Cor de fundo cinza claro
  glClear(GL_COLOR_BUFFER_BIT);            // Limpa o buffer

  drawMenuBar(title); // Desenha barra de menu com título

  int start_y = window_height - BAR_HEIGHT - 30;
  int line_height = 20;

  // Cria cópia mutável dos resultados para strtok (strtok modifica a string)
  char results_copy[MAX_SEARCH_RESULTS];
  strncpy(results_copy, results, MAX_SEARCH_RESULTS);
  results_copy[MAX_SEARCH_RESULTS - 1] = '\0';

  // Divide os resultados em linhas e desenha cada uma
  char *line = strtok(results_copy, "\n");
  while (line != NULL && start_y > BAR_HEIGHT) {
    drawText(20, start_y, line);
    start_y -= line_height;
    line = strtok(NULL, "\n");
  }

  // Mensagem de instrução
  drawText(window_width / 4, BAR_HEIGHT + 30, "Pressione Esc para voltar");
  glutSwapBuffers(); // Troca os buffers para exibir o desenho
}

// Desenha tela de formulário genérico
void drawFormScreen(const char *title, const char *fieldLabel,
                    const char *inputText, bool isActive) {
  glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  drawMenuBar(title);

  // Desenha rótulo e campo de entrada
  drawText(window_width / 4, window_height / 3, fieldLabel);
  drawTextField(window_width / 4 + 150, window_height / 3 - 15, 200, 30,
                isActive, inputText);

  // Mensagem de instrução
  drawText(window_width / 4, BAR_HEIGHT + 30,
           "Pressione Enter para confirmar ou Esc para cancelar");
  glutSwapBuffers();
}

// Desenha tela de adição de pontuação
void drawAddScoreScreen() {
  glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  drawMenuBar("Adicionar Pontuação");

  // Campos para nickname e pontuação
  drawText(window_width / 4, window_height / 2, "Nickname:");
  drawTextField(window_width / 4 + 150, window_height / 2 - 15, 200, 30,
                nicknameFieldActive, searchInput);

  drawText(window_width / 4, window_height / 2 - 50, "Pontuação:");
  drawTextField(window_width / 4 + 150, window_height / 2 - 65, 200, 30,
                scoreFieldActive, scoreInput);

  // Mensagem de instrução
  drawText(window_width / 4, BAR_HEIGHT + 30,
           "Pressione Enter para confirmar ou Esc para cancelar");
  glutSwapBuffers();
}

// Desenha tela de edição de pontuação
void drawEditScoreScreen() {
  glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  drawMenuBar("Editar Pontuação");

  // Campo para nova pontuação
  drawText(window_width / 4, window_height / 2, "Nova Pontuação:");
  drawTextField(window_width / 4 + 150, window_height / 2 - 15, 200, 30, true,
                searchInput);

  // Mensagem de instrução
  drawText(window_width / 4, BAR_HEIGHT + 30,
           "Pressione Enter para confirmar ou Esc para cancelar");
  glutSwapBuffers();
}

// Desenha tela de rankings
void drawRankingsScreen() {
  glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  drawMenuBar("Rankings");

  int start_y = window_height - BAR_HEIGHT - 30;

  // Exibe tempo de ordenação se existir
  if (strlen(timeResults) > 0) {
    glColor3f(0.0f, 0.0f, 0.0f);
    drawText(20, start_y, timeResults);
    start_y -= 30;
  }

  // Cabeçalho da tabela
  drawText(window_width / 4, start_y, "Posição");
  drawText(window_width / 4 + 100, start_y, "Nickname");
  drawText(window_width / 4 + 300, start_y, "Pontuação");
  start_y -= 20;

  // Desenha os 10 primeiros itens do ranking
  Ranking *current = score_list.head;
  int position = 1;

  while (current != NULL && position <= 10) {
    char pos_str[10], score_str[10];
    sprintf(pos_str, "%d", position);
    sprintf(score_str, "%d", current->score);

    // Destaca o item selecionado
    if (position - 1 == selected_menu_item) {
      glColor3f(0.4f, 0.4f, 0.4f);
      glBegin(GL_QUADS);
      glVertex2f(window_width / 4 - 10, start_y + 10);
      glVertex2f(window_width / 4 + 400, start_y + 10);
      glVertex2f(window_width / 4 + 400, start_y - 10);
      glVertex2f(window_width / 4 - 10, start_y - 10);
      glEnd();
      glColor3f(1.0f, 1.0f, 1.0f);
    } else {
      glColor3f(0.0f, 0.0f, 0.0f);
    }

    // Desenha os dados do ranking
    drawText(window_width / 4, start_y, pos_str);
    drawText(window_width / 4 + 100, start_y, current->nickname);
    drawText(window_width / 4 + 300, start_y, score_str);

    current = current->next;
    position++;
    start_y -= 20;
  }

  // Mensagem de instrução
  drawText(window_width / 4, BAR_HEIGHT + 30,
           "Pressione Enter para editar, Esc para voltar");
  glutSwapBuffers();
}

// Desenha tela de menu genérico
void drawMenuScreen(const char *title, const char **items, int itemCount) {
  glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  drawMenuBar(title);

  int start_y = window_height - BAR_HEIGHT - 50;

  // Desenha cada item do menu
  for (int i = 0; i < itemCount; i++) {
    // Destaca o item selecionado
    if (i == selected_menu_item) {
      glColor3f(0.4f, 0.4f, 0.4f);
      glBegin(GL_QUADS);
      glVertex2f(window_width / 4, start_y - i * ITEM_HEIGHT);
      glVertex2f(3 * window_width / 4, start_y - i * ITEM_HEIGHT);
      glVertex2f(3 * window_width / 4,
                 start_y - i * ITEM_HEIGHT + ITEM_HEIGHT - ITEM_PADDING);
      glVertex2f(window_width / 4,
                 start_y - i * ITEM_HEIGHT + ITEM_HEIGHT - ITEM_PADDING);
      glEnd();
      glColor3f(1.0f, 1.0f, 1.0f);
    } else {
      glColor3f(0.0f, 0.0f, 0.0f);
    }

    // Centraliza o texto no item
    int text_width = glutBitmapLength(GLUT_BITMAP_HELVETICA_12,
                                      (const unsigned char *)items[i]);
    drawText(window_width / 2 - text_width / 2,
             start_y - i * ITEM_HEIGHT + (ITEM_HEIGHT - ITEM_PADDING) / 2 + 5,
             items[i]);
  }

  glutSwapBuffers();
}

// #FUNCAO DE CALLBACK

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
  case 27: // Tecla ESC - comportamento depende do estado atual
    if (currentState == STATE_MENU) {
      if (currentMenu != MENU_MAIN) {
        // Volta para o menu principal se estiver em submenu
        currentMenu = MENU_MAIN;
        selected_menu_item = 0;
        timeResults[0] = '\0'; // Limpa o tempo de ordenação
      } else {
        // Sai do programa se estiver no menu principal
        exit(0);
      }
    } else {
      // Volta para o menu principal de qualquer outro estado
      currentState = STATE_MENU;
      currentMenu = MENU_MAIN;
      selected_menu_item = 0;
      nicknameFieldActive = false;
      scoreFieldActive = false;
    }
    break;

  case 13: // Tecla Enter - confirmação de ações
    if (currentState == STATE_MENU) {
      if (currentMenu == MENU_MAIN) {
        // Navegação no menu principal
        switch (selected_menu_item) {
        case 0: // Pesquisar - vai para submenu de pesquisa
          currentMenu = MENU_SEARCH;
          selected_menu_item = 0;
          break;
        case 1: // Ordenar - vai para submenu de ordenação
          currentMenu = MENU_SORT;
          selected_menu_item = 0;
          break;
        case 2: // Adicionar - vai para tela de adição
          currentState = STATE_ADDING;
          searchInput[0] = '\0';
          scoreInput[0] = '\0';
          nicknameFieldActive = true;
          scoreFieldActive = false;
          break;
        case 3: // Exibir Rankings - vai para tela de rankings
          currentState = STATE_RANKINGS;
          selected_menu_item = 0;
          break;
        case 4: // Sair - encerra o programa
          exit(0);
          break;
        }
      } else if (currentMenu == MENU_SEARCH) {
        // Navegação no submenu de pesquisa
        switch (selected_menu_item) {
        case 0: // Pesquisa Binária - vai para formulário
          currentState = STATE_SEARCH_FORM;
          currentSubmenu = 1;
          searchInput[0] = '\0';
          break;
        case 1: // Pesquisa Exponencial - vai para formulário
          currentState = STATE_SEARCH_FORM;
          currentSubmenu = 2;
          searchInput[0] = '\0';
          break;
        case 2: // Voltar - retorna ao menu principal
          currentMenu = MENU_MAIN;
          selected_menu_item = 0;
          break;
        }
      } else if (currentMenu == MENU_SORT) {
        // Navegação no submenu de ordenação
        switch (selected_menu_item) {
        case 0: // Ordenar por pontuação
          selectionSort(&score_list, timeResults);
          currentState = STATE_RANKINGS; // Mostra resultados após ordenar
          break;
        case 1: // Ordenar por nome
          mergeSort(&score_list, timeResults);
          currentState = STATE_RANKINGS; // Mostra resultados após ordenar
          break;
        case 2: // Voltar - retorna ao menu principal
          currentMenu = MENU_MAIN;
          selected_menu_item = 0;
          timeResults[0] = '\0'; // Limpa o tempo de ordenação
          break;
        }
      }
    } else if (currentState == STATE_ADDING) {
      // Confirma adição de nova pontuação
      if (strlen(searchInput) > 0 && strlen(scoreInput) > 0) {
        int score = atoi(scoreInput);
        if (score > 0) {
          insertScore(&score_list, score, searchInput);
          saveData(&score_list); // Salva no arquivo
          currentState = STATE_MENU;
          searchInput[0] = '\0';
          scoreInput[0] = '\0';
        }
      }
    } else if (currentState == STATE_EDITING) {
      // Confirma edição de pontuação
      if (strlen(searchInput) > 0) {
        Ranking *toEdit = getNodeAtIndex(&score_list, selected_menu_item);
        if (toEdit) {
          toEdit->score = atoi(searchInput);
          saveData(&score_list); // Salva no arquivo
          currentState = STATE_RANKINGS;
          searchInput[0] = '\0';
        }
      }
    } else if (currentState == STATE_RANKINGS) {
      // Inicia edição do item selecionado no ranking
      currentState = STATE_EDITING;
      Ranking *toEdit = getNodeAtIndex(&score_list, selected_menu_item);
      if (toEdit) {
        sprintf(searchInput, "%d", toEdit->score);
      }
    } else if (currentState == STATE_SEARCH_FORM) {
      // Executa a pesquisa e mostra resultados
      currentState = STATE_SEARCH_RESULTS;
      if (currentSubmenu == 1) {
        binarySearch(&score_list, searchInput, searchResults);
      } else {
        int score = atoi(searchInput);
        exponentialSearch(&score_list, score, searchResults);
      }
    }
    break;

  case 9: // Tecla Tab - alterna entre campos na tela de adição
    if (currentState == STATE_ADDING) {
      nicknameFieldActive = !nicknameFieldActive;
      scoreFieldActive = !scoreFieldActive;
    }
    break;

  default:
    // Tratamento de entrada de texto nos campos ativos
    if (currentState == STATE_ADDING) {
      char *currentField = nicknameFieldActive ? searchInput : scoreInput;
      int maxLength =
          nicknameFieldActive ? MAX_NICKNAME_LENGTH : MAX_SCORE_LENGTH;

      if (key >= 32 && key <= 126) { // Caracteres imprimíveis
        int len = strlen(currentField);
        if (len < maxLength) {
          // Para o campo de pontuação, só permitir dígitos
          if (!scoreFieldActive || (key >= '0' && key <= '9')) {
            currentField[len] = key;
            currentField[len + 1] = '\0';
          }
        }
      } else if (key == 8) { // Backspace
        int len = strlen(currentField);
        if (len > 0) {
          currentField[len - 1] = '\0';
        }
      }
    } else if (currentState == STATE_EDITING ||
               currentState == STATE_SEARCH_FORM) {
      if (key >= 32 && key <= 126) { // Caracteres imprimíveis
        int len = strlen(searchInput);
        if (len < (currentState == STATE_EDITING ? MAX_SCORE_LENGTH
                                                 : MAX_NICKNAME_LENGTH)) {
          // Para edição de pontuação, só permitir dígitos
          if (currentState != STATE_EDITING || (key >= '0' && key <= '9')) {
            searchInput[len] = key;
            searchInput[len + 1] = '\0';
          }
        }
      } else if (key == 8) { // Backspace
        int len = strlen(searchInput);
        if (len > 0) {
          searchInput[len - 1] = '\0';
        }
      }
    }
    break;
  }
  glutPostRedisplay(); // Solicita redesenho da tela após mudanças
}

void specialKeys(int key, int x, int y) {
  switch (key) {
  case GLUT_KEY_UP:
    // Navegação para cima nos menus
    if (selected_menu_item > 0) {
      selected_menu_item--;
    }
    break;
  case GLUT_KEY_DOWN:
    // Navegação para baixo nos menus
    if (currentState == STATE_MENU) {
      int max_items = 0;
      if (currentMenu == MENU_MAIN)
        max_items = MENU_ITEMS - 1;
      else if (currentMenu == MENU_SEARCH)
        max_items = SUBMENU1_ITEMS - 1;
      else
        max_items = SUBMENU2_ITEMS - 1;

      if (selected_menu_item < max_items) {
        selected_menu_item++;
      }
    } else if (currentState == STATE_RANKINGS) {
      // Navegação no ranking (limita a 10 itens visíveis)
      if (selected_menu_item < score_list.size - 1 && selected_menu_item < 9) {
        selected_menu_item++;
      }
    }
    break;
  }
  glutPostRedisplay(); // Solicita redesenho da tela após mudanças
}

void reshape(int width, int height) {
  window_width = width;
  window_height = height;
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, width, 0, height);
  glMatrixMode(GL_MODELVIEW);
}

// #DISPLAY

void display() {
  switch (currentState) {
  case STATE_MENU:
    if (currentMenu == MENU_MAIN) {
      // Menu principal com 5 opções
      const char *items[MENU_ITEMS] = {"Pesquisar", "Ordenar", "Adicionar",
                                       "Exibir Rankings", "Sair"};
      drawMenuScreen("Menu Principal", items, MENU_ITEMS);
    } else if (currentMenu == MENU_SEARCH) {
      // Submenu de pesquisa com 3 opções
      const char *items[SUBMENU1_ITEMS] = {
          "Pesquisa Binaria (por nome)", "Pesquisa Exponencial (por pontuacao)",
          "Voltar"};
      drawMenuScreen("Submenu Pesquisa", items, SUBMENU1_ITEMS);
    } else {
      // Submenu de ordenação com 3 opções
      const char *items[SUBMENU2_ITEMS] = {"Ordenar por pontuacao",
                                           "Ordenar por nome", "Voltar"};
      drawMenuScreen("Submenu Ordenacao", items, SUBMENU2_ITEMS);
    }
    break;

  case STATE_ADDING:
    // Tela para adicionar nova pontuação com campos para nickname e score
    drawAddScoreScreen();
    break;

  case STATE_EDITING:
    // Tela para editar pontuação de um jogador existente
    drawEditScoreScreen();
    break;

  case STATE_RANKINGS:
    // Tela que exibe o ranking de jogadores (top 10)
    drawRankingsScreen();
    break;

  case STATE_SEARCH_FORM:
    // Formulário de pesquisa (diferente para cada tipo de pesquisa)
    if (currentSubmenu == 1) {
      // Formulário para pesquisa binária (por nome)
      drawFormScreen("Pesquisa Binaria (por nome)",
                     "Nome do Jogador:", searchInput, true);
    } else {
      // Formulário para pesquisa exponencial (por pontuação)
      drawFormScreen("Pesquisa Exponencial (por pontuacao)",
                     "Pontuacao:", searchInput, true);
    }
    break;

  case STATE_SEARCH_RESULTS:
    // Tela de resultados da pesquisa (comum para ambos os tipos)
    const char *title = currentSubmenu == 1
                            ? "Resultados da Pesquisa Binaria"
                            : "Resultados da Pesquisa Exponencial";
    drawSearchResultsScreen(title, searchResults);
    break;
  }
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(window_width, window_height);
  glutCreateWindow("Sistema de Ranking");

  glClearColor(0.75f, 0.75f, 0.75f, 1.0f);

  loadData(&score_list);

  printf("Lista original:\n");
  printScoreList(&score_list);

  selectionSort(&score_list, timeResults);
  printf("\nApós Selection Sort (por pontuação):\n");
  printScoreList(&score_list);

  mergeSort(&score_list, timeResults);
  printf("\nApós Merge Sort (por nome):\n");
  printScoreList(&score_list);

  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(specialKeys);
  glutReshapeFunc(reshape);

  glutMainLoop();

  saveData(&score_list);
  return 0;
}