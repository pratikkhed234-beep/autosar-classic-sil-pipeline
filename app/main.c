#include <stdio.h>
#include <windows.h>
#include "Rte_SpeedSensor_SWC.h"
#include "Rte_Dashboard_SWC.h"

/* Virtual Functional Bus (VFB) shared signal buffer */
static uint16 g_vfb_VehicleSpeed = 0;

/* RTE Write API (Invoked by SpeedSensor_SWC) */
Std_ReturnType Rte_Write_PP_VehicleSpeed_VehicleSpeed(uint16 data)
{
    g_vfb_VehicleSpeed = data;
    return E_OK;
}

/* RTE Read API (Invoked by Dashboard_SWC) */
Std_ReturnType Rte_Read_RP_VehicleSpeed_VehicleSpeed(uint16* data)
{
    if (data == NULL) {
        return E_NOT_OK;
    }
    *data = g_vfb_VehicleSpeed;
    return E_OK;
}

int main(void)
{
    printf("=====================================================\n");
    printf(" AUTOSAR Multi-SWC Virtual Functional Bus Simulation\n");
    printf(" SpeedSensor (20ms) -> RTE VFB -> Dashboard (40ms)\n");
    printf("=====================================================\n\n");

    for (int cycle = 1; cycle <= 20; cycle++)
    {
        int time_ms = cycle * 20;
        printf("--- Time: %3d ms ---\n", time_ms);

        /* Sensor runnable executes every 20 ms */
        SpeedSensor_ReadSpeed();

        /* Dashboard runnable executes every 40 ms (every 2 cycles) */
        if (cycle % 2 == 0) {
            Dashboard_DisplayUpdate();
        }

        printf("\n");
        Sleep(40);
    }

    printf("Multi-SWC Simulation completed successfully.\n");
    return 0;
}