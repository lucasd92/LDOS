#ifndef OS_TASK_H
#define OS_TASK_H

#include "os_config.h"
#include "os_semaforos.h"

extern void tick_handler(void);

//Variables compartidas con otros archivos
extern uint32_t tarea_actual;
extern uint32_t numero_tareas_activas;

typedef void* (*task_type)(void*);


// Esructura para control de las tareas
typedef enum{ACTIVA,EN_ESPERA,DETENIDA}estado_t;

typedef struct{
	uint32_t	id;
	uint32_t	sp;
	estado_t	estado;
	uint32_t	espera;
	uint32_t	prioridad;
	uint32_t	sem_asoc;

}task_t;

// Vector de tareas
task_t tareas[NUMERO_MAX_TAREAS+1];

//Alguna tarea retornó valor y no debería (por ahora)
void task_return_hook(void * ret_val);


//Genero PendSV
void schedule(void);

//Coloco la tarea en modo espera X milisegundos
void temporizar(uint32_t milisegundos);

//Hace la cuenta regresiva para temporización en espera y detenida (semaforo)
void temporizacion(void);

// Calcula próxima tarea segun estado y prioridad
uint32_t get_next_context(uint32_t current_sp);

// Inicializo una nueva tarea
void init_stack(uint32_t stack[],		//vector de stack
				uint32_t stack_size_bytes,	//tamaño del stack
				task_type entry_point,  //nombre de la función
				uint32_t prio,			// prioridad de la tarea
				void * arg);				// argumentos

// Función para inicializar el SO
void initOS();

// Tarea idle declarada como weak
__WEAK void * idle(void * args);

#endif
