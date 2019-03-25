#ifndef OS_CONFIG_H
#define OS_CONFIG_H

#include "stdint.h"
#include "string.h"
#include "board.h"
#include "sapi.h"

#define USO_SEMAFOROS
#define NUMERO_MAX_TAREAS		5

#ifdef USO_SEMAFOROS
	#define NUMERO_MAX_SEMAFOROS	2
#endif

#define STACK_SIZE_B 512


typedef struct{
	uint32_t valor_actual;
	uint32_t valor_maximo;
	uint32_t tarea_detenida;

}semaforo_t;

semaforo_t semaforos[NUMERO_MAX_SEMAFOROS];
uint32_t numero_semaforos = 0;

#endif

