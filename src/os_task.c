#include "os_task.h"


extern void tick_handler(void);

// se inicializa en este número para evitar guardar el contexto del main
uint32_t tarea_actual = 0xFFFFFFF0;

uint32_t numero_tareas_activas = 0;

uint32_t stack0[STACK_SIZE_B/4];

// definiciones y funciones del OS
#define DIR_RETORNO			0xFFFFFFF9
#define	NO_SEM_ASOC			0xFFFFFFFF



#ifdef USO_SEMAFOROS
	extern uint32_t numero_semaforos;
#endif

//Alguna tarea retornó valor y no debería (por ahora)
void task_return_hook(void * ret_val){

	while(1){
		__WFI();
	}
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

	semaforo_activar_tarea();

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
	uint32_t i;	//recorro numero de tareas
	uint32_t j; //recorro prioridad
	uint32_t cambio_pendiente = 0;

	// guardo contexto actual solo si no entro desde el main
	if(tarea_actual < NUMERO_MAX_TAREAS +1)
		tareas[tarea_actual].sp = current_sp;
	else
		tarea_actual = 0;	//salto de main a idle

	i = (tarea_actual);
	//Recorro las prioridades
	for(j = NUMERO_MAX_PRIORIDAD; j >= IDLE_TASK_PRIORITY; j--){
		
		//Recorro vector de tareas buscando una que no esté en espera
		do{

			//llegué al final y vuelvo a empezar
			if((i+1) >= numero_tareas_activas)
				i = 0;
			else
				i++;

			// Si encuentro una tarea activa con la prioridad j
			if((tareas[i].estado == ACTIVA) && (tareas[i].prioridad == j)){
				next_sp = tareas[i].sp;
				tarea_actual = i;
				// al cambiar de contexto
				cambio_pendiente = 1;
				break; // salgo del for tareas

			}

		}while(i != tarea_actual);

		if(cambio_pendiente == 1){
			break; // salgo del for prioridad
		}
	}

	return next_sp;
}

void init_stack(uint32_t stack[],		//vector de stack
				uint32_t stack_size_bytes,	//tamaño del stack
//				uint32_t *sp,			// puntero a stack pointer
				task_type entry_point,  //nombre de la función
				uint32_t prio,			// prioridad de la tarea
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

	//Inicializo la estructura de la tarea (debería ser otra función)
	tareas[numero_tareas_activas].id = numero_tareas_activas;
	tareas[numero_tareas_activas].estado = ACTIVA;
	tareas[numero_tareas_activas].espera = 0;
	tareas[numero_tareas_activas].prioridad = prio;
	tareas[numero_tareas_activas].sem_asoc = NO_SEM_ASOC;
	// si bien son 8 registros los que se pushean automáticamente, contemplo los 16.(r4 a r11) + LR
	tareas[numero_tareas_activas].sp = (uint32_t) &(stack[stack_size_bytes/4-17]);


	//Se inicializaron más tareas de las permitidas por NUMERO_MAX_TAREAS
	if(numero_tareas_activas > NUMERO_MAX_TAREAS){
		while(1);
	}

	//Indico que se suma una nueva tarea el scheduler
	numero_tareas_activas++;

}


void initOS(){
	NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
	init_stack( stack0, STACK_SIZE_B, idle,IDLE_TASK_PRIORITY, (void*) 0x11223344 );
}

__WEAK void * idle(void * args){
	while(1){
		__WFI();
	}
	return NULL;
}
