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

 