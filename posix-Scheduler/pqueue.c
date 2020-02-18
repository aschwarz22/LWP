#include "pqueue.h"
#include <stdio.h>
#include <stdlib.h>



Node *create_node(char* word, int occurance){
    Node *new = (Node*)malloc(sizeof(struct Node));
	new->word = word;
	new->occurance = occurance;
	new->next = NULL;
	return new;
}


int peek_occurance(Node** head) 
{ 
    return (*head)->occurance; 
} 

char* peek_word(Node** head){
	return (*head)->word;
}

void push(Node** head, char* word, int occurance){
	Node *new = create_node(word, occurance);
	Node *start = *head;
	if (new->occurance < start->occurance){
		new->next = start;
		*head = new;
	}
	else{
		while (start->next != NULL && new->occurance >= start->next->occurance){
			start = start->next;
		}
		new->next = start->next;
		start->next = new;

	}

}