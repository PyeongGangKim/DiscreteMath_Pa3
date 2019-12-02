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
	FILE * nf = fopen("../data/train.negative.csv", "r") ;
	FILE * pf = fopen("../data/train.non-negative.csv", "r");
	
	GHashTable * nCounter = g_hash_table_new(g_str_hash, g_str_equal) ;		
	
	struct sb_stemmer * stemmer;
	stemmer = sb_stemmer_new("english", 0x0);
	char * line = 0x0 ;
	size_t n = 0 ;

	while (getline(&line, &n, nf) >= 0) {
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
			d = g_hash_table_lookup(nCounter, s) ;
			if (d == NULL) {
				d = malloc(sizeof(int)) ;
				*d = 1 ;
				g_hash_table_insert(nCounter, strdup(s), d) ;
			}
			else {
				*d = *d + 1 ;
			}
		}
		free(_line) ;
		line = 0x0 ;
	}

	g_hash_table_foreach_remove(nCounter,remove_counter,0x0);
//	g_hash_table_foreach(nCounter, print_counter, 0x0) ;
	GList * list = g_hash_table_get_values(nCounter);
	int total;
	total=0;
	int i=0;
	while(1){
		int *temp = g_list_nth_data(list,i);
		if(temp==NULL) break;
		i++;
		total+=*temp;
	}
//	printf("%d\n",total);
//	printf("bad: %d\n", *((int *) g_hash_table_lookup(nCounter, "bad"))) ;
	fclose(nf);

	GHashTable * pCounter = g_hash_table_new(g_str_hash, g_str_equal) ;		
	
	char * pline = 0x0 ;
	size_t pn = 0 ;

	while (getline(&pline, &pn, pf) >= 0) {
		char * t ;
		char * _line = pline ;
		for (t = strtok(pline, " \n\t") ; t != 0x0 ; t = strtok(0x0, " \n\t")) {
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
			d = g_hash_table_lookup(pCounter, s) ;
			if (d == NULL) {
				d = malloc(sizeof(int)) ;
				*d = 1 ;
				g_hash_table_insert(pCounter, strdup(s), d) ;
			}
			else {
				*d = *d + 1 ;
			}
		}
		free(_line) ;
		pline = 0x0 ;
	}

	g_hash_table_foreach_remove(pCounter,remove_counter,0x0);
//	g_hash_table_foreach(pCounter, print_counter, 0x0) ;
	GList * plist = g_hash_table_get_values(pCounter);
	int ptotal;
	ptotal=0;
	int pi=0;
	while(1){
		int *temp = g_list_nth_data(plist,pi);
		if(temp==NULL) break;
		pi++;
		ptotal+=*temp;
	}
//	printf("%d\n",ptotal);
//	printf("bad: %d\n", *((int *) g_hash_table_lookup(pCounter, "bad"))) ;
	fclose(pf);
}
