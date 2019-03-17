#include "string.h"
#include "board.h"

#include "sapi.h"
#include "stdint.h"

#define STACK_SIZE_B 512
#define DIR_RETORNO	0xFFFFFFF9


extern void tick_handler(void);

uint32_t stack1[STACK_SIZE_B/4];
uint32_t stack2[STACK_SIZE_B/4];

uint32_t sp1;
uint32_t sp2;

uint32_t current_task = 0;

typedef void* (*task_type)(void*);

void * task1(void * args){
	while(1){
		__WFI();
	}
	return NULL;
}

void * task2(void * args){
	while(1){
		__WFI();
	}
	return NULL;
}

static void initHardware(void)
{
	Board_Init();
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000);
}

//Alguna tarea retornó valor y no debería (por ahora)
void task_return_hook(void * ret_val){

	while(1){
		__WFI();
	}
}

uint32_t get_next_context(uint32_t current_sp){

	uint32_t next_sp;

	switch(current_task){
	case 0:
		// este es el contexto de main
		next_sp = sp1;
		current_task = 1;
		break;
	case 1:
		sp1 = current_sp;
		next_sp = sp2;
		current_task = 2;
		break;
	case 2:
		sp2 = current_sp;
		next_sp = sp1;
		current_task = 1;
		break;
	default:
		while(1){
			__WFI();
		}

	}

	return next_sp;
}
void init_stack(uint32_t stack[],		//vector de stack
				uint32_t stack_size_bytes,	//tamaño del stack
				uint32_t *sp,			// puntero a stack pointer
				task_type entry_point,  //nombre de la función
				void * arg)				// argumentos
{
// inicializo el stack en 0
	bzero(stack,stack_size_bytes);

// al inicializar el stack, le ponemos en el xPSR (status register), un 1 en la posición 24 para indicar modo thumb.
	stack[stack_size_bytes/4-1] = 1 << 24; 				// xPSR
// el segundo paso es guardar la posición en que inicia la tarea
	stack[stack_size_bytes/4-2] = (uint32_t) entry_point;			// PC
// el valor de LR es la dirección de retorno de la función
	stack[stack_size_bytes/4-3] = (uint32_t) task_return_hook;		// LR
// el valor de en -8 está el r0 y por defecto pasa los argumentos
	stack[stack_size_bytes/4-8] = (uint32_t) arg;		//paso los argumentos por r0
// cargo el valor de lR  para retorno de la interrupción
	stack[stack_size_bytes/4-9] = DIR_RETORNO;		// LR

// si bien son 8 registros los que se pushean automáticamente, contemplo los 16.(r4 a r11) + LR
	*sp = (uint32_t) &(stack[stack_size_bytes/4-17]);


}

int main(void){

	init_stack( stack1, STACK_SIZE_B, &sp1, task1, (void*) 0x11223344 );
	init_stack( stack2, STACK_SIZE_B, &sp2, task2, (void*) 0x11223344 );

	initHardware();

	tickCallbackSet( (callBackFuncPtr_t) tick_handler,NULL);
	while(1){
		__WFI();
	}
}
