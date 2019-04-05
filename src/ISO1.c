

#include "stdint.h"
#include "string.h"
#include "board.h"
#include "sapi.h"

#include "app.h"






uint32_t stack1[STACK_SIZE_B/4];
uint32_t stack2[STACK_SIZE_B/4];
uint32_t stack3[STACK_SIZE_B/4];



void initHardware(void)
{
	Board_Init();
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000);
}





int main(void){

	initOS();

	init_stack( stack1, STACK_SIZE_B, task1, IDLE_TASK_PRIORITY+1, (void*) 0x11223344 );
	init_stack( stack2, STACK_SIZE_B, task2, IDLE_TASK_PRIORITY+1, (void*) 0x11223344 );
	init_stack( stack3, STACK_SIZE_B, task3, IDLE_TASK_PRIORITY+1, (void*) 0x11223344 );

	initHardware();

	mi_sem = crear_semaforo(1,0);

	tickCallbackSet( (callBackFuncPtr_t) temporizacion,NULL);
	while(1){
		__WFI();
	}
}
