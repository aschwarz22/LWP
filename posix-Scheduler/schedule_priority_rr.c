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
				if (priority < start->next->task->priority)
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

	int quantum = 10;
	struct node *start = *queue;
	int num_tasks = 1;
	int total_time = 0;
	int prev = 0;

	while(start != NULL)
	{
		/* if burst is less than quantum, run for burst then delete node from queeu */
		if (start->task->burst <= quantum)
		{

			run(start->task, start->task->burst);
			total_time += start->task->burst;
			num_tasks ++;
			prev = start->task->burst;


			/* finish if there are no more tasks */
			if (start->next == NULL)
			{
				free(start);
				break;
			}

			/* delete task if there are more tasks, move to next */
			delete(queue, start->task);
			start = start->next;
			continue;
		}

		total_time += quantum;


		/* run task for time quantum, subtract from burst */
		run(start->task, quantum);
		start->task->burst -=quantum;

		/* check for edge case- one task left */
		if (start->next == NULL)
		{
			continue;
		}

		/* re-add the task to the end of the list */
		add(start->task->name, start->task->priority, start->task->burst, queue);

		/* memory management, move to next task */
		delete(queue, start->task);
		start = start->next;

	}
	total_time -= prev;
	printf("The average waiting time is %.3f\n", (float)total_time/num_tasks);
}
