#include<stdio.h>
#include<stdlib.h>
#include "hash_table.h"

void error_out(){
    fprintf(stderr, "Failed to read value\n");
    exit(1);
}

int main()
{   
    printf("______________________________________________________________________________________________________________\n");
    printf("\n\n\t\tWELCOME TO WORLD OF HASH TABLES\n\n");
    ht_hash_table* ht=ht_new();
    printf("=>Your Hash Table is ready to be used !!!\n\n");
    int a;
    char k[20];
    char buf[256];
    char* v = buf; // Using a buffer to store the value
    do{
        printf("_______________________________________________________________________________________\n");
        printf("1.INSERT\n2.SEARCH\n3.DELETE\n4.COUNT\n5.Current Size\n0.EXIT\nEnter your choice: ");
        if(scanf("%d",&a) != 1){
                error_out();
        }
        switch(a){
            case 0: continue;
                    break;

            case 1: 
                    printf("Enter key:");
                    if(scanf("%19s",k) != 1){
                        error_out();
                    }
                    printf("Enter value:");
                    if(scanf("%s",v) != 1){
                        error_out();
                    }
                    ht_insert(ht,k,v);
                    break;

            case 2: 
                    printf("Enter key:");
                    if(scanf("%19s",k) != 1){
                        error_out();
                    }
                    v=ht_search(ht,k);
                    if(v){
                        printf("\nValue : %s\n",v);
                    }else{
                        printf("\nKey is not in the table.\n");
                    }
                    break;

            case 3: 
                    printf("Enter key:");
                    if(scanf("%19s",k) != 1){
                        error_out();
                    }
                    ht_delete(ht,k);
                    break;
                
            case 4: printf("Current count of the table is : %d.\n\n",ht->count);
                    break;

            case 5: printf("Current size of the table is : %d.\n\n",ht->size);
                    break;

            default: printf("Enter correct option!!!\n");

        }
    }while(a!=0);
    ht_del_hash_table(ht);
    printf("=>Your Hash Table has been deleted !!!\n\n");
    printf("\t\tTHANK YOU\n");
    printf("______________________________________________________________________________________________________________");
    return 0;
}