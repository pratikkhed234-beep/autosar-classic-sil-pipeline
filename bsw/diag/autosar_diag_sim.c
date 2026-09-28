#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <windows.h>
#include "Std_Types.h"

/* ============================================================
 * AUTOSAR DEM (Diagnostic Event Manager) Definitions
 * ============================================================ */
typedef uint8_t Dem_EventStatusType;
#define DEM_EVENT_STATUS_PASSED  ((Dem_EventStatusType)0x00)
#define DEM_EVENT_STATUS_FAILED  ((Dem_EventStatusType)0x01)

typedef uint32_t Dem_DTCFormatType;
#define DTC_OVERSPEED_FAULT      ((Dem_DTCFormatType)0xD01200)

/* UDS Status Mask byte (ISO 14229-1) */
#define DTC_STATUS_TEST_FAILED          (0x01)
#define DTC_STATUS_CONFIRMED_DTC        (0x08)

typedef struct {
    Dem_DTCFormatType dtc;
    uint8_t status_byte;
    uint16_t fault_count;
} Dem_EventStorageType;

static Dem_EventStorageType g_dtc_memory = {0, 0x00, 0};

/* DEM API: Called by SWCs via RTE to report fault events */
Std_ReturnType Dem_SetEventStatus(uint16_t EventId, Dem_EventStatusType EventStatus)
{
    if (EventStatus == DEM_EVENT_STATUS_FAILED) {
        g_dtc_memory.dtc = DTC_OVERSPEED_FAULT;
        g_dtc_memory.fault_count++;
        g_dtc_memory.status_byte |= (DTC_STATUS_TEST_FAILED | DTC_STATUS_CONFIRMED_DTC);
        return E_OK;
    } else {
        /* Clear active failed bit if self-healed */
        g_dtc_memory.status_byte &= ~DTC_STATUS_TEST_FAILED;
        return E_OK;
    }
}

/* ============================================================
 * AUTOSAR DCM (Diagnostic Communication Manager) - UDS Service 0x19
 * ============================================================ */
void Dcm_HandleUdsRequest_0x19(void)
{
    printf("\n>>> [UDS Diagnostic Tester] Request: 0x19 0x02 (Read DTC By Status Mask)\n");

    if (g_dtc_memory.status_byte & DTC_STATUS_CONFIRMED_DTC) {
        printf("<<< [DCM Response] Positive Response: 0x59 0x02\n");
        printf("    DTC: 0x%06X | Status Byte: 0x%02X (Confirmed | TestFailed)\n", 
               g_dtc_memory.dtc, g_dtc_memory.status_byte);
        printf("    Failure Occurrence Count: %u\n", g_dtc_memory.fault_count);
    } else {
        printf("<<< [DCM Response] 0x59 0x02 (No DTCs currently active)\n");
    }
    printf("\n");
}

/* ============================================================
 * MAIN SIMULATION RUNNER
 * ============================================================ */
int main(void)
{
    printf("=============================================================\n");
    printf(" AUTOSAR Diagnostic Stack Simulation (DEM + DCM + UDS)\n");
    printf(" SWC Fault Detection -> DEM DTC Logging -> UDS Service 0x19\n");
    printf("=============================================================\n\n");

    uint16 test_speeds[] = {60, 75, 85, 95, 70};
    int steps = sizeof(test_speeds) / sizeof(test_speeds[0]);

    for (int i = 0; i < steps; i++) {
        uint16 speed = test_speeds[i];
        printf("[Time Step %d] Vehicle Speed: %3u km/h\n", i + 1, speed);

        if (speed >= 80) {
            printf("  [SWC] Threshold violated! Calling Dem_SetEventStatus(FAILED)...\n");
            Dem_SetEventStatus(1, DEM_EVENT_STATUS_FAILED);
            printf("  [DEM] DTC 0xD01200 logged to Non-Volatile Memory (NVM).\n");
        } else {
            printf("  [SWC] Speed normal. Dem_SetEventStatus(PASSED).\n");
            Dem_SetEventStatus(1, DEM_EVENT_STATUS_PASSED);
        }
        Sleep(50);
    }

    /* Simulate an external Diagnostic Tester connecting over OBD-II port */
    Dcm_HandleUdsRequest_0x19();

    printf("Diagnostic lifecycle simulation completed.\n");
    return 0;
}
