#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include<locale.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "my402list.h"
#include <ctype.h>



// struct for holding transaction data
typedef struct bankTransaction{
	char symbol;
	time_t time;
	unsigned long amount;
	char *description;
}bankData;


int GetNegativeFlag(char c)
{
	if(c=='+')
		return 0;
	else
		return 1;
}

void GetDecimal(unsigned long amount,char *amount_decimal_value)
{
	int num1,num2=0;
	num2=amount%10;
	amount=amount/10;
	num1=amount%10;
	sprintf(amount_decimal_value,"%d%d",num1,num2);
	return;
}
unsigned long GetAmount(unsigned long amount,char *amount_decimal_value)
{
	unsigned long tempamount=amount/100;
	GetDecimal(amount,amount_decimal_value);
	return tempamount;

}
void GetTime(time_t transaction_time,char *timestamp)
{
	struct tm *local;
	local=localtime(&transaction_time);
	strftime(timestamp,256,"%a %b %e %Y",local);
	return;
}

void GetBalanceDecimalValues(long balance,char *balance_decimal_value)
{
	int num1,num2=0;
	num2=balance%10;
	if(num2<0)
		num2=num2*(-1);
	balance=balance/10;
	num1=balance%10;
	if(num1<0)
	num1=num1*(-1);
	sprintf(balance_decimal_value,"%d%d",num1,num2);
	return;
}
long GetBalance(int negative_amount_flag,long balance,unsigned long amount,char *balance_decimal_value)
{
	if(negative_amount_flag)
	{
		amount=amount*-1;
		balance=balance+amount;
	}
	else
		balance=balance+amount;
	GetBalanceDecimalValues(balance,balance_decimal_value);
	return balance;
}
void displayResult(	My402List *list)
{
	long balance=0;
	int negative_amount_flag=0;
	char *amount_decimal_value=(char*)malloc(sizeof(10));
	char *balance_decimal_value=(char*)malloc(sizeof(10));
	unsigned long amountprev=0;
	char *timestamp =(char*)malloc(sizeof(15));
	char* description=(char*)malloc(sizeof(50));
	setlocale(LC_NUMERIC,"en_US");

	printf("+-----------------+--------------------------+----------------+----------------+\n");
	printf("|       Date      | Description              |         Amount |        Balance |\n");
	printf("+-----------------+--------------------------+----------------+----------------+\n");
	My402ListElem *temp=list->anchor.next;
	while(temp->next!=list->anchor.next)
	{
		negative_amount_flag=GetNegativeFlag(((bankData*)temp->obj)->symbol);
		description=((bankData*)temp->obj)->description;
		amountprev=GetAmount(((bankData*)temp->obj)->amount,amount_decimal_value);

		GetTime(((bankData*)temp->obj)->time,timestamp);
		balance=GetBalance(negative_amount_flag,balance,(((bankData*)temp->obj)->amount),balance_decimal_value);

		if(balance<10000000)
		{
			if(balance>0)
			{
				if(negative_amount_flag)
					printf("| %14s | %-24s | (%'9lu.%s) | %'10ld.%s  |\n",timestamp,description,amountprev,amount_decimal_value,balance/100,balance_decimal_value);
				else
					printf("| %14s | %-24s | %'10lu.%s  | %'10ld.%s  |\n",timestamp,description,amountprev,amount_decimal_value,balance/100,balance_decimal_value);
			}
			else
			{
				if(negative_amount_flag)
					printf("| %14s | %-24s | (%'9lu.%s) | (%'9ld.%s) |\n",timestamp,description,amountprev,amount_decimal_value,(balance * -1)/100,balance_decimal_value);
				else
					printf("| %14s | %-24s | %'10lu.%s  | (%'9ld.%s) |\n",timestamp,description,amountprev,amount_decimal_value,(balance*-1)/100,balance_decimal_value);
			}
		}
		else
		{
			char *question_display="?,???,???.??";
			if(negative_amount_flag)
					printf("| %14s | %-24s | (%'9lu.%s) |  %s  |\n",timestamp,description,amountprev,amount_decimal_value,question_display);
			else
					printf("| %14s | %-24s | %'10lu.%s  |  %s  |\n",timestamp,description,amountprev,amount_decimal_value,question_display);
		}
		temp=temp->next;
	}
		printf("+-----------------+--------------------------+----------------+----------------+\n");
		return;
}

