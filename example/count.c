#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "../include/libstemmer.h"

void 
print_counter (gpointer key, gpointer value, gpointer userdata) 
{
	char * t = key ;
	int * d = value ;

	if(key!=NULL&&value!=NULL){
		printf("(%s, %d)\n", t, *d) ;
	}
}

gboolean remove_counter(gpointer key, gpointer value, gpointer user_data){
	char * t = key;
	int * d = value;

	if(*d<=5 || *d>=500){
		free(key);
		free(value);
		return TRUE;
	}
	return FALSE;
}

int 
main () 
{
	FILE * f = fopen("../data/train.negative.csv", "r") ;

	GHashTable * counter = g_hash_table_new(g_str_hash, g_str_equal) ;		
	
	struct sb_stemmer * stemmer;
	stemmer = sb_stemmer_new("english", 0x0);
	char * line = 0x0 ;
	size_t r ; 
	size_t n = 0 ;

	while (getline(&line, &n, f) >= 0) {
		char * t ;
		char * _line = line ;
		for (t = strtok(line, " \n\t") ; t != 0x0 ; t = strtok(0x0, " \n\t")) {
			int i=0;
			while(t[i]!=0x0){
				if(!(t[i]>='a'&&t[i]<='z')){
					if(t[i]>='A'&&t[i]<='Z'){
						t[i]=t[i]+32;
					}
					else{
						for(int tempNum=i; t[tempNum]!=0x0; tempNum++){
							t[tempNum]=t[tempNum+1];
						}
						i--;
					}
				}
				i++;	
			}
			const char * s;
			s = sb_stemmer_stem(stemmer,t,strlen(t)) ;
			int * d ;			
			d = g_hash_table_lookup(counter, s) ;
			if (d == NULL) {
				d = malloc(sizeof(int)) ;
				*d = 1 ;
				g_hash_table_insert(counter, strdup(s), d) ;
			}
			else {
				*d = *d + 1 ;
			}
		}
		free(_line) ;
		line = 0x0 ;
	}
	g_hash_table_foreach_remove(counter,remove_counter,0x0);
	g_hash_table_foreach(counter, print_counter, 0x0) ;
	GList * list = g_hash_table_get_values(counter);
	int total;
	total=0;
	int i=0;
	while(1){
		int *temp = g_list_nth_data(list,i);
		if(temp==NULL) break;
		i++;
		total+=*temp;
	}
	printf("%d\n",total);
	printf("last: %d\n", *((int *) g_hash_table_lookup(counter, "last"))) ;

	fclose(f) ;
}
