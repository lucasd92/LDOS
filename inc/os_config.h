#ifndef OS_CONFIG_H
#define OS_CONFIG_H

#include "stdint.h"
#include "string.h"
#include "board.h"
#include "sapi.h"

#define USO_SEMAFOROS
#define NUMERO_MAX_TAREAS		5
#define NUMERO_MAX_PRIORIDAD	5	//Mayor prioridad implica más tiempo en el scheduler
#define	IDLE_TASK_PRIORITY		0

#ifdef USO_SEMAFOROS
	#define NUMERO_MAX_SEMAFOROS	2
#endif

#define STACK_SIZE_B 512





#endif