int verify_file(char* fname)
{
        struct stat s;
        if ( access(fname, F_OK) == -1 )
        {
                fprintf(stderr, "File does not exist\n!");
                exit(1);
        }
        else if ( access(fname, R_OK) == -1 )
        {
                fprintf(stderr, "File does not have read permissions !\n");
                exit(1);
        }
        else if ( ! stat (fname, &s) )
        {
                if ( s.st_mode & S_IFDIR )
                {
                        fprintf(stderr, "Specified File is Directory !\n");
                        exit(1);
                }
        }

        return TRUE;
}

void BubbleForward(My402List *pList, My402ListElem **pp_elem1, My402ListElem **pp_elem2)
	{
	    My402ListElem *elem1=(*pp_elem1), *elem2=(*pp_elem2);
	    void *obj1=elem1->obj, *obj2=elem2->obj;
	    My402ListElem *elem1prev=My402ListPrev(pList, elem1);
	    My402ListElem *elem2next=My402ListNext(pList, elem2);

	    My402ListUnlink(pList, elem1);
	    My402ListUnlink(pList, elem2);
	    if (elem1prev == NULL) {
	        (void)My402ListPrepend(pList, obj2);
	        *pp_elem1 = My402ListFirst(pList);
	    } else {
	        (void)My402ListInsertAfter(pList, obj2, elem1prev);
	        *pp_elem1 = My402ListNext(pList, elem1prev);
	    }
	    if (elem2next == NULL) {
	        (void)My402ListAppend(pList, obj1);
	        *pp_elem2 = My402ListLast(pList);
	    } else {
	        (void)My402ListInsertBefore(pList, obj1, elem2next);
	        *pp_elem2 = My402ListPrev(pList, elem2next);
	    }
	}

	void sortList(My402List *pList, int num_items)
	{
	    My402ListElem *elem=NULL;
	    int i=0;

	    if (My402ListLength(pList) != num_items) {
	        fprintf(stderr, "List length is not %1d in BubbleSortForwardList().\n", num_items);
	        exit(1);
	    }
	    for (i=0; i < num_items; i++) {
	        int j=0, something_swapped=FALSE;
	        My402ListElem *next_elem=NULL;

	        for (elem=My402ListFirst(pList), j=0; j < num_items-i-1; elem=next_elem, j++)
	        {
	            int cur_val=(int)((bankData*)elem->obj)->time, next_val=0;

	            next_elem=My402ListNext(pList, elem);
	            next_val =(int)((bankData*)next_elem->obj)->time;
	            //verification for time
//	            time_t diftym;

	            if (cur_val == next_val || cur_val >= time(NULL))
	            {
	            	fprintf(stderr,"THere is error in timestamp\n");
	            	exit(1);
	            }
	            if (cur_val > next_val)
	            {
	                BubbleForward(pList, &elem, &next_elem);
	                something_swapped = TRUE;
	            }
	        }
	        if (!something_swapped) break;
	    }

	}
