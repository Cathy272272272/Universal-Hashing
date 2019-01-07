#include<stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <math.h>
#define WORD_LEN 64
#define DEF_LEN 256
#define USER_INPUT_LEN 512
#define COMMAND_LEN 32
#define INITIAL_SIZE (16)
#define GROWTH_FACTOR (2)
#define MAX_LOAD_FACTOR (0.75)
/* keyValue pairs */
/* where in this case, key is the hashValue derived from given structure */
struct kv{
    char * key;
    char * value;
    struct kv * next;
};
typedef struct kv keyValue;
typedef unsigned int (* hash)(char * word,int nbins);
struct dict {
    int size;          
    int n;              
    struct kv **table;
    hash func;
    int * num;
};
int a[WORD_LEN];
int type;
typedef struct dict *Dict;
Dict Initialize(unsigned int (* hash)(char * word,int nbins));
Dict Initialize_helper(int size);
static void rehash(Dict d, int new_size);
void Destroy(Dict);
void stats(Dict);
int insert(Dict, char *key, char *value);
void print_dictionary(Dict d);
int search(Dict, char *key);
int delete(Dict, char *key);
void random_a(int nbins){
    srand(time(NULL));
    for ( int i = 0; i < WORD_LEN; i++ ){
	    a[i] = rand() % nbins;
	}
}
unsigned int universal_hash(char *s, int nbins)
{
    unsigned const char *us;
    unsigned int h;
    int i = 0;
    h = 0;
    for(us = (unsigned const char *) s; *us; us++) {
	    h += (*us % nbins) * a[i++];
    }
    return h % nbins;
}
unsigned int naive_hash(char *word,int nbins){
    unsigned int h=0;
    int c;
    while(c = *word++ ) h += c;
    return h % nbins;
}
unsigned int bernstein_hash(char *word,int nbins){
    unsigned int h = 5381;
    int c;
    while( c = *word++) h = 33 * h + c;
    return h % nbins;
}
unsigned int FNV_hash(char *word, int nbins){
    unsigned long h = 14695981039346656037lu;
    char c;
    while( c = *word++ ){
        h = h * 1099511628211lu;
        h = h^c;
    }
    return h % nbins;
}
void stats(Dict d){
    int bin = d->size;
    double occupancy = (double)d->n / bin;
    int cnt = 0;
    int max = 0, tmp = 0;
    keyValue * e;
    for ( int i = 0; i < bin; i++ ){
        if ( d->table[i] != 0 ) cnt++;
        tmp = 0;
        for(e = d->table[i]; e != 0; e = e->next) {
            tmp++;
        }
        if ( tmp > max ) max = tmp;
    }
    double fraction = (double)cnt / bin;
    printf("Bins: %d occupancy: %.3f used bin fraction: %.3f max entries in a bin: %d \n", bin, occupancy, fraction, max);
}
Dict Initialize_helper(int size)
{
    Dict d;
    int i;
    d = malloc(sizeof(*d));
    d->size = size;
    /* find next prime */
    if ( type == 4 ){
    int flag = 1;
    while ( flag == 1 ){
    	flag = 0;
	    for ( i = 2; i <= sqrt(d->size); i++ )
		    if ( d->size % i == 0 ){
			    flag = 1;
			    break;
		    }
	    if ( flag == 1 ) d->size++;
        }
    }
    d->n = 0;
    d->table = malloc(sizeof(keyValue  *) * d->size);
    d->num = malloc(sizeof(int) * d->size);
    for(i = 0; i < d->size; i++) {
	    d->table[i] = 0;
        d->num[i] = 0;
    }
    if ( type == 4 ) random_a(size);
    return d;
}

Dict Initialize(unsigned int (* hash)(char * word,int nbins))
{
    Dict d = Initialize_helper(INITIAL_SIZE);
    d->func = hash;
    return d;
}

void Destroy(Dict d)
{
    int i;
    keyValue *e;
    keyValue *next;
    for(i = 0; i < d->size; i++) {
        for(e = d->table[i]; e != 0; e = next) {
            next = e->next;
    	    free(e->key);
	        free(e->value);
            free(e);
        }
	    d->table[i] = e;
    }
    d->n = 0;
}
static void rehash(Dict d, int new_size)
{
    Dict d2;        
    struct dict tmp; 
    int i;
    keyValue *e, *next;
    d2 = Initialize_helper(new_size);
    d2->func = d->func;
    for(i = 0; i < d->size; i++) {
        for(e = d->table[i]; e != 0; e = next) {
	        next = e->next;
            insert(d2, e->key, e->value);
	        free(e->key);
	        free(e->value);
	        free(e);
        }
        d->table[i] = e;
    }

    tmp = *d;
    *d = *d2;
    *d2 = tmp;
    free(d2->table);
    free(d2->num);
    free(d2);
    d2 = NULL;
}
int check(Dict d, char *key)
{
    keyValue *e;
    for(e = d->table[d->func(key, d->size)]; e != 0; e = e->next) {
        if(!strcmp(e->key, key)) {
            return 0;
        }
    }
    return 1;
}
int insert(Dict d, char *key, char *value)
{
    if ( check(d, key) == 0 ) return 1;
    if(d->n >= d->size * MAX_LOAD_FACTOR) {
        rehash(d, GROWTH_FACTOR * d->size);
    }
    keyValue *e;
    unsigned int h;
    e = malloc(sizeof(*e));
    e->key = strdup(key);
    e->value = strdup(value);
    h = d->func(key, d->size);
    e->next = d->table[h];
    d->table[h] = e;
    d->n++;
    d->num[h]++;
    if( d->n >= d->size * MAX_LOAD_FACTOR) {
        rehash(d, GROWTH_FACTOR * d->size);
    }
    else if ( type == 4 && d->num[h] > d->n / 4 && d->n > 10 ) rehash(d, d->size);
    return 0;
}

