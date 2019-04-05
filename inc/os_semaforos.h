#ifndef OS_SEMAFOROS_H
#define OS_SEMAFOROS_H

	//#include "os_config.h"
	#include "os_task.h"

	#ifdef USO_SEMAFOROS
		//Variables compartidas con otros archivos
		extern uint32_t numero_semaforos;

		//Estructura para manejo de los semáforos
		typedef struct{
			uint32_t valor_actual;
			uint32_t valor_maximo;
			uint32_t tarea_detenida;

		}semaforo_t;
		//Vector de semáforos
		semaforo_t semaforos[NUMERO_MAX_SEMAFOROS];


		//si suma 1 al semáforo, devuelve 1
		// caso contrario porque alcanzó el máximo, devuelve 0
		uint32_t dar_semaforo(semaforo_t *sem);

		//Bloquea la tarea en temporizar(espera); 
		//hasta que pase el tiempo(ret 0) o se libere el semáforo(ret 1)
		uint32_t tomar_semaforo(semaforo_t *sem, uint32_t espera);

		// creo semáforo sin validar datos
		semaforo_t *crear_semaforo(uint32_t max, uint32_t actual);

	#endif



#endif
