#include "sem.h"

int call_type;
endpoint_t SELF_E;

static endpoint_t who_e;
static int callnr;

static void get_work(message *m_ptr);

static void sef_local_startup(void);

/*==================================================*
 *                     main                         *
 *==================================================*/

int main(int argc, char **argv){
  message m;
  int result;

  env_setargs(argc, argv);
  sef_local_startup();

  while(TRUE){
    get_work(&m);

    if(is_notify(callnr)){
      printf("SEM: warning, got illegal notify from: %d\n", m.m_source);
      result = EINVAL;
      goto send_reply;
    }

    switch(callnr){
    case SEM_INIT:
      result = do_sem_init(&m);
      break;
    case SEM_RELEASE:
      result = do_sem_release(&m);
      break;
    case SEM_UP:
      result = do_sem_up(&m);
      break;
    case SEM_DOWN:
      result = do_sem_down(&m);
      break;
    default:
      printf("SEM: warning, got illegal notify from: %d\n", m.m_source);
      result = EINVAL;
    }

  send_reply:
    if(result != EDONTREPLY){
      m.m_type = result;
      reply(who_e, &m);
    }
  }
  return(OK);
}

/*================================================*
 *                sef_local_startup               *
 *================================================*/

static void sef_local_startup(){
  sef_startup();
}

/*================================================*
 *                   get_work                     *
 *================================================*/

static void get_work(message *m_ptr){
  int status = sef_receive(ANY, m_ptr);
  if(OK!=status)
    panic("failed to receive message!: %d", status);
  who_e = m_ptr->m_source;
  callnr = m_ptr->m_type;
}
