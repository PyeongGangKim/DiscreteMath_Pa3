#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "../include/libstemmer.h"

gboolean find_value(gpointer key, gpointer value, gpointer user_data){
	char * d = user_data;
	char * v = value;
	if(strcmp(d,v)==0) return TRUE;
	return FALSE;
}
void 
print_counter (gpointer key, gpointer value, gpointer userdata) 
{
	char * t = key ;
	int * d = value ;

	if(key!=NULL&&value!=NULL){
		printf("(%s, %d)\n", t, *d) ;
	}
}
void 
print_counter_string (gpointer key, gpointer value, gpointer userdata) 
{
	int * t = key ;
	char * d = value ;

	if(key!=NULL&&value!=NULL){
		printf("(%d, %s)\n", *t, d) ;
	}
}
gboolean remove_Ncounter(gpointer key, gpointer value, gpointer user_data){
	char * t = key;
	int * d = value;

	if(*d<=9){
		free(key);
		free(value);
		return TRUE;
	}
	return FALSE;
}

gboolean remove_Pcounter(gpointer key, gpointer value, gpointer user_data){
	char * t = key;
	int * d = value;

	if(*d<=5){
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
	FILE * cf = fopen("model.csv","w");

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
//Tokenization-Negative Sentences


	g_hash_table_foreach_remove(nCounter,remove_Ncounter,0x0);
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
	fclose(nf);

//make hash table "nCounter" that includes the appearance frequency of each word in the bag of negative words


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

//Tokenization-Positive Sentences


	g_hash_table_foreach_remove(pCounter,remove_Pcounter,0x0);
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
	fclose(pf);

//make hash table "pCounter" that includes the appearance frequency of each word in the bag of positive words


	list=g_hash_table_get_keys(nCounter);
	plist=g_hash_table_get_keys(pCounter);
	GHashTable * kCounter = g_hash_table_new(g_str_hash,g_int_equal);
	i=0;
	while(1){
		char* temp = g_list_nth_data(list,i);
		if(temp==NULL) break;
		int * d =malloc(sizeof(int));
		*d=i;
		g_hash_table_insert(kCounter,d,strdup(temp));
		i++;
	}
	pi=0;
	while(1){
		char * temp=g_list_nth_data(plist,pi);
		if(temp==NULL) break;
		pi++;
		
		int * d =malloc(sizeof(int));
		*d=i;
		char * checkVal=g_hash_table_find(kCounter,find_value,temp);
		if(checkVal==NULL) g_hash_table_insert(kCounter,d,strdup(temp));
		i++;
	}
//make hash table "kCounter" that includes all words in the negative and positive sentences.(kCOunter means key-Counter)


	int num=0;
	float nprob,pprob;
	float prob;
	int smoothing=5;
	int *nNum;
	int *pNum;
	char *temp;

	GList * klist = g_hash_table_get_values(kCounter);
	while(1){
		
		temp = g_list_nth_data(klist,num);

		if(temp==NULL) break;

		nNum=g_hash_table_lookup(nCounter,temp);
		pNum=g_hash_table_lookup(pCounter,temp);

		if(nNum==NULL){
			nNum=malloc(sizeof(int));
			*nNum=0;
		}
		if(pNum==NULL){
			pNum=malloc(sizeof(int));
			*pNum=0;
		}
		if(*nNum>900)*nNum=900;
		if(*pNum>500)*pNum=500;
		pprob=(float)(*pNum+smoothing)/(5565+smoothing*2);
		nprob=(float)(*nNum+smoothing)/(9078+smoothing*2);
		//prob=(nprob)/(nprob+pprob);
		fprintf(cf,"%s,%f,%f\n",temp,nprob,pprob);
		num++;

		free(nNum);
		free(pNum);
		
	}
	fclose(cf);

//make "model.csv" file. It includes 'word,negative possibility, positive possibility' info.
}
