
#include <GPIO_E.h>


/* GPIO input configuration*/
port_pin_config_t input_config = {
	kPORT_PullUp,
	kPORT_FastSlewRate,
	kPORT_PassiveFilterDisable,
	kPORT_LowDriveStrength,
	kPORT_MuxAsGpio,
};

/* ADC */
adc16_config_t adc16ConfigStruct;
adc16_channel_config_t adc16ChannelConfigStruct;

/* PWM */
tpm_config_t tpmInfo;
tpm_chnl_pwm_signal_param_t tpmParam;
//tpm_pwm_level_select_t pwmLevel = kTPM_LowTrue;
tpm_pwm_level_select_t pwmLevel = kTPM_HighTrue;

/* PIT */
uint8_t PIT_FLAG;

void GPIO_E_INIT(void)
{
	CLOCK_EnableClock(kCLOCK_PortE);

	/* Multiplexer configuration for output pins */
	PORT_SetPinMux(PORTE, PIN_E2_LED_1, kPORT_MuxAsGpio);
	PORT_SetPinMux(PORTE, PIN_E3_LED_2, kPORT_MuxAsGpio);
	PORT_SetPinMux(PORTE, PIN_E4_LED_3, kPORT_MuxAsGpio);
	PORT_SetPinMux(PORTE, PIN_E21_LED_A, kPORT_MuxAsGpio);
	PORT_SetPinMux(PORTE, PIN_E22_LED_B, kPORT_MuxAsGpio);

	/* Specifying output configuration */
	GPIO_PinInit(GPIOE, PIN_E2_LED_1, (gpio_pin_config_t *)kGPIO_DigitalOutput);
	GPIO_PinInit(GPIOE, PIN_E3_LED_2, (gpio_pin_config_t *)kGPIO_DigitalOutput);
	GPIO_PinInit(GPIOE, PIN_E4_LED_3, (gpio_pin_config_t *)kGPIO_DigitalOutput);
	GPIO_PinInit(GPIOE, PIN_E21_LED_A, (gpio_pin_config_t *)kGPIO_DigitalOutput);
	GPIO_PinInit(GPIOE, PIN_E22_LED_B, (gpio_pin_config_t *)kGPIO_DigitalOutput);

	/*  Digital pins as input */
	PORT_SetPinConfig(PORTE, PIN_E5_B1, &input_config);
	PORT_SetPinConfig(PORTE, PIN_E29_B2, &input_config);
	PORT_SetPinConfig(PORTE, PIN_E30_B3, &input_config);

	/*  Digital pins as output */
	GPIO_SetPinsOutput(GPIOE, INDICATOR_MASK);

}

/*****------------------------- ADC Function -------------------------------------*****/

void ADC_INIT(void)
{
	PORT_SetPinMux(PORTE, PIN_E20_AN0_LED_4, kPORT_PinDisabledOrAnalog);

	ADC16_GetDefaultConfig(&adc16ConfigStruct);
#ifdef BOARD_ADC_USE_ALT_VREF
    adc16ConfigStruct.referenceVoltageSource = kADC16_ReferenceVoltageSourceValt;
#endif
    ADC16_Init(ADC0, &adc16ConfigStruct);
    ADC16_EnableHardwareTrigger(ADC0, false); /* Make sure the software trigger is used. */
#if defined(FSL_FEATURE_ADC16_HAS_CALIBRATION) && FSL_FEATURE_ADC16_HAS_CALIBRATION
    if (kStatus_Success == ADC16_DoAutoCalibration(ADC0))
    {
        //PRINTF("ADC16_DoAutoCalibration() Done.\r\n");
    }
    else
    {
        //PRINTF("ADC16_DoAutoCalibration() Failed.\r\n");
    }
#endif /* FSL_FEATURE_ADC16_HAS_CALIBRATION */
    adc16ChannelConfigStruct.channelNumber = 0U;
    adc16ChannelConfigStruct.enableInterruptOnConversionCompleted = false;
#if defined(FSL_FEATURE_ADC16_HAS_DIFF_MODE) && FSL_FEATURE_ADC16_HAS_DIFF_MODE
    adc16ChannelConfigStruct.enableDifferentialConversion = false;
#endif /* FSL_FEATURE_ADC16_HAS_DIFF_MODE */
}

uint32_t ADC_READ(void)
{
	ADC16_SetChannelConfig(ADC0, 0U, &adc16ChannelConfigStruct);
	while (0U == (kADC16_ChannelConversionDoneFlag &
				  ADC16_GetChannelStatusFlags(ADC0, 0U)))
	{
	}
	return ADC16_GetChannelConversionValue(ADC0, 0U);
}

/*----------------------------- PWM Function -----------------------------------------*/

void PWM_INIT(void)
{
	PORT_SetPinMux(PORTE, PIN_E23_PWM_LED_4, kPORT_MuxAlt3);

	/* Configure tpm params with frequency 24kHZ */
	tpmParam.chnlNumber = (tpm_chnl_t)1U;
	tpmParam.level = pwmLevel; // kTPM_LowTrue; //pwmLevel;
	tpmParam.dutyCyclePercent = 0;

	/* Select the clock source for the TPM counter as kCLOCK_PllFllSelClk */
	CLOCK_SetTpmClock(1U);
	TPM_GetDefaultConfig(&tpmInfo);

	/* Initialize TPM module */
	tpmInfo.prescale = kTPM_Prescale_Divide_128;
	TPM_Init(TPM2, &tpmInfo);
	//TPM_SetupPwm(TPM2, &tpmParam, 1U, kTPM_CenterAlignedPwm, 24000U, CLOCK_GetFreq(kCLOCK_PllFllSelClk));
	TPM_SetupPwm(TPM2, &tpmParam, 1U, kTPM_EdgeAlignedPwm, 50U, CLOCK_GetFreq(kCLOCK_PllFllSelClk));
	TPM_StartTimer(TPM2, kTPM_SystemClock);




}

void PWM_PERCENTAGE(uint8_t percentage)
{
	TPM_UpdatePwmDutycycle(TPM2, (tpm_chnl_t)1U, kTPM_EdgeAlignedPwm, percentage);

}

/*------------------------------- PIT Function -------------------------------------*/



void PIT_INIT(void)
{
	pit_config_t pitConfig;
	PIT_GetDefaultConfig(&pitConfig);

	/* Init pit module */
	PIT_Init(PIT, &pitConfig);

	/* Set timer period for channel 0 */
	PIT_SetTimerPeriod(PIT, kPIT_Chnl_0, MSEC_TO_COUNT(1,CLOCK_GetFreq(kCLOCK_BusClk)));

	/* Enable timer interrupts for channel 0 */
	PIT_EnableInterrupts(PIT, kPIT_Chnl_0, kPIT_TimerInterruptEnable);

	/* Enable at the NVIC */
	EnableIRQ(PIT_IRQn);

	/* Start channel 0 */
	PIT_StartTimer(PIT, kPIT_Chnl_0);
}


void PIT_IRQHandler(void)
{

	PIT_FLAG = 1;
	PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);
}





