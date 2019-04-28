#include "app.h"
#include "sapi_tick.h"



enum {LIBERADA,PRESIONADA,ASCENDENTE,DESCENDENTE};

enum{VERDE,ROJO,AMARILLO,AZUL};

uint32_t tiempo_LED1;
uint32_t tiempo_LED2;
uint32_t tiempo_LED3;
uint32_t tiempo_LED4;

uint32_t evento(estructura_tecla_t tecla){

	while(TRUE){
		switch(tecla.flanco){
			case ASCENDENTE:
				if(tecla.tec == TEC_B1)
					return B1_asc;
				else
					return B2_asc;
				break;
			case DESCENDENTE:
				if(tecla.tec == TEC_B1)
					return B1_desc;
				else
					return B2_desc;
				break;
		}
	}
}


void * task1(void * args){
	while(1){
		tomar_semaforo(sem_LED1,portMAX_DELAY);
		gpioWrite(LED1,TRUE);
		temporizar(tiempo_LED1);
		gpioWrite(LED1,FALSE);

	}
	return NULL;
}

void * task2(void * args){
	while(1){
		tomar_semaforo(sem_LED2,portMAX_DELAY);
		gpioWrite(LED2,TRUE);
		temporizar(tiempo_LED2);
		gpioWrite(LED2,FALSE);
	}
	return NULL;
}

void * task3(void * args){
	while(1){
		tomar_semaforo(sem_LED3,portMAX_DELAY);
		gpioWrite(LED3,TRUE);
		temporizar(tiempo_LED3);
		gpioWrite(LED3,FALSE);
	}
	return NULL;
}

void * task4(void * args){
	while(1){
		tomar_semaforo(sem_LED4,portMAX_DELAY);
		gpioWrite(LEDB,TRUE);
		temporizar(tiempo_LED4);
		gpioWrite(LEDB,FALSE);
	}
	return NULL;
}