int search(Dict d, char *key)
{
    keyValue *e;
    for(e = d->table[d->func(key, d->size)]; e != 0; e = e->next) {
        if(!strcmp(e->key, key)) {
            printf("%s %s \n", e->key, e->value);
            return 0;
        }
    }
    return 1;
}

int delete(Dict d, char *key)
{
    if ( search(d, key) == 1 ) return 1;
    keyValue **prev;          
    keyValue *e;              
    for(prev = &(d->table[d->func(key, d->size)]); *prev != 0; prev = &((*prev)->next)) {
        if(!strcmp((*prev)->key, key)) {
    	    d->n--;
            e = *prev;
            *prev = e->next;
            free(e->key);
            free(e->value);
            free(e);
            return 0;
        }
    }
    return 1;
}
int import_file(Dict d, char * fname)
{
	int word_cnt = 0;
	FILE* fp = fopen(fname, "r");
	if(NULL == fp)
	{

		return 1;
	}
	else
	{
		char word[WORD_LEN];
		char definition[DEF_LEN];
		char* pos;
		int error = 100;
		while(EOF != fscanf(fp, "%s", word))
		{
			fgets(definition, DEF_LEN, fp);
			if(NULL != (pos = strchr(definition, '\n')))
				*pos = '\0';
    			insert(d, word, definition);
			    if(0 == error)
				    word_cnt++;
			    memset(word, 0x00, sizeof(char) * WORD_LEN);
			    memset(definition, 0x00, sizeof(char) * DEF_LEN);
		}
	}

	fclose(fp);
	fp = NULL;
	return 0;	
}
void print_dictionary(Dict d)
{
unsigned long i, j = 0, min;
keyValue *e;
keyValue *arr[d->n];
for(i = 0; i < d->size; i++){
    if ( d->table[i] == 0 ) continue;
    for(e = d->table[i]; e != 0; e = e->next) {
	    arr[j++] = e;
    }
}
for (i = 0; i < d->n; i++){
	min = i;
	for ( j = i + 1; j < d->n; j++){
		if (strcmp(arr[j]->key, arr[min]->key) < 0 )
			min = j;
	}
	e = arr[min];
	arr[min] = arr[i];
	arr[i] = e;
	puts(arr[i]->key);
	puts(e->value);
}	
}
int main(int argc, char **argv)
{
    Dict d;
    int i = atoi(argv[1]);
    type = i;
    if ( i == 1 ) d = Initialize(naive_hash);
    else if ( i == 2 ) d = Initialize(bernstein_hash);
    else if ( i == 3 ) d = Initialize(FNV_hash);
    else {
        d = Initialize(universal_hash);
    }
   	while(1)
	{
		int error;
		char command[COMMAND_LEN];

		/* Get the User's Command */
		printf(">$ ");
		scanf("%s", command);

		/* Add word */
		if( strncmp(command, "add", COMMAND_LEN) == 0 )
		{
			char word[WORD_LEN];
			char definition[DEF_LEN];
			char * pos;
			scanf("%s ",word);
			/* Gets the rest of the input */
			fgets(definition, DEF_LEN, stdin);
			if ((pos=strchr(definition, '\n')) != NULL)
				*pos = '\0';

			/* Adds to dictionary */
            error = insert(d, word, definition);
			if( error == 0 )
				printf("The word \"%s\" has been added successfully.\n", word);
			else
				printf("Error when adding word \"%s\".\n", word);

		}
		/* Import file */
		if( strncmp(command, "import", COMMAND_LEN) == 0 )
		{
			char fname[DEF_LEN];
			scanf("%s",fname);
			error = import_file(d, fname);
			if( error == 0 )
				printf("The file \"%s\" has been imported successfully.\n", fname);
			else
				printf("Error when adding file \"%s\".\n", fname);
		}
		/* Delete word */
		if( strncmp(command, "delete", COMMAND_LEN) == 0 )
		{
			char word[WORD_LEN];
			scanf("%s",word);
			error = delete(d, word);
			if( error == 0 )
				printf("The word \"%s\" has been deleted successfully.\n", word);
			else
				printf("Error when deleting word \"%s\".\n", word);
		}
		/* Find word */
		if( strncmp(command, "find", COMMAND_LEN) == 0 )
		{
			char word[WORD_LEN];
			scanf("%s",word);
			error = search(d, word);
			if( error == 1 )
				printf("Error when finding word \"%s\".\n", word);
		}
		/* Print dictionary */
		if( strncmp(command, "print", COMMAND_LEN) == 0 )
			print_dictionary(d);
		/* clear dictionary */
		if( strncmp(command, "clear", COMMAND_LEN) == 0 )
			Destroy(d);
        if( strncmp(command, "stats", COMMAND_LEN) == 0 )
            stats(d);
		/* quit program */
		if( strncmp(command, "quit", COMMAND_LEN) == 0 )
			break;
	}
    /* free the dictionary. */
    free(d);
    return 0;
}
