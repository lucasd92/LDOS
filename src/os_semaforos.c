#include "os_semaforos.h"

#ifdef USO_SEMAFOROS

	uint32_t numero_semaforos = 0;

	uint32_t dar_semaforo(semaforo_t *sem){
		if(sem->valor_actual < sem->valor_maximo){
			sem->valor_actual++;
			return 1;
		}
		return 0;
	}

	uint32_t tomar_semaforo(semaforo_t *sem, uint32_t espera){
		if(sem->valor_actual > 0){
			sem->valor_actual--;
		}
		else{
			tareas[tarea_actual].estado = DETENIDA;
			sem->tarea_detenida = tarea_actual;
			temporizar(espera);
		}
		if(tareas[tarea_actual].espera > 0){
			return 1;
			tareas[tarea_actual].espera = 0;
		}
		else
			return 0;

	}


	semaforo_t *crear_semaforo(uint32_t max, uint32_t actual){
		semaforos[numero_semaforos].valor_actual = actual;
		semaforos[numero_semaforos].valor_maximo = max;

		numero_semaforos++;

		return &semaforos[numero_semaforos-1];
	}

#endif