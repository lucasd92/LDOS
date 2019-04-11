

	/**
	 * Directiva al ensablador que permite indicar que se encarga de buscar
	 * la instruccion mas apropiada entre thumb y thumb2
	 */
	.syntax unified

	/**
	 * .text permite indicar una seccion de codigo.
	 */
	.text

	/**
	 * .global permite definir un simbolo exportable,
	 * es decir que podemos verlo desde otros modulos (equivalente a extern).
     * Definimos la rutina como global para que sea visible desde otros modulos.
     */
	.global PendSV_Handler

	// declaro una funci externa
	.extern get_next_context
	// el extern me permite tener la dirección de sp1 declarado en C
	//.extern sp1

	.thumb_func

PendSV_Handler:

	// Contexto FPU
	tst lr,0x10			//hago una AND con el bit 4 del LR (EXC_RETURN)
	it eq 				// Si el resultado da cero
	vpusheq {s16-s31}	//pusheo s16-s31 a la pila y fuerzo el stacking de s0-s15, fpscr

	push {r4-r11,lr}

	mrs r0,msp			// r0 = msp
	bl get_next_context	// get_next_context(msp). bl = branch with link
	msr msp,r0			// msp = retorno de get_next_context

	pop {r4-r11,lr}		//Necesito recuperar lr antes de preguntar por uso de FPU

	// Contexto FPU
	tst lr,0x10			//hago una AND con el bit 4 del LR (EXC_RETURN)
	it eq 				// Si el resultado da cero
	vpopeq {s16-s31}
return:
	bx lr
