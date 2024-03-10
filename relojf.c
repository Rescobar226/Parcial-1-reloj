#include <stdint.h>

	#define RCC_C 0X40021000U // Dirección base de los registros del reloj
	#define PORTA 0x50000000U // Dirección base del puerto A
	#define PORTB 0X50000400U // Dirección base del puerto B
	#define PORTC 0X50000800U // Dirección base del puerto C


	// Estructura que representa los registros de un pin GPIO

	typedef struct{
			uint32_t MODER;
			uint32_t OTYPER;
			uint32_t OSPEEDR;
			uint32_t PUPDR;
			uint32_t IDR;
			uint32_t ODR;
			uint32_t BSRR;
			uint32_t LCKR;
			uint32_t AFR[2];
			uint32_t BRR;

	}GPIO_RegDef_t;


	// Estructura que representa los registros del RCC (Reset and Clock Control)

	typedef struct{

			uint32_t CR;
			uint32_t ICSCR;
			uint32_t CRRCR;
			uint32_t CFGR;
			uint32_t CIER;
			uint32_t CIFR;
			uint32_t CICR;
			uint32_t IOPRSTR;
			uint32_t AHBRSTR;
			uint32_t APB2RSTR;
			uint32_t APB1RSTR;
			uint32_t IOPENR;

	}RCC_RegDef_t;


	// Estructura para llevar el control de variables de tiempo

	struct Time_t{

		uint8_t hora_dec;
		uint8_t hora_uni ;

		uint8_t minuto_dec ;
		uint8_t minuto_uni ;

		uint8_t segundo_dec ;
		uint8_t segundo_uni ;

	};

	// Estructura para representar la alarma

	struct alarm{

			uint8_t hora_dec;
			uint8_t hora_uni ;

			uint8_t minuto_dec ;
			uint8_t minuto_uni  ;

			uint8_t segundo_dec ;
			uint8_t segundo_uni ;


	};

	// Estructura para representar el temporizador

	struct temporizador{

				uint8_t hora_dec ;
				uint8_t hora_uni ;

				uint8_t minuto_dec ;
				uint8_t minuto_uni ;

				uint8_t segundo_dec ;
				uint8_t segundo_uni ;

		};

	// Macros para acceder a las estructuras de los registros GPIO y RCC

	#define GPIOA ((GPIO_RegDef_t*)PORTA)
	#define GPIOB ((GPIO_RegDef_t*)PORTB)
	#define GPIOC ((GPIO_RegDef_t*)PORTC)
	#define RCC ((RCC_RegDef_t*)RCC_C)

	uint8_t enc_dis = 0x00; // Variable para controlar la secuencia de encendido de los displays
	int acc = 0; // Variable para controlar el formato de 24 horas, 12 horas o temporizador
	uint32_t inc_segundos; // Contador de segundos para el temporizador
	void delay_ms(uint16_t n); // Función de retardo en milisegundos

	uint8_t decoder(uint8_t value_to_decode); // Función de decodificación para el display de 7 segmentos


	// Definición de los patrones de los dígitos para el display de 7 segmentos (cátodo común)

	#define digit0 0x3F
	#define digit1 0X06
	#define digit2 0X5B
	#define digit3 0X4F
	#define digit4 0X66
	#define digit5 0X6D
	#define digit6 0X7D
	#define digit7 0X07
	#define digit8 0X7F
	#define digit9 0X6F




	int main(void){
		// Inicialización de las estructuras para el reloj, alarma y temporizador
		// Configuración de los valores iniciales de tiempo

		struct Time_t watch;

		watch.segundo_uni = 0x00;
		watch.segundo_dec = 0x00;
		watch.minuto_uni = 0x00;
		watch.minuto_dec = 0x00;
		watch.hora_uni = 0x00;
		watch.hora_dec = 0x00;

		struct alarm alarma;
		alarma.minuto_uni = 0x04;
		alarma.minuto_dec = 0x00;
		alarma.hora_uni = 0x00;
		alarma.hora_dec = 0x00;

		struct temporizador temp;
		temp.segundo_uni = 0x00;
		temp.segundo_dec = 0x00;
		temp.minuto_uni = 0x01;
		temp.minuto_dec = 0x00;
		temp.hora_uni = 0x00;
		temp.hora_dec = 0x00;

		int modo =0;
		int i;



		 // Configuración de los pines del puerto A

		RCC ->IOPENR |= 1 << 0; // Habilitar reloj para el puerto A


		// Configuración de botones (entrada) en el puerto A (A0, A1)

		GPIOA ->MODER &= ~(1 << 0); //Pin A0 control segundos
		GPIOA ->MODER &= ~(1 << 1);
		GPIOA ->PUPDR &= ~(1 << 0); //Resistencia de pulldown
		GPIOA ->PUPDR |= 1 << 1;

		GPIOA ->MODER &= ~(1 << 8); //Pin A4
		GPIOA ->MODER &= ~(1 << 9);
		GPIOA ->PUPDR &= ~(1 << 8); //Resistencia de pulldown
		GPIOA ->PUPDR |= 1 << 9;


		// Configuración de indicadores lumínicos LED en el puerto A (A8, A6, A10)

		GPIOA ->MODER |= 1<<16;  //Pin A8
		GPIOA ->MODER &= ~(1 << 17);

		GPIOA ->MODER |= 1<<12;  //Pin A6
		GPIOA ->MODER &= ~(1 << 13);

		GPIOA ->MODER |= 1<<20;  //Pin A10
		GPIOA ->MODER &= ~(1 << 21);


		// Configuración de los pines del puerto B

		RCC -> IOPENR |= 1 << 1; // Habilitar reloj para el puerto B


		// Configuración de pines como salida para el display de 7 segmentos multiplexado en el puerto B

		GPIOB ->MODER |= 1<<0; //Pin B0
		GPIOB ->MODER &= ~(1 << 1);

		GPIOB ->MODER |= 1<< 2; //Pin B1
		GPIOB ->MODER &= ~(1 << 3);

		GPIOB ->MODER |= 1 << 4; //Pin B2
		GPIOB ->MODER &= ~(1 << 5);

		GPIOB ->MODER |= 1 << 6; //Pin B3
		GPIOB ->MODER &= ~(1 << 7);

		GPIOB ->MODER |= 1 << 8; //Pin B4
		GPIOB ->MODER &= ~(1 << 9);

		GPIOB ->MODER |= 1 << 10; //Pin B5
		GPIOB ->MODER &= ~(1 << 11);

		GPIOB ->MODER |= 1 << 12; //Pin B6
		GPIOB ->MODER &= ~(1 << 13);

		GPIOB ->MODER |= 1 << 14; //Pin B7
		GPIOB ->MODER &= ~(1 << 15);


		// Configuraciones para el puerto C (control de display multiplexado)

		RCC ->IOPENR |= 1 << 2; //Habilitar puerto C reloj

		//Pines configurados como salida del puerto C

		// Pines de control del display multiplexado

		GPIOC ->MODER |= 1 << 10; //Pin C5
		GPIOC ->MODER &= ~(1 << 11);

		GPIOC ->MODER |= 1 << 12; //Pin C6
		GPIOC ->MODER &= ~(1 << 13);

		GPIOC ->MODER |= 1 << 16; //Pin C8
		GPIOC ->MODER &= ~(1 << 17);

		GPIOC ->MODER |= 1 << 18; //Pin C9
		GPIOC ->MODER &= ~(1 << 19);

		GPIOC ->MODER |= 1 <<20; //Pin C10
		GPIOC ->MODER &= ~(1 << 21);

		GPIOC ->MODER |= 1 <<22; //Pin C11
		GPIOC ->MODER &= ~(1 << 23);

		//Pin alarma C0
		GPIOC ->MODER |= 1 <<0;
		GPIOC ->MODER &= ~(1 << 1);


		while(1)
		{

			// Máquina de estados finitos para controlar el multiplexado de los displays

			switch(enc_dis){

			case 0:{

				//Para el display 1 unidad segundos
				enc_dis++;
				GPIOB ->BSRR |= 0XFF << 16; //Limpiar display
				GPIOC ->BSRR |= 0X01 << 27; //Limpiar D5/C11
				GPIOC ->BSRR |= 0X01 << 5; //Habilita D0/C5
				GPIOB ->BSRR |= decoder(watch.segundo_uni) << 0; //Manda a imprimir display 1

				break;
			}

			case 1:{

				//Para el display 2 decena segundos
				enc_dis++;
				GPIOB ->BSRR |= 0XFF << 16; //Limpiar display
				GPIOC ->BSRR |= 0X01 << 21; //Limpiar D0/C5
				GPIOC ->BSRR |= 0X01 << 6 ; //Habilita D1/C6
				GPIOB ->BSRR |= decoder(watch.segundo_dec) << 0; //Manda a imprimir display 2

				break;
			}

			case 2:{

				//Para el display 3 unidad minutos
				enc_dis++;
				GPIOB ->BSRR |= 0XFF << 16; //Limpiar display
				GPIOC ->BSRR |= 0X01 << 22; //Limpiar D1/C6
				GPIOC ->BSRR |= 0X01 << 8; //Habilita D2/C8
				GPIOB ->BSRR |= decoder(watch.minuto_uni) << 0; //Manda a imprimir display 3

				break;
			}

			case 3:{

				//Para el display 4 decena minutos
				enc_dis ++;
				GPIOB ->BSRR |= 0XFF << 16; //Limpiar display
				GPIOC ->BSRR |= 0X01 << 24; //Limpiar D2/C8
				GPIOC ->BSRR |= 0X01 << 9; //Habilita D3/C9
				GPIOB ->BSRR |= decoder(watch.minuto_dec) << 0; //Manda a imprimir display 4

				break;
			}

			case 4:{

				//Para el display 5 unidad horas
				enc_dis ++;
				GPIOB ->BSRR |= 0XFF << 16; //Limpiar display
				GPIOC ->BSRR |= 0X01 << 25; //Limpiar D3/C9
				GPIOC ->BSRR |= 0X01 << 10; //Habilita D4/C10
				GPIOB ->BSRR |= decoder(watch.hora_uni) << 0; //Manda a imprimir display 5

				break;
			}

			case 5:{

				//Para el display 6 decena horas
				enc_dis = 0;
				GPIOB ->BSRR |= 0XFF << 16; //Limpiar display
				GPIOC ->BSRR |= 0X01 << 26 ; //Limpiar D4/C10
				GPIOC ->BSRR |= 0X01 << 11; //Habilita D5/C11
				GPIOB ->BSRR |= decoder(watch.hora_dec) << 0; //Manda a imprimir display 6

				break;
			}

			default:

			{
				//Caso no toma en cuenta ninguno enc_dis por proteccion
				enc_dis =0 ;
				break;
			}

			}

			//Termina maquina de estados finitos


	        // Lógica para el incremento del tiempo y acciones según el modo

						if(modo == 0){
							delay_ms(1);
							inc_segundos++;

						if (inc_segundos == 1000) {
							inc_segundos = 0;
							watch.segundo_uni--;

							}

						if (watch.segundo_uni < 0){
							watch.segundo_uni = 9;
							watch.segundo_dec--;
							}

						if (watch.segundo_dec < 0){
							watch.segundo_dec = 5;
							watch.minuto_uni--;
							}

						if (watch.minuto_uni < 0){
							watch.minuto_uni = 9;
							watch.minuto_dec--;
							}

						if (watch.minuto_dec < 0){
							watch.minuto_dec = 5;
							watch.hora_uni--;
							}

						if (watch.hora_uni  < 0){
							watch.hora_uni = 9;
							watch.hora_dec--;
							}

						if (watch.hora_dec < 0){
							watch.hora_dec = 0;
							}

						}
			}

		// Lógica para manejar el botón y cambiar de modo

		if(modo == 1 || modo == 2){

			//Incremento segundos
			delay_ms(1);
			inc_segundos++;

			//Para la unidad de segundos
			if (inc_segundos == 1000 || GPIOA->IDR  & (1<<0)) {
		    inc_segundos = 0;
		    watch.segundo_uni++;
			}

			if (watch.segundo_uni == 10){
			watch.segundo_uni= 0;
			watch.segundo_dec++;
			}

			if (watch.segundo_dec ==6){
			watch.segundo_dec = 0;
			watch.minuto_uni++;
			}

			if (watch.minuto_uni==10){
			watch.minuto_uni =0;
			watch.minuto_dec++;
			}

			if (watch.minuto_dec==6){
			watch.minuto_dec=0;
			watch.hora_uni++;
			}

			if (watch.hora_uni==10){
			watch.hora_uni =0;
			watch.hora_dec++;
			}

			if (modo == 1){
				if (watch.hora_dec==1){ //Formato 12 horas
					if(watch.hora_uni==3){
						watch.hora_dec=0;
						watch.hora_uni=1;
					}
				}
			}

			if(modo == 2){
				if(watch.hora_dec == 2){ //Formato 24 horas
					if(watch.hora_uni==4){
						delay_ms(100);
						watch.hora_dec=0;
						watch.hora_uni=0;
					}
				}
			}

			if(watch.hora_dec == alarma.hora_dec && watch.hora_uni == alarma.hora_uni && watch.minuto_dec == alarma.minuto_dec && watch.minuto_uni == alarma.minuto_uni &&(modo == 1 ||modo == 2)){
				watch.minuto_uni++;
				for(i=0; i<10;i++){
				delay_ms(500);
				GPIOC ->ODR ^= 1 << 0;

				}

			}

			switch(acc){

				case 0:{

					modo = 0; //Cronometro
					GPIOA->ODR |= 1 << 8;
					GPIOA->ODR  &= ~(1 << 6);
					GPIOA->ODR  &= ~(1 << 10);

					if(GPIOA ->IDR  & (1<<4)){
					watch.segundo_uni = 0x00;
					watch.segundo_dec = 0x00;
					watch.minuto_uni = 0x00;
					watch.minuto_dec = 0x00;
				    watch.hora_uni = 0x02;
					watch.hora_dec = 0x01;
					delay_ms(500);
					acc++;

					}

					break;
				}

				case 1:{

					modo = 1; //Formato 24 horas
					GPIOA->ODR |= 1 << 6;
					GPIOA->ODR  &= ~(1 << 8);
					GPIOA->ODR  &= ~(1 << 10);

					if(GPIOA ->IDR  & (1<<4)){
						watch.segundo_uni = 0x00;
						watch.segundo_dec = 0x00;
						watch.minuto_uni = 0x00;
						watch.minuto_dec = 0x00;
						watch.hora_uni = 0x00;
						watch.hora_dec = 0x00;
						delay_ms(500);
						acc++;
					}
					break;
				}

				case 2:{

					modo = 2; //Formato 12 horas
					GPIOA->ODR |= 1 << 10;
					GPIOA->ODR  &= ~(1 << 6);
					GPIOA->ODR  &= ~(1 << 8);

					if(GPIOA ->IDR  & (1<<4)){
						watch.segundo_uni = 0x00;
						watch.segundo_dec = 0x00;
						watch.minuto_uni = 0x00;
						watch.minuto_dec = 0x00;
						watch.hora_uni = 0x00;
						watch.hora_dec = 0x00;
						delay_ms(500);
						acc = 0;
					}

					break;
				}

//Termina condicion del boton
				}

			//Termina el while
			}
	}


	// Definición de la función de retardo en milisegundos

	void delay_ms(uint16_t n){

		uint16_t i;
		for(; n>0; n--)
			for(i=0; i<140; i++);
	}

	// Definición de la función de decodificación para el display de 7 segmentos

	uint8_t decoder(uint8_t value_to_decode){
		switch(value_to_decode){

		case 0:{
			return(digit0);
		}

		case 1:{

			return(digit1);
		}

		case 2:{
			return(digit2);
		}

		case 3:{
			return(digit3);
		}

		case 4:{
			return(digit4);
		}

		case 5:{
			return(digit5);
		}

		case 6:{
			return(digit6);
		}

		case 7:{
			return(digit7);
		}
		case 8:{
			return(digit8);
		}

		case 9:{
			return(digit9);
		}

		default:
		{}

		return 0x00;
		}
	}
