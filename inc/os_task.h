#ifndef OS_TASK_H
#define OS_TASK_H

#include "os_config.h"

extern void tick_handler(void);

// definiciones y funciones del OS
#define DIR_RETORNO			0xFFFFFFF9


typedef enum{ACTIVA,EN_ESPERA,DETENIDA}estado_t;

volatile uint32_t tarea_actual = 0xFFFFFFFF;
volatile uint32_t numero_tareas_activas = 0;

typedef void* (*task_type)(void*);

typedef struct{
	uint32_t	id;
	uint32_t	sp;
	estado_t	estado;
	uint32_t	espera;

}task_t;

task_t tareas[NUMERO_MAX_TAREAS+1];


//Alguna tarea retornó valor y no debería (por ahora)
void task_return_hook(void * ret_val){

	while(1){
		__WFI();
	}
}

void initOS(){
	NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
	init_stack( stack0, STACK_SIZE_B, idle, (void*) 0x11223344 );
}

void schedule(){

	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;

	__ISB();

	__DSB();

}

void temporizar(uint32_t milisegundos){
	if(milisegundos != 0){
		// si no está detenida (semaforo)
		if(tareas[tarea_actual].estado != DETENIDA){
			tareas[tarea_actual].estado = EN_ESPERA;
		}
		tareas[tarea_actual].espera = milisegundos;
	}

	if(tareas[tarea_actual].estado != ACTIVA){
		schedule();
	}

	return;
}

void temporizacion(){
	uint32_t i;
#ifdef USO_SEMAFOROS
	for(i = 0; i < numero_semaforos; i++){
		if((semaforos[i].valor_actual > 0) &&							//se entregó el semaforo
			(tareas[semaforos[i].tarea_detenida].estado == DETENIDA))	// y la tarea está detenida
		{
			tareas[semaforos[i].tarea_detenida].estado = ACTIVA;		// la activo
		}
	}
#endif
	//resto a todos, menos idle
	for(i = 1; i < numero_tareas_activas; i++){
		//Si no está en espera
		if(tareas[i].espera == 0){
			tareas[i].estado = ACTIVA;
		}
		//Si está en espera
		if(tareas[i].espera > 0){
			tareas[i].espera--;
		}
	}

	schedule();
}

uint32_t get_next_context(uint32_t current_sp){

	uint32_t next_sp;
	uint32_t i;
	uint32_t idle = 1;

	// guardo contexto actual si no entro desde el main
	if(tarea_actual < NUMERO_MAX_TAREAS +1)
		tareas[tarea_actual].sp = current_sp;
	else
		tarea_actual = 0;	//salto de main a idle
	//recorro vector de tareas buscando una que no esté en espera
	for(i = (tarea_actual+1); i != tarea_actual; i++){

		//llegué al final y vuelvo a empezar
		if(i >= numero_tareas_activas){
			i = 0;

		}

		if(tareas[i].estado == ACTIVA){
			next_sp = tareas[i].sp;
			tarea_actual = i;
			// al cambiar de contexto, evito entrar en modo idle
			idle = 0;
			break;
		}

	}
	// no hay tareas activas
	if(idle == 1){
		next_sp = tareas[0].sp;
		tarea_actual = 0;
	}

	return next_sp;
}
void init_stack(uint32_t stack[],		//vector de stack
				uint32_t stack_size_bytes,	//tamaño del stack
//				uint32_t *sp,			// puntero a stack pointer
				task_type entry_point,  //nombre de la función
				void * arg)				// argumentos
{
// inicializo el stack en 0
	bzero(stack,stack_size_bytes);

// al inicializar el stack, le ponemos en el xPSR (status register), un 1 en la posición 24 para indicar modo thumb.
	stack[stack_size_bytes/4-1] = 1 << 24; 				// xPSR
// el segundo paso es guardar la posición en que inicia la tarea
	stack[stack_size_bytes/4-2] = (uint32_t) entry_point;			// PC
// el valor de LR es la dirección de retorno de la tarea (aunque no debería volver)
	stack[stack_size_bytes/4-3] = (uint32_t) task_return_hook;		// LR
// el valor de en -8 está el r0 y por defecto pasa los argumentos
	stack[stack_size_bytes/4-8] = (uint32_t) arg;		//paso los argumentos por r0
// cargo el valor de lR  para retorno de la interrupción
	stack[stack_size_bytes/4-9] = DIR_RETORNO;		// LR

// si bien son 8 registros los que se pushean automáticamente, contemplo los 16.(r4 a r11) + LR
//	*sp = (uint32_t) &(stack[stack_size_bytes/4-17]);

	//Inicializo la estructura de la tarea
	tareas[numero_tareas_activas].id = numero_tareas_activas;
	tareas[numero_tareas_activas].estado = ACTIVA;
	tareas[numero_tareas_activas].espera = 0;
	// si bien son 8 registros los que se pushean automáticamente, contemplo los 16.(r4 a r11) + LR
	tareas[numero_tareas_activas].sp = (uint32_t) &(stack[stack_size_bytes/4-17]);


	//Se inicializaron más tareas de las permitidas por NUMERO_MAX_TAREAS
	if(numero_tareas_activas > NUMERO_MAX_TAREAS){
		while(1);
	}

	//Indico que se suma una nueva tarea el scheduler
	numero_tareas_activas++;

}




#endif
