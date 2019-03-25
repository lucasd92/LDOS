

uint32_t tiempo_global;
enum {LIBERADA,DESCENDIENDO,PRESIONADA,ASCENDIENDO};

//esto es de la aplicación
semaforo_t *mi_sem;

void * idle(void * args){
	while(1){
		__WFI();
	}
	return NULL;
}


void * task1(void * args){
	while(1){
		temporizar(200);
		gpioToggle(LED1);
	}
	return NULL;
}

void * task2(void * args){
	   // ---------- CONFIGURACIONES ------------------------------

	   // ---------- REPETIR POR SIEMPRE --------------------------
	   while(TRUE) {

		   tomar_semaforo( mi_sem, 20000 );

		   gpioWrite(LEDB,TRUE);
		   temporizar(tiempo_global*10 );



			gpioWrite(LEDB,FALSE);

	   }
}

void * task3(void * args){

	   // ---------- CONFIGURACIONES ------------------------------
		uint32_t Led_time = 0;
		uint32_t est_bot = 0;
		bool_t Estado;
	   // ---------- REPETIR POR SIEMPRE --------------------------
	   while(TRUE) {


		   Estado = !gpioRead(TEC1);
		   switch(est_bot)
		   {
		   	   case LIBERADA: // Liberado
		   		   Led_time = 0;
		   		   if (Estado)est_bot = DESCENDIENDO; //descendiendo
		   		   break;
		   	   case DESCENDIENDO: //Descendiendo
		   		   if(Estado) est_bot = PRESIONADA; else est_bot = LIBERADA;
		   		   break;
		   	   case PRESIONADA:	//Presionado
		   		   Led_time++;
		   		   if (!Estado)est_bot = ASCENDIENDO; //subiendo
		   		   break;
		   	   case ASCENDIENDO:	//Subiendo
		   		   if(!Estado) {est_bot = LIBERADA;tiempo_global = Led_time;dar_semaforo(mi_sem);} else {est_bot = PRESIONADA;}
		   		   break;

		   }
		   temporizar(10 );
	   }
}
