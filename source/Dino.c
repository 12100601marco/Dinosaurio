/*
 * Dino.c
 *
 *  Created on: 27 may. 2020
 *      Author: Marco
 */
#include <Dino.h>
#include <GPIO_E.h>

typedef enum{
	servo_up,
	servo_down

}FSM_STATES;

FSM_STATES FSM_state = servo_up;

int valor_ADC = 0;

void servo_estates(){

	valor_ADC = ADC_READ();

	switch(FSM_state)
				{
					case servo_up:
					{

						PWM_PERCENTAGE(6);

						if(valor_ADC < umbral)
						{

							FSM_state = servo_down;           //go to the next state
						}
						else
						{
							FSM_state = servo_up;           //remains in the state
						}
					}break;

					case servo_down:
					{

						PWM_PERCENTAGE(5);

						if(valor_ADC > umbral)
						{

							FSM_state = servo_up;           //go to the next state
						}
						else
						{
							FSM_state = servo_down;           //remains in the state
						}
					}break;

					default:
						{
							FSM_state = servo_up;		//default state
						}

				}



}

