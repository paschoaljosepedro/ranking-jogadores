#include <sys/types.h>
#include <stdbool.h>

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

struct RANKING* get_node_at_index(struct DESCRITOR *list, int index);
void swapNodes(struct DESCRITOR *score_list, struct RANKING *a, struct RANKING *b);