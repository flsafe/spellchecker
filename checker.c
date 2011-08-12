#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORD 64

typedef struct s_wlist{
  char word[MAX_WORD];
  int count;
  struct s_wlist *next;
} wlist;

int push(char *w, int count, wlist **l){
  wlist *elem = malloc(sizeof(wlist));   
  if (!elem) return 0; 

  strncpy(elem->word, w, MAX_WORD - 1); 
  elem->word[MAX_WORD-1] = '\0';
  elem->count = count; 
  elem->next = *l;
  *l = elem;

  return 1;
}

void filter_known(wlist *l){

}

void empty(wlist **l){
  wlist *t, *cur;
  
  cur = *l;
  while (cur){
    t = cur;
    cur = cur->next;
    free(t);
  }
  *l = NULL;
}

#define WTAB 20000 

wlist *wtab[WTAB];

int hash(char *s){
  unsigned int h = 0;
  unsigned char *p = (unsigned char *) s;

  while(*p){
    h = h * 37 + *p;
    p++;
  }
  return h % WTAB;
}

int put(char *s, int count){
  return push(s, count, &wtab[hash(s)]);
}

int get(char *s){
  wlist *cur;

  for (cur = wtab[hash(s)] ; cur ; cur = cur->next)
    if (strcmp(cur->word, s) == 0)
      return cur->count;

  return -1;
}

void load_lang_model(char *f){
  FILE *file;
  char word[MAX_WORD], fmt[MAX_WORD];
  int count;

  file = fopen(f, "r");
  if (!file) return;

  while (fscanf(file, "%63s %d", word, &count) != EOF){
    put(word, count); 
  }
}

void delchar(char *w, int i){
  int read, write;
  char c;
  
  read = write = 0;
  do {
    c = w[read];
    if (read != i)
      w[write++] = c;
    read++;
  } while (c);
}

void rcorrections(char *s, int start, wlist **list){
  int i, j;
  char w[MAX_WORD];

  if (!s[start]) return;

  strncpy(w, s, MAX_WORD - 1);
  w[MAX_WORD-1] = '\0';

  for (i = start ; w[i+1] && w[i] != w[i+1] ; i++)
    ; /* Skip to first repeating char */

  while (w[i] == w[i+1]){
    delchar(w, i);
    push(w, 0, list); 

    for (j = i; w[j] == w[i] ; j++)
      ; /* Skip to the non-repeating chars */

    rcorrections(w, j, list);
  }
}

void get_r_corrections(char *s, wlist **list){rcorrections(s,0,list);}

char *vowels = "aeiou";

int vowel(char c){
  switch(c){
    case 'a': case 'e':
    case 'i': case 'o': case 'u':
      return 1;
      break;
    default: 
      return 0;
      break;
  }
}

void get_v_corrections(char *s, int start, wlist **list){
  int i, j;
  char w[MAX_WORD];

  if (!s[start]) return;

  strncpy(w, s, MAX_WORD - 1);
  w[MAX_WORD-1] = '\0';

  for (i = start ; w[i] && !vowel(w[i]) ; i++)
    ; /* skip to vowel */

  if (!w[i]) return;

  for (j = 0 ; vowels[j] ; j++){
    w[i] = vowels[j];
    push(w, 0, list);
    get_v_corrections(w, i+1, list);
  }
}

int main(){
  wlist *cur;
  int i;

  load_lang_model("freqs.txt"); 
  for (i = 0 ; i < WTAB ; i++)
    for (cur = wtab[i] ; cur ; cur = cur->next) 
      printf("%s %d\n", cur->word, cur->count);
  
  return 0;
}
