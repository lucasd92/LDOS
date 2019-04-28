#ifndef APP_H
#define APP_H

#include "os_config.h"
#include "os_semaforos.h"
#include "os_task.h"


#define MEF_DELAY	30
#define portMAX_DELAY	0xFFFFFFFF
//enum {ASCENDENTE,DESCENDENTE};
enum {TEC_B1 = 1,TEC_B2 = 2};
enum {B1_asc,B1_desc,B2_asc,B2_desc};
//esto es de la aplicación

extern semaforo_t *sem_LED1;
extern semaforo_t *sem_LED2;
extern semaforo_t *sem_LED3;
extern semaforo_t *sem_LED4;
extern semaforo_t *sem_B1;
extern semaforo_t *sem_B2;
extern semaforo_t *sem_calc;
extern semaforo_t *sem_imprimir;

// estructuras que usan las colas
typedef struct{
   uint8_t tec;
   tick_t tiempo;
   int8_t   flanco;

}estructura_tecla_t;

typedef struct{
	uint32_t led;
	tick_t delta_asc;
	tick_t delta_des;

}estructura_uart_t;

// semáforo + buffer asociado hace de cola

extern RINGBUFF_T ring_B1;
extern estructura_tecla_t est_tec_B2[10];
extern RINGBUFF_T ring_B2;
extern estructura_tecla_t est_tec_B2[10];
extern RINGBUFF_T ring_calc;
extern estructura_tecla_t est_calc[10];
extern RINGBUFF_T ring_uart;
extern estructura_uart_t est_uart[10];





// lista de tareas
// Controla encendido LED
void * task1(void * args);
// Controla encendido LED
void * task2(void * args);
// Controla encendido LED
void * task3(void * args);
// Controla encendido LED
void * task4(void * args);

// Controla debbounce Tecla1
void * TaskMef_B1(void * args);
// Controla debbounce Tecla2
void * TaskMef_B2(void * args);

// Calcula tiempos y envía a la uart
void * TaskCalc(void * args);

// gestiona la uart
void * TaskUart(void * args);
// Pollea estado de las teclas
void * TaskPoll(void * args);

#endif
