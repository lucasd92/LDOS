#ifndef APP_H
#define APP_H

#include "os_config.h"
#include "os_semaforos.h"
#include "os_task.h"


//esto es de la aplicación
semaforo_t *mi_sem;



void * task1(void * args);

void * task2(void * args);

void * task3(void * args);

#endif
