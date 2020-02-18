#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "schedulers.h"
#include "task.h"
#include "cpu.h"

struct node *create_task(char *name, int priority, int burst)
{
	struct node *n = malloc(sizeof(struct node));
	struct task *t = malloc(sizeof(struct task));

	t->name = name;
	t->priority = priority;
	t->burst = burst;

	n->task = t;
	return n;
}

void add(char *name, int priority, int burst, struct node **queue)
{
	struct node *new = create_task(name, priority, burst);
	struct node *start = *queue;
	if (start->init == 1)
	{
		*queue = new;
	}
	else
	{
		if (priority < start->task->priority){
			new->next = start;
			*queue = new;
		}
		else {
			while (start->next != NULL)
			{
				if (priority <= start->next->task->priority)
				{
					break;
				}
				start = start->next;
			}	
			new->next = start->next;
			start->next = new;
		}
	}

}

void schedule(struct node **queue)
{
	struct node *start = *queue;
	struct node *tmp;
	int num_tasks = 1;
	int total_time = 0;
	while(start->next != NULL)
	{
		num_tasks++;
		total_time += start->task->burst;
		run(start->task, start->task->burst);
		tmp = start;
		start = start->next;
		free(tmp);
	}
	if (start->task != NULL){
		run(start->task, start->task->burst);
	}
	free(start);
	printf("The average waiting time is %.3f\n", (float)total_time/num_tasks);

}

