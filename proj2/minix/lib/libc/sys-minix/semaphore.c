#include <sys/cdefs.h>
#include "namespace.h"
#include <lib.h>
#include <unistd.h>
#include <minix/com.h>

#include <sys/stat.h>

#ifdef __weak_alias
__weak_alias(sem_init, _sem_init)
#endif

/*==============================================*
 *                 sem_init                     *
 *==============================================*/

int sem_init(int start_value)
{
	message m;
	int r;
	
	m.SEM_VALUE = start_value;
	
	r = _syscall(SEM_PROC_NR, SEM_INIT, &m);

	return r; 
}

/*==============================================*
 *                   sem_up                     *
 *==============================================*/

int sem_up(int semaphore_number)
{
	message m;
	int r;
	
	m.SEM_NUMBER = semaphore_number;
	r = _syscall(SEM_PROC_NR, SEM_UP, &m);
	
	return r;
}

/*==============================================*
 *                 sem_down                     *
 *==============================================*/

int sem_down(int semaphore_number)
{
	message m;
	int r;
	
	m.SEM_NUMBER = semaphore_number;
	r = _syscall(SEM_PROC_NR, SEM_DOWN, &m);
	
	return r;
}

/*==============================================*
 *                 sem_release                  *
 *==============================================*/

int sem_release(int semaphore)
{
	message m;
	int r;

	m.SEM_NUMBER = semaphore;
	
	r = _syscall(SEM_PROC_NR, SEM_RELEASE, &m);

	return r;
}