char* spacetrimmer(char* stringdesc){
	int i=0;
	for(i=0;isspace(stringdesc[i]);i++){}
	return stringdesc+i;
}


 unsigned long Processtoken2(char *tokenizedData)
{
	char *amounttoken[2];
	if(tokenizedData[0] == '.')
	{
		fprintf(stderr,"Amount cannot be less than 1\n");
		exit(1);
	}
	amounttoken[0]=strtok(tokenizedData,".");
	amounttoken[1]=strtok(NULL,"");
	if( (strlen(amounttoken[0]) > 7 ))
		{
			fprintf(stderr,"Amount cannot be greater than 10000000\n");
			exit(1);
		}


	if( (strlen(amounttoken[1]) > 2 ) || (strlen(amounttoken[1]) < 2 ) )
	{
		fprintf(stderr,"Decimal places cant be greater than 2 or less than 2\n");
		exit(1);
	}
	strcat(amounttoken[0],amounttoken[1]);
	return (atol(amounttoken[0]));
}
void readingFile(FILE* fpointer,My402List* list)
{
	char buff[1024];
	while(fgets(buff,sizeof(buff),fpointer)!=NULL)
	{
		//check length of one transaction is more than 81 chars.
		if(sizeof(buff)>1024)
		{
			fprintf(stderr,"File too long , only 80 chars needed\n");
			exit(1);
		}

		bankData *object=(bankData*)malloc(sizeof(bankData));
		char *tokens[5];
		tokens[0]=strtok(buff,"\t");
		if(tokens[0]==NULL)
			{
				fprintf(stderr,"Transaction Invalid\n");
				exit(1);
			}
		object->symbol=*tokens[0];

		if(object->symbol !='+' && object->symbol !='-')
			{
				fprintf(stderr,"exiting as type of transaction not valid\n");
				exit(1);
			}

		tokens[1]=strtok(NULL,"\t");
		if(tokens[1]==NULL)
			{
				fprintf(stderr,"Transaction Invalid\n");
				exit(1);
			}
		object->time=atol(tokens[1]);  // we will verify time after sorting


		// now check if time in file is greater than current time

		tokens[2]=strtok(NULL,"\t");
		if(tokens[2]==NULL)
			{
				fprintf(stderr,"Transaction Invalid\n");
				exit(1);
			}

		tokens[3]=strtok(NULL,"\t\n");
		if(tokens[3]==NULL)
			{
				fprintf(stderr,"Transaction Invalid\n");
				exit(1);
			}
		if(strtok(NULL,"\n")!=NULL)
				{
					fprintf(stderr,"Too many fields\n");
					exit(1);
				}

		char* str_container=(char*)malloc(sizeof(50));
		str_container=spacetrimmer(tokens[3]);
		if(str_container==NULL)
			{
			 fprintf(stderr,"Description cant be zero\n");
			 exit(1);
			}



		//verification of amount
		object->amount=Processtoken2(tokens[2]);
		if(strlen(tokens[3])>24)
		{
			tokens[3][24]='\0';
		}
		object->description=strdup(str_container);
		//tokens[4]=strtok(NULL,"\n");

		My402ListAppend(list,object);
	}
	return;
}

int main(int argc, char* argv[])
{
	int flag_file=0;
	//file pointer
	FILE *fpointer;
	if(argc==0 || argc==1)
	{
		fprintf(stderr,"Not enough commands\n");
		exit(1);
	}
	if(argc==2)
	{
		if (!strcmp(argv[1], "sort") )
			{
				fpointer=stdin;
				flag_file=1;
			}
		else
		{
			fprintf(stderr,"Sort should be second argument\n");
			exit(1);
		}
	}
	char* filename;

	if(argc == 3)
		{
			if (!strcmp(argv[1], "sort"))
	         {
					filename = argv[2];
	               	struct stat s;
	               	if ( access(filename, F_OK) == -1 )
	               	{
	               		fprintf(stderr, "file does not exist\n");
	               		exit(1);
	                }
	               	if ( access(filename, R_OK) == -1 )
	               	   	   {
	               		   	   fprintf(stderr, "file doesn't have read permissions\n");
	               		   	   	exit(1);
	               	   	   }
	               	 if ( ! stat (filename, &s) )
	                	    {
	               		   	   if ( s.st_mode & S_IFDIR )
	               		   	   	   {
	               		   		   	   fprintf(stderr, "specified File is Directory\n");
	               		   		   	   exit(1);
	               		   	   	   }
	            }
	            fpointer = fopen(filename, "r");
	            if(fpointer==NULL)
	            {
	            	exit(1);
	            }
	        }
	         else
	         {
	              fprintf(stderr, "sort should be second argument\n");
	              exit(1);
	         }
	   }


	if(argc>3)
	{
		printf("Too many arguments\n");
		exit(1);
	}
	if(!flag_file)
	{
		fpointer=fopen(argv[2],"r");
		if(fpointer==NULL)
			{
				exit(1);
			}
	}
	My402List list;
	if(!My402ListInit(&list))
	{
		fprintf(stderr,"Not able to initialize\n");
		exit(1);
	}

	readingFile(fpointer,&list);
	sortList(&list,list.num_members);
	displayResult(&list);
	fclose(fpointer);
	return 0;
}

