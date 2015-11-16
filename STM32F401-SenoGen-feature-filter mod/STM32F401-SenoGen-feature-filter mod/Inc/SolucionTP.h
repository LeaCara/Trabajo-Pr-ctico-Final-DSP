#ifndef SOLUCIONTP_H
#define SOLUCIONTP_H

#include "application.h"
#include "ff.h"
#include "waveplayer.h"
#include "waverecorder.h"
#include "ff.h"    
#include "ff_gen_drv.h"
#include "usbh_diskio.h"
#include "main.h"
#include "utils.h"
#include "audioFilter.h"
#include <stdio.h>
#include "stm32f4xx_hal.h"

typedef void (*PeriodicCalledFunction_type)(void);

void PulseSM(int16_t *pBuff, int32_t length);
void PeriodicCaller_Init(void);
void PeriodicCaller_Start(PeriodicCalledFunction_type PeriodicCalledFunction);
void Timer(void);

#endif /* SOLUCIONTP_H */
