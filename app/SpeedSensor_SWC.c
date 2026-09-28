#include "Rte_SpeedSensor_SWC.h"
#include <stdio.h>

/*
 * Implementation of Runnable: SpeedSensor_ReadSpeed
 * Triggered by: TE_20ms (Every 20 ms)
 */
void SpeedSensor_ReadSpeed(void)
{
    static uint16 simulated_speed = 0;

    /* Simulate vehicle acceleration */
    if (simulated_speed < 120) {
        simulated_speed += 5;
    } else {
        simulated_speed = 0;
    }

    /* Send data out through the RTE Write API */
    Std_ReturnType status = Rte_Write_PP_VehicleSpeed_VehicleSpeed(simulated_speed);

    if (status == E_OK) {
        printf("[SWC SpeedSensor] Wrote speed: %3u km/h to RTE\n", simulated_speed);
    } else {
        printf("[SWC SpeedSensor] RTE Write failed!\n");
    }
}