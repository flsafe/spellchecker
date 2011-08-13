#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORD 64
#define MAX_VOWELS 7
#define MAX_CONSEC 2
#define WTAB 20000 

typedef struct s_wlist{
  char word[MAX_WORD];
  int count;
  struct s_wlist *next;
} wlist;

wlist *wtab[WTAB];

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

  return 0;
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

void filter_known(wlist **l){
  wlist *cur, *prev, *t;
  
  prev = NULL;
  cur = *l;
  while (cur){
    if (!get(cur->word)){
      if (NULL == prev){
        t = cur;
        cur = cur->next;
        free(t);
        *l = cur;
      }
      else{
        prev->next = prev->next->next;
        free(cur);
        cur = prev->next;
      }
    }
    else{
      prev = cur;
      cur = cur->next;
    }
  }
}

void load_lang_model(char *f){
  FILE *file;
  char word[MAX_WORD] = "";
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
    for (j = i; w[j] == w[i] ; j++)
      ; /* Skip to the non-repeating chars */
    rcorrections(w, j, list);

    delchar(w, i);
    push(w, 0, list); 

    for (j = i; w[j] == w[i] ; j++)
      ; /* Skip to the non-repeating chars */
    rcorrections(w, j, list);
  }
}

void trim_consec(char *s, int p){
  int read, write, count;
  char c, prev;

  prev = '\0';
  read = write = count = 0;
  do {
    c = s[read]; 

    if (prev == c)
      count++;
    else 
      count = 0;

    if (count < p)
      s[write++] = c;

    prev = c;
    read++;
  } while(c);
}

void get_r_corrections(char *s, wlist **list){
  trim_consec(s, MAX_CONSEC);
  rcorrections(s, 0, list);
}

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

int nvowels(char *s){
  int count;

  count = 0;
  while(*s++)
    if (vowel(*s))
      count++;

  return count;
}

void vcorrections(char *s, int start, wlist **list){
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
    vcorrections(w, i+1, list);
  }
}

void get_v_corrections(char *s, wlist **list){vcorrections(s, 0, list);}

void get_cap_corrections(char *s, wlist **list){
  int correction;
  char *c; 
  char word[MAX_WORD];

  strncpy(word, s, MAX_WORD-1);
  word[MAX_WORD-1] = '\0';

  correction = 0;
  for (c = word ; *c ; c++)
    if (isupper(*c)){
      *c = tolower(*c);
      correction = 1;
    }

  if (correction)
    push(word, 0, list);
}

char *get_most_freq(wlist *l){
  wlist *cur;
  char *mostfreq;
  int maxcount;

  if (l == NULL) return NULL; 
  
  mostfreq = l->word;
  maxcount = l->count;
  for (cur = l ; cur ; cur = cur->next){
    if (get(cur->word) > maxcount)
      mostfreq = cur->word; 
  }

  return mostfreq;
}

void get_all_corrections(char *s, wlist **l){
  wlist *cur;

  push(s, 0, l);
  get_cap_corrections(s, l);
  for (cur = *l ; cur ; cur = cur->next)
    get_r_corrections(cur->word, l);
  for (cur = *l ; cur ; cur = cur->next)
    if (nvowels(cur->word) <= MAX_VOWELS)
      get_v_corrections(cur->word, l);
}

int main(){
  char *correction;
  char word[MAX_WORD];
  wlist *words;

  load_lang_model("freqs.txt");

  words = NULL; 
  while (scanf("%63s", word) != EOF){
    printf(">%s\n", word);

    if (get(word))
      printf("%s\n", word);
    else{
      get_all_corrections(word, &words);

      filter_known(&words);
      correction = get_most_freq(words);

      if (NULL == correction)
        printf("%s\n", "NO SUGGESTION"); 
      else
        printf("%s\n", correction);
    }
    empty(&words); 
  }

  return 0;
}
