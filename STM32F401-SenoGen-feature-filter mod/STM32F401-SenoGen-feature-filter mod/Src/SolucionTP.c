/* Includes ------------------------------------------------------------------*/
#include "SolucionTP.h"

// Estados posibles de la Maquina de estado que detecta pulsos
typedef enum {
	ST_IDLE = 0,
	ST_PULSE,
} StPulse;

#define TRUE		1
#define FALSE		0

#define WAIT_MAX	1500	// Tiempo a esperar por un maximo [us]
#define UMBRAL		2000	// Valor a partir del cual se considera presencia de un pulso

StPulse PState	= ST_IDLE;	// Estado SM pulsos
int16_t max		= 0;		// Maximo valor dentro de una muestra
UINT	cont	= 0;		// Contador de tonos

static TIM_HandleTypeDef               TIM_Handle2;
static PeriodicCalledFunction_type     PeriodicCalledFcn;
unsigned int cont_seg = 0;

/*
	Consulta si hay algun valor mayor al umbral en el buffer
*/
int16_t MayorUmbral(int16_t *pBuff, int32_t length) {
	UINT i;
	
	for(i = 0; i < length; i++) {
		if(pBuff[i] >= UMBRAL)
			return TRUE;
	}
	
	return FALSE;
}

/*
	Maquina de estados detectora de pulsos
*/
void PulseSM(int16_t *pBuff, int32_t length) {
	switch(PState) {
		// Esperando Pulso
		case ST_IDLE:
			if(MayorUmbral(pBuff, length) == TRUE) {
				if(++cont < 6)
					BSP_LED_On(LED6);
				else {
					BSP_LED_On(LED3);
					HAL_NVIC_DisableIRQ(TIM2_IRQn);// Inhibir interrupcion timer 2
					//if(cont_seg > 55)
						cont_seg = 60;
					HAL_NVIC_EnableIRQ(TIM2_IRQn);// Habilitar interrupcion timer 2
				}
				
				if(cont == 6)
					cont = 0;
				
				TickTock_Start();
				PState = ST_PULSE;
			}
			break;
		
		// Procesando Pulso
		case ST_PULSE:
			if(MayorUmbral(pBuff, length) == TRUE) {
				TickTock_Stop();
				TickTock_Start();
			}

			if(TickVal() > WAIT_MAX) {
				BSP_LED_Off(LED6);
				BSP_LED_Off(LED3);
				
				PState = ST_IDLE;
			}
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////
void PeriodicCaller_Init(void)
{
	__TIM2_CLK_ENABLE();

	TIM_Handle2.Instance = TIM2;
	TIM_Handle2.Init.Period = 999999;
	TIM_Handle2.Init.Prescaler = 83;
	TIM_Handle2.Init.ClockDivision = 0;
	TIM_Handle2.Init.CounterMode = TIM_COUNTERMODE_UP;
	HAL_TIM_Base_Init(&TIM_Handle2);

	HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

void PeriodicCaller_Start(PeriodicCalledFunction_type PeriodicCalledFunction)
{
	PeriodicCalledFcn = PeriodicCalledFunction;
	BSP_LED_Off(LED5);
	HAL_TIM_Base_Start(&TIM_Handle2);
	HAL_TIM_Base_Start_IT(&TIM_Handle2);
	__HAL_TIM_SetCounter(&TIM_Handle2,0);
}

void PeriodicCaller_Reset(void)
{
	__HAL_TIM_SetCounter(&TIM_Handle2,0);
}

void PeriodicCaller_Stop(void)
{
	HAL_TIM_Base_Stop(&TIM_Handle2);
}

void TIM2_IRQHandler(void)
{
	if (__HAL_TIM_GET_FLAG(&TIM_Handle2, TIM_FLAG_UPDATE) != RESET)      //Incase other interrupts are also running
	{
		if (__HAL_TIM_GET_ITSTATUS(&TIM_Handle2, TIM_IT_UPDATE) != RESET)
		{
			__HAL_TIM_CLEAR_FLAG(&TIM_Handle2, TIM_FLAG_UPDATE);
			PeriodicCalledFcn();
		}
	}
}

void Timer(void){
		
	if(cont_seg == 60)
	{
		BSP_LED_On(LED5);
		cont_seg = 0;
	}
	else
		BSP_LED_Off(LED5);


		

	printf("Seg: %d\r\n", cont_seg++);
}
