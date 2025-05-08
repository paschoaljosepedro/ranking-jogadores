#include <stdio.h>
 #include <stdlib.h>
 #include <string.h>

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
     int posicao = 1;
     while(current){
         printf("Posicao : %d |Nick : %-25s | 3Score: %d\n",posicao,current->nickname, current->score);
         current = current->next;
         posicao++;
     }
     printf("Total de jogadores: %d\n",lista->size);
 }
 
void salvarDados(struct DESCRITOR *lista)
{
   FILE *arquivo = fopen("dados.txt","w");
   if ( arquivo == NULL)
   {
    printf("Erro ao abrir arquivo para escrita\n");
    return;
   } 
   struct RANKING *current = lista -> head;
   while(current!=NULL)
   {
    fprintf(arquivo, "%d,%s\n",current->score,current->nickname);
    current = current->next;
   }
   fclose(arquivo);
   printf("Dados salvos");
}

void carregarDados(struct DESCRITOR *lista)
{
    FILE *arquivo = fopen("dados.txt", "r");
    if (arquivo == NULL)
    {
        printf("Arquivo nao encontrado. Criando novo...\n");
    }
    char linha[100];
 
    while (fgets(linha, sizeof(linha), arquivo))
    {
        // remove o /n do final da linha
        linha[strcspn(linha, "\n")] = 0;

        char *token = strtok(linha, ",");
        int score = atoi(token);
        token = strtok(NULL, ",");
        char nick[25];
        if(token != NULL)
        {
            strncpy(nick, token, sizeof(nick) -1);
            nick[sizeof(nick) -1] = '\0';
            inserirFim(lista,score,nick);
        }
    }
    fclose(arquivo); 
}

 int main(){
     struct DESCRITOR ranking = {NULL, NULL,0};
     carregarDados(&ranking);
     int choice, score;
     char nick[25];
     while(1){
         printf("\nMenu:\n");
         printf("1. Adicionar jogador ao ranking\n");
         printf("2. Ver ranking completo\n");
         printf("3. Sair\n");
         scanf("%d", &choice);
         switch (choice)
         {
         case 1: 
             printf("Digite o nickname:\n");
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
             salvarDados(&ranking);
             exit(0);
         
         default:
             printf("Opção invalida");
             break;
         }
     }
   return 0;
 }