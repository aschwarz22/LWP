#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include "lwp.h"

rfile saved;
int proc_IDs = 0;
thread *curr_head= NULL;
static struct scheduler schedule = {NULL, NULL, admit, remove, next};
scheduler sched = &schedule;


void *admit(thread new) {
    if (*curr_head == NULL) {
        *curr_head = new;
    } else {
        *tmp = *proc_list;
        while(tmp->lib_two != NULL){
            tmp = tmp->lib_two;
        }
        tmp->lib_two = new;
    }
}

void *remove(thread victim) {
    thread tmp;

    /* If there is no victim or the victim is the only thread in the scheduler */
    /* stop the program */
    if(victim == NULL || sched->next() == NULL)
    {
        lwp_stop();
    }
    else
    {
        /* if victim is the front of the scheduler, just point the scheduler
        /* to the next thread in the list */
        if (sched->next()->tid == victim->tid)
        {
            *curr_head = victim->lib_two;
        }

        /* otherwise find the thread in the list and remove it there */
        else
        {
            tmp = sched->next();
            while(tmp != NULL && tmp->lib_two != NULL)
            {
                /* if the next thread in the list is the victim, remove it */
                if (tmp->lib_two->tid == victim->tid)
                {

                    /* 'remove it' meaning change the current pointer to the victim's */
                    /*  next pointer */
                    tmp->lib_two = victim->lib_two;
                    victim->lib_two = NULL;
                }
            }
        }
    }
}

thread *next() {
    if(head != NULL){   
        return *curr_head;
    } 
    else
    {
        return NULL;
    }
}


tid_t lwp_create(lwpfun func, void * arg, size_t stack_size)
{

    /* base and stack pointers */
    unsigned long *stack_ptr
    unsigned long *base_ptr

    /* allocate space for the new thread */
    thread new = malloc(sizeof(struct threadinfo_st));
    new->tid = ++proc_IDs;
    new->stack = malloc(stack_size*sizeof(long));
    new->stacksize = (int)stack_size;
    new->state.fxsave = FPU_INIT;

    /* make sure you pass an address as the argument */
    if (arg != NULL)
    {
        new->state->rdi = arg;
    }

    /* stack is upside-down, starting at the 'top' */
    base_ptr = new->stack + stacksize*sizeof(long);

    /* push return address */
    *base_ptr = lwp_exit;
    base_ptr--;

    /* push function */
    *base_ptr = func;
    base_ptr--;

    /* set stack fram to look like post-call, pre-function body */
    stack_ptr = base_ptr;

    /* save stack registers */
    new->state.rbp = base_ptr;
    new->state.rsp = stack_ptr;
    new->lib_one = NULL;
    new->lib_two = NULL;

    /* add to back of scheduler */
    sched->admit(new);

    return new->tid;


    

}

void  lwp_exit(void);
{
    /* if there is no current or next thread, stop the program */
    if (curr_head == NULL || sched->next() == NULL || sched->next()->lib_two == NULL)
    {
        lwp_stop();
    }

    /* otherwise, save the current thread context, remove it from the scheduler */
    /* and load the next context */
    else
    {
        save_context(sched->next());
        sched->remove(sched->next());
        load_context(sched->next());
    }
}

tid_t lwp_gettid(void)
{
    return (*proc_list)->tid;
}

void  lwp_yield(void)
{
    thread curr = sched->next();

}

void  lwp_start(void)
{
    thread curr = sched->next();
    save_context(&saved);
    load_context(&(curr->state));
}

void  lwp_stop(void)
{
    load_context(&saved);
}


