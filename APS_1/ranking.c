#include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 //lista com descritor
 
 struct RANKING{
     struct RANKING *previous;
     int score;
     char nickname[25];
     struct RANKING *next;
 };
 
 struct DESCRITOR{
     struct RANKING *head;
     struct RANKING *tail;
     int size;
     
 };
 void inserirFim(struct DESCRITOR *lista, int score, const char *nick){
     struct RANKING *new = malloc(sizeof(struct RANKING));
     if(!new){
         printf("Erro ao alocar memoria!");
         return;
     }
     new->score = score;
     strncpy(new->nickname, nick, sizeof(new->nickname) - 1);
     new->nickname[sizeof(new->nickname) - 1] = '\0';
     new->next = NULL;
     new->previous = lista->tail;
     if(lista->size ==0)
         lista->head = new;
     else 
         lista->tail->next = new;
     lista->tail = new;
     lista->size++;     
 };
 
 struct RANKING* split(struct RANKING *head){
     struct RANKING *slow = head, *fast = head;
     while (fast->next && fast->next->next)
     {
         fast = fast->next->next;
         slow = slow->next;
     }
     struct RANKING *temp = slow->next;
     slow->next = NULL;
     if(temp)
         temp->previous= NULL;
         return temp;
 };
 
 struct RANKING* merge(struct RANKING *first, struct RANKING *second){
     if(!first)
         return second;
     if(!second)
         return first;
     if(first->score >= second->score){
         first->next = merge(first->next,second);
         first->next->previous= first;
         first->previous = NULL;
         return first;
     } else{
         second->next = merge(first, second->next);
         second->next->previous = second;
         second->previous = NULL;
         return second;
     }
 }
 
 struct RANKING* mergeSort(struct RANKING *head){
     if(!head || !head->next)
         return head;
     struct RANKING *second = split(head);
     head = mergeSort(head);
     second = mergeSort(second);
     return merge(head,second);
 }
 
 void ordenarLista(struct DESCRITOR *lista){
     lista->head = mergeSort(lista->head);
     
     struct RANKING *temp = lista->head;
     while(temp && temp->next){
         temp = temp->next;
     }
     lista->tail = temp;
 }
 
 void exibirRanking(struct DESCRITOR *lista){
     printf("\nRANKING\n");
     struct RANKING *current = lista->head;
     while(current){
         printf("Nick : %-10s | Score: %d\n",current->nickname, current->score);
         current = current->next;
     }
     printf("Total de jogadores: %d\n",lista->size);
 }
 
 int main(){
     struct DESCRITOR ranking = {NULL, NULL,0};
     int choice, score;
     char nick[10];
     while(1){
         printf("\nMenu:\n");
         printf("1. Adicionar jogador ao ranking\n");
         printf("2. Ver ranking completo\n");
         printf("3. Sair\n");
         scanf("%d", &choice);
         switch (choice)
         {
         case 1: 
             printf("Dite o nickname:\n");
             scanf("%s",nick);
             printf("Digite o score\n");
             scanf("%d",&score);
             inserirFim(&ranking, score, nick);
             break;
         case 2:
             ordenarLista(&ranking);
             exibirRanking(&ranking);
             break;
         case 3:
             exit(0);
         
         default:
             printf("Opção invalida");
             break;
         }
     }
   return 0;
 }
