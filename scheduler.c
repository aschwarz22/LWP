#include <stdio.h>
#include <stdlib.h>

#include "lwp.h"

void *init() {
    
}

void *shutdown() {
    
}

void *admit(thread new) {
    if (head == NULL) {
        head = &new;
    } else {
        *tmp = *head;
        while(tmp->lib_two != NULL){
            tmp = tmp->lib_two;
        }
        tmp->lib_two = new;
    }
}

void *remove(thread victim) {
    head = victim->lib_two;
    free(victim);
    /* readmit */
}

thread *next() {
    if(head != NULL){   
        return *head;
    } else{
        return NULL;
    }
}