void * TaskCalc(void * args){
	// tiempos de flancos
	tick_t tiempo_b1_asc, tiempo_b1_desc;
	tick_t tiempo_b2_asc, tiempo_b2_desc;

	// estructura para recibir datos
	estructura_tecla_t tecla;
	// estructura para enviar datos a la uart
	estructura_uart_t	uart;

	while(TRUE){
		tomar_semaforo(sem_calc,portMAX_DELAY);	//Espero un evento
		RingBuffer_Pop( &ring_calc, &tecla );

		switch(evento(tecla)){
			case B1_desc:	// baja B1 --> primeros 2 casos
				tiempo_b1_desc = tecla.tiempo;

				tomar_semaforo(sem_calc,portMAX_DELAY); //Espero otro evento
				RingBuffer_Pop( &ring_calc, &tecla );

				if(evento(tecla) == B2_desc){	// ambos están abajo
					tiempo_b2_desc = tecla.tiempo;

					tomar_semaforo(sem_calc,portMAX_DELAY); //Espero otro evento
					RingBuffer_Pop( &ring_calc, &tecla );

					switch(evento(tecla)){
						case B1_asc:	// sólo queda B2 abajo
							tiempo_b1_asc = tecla.tiempo;

							tomar_semaforo(sem_calc,portMAX_DELAY); //Espero otro evento
							RingBuffer_Pop( &ring_calc, &tecla );

							if(evento(tecla) == B2_asc){	// Verde
								tiempo_b2_asc = tecla.tiempo;
								//Calculo los tiempos y envía a la uart
								uart.led = VERDE;
								uart.delta_des = tiempo_b2_desc - tiempo_b1_desc;
								uart.delta_asc = tiempo_b2_asc - tiempo_b1_asc;
								RingBuffer_Insert(&ring_uart, &uart );
								dar_semaforo(sem_imprimir);


							}
						break;
						case B2_asc:	// sólo queda B1 abajo
							tiempo_b2_asc = tecla.tiempo;

							tomar_semaforo(sem_calc,portMAX_DELAY); //Espero otro evento
							RingBuffer_Pop( &ring_calc, &tecla );

							if(evento(tecla) == B1_asc){	// Rojo
								tiempo_b1_asc = tecla.tiempo;

								//Calculo los tiempos y envía a la uart
								uart.led = ROJO;
								uart.delta_des = tiempo_b2_desc - tiempo_b1_desc;
								uart.delta_asc = tiempo_b1_asc - tiempo_b2_asc;
								RingBuffer_Insert(&ring_uart, &uart );
								dar_semaforo(sem_imprimir);

							}
						break;
						default:	// cualquier otro caso, salgo
						break;
					}
				}
				break;

			case B2_desc:	// baja B2 --> últimos 2 casos
				tiempo_b2_desc = tecla.tiempo;

				tomar_semaforo(sem_calc,portMAX_DELAY); //Espero otro evento
				RingBuffer_Pop( &ring_calc, &tecla );

				if(evento(tecla) == B1_desc){	// ambos están abajo
					tiempo_b1_desc = tecla.tiempo;

					tomar_semaforo(sem_calc,portMAX_DELAY); //Espero otro evento
					RingBuffer_Pop( &ring_calc, &tecla );

					switch(evento(tecla)){
						case B1_asc:	// sólo queda B2 abajo
							tiempo_b1_asc = tecla.tiempo;

							tomar_semaforo(sem_calc,portMAX_DELAY); //Espero otro evento
							RingBuffer_Pop( &ring_calc, &tecla );

							if(evento(tecla) == B2_asc){	// Amarillo
								tiempo_b2_asc = tecla.tiempo;

								//Calculo los tiempos y envía a la uart
								uart.led = AMARILLO;
								uart.delta_des = tiempo_b1_desc - tiempo_b2_desc;
								uart.delta_asc = tiempo_b2_asc - tiempo_b1_asc;
								RingBuffer_Insert(&ring_uart, &uart );
								dar_semaforo(sem_imprimir);

							}
						break;
						case B2_asc:	// sólo queda B2 abajo
							tiempo_b2_asc = tecla.tiempo;

							tomar_semaforo(sem_calc,portMAX_DELAY); //Espero otro evento
							RingBuffer_Pop( &ring_calc, &tecla );

							if(evento(tecla) == B1_asc){	// Azul
								tiempo_b1_asc = tecla.tiempo;

								//Calculo los tiempos y envía a la uart
								uart.led = AZUL;
								uart.delta_des = tiempo_b1_desc - tiempo_b2_desc;
								uart.delta_asc = tiempo_b1_asc - tiempo_b2_asc;
								RingBuffer_Insert(&ring_uart, &uart );
								dar_semaforo(sem_imprimir);


							}
						break;
						default:	// cualquier otro caso, salgo
						break;
					}
				}
				break;

			default:	// cualquier otro caso, salgo
				break;


		}
	}
	return NULL;
}

// Maquina de estados Boton 1
void * TaskMef_B1( void* taskParmPtr )
{
	// ---------- CONFIGURACIONES ------------------------------

	estructura_tecla_t   est_b1;
	// ---------- REPETIR POR SIEMPRE --------------------------
	while(TRUE) {


		if(tomar_semaforo( sem_B1, portMAX_DELAY) == TRUE ){	// quedo a la espera de un evento
	    	 RingBuffer_Pop( &ring_B1, &est_b1 );				// si se da, leo la cola

	    	 if(tomar_semaforo( sem_B1, MEF_DELAY ) == FALSE ){ // sin eventos durante tiempo de debbounce significa que no hubo rebote
	    		 RingBuffer_Insert(&ring_calc, &est_b1 );			// reenvío el valor para ser calculado
	    		 dar_semaforo(sem_calc);
	         }else
	        	 RingBuffer_Pop( &ring_B1, &est_b1 );		// caso contrario limpio el valor de la cola
		}
	}
	return NULL;
}
// Maquina de estados Boton 2 IDEM 1
void * TaskMef_B2( void* taskParmPtr )
{
   // ---------- CONFIGURACIONES ------------------------------

   estructura_tecla_t   est_b2;
   // ---------- REPETIR POR SIEMPRE --------------------------
   while(TRUE) {


      if(tomar_semaforo( sem_B2, portMAX_DELAY) == TRUE ){	// quedo a la espera de un evento
    	 RingBuffer_Pop( &ring_B2, &est_b2 );				// si se da, leo la cola

         if(tomar_semaforo( sem_B2, MEF_DELAY ) == FALSE ){ // sin eventos durante tiempo de debbounce significa que no hubo rebote
       	  RingBuffer_Insert(&ring_calc, &est_b2 );			// reenvío el valor para ser calculado
       	  dar_semaforo(sem_calc);
         }else
       	  RingBuffer_Pop( &ring_B2, &est_b2 );		// caso contrario limpio el valor de la cola
      }
   }
   return NULL;
}


