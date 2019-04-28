

#include "stdint.h"
#include "string.h"
#include "board.h"


#include "sapi.h"
#include "ring_buffer.h"

#include "app.h"

#ifndef UART_DEBUG
   #define UART_DEBUG UART_USB
#endif




uint32_t stack1[STACK_SIZE_B/4];
uint32_t stack2[STACK_SIZE_B/4];
uint32_t stack3[STACK_SIZE_B/4];
uint32_t stack4[STACK_SIZE_B/4];
uint32_t stack5[STACK_SIZE_B/4];
uint32_t stack6[STACK_SIZE_B/4];
uint32_t stack7[STACK_SIZE_B/4];
uint32_t stack8[STACK_SIZE_B/4];
uint32_t stack9[STACK_SIZE_B/4];


//esto es de la aplicación

semaforo_t *sem_LED1;
semaforo_t *sem_LED2;
semaforo_t *sem_LED3;
semaforo_t *sem_LED4;
semaforo_t *sem_B1;
semaforo_t *sem_B2;
semaforo_t *sem_calc;
semaforo_t *sem_imprimir;

// semáforo + buffer asociado hace de cola

RINGBUFF_T ring_B1;
estructura_tecla_t est_tec_B1[10];
RINGBUFF_T ring_B2;
estructura_tecla_t est_tec_B2[10];
RINGBUFF_T ring_calc;
estructura_tecla_t est_calc[10];
RINGBUFF_T ring_uart;
estructura_uart_t est_uart[10];

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
	init_stack( stack4, STACK_SIZE_B, task4, IDLE_TASK_PRIORITY+1, (void*) 0x11223344 );
	init_stack( stack5, STACK_SIZE_B, TaskMef_B1, IDLE_TASK_PRIORITY+1, (void*) 0x11223344 );
	init_stack( stack6, STACK_SIZE_B, TaskMef_B2, IDLE_TASK_PRIORITY+1, (void*) 0x11223344 );
	init_stack( stack7, STACK_SIZE_B, TaskCalc, IDLE_TASK_PRIORITY+1, (void*) 0x11223344 );
	init_stack( stack8, STACK_SIZE_B, TaskUart, IDLE_TASK_PRIORITY+1, (void*) 0x11223344 );
	init_stack( stack9, STACK_SIZE_B, TaskPoll, IDLE_TASK_PRIORITY+3, (void*) 0x11223344 );

	initHardware();
	uartConfig( UART_USB, 115200 );

	sem_LED1 = crear_semaforo(1,0);
	sem_LED2 = crear_semaforo(1,0);
	sem_LED3 = crear_semaforo(1,0);
	sem_LED4 = crear_semaforo(1,0);
	sem_B1 = crear_semaforo(10,0);
	sem_B2 = crear_semaforo(10,0);
	sem_calc = crear_semaforo(10,0);
	sem_imprimir = crear_semaforo(10,0);

	RingBuffer_Init(&ring_B1, est_tec_B1, sizeof(estructura_tecla_t), 10);
	RingBuffer_Init(&ring_B2, est_tec_B2, sizeof(estructura_tecla_t), 10);
	RingBuffer_Init(&ring_calc, est_calc, sizeof(estructura_tecla_t), 10);
	RingBuffer_Init(&ring_uart, est_uart, sizeof(estructura_uart_t), 10);

	tickCallbackSet( (callBackFuncPtr_t) temporizacion,NULL);
	while(1){
		__WFI();
	}
}

/*==================[fin del archivo]========================================*/
