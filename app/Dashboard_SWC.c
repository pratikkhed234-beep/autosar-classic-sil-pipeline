#include "Rte_Dashboard_SWC.h"
#include <stdio.h>

void Dashboard_DisplayUpdate(void)
{
    uint16 current_speed = 0;
    Std_ReturnType status = Rte_Read_RP_VehicleSpeed_VehicleSpeed(&current_speed);

    if (status == E_OK) {
        printf("    [SWC Dashboard] Display Speed: %3u km/h", current_speed);
        if (current_speed >= 80) {
            printf("  --> [!] WARNING: OVERSPEED LIMIT REACHED!\n");
        } else {
            printf("  --> [OK] Speed Normal\n");
        }
    } else {
        printf("    [SWC Dashboard] Error: Could not read speed from RTE!\n");
    }
}