void * TaskUart(void * args){
	estructura_uart_t dato;
	uint32_t tiempo_encendido;
	while(1){
		tomar_semaforo( sem_imprimir, portMAX_DELAY);
		RingBuffer_Pop( &ring_uart, &dato );

		tiempo_encendido = (uint32_t)dato.delta_des + (uint32_t)dato.delta_asc;

		switch(dato.led){
			case VERDE:
				printf(" \n \r Led verde encendido: \n \r");
				tiempo_LED3 = tiempo_encendido;
				dar_semaforo(sem_LED3);
				break;
			case ROJO:
				printf(" \n \r Led rojo encendido: \n \r");
				tiempo_LED2 = tiempo_encendido;
				dar_semaforo(sem_LED2);
				break;
			case AMARILLO:
				printf(" \n \r Led amarillo encendido: \n \r");
				tiempo_LED1 = tiempo_encendido;
				dar_semaforo(sem_LED1);
				break;
			case AZUL:
				printf(" \n \r Led azul encendido: \n \r");
				tiempo_LED4 = tiempo_encendido;
				dar_semaforo(sem_LED4);
				break;
		}
		printf("\t Tiempo encendido: %u ms \n\r",tiempo_encendido);
		printf("\t Tiempo entre flancos descendentes: %u ms \n\r",(uint32_t)dato.delta_des);
        printf( "\t Tiempo entre flancos ascendentes: %u ms \n\r",(uint32_t)dato.delta_asc);

	}
	return NULL;
}
// Polleo estado de las teclas. Si cambian , envío a MEF el flanco.
void * TaskPoll(void * args){

	   // ---------- CONFIGURACIONES ------------------------------
	   estructura_tecla_t   est_b1;
	   estructura_tecla_t   est_b2;
		uint32_t Estado_TEC1;
		uint32_t Estado_TEC2;
		uint32_t Estado_TEC1_ant = LIBERADA;
		uint32_t Estado_TEC2_ant = LIBERADA;
	   // ---------- REPETIR POR SIEMPRE --------------------------
	   while(TRUE) {


		   Estado_TEC1 = !gpioRead(TEC1); //Leo estado de tecla
		   if(Estado_TEC1 != Estado_TEC1_ant){	// Si es distinto de anterior

			   Estado_TEC1_ant = Estado_TEC1;

			   if(Estado_TEC1 == PRESIONADA){	// y está presionada
				   est_b1.tec = 1;
				   est_b1.tiempo = tickRead();	// envío tiempo
				   est_b1.flanco = DESCENDENTE;	// y tipo de flanco

				   RingBuffer_Insert(&ring_B1, &est_b1);
				   dar_semaforo(sem_B1);	//por el buffer + semaforo = cola

			   }
			   else{
				   est_b1.tec = 1;
				   est_b1.tiempo = tickRead();
				   est_b1.flanco = ASCENDENTE;

				   RingBuffer_Insert(&ring_B1, &est_b1);
				   dar_semaforo(sem_B1);

			   }

		   }

		   Estado_TEC2 = !gpioRead(TEC2);
		   if(Estado_TEC2 != Estado_TEC2_ant){

			   Estado_TEC2_ant = Estado_TEC2;

			   if(Estado_TEC2 == PRESIONADA){
				   est_b2.tec = 2;
				   est_b2.tiempo = tickRead();
				   est_b2.flanco = DESCENDENTE;

				   RingBuffer_Insert(&ring_B2, &est_b2);
				   dar_semaforo(sem_B2);

			   }
			   else{
				   est_b2.tec = 2;
				   est_b2.tiempo = tickRead();
				   est_b2.flanco = ASCENDENTE;

				   RingBuffer_Insert(&ring_B2, &est_b2);
				   dar_semaforo(sem_B2);

			   }

		   }

		   temporizar(1);
	   }

}
