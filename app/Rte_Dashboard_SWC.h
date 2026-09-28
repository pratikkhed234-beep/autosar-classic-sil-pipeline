#ifndef RTE_DASHBOARD_SWC_H
#define RTE_DASHBOARD_SWC_H

#include "Std_Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* RTE Read API for the Dashboard SWC */
Std_ReturnType Rte_Read_RP_VehicleSpeed_VehicleSpeed(uint16* data);

/* Runnable declaration */
void Dashboard_DisplayUpdate(void);

#ifdef __cplusplus
}
#endif

#endif /* RTE_DASHBOARD_SWC_H */
