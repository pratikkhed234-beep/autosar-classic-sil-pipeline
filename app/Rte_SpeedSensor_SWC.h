#ifndef RTE_SPEEDSENSOR_SWC_H
#define RTE_SPEEDSENSOR_SWC_H

#include "Std_Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * RTE Write API generated from:
 * Port: PP_VehicleSpeed, DataElement: VehicleSpeed 
 */
Std_ReturnType Rte_Write_PP_VehicleSpeed_VehicleSpeed(uint16 data);

/* 
 * Runnable entry function declaration generated from:
 * <SYMBOL>SpeedSensor_ReadSpeed</SYMBOL>
 */
void SpeedSensor_ReadSpeed(void);

#ifdef __cplusplus
}
#endif

#endif /* RTE_SPEEDSENSOR_SWC_H */