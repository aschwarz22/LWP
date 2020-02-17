#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include "lwp.h"

int proc_IDs = 0;
thread *proc_list= NULL;

tid_t lwp_create(lwpfun func, void * arg, size_t stack_size)
{

    /* base and stack pointers */
    unsigned long *stack_ptr
    unsigned long *base_ptr
    thread temp;

    /* allocate space for the new thread */
    thread new = malloc(sizeof(struct threadinfo_st));
    new->tid = ++proc_IDs;
    new->stack = malloc(stack_size*sizeof(long));
    new->stacksize = (int)stack_size;
    new->state.fxsave = FPU_INIT;

    /* make sure you pass an address as the argument */
    if (arg != NULL)
    {
        new->state->rdx = arg;
    }


    base_ptr = new->stack + stacksize*sizeof(long);
    *base_ptr = lwp_exit;
    base_ptr--;
    *base_ptr = func;
    base_ptr--;
    stack_ptr = base_ptr;
    new->state.rbp = base_ptr;
    new->state.rsp = stack_ptr;
    new->lib_one = NULL;
    new->lib_two = NULL;

    if (proc_list == NULL)
    {
        *proc_list = new;
    }
    else
    {
        temp = *proc_list;
        while(temp->lib_two)
        {
            temp = temp->lib_two;
        }
        temp->lib_two = new;
        new->lib_one = temp;
    }
    /* TODO: scheduler */


    return new->tid;


    

}

void  lwp_exit(void);
{
    if (!proc_list || !(*proc_list)->lib_two)
    {
        lwp_stop();
    }
    else
    {
        proc_list = &(*proc_list)->lib_two;
        load_context((*proc_list)->state);
    }
}

tid_t lwp_gettid(void)
{
    return (*proc_list)->tid;
}

void  lwp_yield(void)
{

}
void  lwp_start(void)
{
    /* gets head of scheduler list */
}
void  lwp_stop(void)
{

}


