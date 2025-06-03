#include "header/x11.h"
#include "header/general.h"

void drawSearchResults(Display *display, Window w, int width, int height, GC gc, int current_submenu, char search_results[1024]) {
    XSetForeground(display, gc, 0xC0C0C0);
    XFillRectangle(display, w, gc, 0, 0, width, height);

    XSetForeground(display, gc, 0x808080);
    XFillRectangle(display, w, gc, 0, 0, width, BAR_HEIGHT);
    XFillRectangle(display, w, gc, 0, height - BAR_HEIGHT, width, BAR_HEIGHT);

    XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
    const char *title = current_submenu == 1 ? "Resultados da Pesquisa Binaria" : "Resultados da Pesquisa Exponencial";
    XDrawString(display, w, gc, 20, BAR_HEIGHT / 2 + 5, title, strlen(title));

    XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
    int start_y = BAR_HEIGHT + 30;
    int line_height = 20;
    
    // Desenha os resultados
    char *line = strtok(search_results, "\n");
    while (line != NULL && start_y < height - BAR_HEIGHT) {
        XDrawString(display, w, gc, 20, start_y, line, strlen(line));
        start_y += line_height;
        line = strtok(NULL, "\n");
    }

    XDrawString(display, w, gc, width/4, height - BAR_HEIGHT - 30, "Pressione Esc para voltar", 23);
}

void drawSearchForm(Display *display, Window w, int width, int height, GC gc, int current_submenu, char search_score[25]) {
    XSetForeground(display, gc, 0xC0C0C0);
    XFillRectangle(display, w, gc, 0, 0, width, height);

    XSetForeground(display, gc, 0x808080);
    XFillRectangle(display, w, gc, 0, 0, width, BAR_HEIGHT);
    XFillRectangle(display, w, gc, 0, height - BAR_HEIGHT, width, BAR_HEIGHT);

    XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
    const char *title = current_submenu == 1 ? "Pesquisa Binaria (por nome)" : "Pesquisa Exponencial (por pontuacao)";
    XDrawString(display, w, gc, 20, BAR_HEIGHT / 2 + 5, title, strlen(title));

    XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
    XDrawString(display, w, gc, width/4, height/3, 
               current_submenu == 1 ? "Nome do Jogador:" : "Pontuacao:", 
               current_submenu == 1 ? 16 : 10);
    XDrawRectangle(display, w, gc, width/4 + 150, height/3 - 15, 200, 30);
    XDrawString(display, w, gc, width/4 + 155, height/3 + 5, search_score, strlen(search_score));

    XDrawString(display, w, gc, width/4, height - BAR_HEIGHT - 30, "Pressione Enter para pesquisar ou Esc para cancelar", 50);
}

//parou de refatorar aqui
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
    
    XDrawString(display, w, gc, width/4, start_y, "Posição", 7);
    XDrawString(display, w, gc, width/4 + 100, start_y, "Nickname", 8);
    XDrawString(display, w, gc, width/4 + 300, start_y, "Pontuação", 9);
    start_y += line_height;
    
    while (current != NULL && position <= 10) {
        char pos_str[10];
        char score_str[10];
        
        sprintf(pos_str, "%d", position);
        sprintf(score_str, "%d", current->score);
        
        XDrawString(display, w, gc, width/4, start_y, pos_str, strlen(pos_str));
        XDrawString(display, w, gc, width/4 + 100, start_y, current->nickname, strlen(current->nickname));
        XDrawString(display, w, gc, width/4 + 300, start_y, score_str, strlen(score_str));
        
        current = current->next;
        position++;
        start_y += line_height;
    }

    XDrawString(display, w, gc, width/4, height - BAR_HEIGHT - 30, "Pressione Esc para voltar", 23);
}

void drawAddScoreForm(Display *display, Window w, int width, int height, GC gc) {
    XSetForeground(display, gc, 0xC0C0C0);
    XFillRectangle(display, w, gc, 0, 0, width, height);

    XSetForeground(display, gc, 0x808080);
    XFillRectangle(display, w, gc, 0, 0, width, BAR_HEIGHT);
    XFillRectangle(display, w, gc, 0, height - BAR_HEIGHT, width, BAR_HEIGHT);

    XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
    XDrawString(display, w, gc, 20, BAR_HEIGHT / 2 + 5, "Adicionar Pontuacao", 19);

    XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
    XDrawString(display, w, gc, width/4, height/3, "Nickname:", 9);
    XDrawRectangle(display, w, gc, width/4 + 100, height/3 - 15, 200, 30);
    XDrawString(display, w, gc, width/4 + 105, height/3 + 5, input_nickname, strlen(input_nickname));

    XDrawString(display, w, gc, width/4, height/3 + 50, "Pontuacao:", 10);
    XDrawRectangle(display, w, gc, width/4 + 100, height/3 + 35, 200, 30);
    XDrawString(display, w, gc, width/4 + 105, height/3 + 55, input_score, strlen(input_score));

    XDrawString(display, w, gc, width/4, height - BAR_HEIGHT - 30, "Pressione Enter para confirmar ou Esc para cancelar", 50);
}

void drawMenu(Display *display, Window w, int width, int height,
             MenuItem *items, int items_count, GC gc, GC selected_gc) {
    XSetForeground(display, gc, 0xC0C0C0);
    XFillRectangle(display, w, gc, 0, 0, width, height);

    XSetForeground(display, gc, 0x808080);
    XFillRectangle(display, w, gc, 0, 0, width, BAR_HEIGHT);
    XFillRectangle(display, w, gc, 0, height - BAR_HEIGHT, width, BAR_HEIGHT);

    XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
    const char *title = current_menu == 0 ? "Menu Principal" : 
                       (current_submenu == 1 ? "Submenu Pesquisa" : "Submenu Ordenacao");
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
                                      items[i].label, strlen(items[i].label)) / 2,
                   start_y + i * ITEM_HEIGHT + (ITEM_HEIGHT - ITEM_PADDING) / 2 + 5,
                   items[i].label, strlen(items[i].label));
    }
}

void handleSubmenuSelection(int selected_index) {
    if (selected_index == SUBMENU1_ITEMS - 1 || selected_index == SUBMENU2_ITEMS - 1) {
        current_menu = 0;
    } else if (current_submenu == 1) { // Submenu Pesquisa
        if (selected_index == 0 || selected_index == 1) { // Binária ou Exponencial
            searching_score = true;
            search_score[0] = '\0';
            search_results[0] = '\0';
            // Ordena por nome antes de pesquisar (para pesquisa binária)
            if (selected_index == 0) {
                merge_sort_wrapper(&score_list);
            }
        }
    } else if (current_submenu == 2) { // Submenu Ordenação
        if (selected_index == 0) { // Selection Sort (por pontuação)
            selection_sort(&score_list);
        } else if (selected_index == 1) { // Merge Sort (por nome)
            merge_sort_wrapper(&score_list);
        }
    }
}