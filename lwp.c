#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include "lwp.h"

rfile saved;
int proc_IDs = 0;
thread *curr_head= NULL;
thread in_exec = NULL;
static struct scheduler schedule = {NULL, NULL, admit, remove, next};
scheduler sched = &schedule;


void *admit(thread new) {
    thread tmp;
    if (*curr_head == NULL) {
        *curr_head = new;
    } 
    else 
    {
        tmp = *proc_list;
        while(tmp->lib_two != NULL)
        {
            tmp = tmp->lib_two;
        }
        tmp->lib_two = new;
    }
}

void *remove(thread victim) {
    thread tmp;

    /* If there is no victim or the victim is the only thread in the scheduler */
    /* stop the program */
    if(victim == NULL || curr_head == NULL || *curr_head == NULL)
    {
        lwp_stop();
    }
    else
    {
        /* if victim is the front of the scheduler, just point the scheduler
        /* to the next thread in the list */
        if ((*curr_head)->tid == victim->tid)
        {
            *curr_head = victim->lib_two;
        }

        /* otherwise find the thread in the list and remove it there */
        else
        {
            tmp = *curr_head;
            while(tmp != NULL && tmp->lib_two != NULL)
            {
                /* if the next thread in the list is the victim, remove it */
                if (tmp->lib_two->tid == victim->tid)
                {

                    /* 'remove it' meaning change the current pointer to the victim's */
                    /*  next pointer */
                    tmp->lib_two = victim->lib_two;
                    victim->lib_two = NULL;
                    break;
                }
                tmp = tmp->lib_two;
            }
        }
    }
    /* cleanup */
    free(victim->stack);
    free(victim);
}


/* changes the current thread in execution */
/* removes from head of list and saves in in_exec */
thread next() {
    thread tmp;
    /* exit the program if there are no more threads in the scheduler */
    if (curr_head == NULL || *curr_head == NULL)
    {
        lwp_stop();
        return NULL;
    }
    /* save the current head of the list to be executed */
    tmp = *curr_head;

    /* set the head of the current list to the next thread */
    *curr_head = (*curr_head)->lib_two;

    /* set the currently executing thread */
    in_exec = tmp;

    /* return the current thread */
    return tmp;
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
    thread next;
    /* Null checks for the current thread & next thread */
    if (in_exec == NULL || curr_head == NULL || *curr_head == NULL || (*curr_head)->lib_two == NULL)
    {
        lwp_stop();
    }

    /* otherwise, save the current thread context, remove it from the scheduler */
    /* and load the next context */
    else
    {
        /* save the context of the curretnly executing thread (unsure if necessary) */
        save_context(&in_exec);

        /* remove the thread from the scheduler */
        sched->remove(in_exec);

        /* load the context of the next thread in the scheduler */
        load_context(sched->next());
    }
}

tid_t lwp_gettid(void)
{
    return in_exec->tid;
}

void  lwp_yield(void)
{
    thread next;
    /* save context of current thread in execution */
    save_context(&in_exec);

    /* re-admit the old thread */
    sched->admit(in_exec);

    /* if there is no current or next thread, sop the program */
    if (in_exec == NULL || (next = sched->next()) == NULL)
    {
        lwp_stop()
    }
    else
    {
        /* otherwise load the context of the next thread to be executed */
        load_context(&next);
    }

}

/* selects the first thread in the list and runs it */
void  lwp_start(void)
{
    in_exec = sched->next();
    save_context(&saved);
    load_context(&(in_exec->state));
}

void  lwp_stop(void)
{
    thread tmp;
    /* save the old context */
    load_context(&saved);

    if (in_exec != NULL)
    {
        free(in_exec->stack);
        free(in_exec);
    }
    while((tmp = sched->next()) != NULL)
    {
        sched->remove(tmp);
    }

}


