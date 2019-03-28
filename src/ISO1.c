

#include "os_task.h"
#include "os_semaforos.h"
#include "app.h"





uint32_t stack0[STACK_SIZE_B/4];
uint32_t stack1[STACK_SIZE_B/4];
uint32_t stack2[STACK_SIZE_B/4];
uint32_t stack3[STACK_SIZE_B/4];



static void initHardware(void)
{
	Board_Init();
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000);
}




int main(void){

	initOS();

	init_stack( stack1, STACK_SIZE_B, task1, (void*) 0x11223344 );
	init_stack( stack2, STACK_SIZE_B, task2, (void*) 0x11223344 );
	init_stack( stack3, STACK_SIZE_B, task3, (void*) 0x11223344 );

	initHardware();

	mi_sem = crear_semaforo(1,0);

	tickCallbackSet( (callBackFuncPtr_t) temporizacion,NULL);
	while(1){
		__WFI();
	}
}
