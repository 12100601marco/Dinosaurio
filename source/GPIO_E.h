
#ifndef _GPIO_E_H_
#define _GPIO_E_H_


#include "fsl_adc16.h"
#include "fsl_port.h"
#include "fsl_tpm.h"
#include "fsl_pit.h"
#include "board.h"

/*GPIO equivalents*/

#define PIN_E2_LED_1                    2u //Led 1
#define PIN_E3_LED_2				    3u //Led 2
#define PIN_E4_LED_3                    4u //Led 3

#define PIN_E21_LED_A					21u //Track 1
#define PIN_E22_LED_B				    22u //Track 2

#define PIN_E5_B1						 5u //Button 0
#define PIN_E29_B2						29u //Button 1
#define PIN_E30_B3						30u //Button 2

#define PIN_E20_AN0_LED_4				20u //Analog
#define PIN_E23_PWM_LED_4			    23u //PWM

/* LED with negative logic */
#define ON  0
#define OFF 1

																		//\ line change
#define INDICATOR_MASK (uint32_t)((0x00000000)|(1u << PIN_E2_LED_1)|\
		(1u << PIN_E3_LED_2)|(1u << PIN_E4_LED_3)|(1u << PIN_E21_LED_A)|\
		(1u << PIN_E22_LED_B))

#define write_pin(pin, state) GPIO_WritePinOutput(GPIOE, pin, state)
#define read_pin(pin) GPIO_ReadPinInput(GPIOE, pin)

/* Port function */
void GPIO_E_INIT(void);

/* ADC function */
void ADC_INIT(void);
uint32_t ADC_READ(void);

/* PWM function */
void PWM_INIT(void);
void PWM_PERCENTAGE(uint8_t);

/* PIT function */
void PIT_INIT(void);
void PIT_IRQHandler(void);


#endif
