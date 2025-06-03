#include "header/ranking.h"
#include "header/x11.h"

int main() {
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
  
  struct DESCRITOR score_list = {NULL, NULL, 0};

  initGui();

  return 0;
}