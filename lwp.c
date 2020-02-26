
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include "lwp.h"

rfile saved;
int proc_IDs = 0;
static thread curr_head = NULL;
thread in_exec = NULL;
static struct scheduler schedule = {NULL, NULL, rr_admit, rr_remove, rr_next};
scheduler RoundRobin = &schedule;


void *rr_admit(thread new) {
    thread tmp;
    if (curr_head == NULL) {
        curr_head = new;
    } 
    else 
    {
        tmp = curr_head;
        while(tmp->lib_two != NULL)
        {
            tmp = tmp->lib_two;
        }
        tmp->lib_two = new;
    }
    return;
}

void *rr_remove(thread victim) {
    thread tmp;

    /* If there is no victim or the victim is the only thread in the scheduler */
    /* stop the program */
    if(victim == NULL || curr_head == NULL)
    {
        lwp_stop();
    }
    else
    {
        /* if victim is the front of the scheduler, just point the scheduler */
        /* to the next thread in the list */
        if (curr_head->tid == victim->tid)
        {
            curr_head = victim->lib_two;
        }

        /* otherwise find the thread in the list and remove it there */
        else
        {
            tmp = curr_head;
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

    free(victim->stack);
    free(victim);
}


/* changes the current thread in execution */
/* removes from head of list and saves in in_exec */
thread rr_next() {
    thread tmp;
    /* exit the program if there are no more threads in the scheduler */
    if (curr_head == NULL)
    {
        lwp_stop();
        return NULL;
    }
    /* save the current head of the list to be executed */
    tmp = curr_head;

    /* set the head of the current list to the next thread */
    curr_head = curr_head->lib_two;
    
    /* remove the current head from the linked list */
    tmp->lib_two = NULL;



    /* set the currently executing thread */
    in_exec = tmp;

    /* re-admit the current thread in execution */
    rr_admit(tmp);

    /* return the current thread */
    return tmp;
}


tid_t lwp_create(lwpfun func, void * arg, size_t stacksize)
{
    unsigned long *base_ptr = NULL;

    /* allocate space for the new thread */
    thread new = malloc(sizeof(struct threadinfo_st));
    new->tid = ++proc_IDs;

    /* allocate space for the stack */
    new->stack = (unsigned long*)malloc(stacksize*sizeof(unsigned long));
    new->stacksize = stacksize;


    new->state.rdi = (unsigned long)arg;

    if(!new->stack)
    {
        perror("no stack sorry");
        return (tid_t)0;
    }

    base_ptr = (unsigned long *)((unsigned long)new->stack + (unsigned long)new->stacksize*sizeof(unsigned long));
    base_ptr--;

    /* push return address */
    *base_ptr = (unsigned long)lwp_exit;
    base_ptr--;

    /* push function */
    *base_ptr = (unsigned long)func; 
    base_ptr--;


    /* set stack and base registers */
    new->state.rsp = (unsigned long)base_ptr;
    new->state.rbp = (unsigned long)base_ptr;

    /* from specs */
    new->state.fxsave = FPU_INIT;

    new->lib_two = NULL;
    // new->state.fxsave = FPU_INIT;
    /* add to back of scheduler */
    RoundRobin->admit(new);

    return new->tid;
}


void lwp_exit(void)
{

    /* Null checks for the current thread & next thread */
    if (in_exec == NULL || curr_head == NULL || curr_head->lib_two == NULL)
    {
        lwp_stop();
    }

    /* otherwise, save the current thread context, remove it from the scheduler */
    /* and load the next context */
    else
    {

        /* remove the thread from the scheduler */
        RoundRobin->remove(in_exec);

        /* load the context of the next thread in the scheduler */
        load_context(&(RoundRobin->next()->state));
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
    save_context(&(in_exec->state));


    in_exec->lib_two = NULL;



    /* if there is no current or next thread, stop the program */
    if (in_exec == NULL || curr_head == NULL)
    {
        lwp_stop();
    }
    else
    {
        next = RoundRobin->next();
        /* otherwise load the context of the next thread to be executed */
        load_context(&(next->state));
    }

}

/* selects the first thread in the list and runs it */
void  lwp_start(void)
{


    save_context(&saved);


    /* pick the first thread in the scheduler */
    thread start = RoundRobin->next();

    /* begin executing the first thread */
    load_context(&(start->state));
}

void  lwp_stop(void)
{
    thread tmp;

    /* load the old context */
    load_context(&saved);

    /* cleanup */
    if (in_exec != NULL)
    {
        free(in_exec->stack);
        free(in_exec);
    }
    while(curr_head != NULL)
    {
        tmp = curr_head->lib_two;
        rr_remove(curr_head);
        curr_head = tmp;
    }

}

thread tid2thread(tid_t tid)
{
    thread tmp;

    /* if the tid matches the currently executing thread, return it */
    if (tid == in_exec->tid)
    {
        return in_exec;
    }

    tmp = curr_head;

    /* iterate through the threads in the linked list */
    while(tmp != NULL)
    {
        if (tmp->tid == tid)
        {
            return tmp;
        }
        tmp = tmp->lib_two;
    }

    /* return NULL if the thread with the tid isn't found */
    return NULL;
}

\
void lwp_set_scheduler(scheduler fun) 
{
    thread tmp;

    tmp = curr_head;

    /* unlink each thread from the current linked list */
    /* and admit them into the new scheduler */
    while(tmp != NULL)
    {
        tmp->lib_two = NULL;
        fun->admit(tmp);
    }

    if (fun->init != NULL) 
    {
        fun->init();
    }


    RoundRobin = fun;
}
