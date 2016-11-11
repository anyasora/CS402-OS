#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cs402.h"
#include "my402list.h"
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// finding the length
int  My402ListLength(My402List* list)
{
	return list->num_members;
}

// is list empty or not??
int  My402ListEmpty(My402List* list)
{
	if(list->anchor.next== &(list->anchor))
		return TRUE;
	else
		return FALSE;
}

//appending to the list
int  My402ListAppend(My402List* list, void* object)
{
	My402ListElem *temp= malloc(sizeof(My402ListElem));
	if(temp==NULL)
		return FALSE;

	temp->obj=object;
	temp->next=NULL;
	temp->prev=NULL;

	if(list->anchor.next==&(list->anchor))
	{
		temp->next=&(list->anchor);
		temp->prev=&(list->anchor);
		list->anchor.next=temp;
		list->anchor.prev=temp;
		list->num_members++;
		return TRUE;
	}
	else
	{
		My402ListElem *last=list->anchor.prev;

		temp->next=&(list->anchor);
		temp->prev=last;
		last->next=temp;
		list->anchor.prev=temp;
		list->num_members++;
		return TRUE;
	}
	return FALSE;
}

// prepending to the list
int  My402ListPrepend(My402List* list, void* elem)
{
	My402ListElem *temp= malloc(sizeof(My402ListElem));
	if(temp==NULL)
		return FALSE;

	temp->obj=elem;
	temp->next=NULL;
	temp->prev=NULL;
	if(list->anchor.next==&(list->anchor))
	{
		temp->next=&(list->anchor);
		temp->prev=&(list->anchor);
		list->anchor.next=temp;
		list->anchor.prev=temp;
		list->num_members++;
		return TRUE;
	}
	else
	{
		My402ListElem *first=list->anchor.next;
		temp->next=first;
		temp->prev=&(list->anchor);
		list->anchor.next=temp;
		first->prev=temp;
		list->num_members++;
		return TRUE;
	}
}

//UNlinking the element in the list.
void My402ListUnlink(My402List* list, My402ListElem* elem)
{
	My402ListElem *previous;
	My402ListElem *nextelement;
	previous=elem->prev;
	nextelement=elem->next;
	previous->next=nextelement;
	nextelement->prev=previous;
	free(elem);
	list->num_members--;
	return;
}

//UNlinking all the elements
void My402ListUnlinkAll(My402List* list)
{
	if(list->num_members==0)
		return;
	else
	{
		My402ListElem *header=list->anchor.next;
		My402ListElem *headernext=header->next;
		while(header->next != &(list->anchor))
		{
			headernext=header->next;
			headernext->prev=header;
			list->anchor.next=headernext;
			headernext->prev=&(list->anchor);
			free(header);
			header=headernext;
		}
		free(header);
		list->anchor.next=&(list->anchor);
		list->anchor.prev=&(list->anchor);
		list->num_members=0;
	}
}

//Inserting after specific
int  My402ListInsertAfter(My402List* list, void* object, My402ListElem* element)
{
	if(element==NULL)
		return My402ListAppend(list,object);

	My402ListElem *temp=malloc(sizeof(My402ListElem));
	if(temp==NULL)
		return FALSE;

	temp->obj=object;
	temp->next=NULL;
	temp->prev=NULL;

	My402ListElem *nextelement=element->next;

	element->next=temp;
	temp->prev=element;
	temp->next=nextelement;
	nextelement->prev=temp;
	list->num_members++;
	return TRUE;
}

// INserting BEFORE specific element
int  My402ListInsertBefore(My402List* list, void* object, My402ListElem* element)
{
	if(element == NULL)
	{
		return My402ListPrepend(list,object);
	}

	My402ListElem *temp=malloc(sizeof(My402ListElem));
	if(temp==NULL)
		return FALSE;

	temp->obj=object;
	temp->next=NULL;
	temp->prev=NULL;

	My402ListElem *prevelement=element->prev;
	element->prev=temp;
	temp->next=element;
	temp->prev=prevelement;
	prevelement->next=temp;
	list->num_members++;
	return TRUE;
}

//Return first element
My402ListElem *My402ListFirst(My402List* list)
{
	if(list->num_members==0)
		return NULL;
	return list->anchor.next;

}

//Return last element
My402ListElem *My402ListLast(My402List* list)
{	if(list->num_members==0)
		return NULL;
	return list->anchor.prev;
}

//finding next element
My402ListElem *My402ListNext(My402List* list, My402ListElem* element)
{
    //Provided element is first element so return NULl
	if(element->next == &(list->anchor))
		return NULL;
	return element->next;
}

//finding previous element
My402ListElem *My402ListPrev(My402List* list, My402ListElem* element)
{
	//Provided element is last element so return NULL
	if(element->prev == &(list->anchor))
		return NULL;
	return element->prev;
}

// finding an element
My402ListElem *My402ListFind(My402List* list, void* object)
{
	My402ListElem *temp=list->anchor.next;

	while(temp->next != &(list->anchor))
	{
		if((int*)temp->obj == (int*)object)
			return temp;
			temp=temp->next;
	}
	if((int*)temp->obj == (int*)object)
		return temp;
	return NULL;
}

//Initialize into an empty list
int My402ListInit(My402List* list)
{
	if(list==NULL)
		return FALSE;

	list->num_members=0;
	list->anchor.next=&(list->anchor);
	list->anchor.prev=&(list->anchor);
	return TRUE;
}






