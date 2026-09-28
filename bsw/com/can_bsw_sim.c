#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <windows.h>
#include "Std_Types.h"

/* Structure representing a standard CAN 2.0B Frame */
typedef struct {
    uint32_t id;      /* CAN Identifier (0x100) */
    uint8_t  dlc;     /* Data Length Code (8 bytes) */
    uint8_t  data[8]; /* 8-byte payload */
} Can_PduType;

/* Global Virtual CAN Bus Line */
static Can_PduType g_virtual_can_bus;

/* ============================================================
 * TRANSMITTER SIDE (ECU 1: Powertrain / Speed Sensor)
 * ============================================================ */

/* AUTOSAR Can Interface / Driver */
void Can_Write(Can_PduType* pdu)
{
    memcpy(&g_virtual_can_bus, pdu, sizeof(Can_PduType));
    printf("  [CAN Bus] TX -> ID: 0x%03X | DLC: %d | Data: [ ", pdu->id, pdu->dlc);
    for (int i = 0; i < pdu->dlc; i++) {
        printf("%02X ", pdu->data[i]);
    }
    printf("]\n");
}

/* AUTOSAR COM Module: Packs signal into PDU */
void Com_SendSignal_VehicleSpeed(uint16 speed)
{
    Can_PduType pdu;
    pdu.id = 0x100;
    pdu.dlc = 8;
    memset(pdu.data, 0x00, sizeof(pdu.data));

    /* Little-Endian (Intel) byte packing */
    pdu.data[0] = (uint8_t)(speed & 0x00FF);
    pdu.data[1] = (uint8_t)((speed >> 8) & 0x00FF);

    /* Byte 2: Signal status (0x01 = Valid) */
    pdu.data[2] = 0x01;

    Can_Write(&pdu);
}

/* ============================================================
 * RECEIVER SIDE (ECU 2: Instrument Cluster / Dashboard)
 * ============================================================ */

/* AUTOSAR COM Module: Unpacks signal from received PDU */
Std_ReturnType Com_ReceiveSignal_VehicleSpeed(uint16* speed)
{
    if (g_virtual_can_bus.id != 0x100) {
        return E_NOT_OK;
    }

    /* Little-Endian unpacking */
    uint16 lsb = (uint16)g_virtual_can_bus.data[0];
    uint16 msb = (uint16)g_virtual_can_bus.data[1];
    *speed = (uint16)((msb << 8) | lsb);

    return E_OK;
}

/* ============================================================
 * MAIN SIMULATION RUNNER
 * ============================================================ */
int main(void)
{
    printf("=============================================================\n");
    printf(" AUTOSAR Inter-ECU Communication Simulation via CAN\n");
    printf(" ECU 1 (SpeedSensor) -> COM Stack -> CAN Bus -> ECU 2 (Dashboard)\n");
    printf("=============================================================\n\n");

    uint16 speed_profile[] = {0, 25, 50, 75, 80, 95, 110, 120};
    int total_cycles = sizeof(speed_profile) / sizeof(speed_profile[0]);

    for (int i = 0; i < total_cycles; i++)
    {
        uint16 tx_speed = speed_profile[i];
        printf("--- Transmission Cycle %d ---\n", i + 1);
        printf("1. [ECU 1 SWC] Sensor Reading: %3u km/h\n", tx_speed);

        /* ECU 1 COM Module serializes and transmits */
        Com_SendSignal_VehicleSpeed(tx_speed);

        /* ECU 2 COM Module receives and unpacks */
        uint16 rx_speed = 0;
        if (Com_ReceiveSignal_VehicleSpeed(&rx_speed) == E_OK) {
            printf("2. [ECU 2 SWC] Dashboard Display: %3u km/h", rx_speed);
            if (rx_speed >= 80) {
                printf("  --> [!] WARNING: OVERSPEED LIMIT REACHED!\n");
            } else {
                printf("  --> [OK] Speed Normal\n");
            }
        }
        printf("\n");
        Sleep(60);
    }

    printf("CAN Inter-ECU simulation completed successfully.\n");
    return 0;
}
