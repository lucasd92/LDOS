#include "os_semaforos.h"

#ifdef USO_SEMAFOROS

	uint32_t numero_semaforos = 0;

	uint32_t dar_semaforo(semaforo_t *sem){
		uint32_t i;
		uint32_t llamar_scheduler = 0;

	  	for(i = 0; i < numero_tareas_activas; i++){							// recorro todas las tareas
			if((tareas[i].sem_asoc == sem->id) &&							// si alguna tiene este semáforo asociado
				(tareas[i].prioridad > tareas[tarea_actual].prioridad	)){	// y mayor prioridad
				llamar_scheduler = 1;										// llamo al scheduler
			}
		}

		if(sem->valor_actual < sem->valor_maximo){		// Incremento el semáforo si se encuentra en el rango
			sem->valor_actual++;

			if(llamar_scheduler)
				schedule();
			return 1;
		}

		if(llamar_scheduler)
			schedule();
		return 0;
	}

	uint32_t tomar_semaforo(semaforo_t *sem, uint32_t espera){
		if(sem->valor_actual > 0){
			sem->valor_actual--;
		}
		else{
			tareas[tarea_actual].estado = DETENIDA;
			tareas[tarea_actual].sem_asoc = sem->id;
			//sem->tarea_detenida = tarea_actual;
			temporizar(espera);
		}
		if(tareas[tarea_actual].espera > 0){
			tareas[tarea_actual].espera = 0;
			return 1;

		}
		else
			return 0;

	}


	semaforo_t *crear_semaforo(uint32_t max, uint32_t actual){
		semaforos[numero_semaforos].valor_actual = actual;
		semaforos[numero_semaforos].valor_maximo = max;
		semaforos[numero_semaforos].id = numero_semaforos;
		numero_semaforos++;

		return &semaforos[numero_semaforos-1];
	}

	void semaforo_activar_tarea(){
		uint32_t i;
	  	for(i = 0; i < numero_tareas_activas; i++){
			if((semaforos[tareas[i].sem_asoc].valor_actual > 0) &&			//el valor actual del semaforo asociado es mayor  a 0
				(tareas[i].estado == DETENIDA))								// y la tarea está detenida
			{
				tareas[i].estado = ACTIVA;		// la activo
			}
		}

	/*
	  	for(i = 0; i < numero_semaforos; i++){
			if((semaforos[i].valor_actual > 0) &&							//se entregó el semaforo
				(tareas[semaforos[i].tarea_detenida].estado == DETENIDA))	// y la tarea está detenida
			{
				tareas[semaforos[i].tarea_detenida].estado = ACTIVA;		// la activo
			}
		}
	*/
	}

#endif
