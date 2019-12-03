#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "../include/libstemmer.h"

void 
print_counter (gpointer key, gpointer value, gpointer userdata) 
{
	char * t = key ;
	char * d = value ;

	if(key!=NULL&&value!=NULL){
		printf("(%s, %s)\n", t, d) ;
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

int main(){
	GHashTable* negativeTable = g_hash_table_new(g_str_hash,g_str_equal); 
	GHashTable* nonNegativeTable = g_hash_table_new(g_str_hash,g_str_equal); 
	FILE * f = fopen("model.csv","r");
	char * tempKey;		
	char * line = 0x0 ;
	size_t n = 0 ;
	int num;
	while (getline(&line, &n, f) >= 0) {
		char * t ;
		num=0;
		char * _line = line ;
		for (t = strtok(line, ",") ; t != 0x0 ; t = strtok(0x0, ",")) {
			num++;
			if(num==1) tempKey=t;
			else{	
				if(num==2) g_hash_table_insert(negativeTable,strdup(tempKey),strdup(t));	
				if(num==3){
					 for(int i=0;i<=strlen(t);i++)
						if(t[i]=='\n')t[i]='\0';
					 g_hash_table_insert(nonNegativeTable,strdup(tempKey),strdup(t));	
				}
			}

		}
		free(_line) ;
		line = 0x0 ;
	}
	printf("negative\n");
	g_hash_table_foreach(negativeTable,print_counter,0x0);
	printf("non-negative\n");
	g_hash_table_foreach(nonNegativeTable,print_counter,0x0);
}



