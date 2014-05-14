#include "sem.h"

static int num_semaphores;
static int list_size;

typedef struct Semaphore *pSem;
pSem *sem_list;

/*===========================================================================*
 *				Sem_init					     *
 *===========================================================================*/

int do_sem_init(message *m)
{
	pSem *temp_list;
	
	if (list_size == 0)
		list_size = 100;
	if(!sem_list)
		sem_list = malloc (sizeof(pSem) * list_size);
	temp_list = sem_list;
	if (num_semaphores  == list_size -1)
	{ 
		list_size = list_size*2;
		temp_list = realloc (sem_list,  list_size * sizeof(pSem));
	}
	if (temp_list == NULL)
	{
		list_size = list_size/2;
		return ENOMEM;
	}
	sem_list = temp_list;
	sem_list[num_semaphores] = malloc (sizeof (semaphore));
	sem_list[num_semaphores]->value = m->SEM_VALUE;
	sem_list[num_semaphores]->q = NULL; 
	sem_list[num_semaphores]->q_size = 0;
	
	return ++num_semaphores;
}

/*===========================================================================*
 *				Sem_release					     *
 *===========================================================================*/

int do_sem_release(message *m)
{
	if(!sem_list)
		return EINVAL;
	if (!sem_list[m->SEM_NUMBER])
		return EINVAL;
	if (sem_list[m->SEM_NUMBER]->q_size > 0)
		return EINUSE;
	free(sem_list[m->SEM_NUMBER]);
	sem_list[m->SEM_NUMBER] = NULL;
	return OK;
}

/*===========================================================================*
 *				Sem_up					     *
 *===========================================================================*/

int do_sem_up(message *m)
{
	message *m_ptr;
	
	if(!sem_list)
		return EINVAL;
	m_ptr = (message *) malloc (sizeof(message));
	if (sem_list[m->SEM_NUMBER] == NULL)
		return EINVAL;
	sem_list[m->SEM_NUMBER]->value++;
	if (sem_list[m->SEM_NUMBER]->q_size > 0)
	{
		m_ptr->m_type = OK;
		m_ptr->m_source = dequeue(m->SEM_NUMBER);
		sem_list[m->SEM_NUMBER]->value--;
		sem_list[m->SEM_NUMBER]->q_size--;
		reply (m_ptr->m_source, m_ptr);
	}
	return OK;
}

/*===========================================================================*
 *				Sem_Down					     *
 *===========================================================================*/

int do_sem_down(message *m)
{	
	if(!sem_list)
		return EINVAL;
	if(sem_list[m->SEM_NUMBER] == NULL)
		return EINVAL;
	if (sem_list[m->SEM_NUMBER]->value > 0)
	{
		sem_list[m->SEM_NUMBER]->value--;
		return OK;
	}
	enqueue (m->m_source, m->SEM_NUMBER);
	sem_list[m->SEM_NUMBER]->q_size++;
	
	return (EDONTREPLY);
}

/*===========================================================================*
 *				enqueue					     *
 *===========================================================================*/

void enqueue(endpoint_t source, int sem_num)
{
	queue *q; 
	if (sem_list[sem_num]->q == 0)
	{
		q = (queue *) malloc (sizeof(queue));
		q->next = NULL;
		q->source = source;
		sem_list[sem_num]->q = q;
		return;
	}
	q = sem_list[sem_num]->q;
	while(q->next)
		q = q->next;
	q->next = (queue *) malloc (sizeof(queue));
	q = q->next;
	q->next = NULL;
	q->source = source;
}

/*===========================================================================*
 *				dequeue					     *
 *===========================================================================*/

endpoint_t dequeue (int sem_num)
{
	endpoint_t source;
	queue *tmp;
	
	source = sem_list[sem_num]->q->source;
	tmp = sem_list[sem_num]->q;
	sem_list[sem_num]->q = sem_list[sem_num]->q->next;
	free (tmp);
	return source;
}

/*===========================================================================*
 *				reply					     *
 *===========================================================================*/
void reply(
  endpoint_t who_e,			/* destination */
  message *m_ptr			/* message buffer */
)
{
    int s = send(who_e, m_ptr);    /* send the message */
    if (OK != s)
        printf("SEM: unable to send reply to %d: %d\n", who_e, s);
}