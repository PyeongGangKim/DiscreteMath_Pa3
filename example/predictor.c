#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <glib.h>
#include "../include/libstemmer.h"

void
print_counter (gpointer key, gpointer value, gpointer userdata)
{
    char * t =key;
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

    struct sb_stemmer * stemmer;
    stemmer = sb_stemmer_new("english",0x0);
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

    while(1){
	char * in_feed;
	in_feed = (char *)malloc(sizeof(char)*1000);
	if(fgets(in_feed,sizeof(in_feed)*1000,stdin) == NULL) break;
	char * temp;
	char * string = (char*)malloc(sizeof(char)*1000);
	double sum_n = 0,sum_p = 0;
	strcpy(string,in_feed);
	for(temp = strtok(string," "); temp != 0x0 ; temp = strtok(0x0," "))
	{    

	    int i = 0;
	    while(temp[i]!= 0x0)
	    {
		if(!(temp[i] >= 'a' && temp[i] <= 'z'))
		{
		    if(temp[i] >= 'A' && temp[i] <= 'Z')
		    {
			temp[i] = temp[i] + 32;
		    }
		    else
		    {
			for(int tempNum=i;temp[tempNum]!=0x0;tempNum++)
			{
			    temp[tempNum]=temp[tempNum+1];
			}
			i--;
		    }
		}
		i++;
	    }
//       printf("%s\n",temp);
   
	    const char *s;
   
	    s = sb_stemmer_stem(stemmer,temp,strlen(temp));
	    char * dn;
	    char * dp;
	    char * prob_s = (char*)malloc(sizeof(char) * 10);
	    float pprob, nprob;
   
	    dn = g_hash_table_lookup(negativeTable,s);
   
	    if(dn != NULL)
	    {
		nprob = atof(dn);
	    }

	    dp = g_hash_table_lookup(nonNegativeTable,s);
   
	    if(dp != NULL)
	    {   
		pprob = atof(dp);
	    }
	    if(dp != NULL &&dn != NULL){
		sum_p += log(pprob);
		sum_n += log(nprob);
	    }
	}
    // receive input, and torkenazate the sentense, and push into negative, nonnegative table keys and values.    
	double exp_p = exp(sum_p);
	double exp_n = exp(sum_n);
	double result = exp_n / (exp_p + exp_n);
	if(result > 0.964){
	    printf("negative\n");
	}else{
	    printf("non-negative\n");
	}
    }
}
