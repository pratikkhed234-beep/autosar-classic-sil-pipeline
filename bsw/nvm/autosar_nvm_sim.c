#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <windows.h>
#include "Std_Types.h"

#define NVM_STORAGE_FILE "ecu_nvm_storage.bin"
#define NVM_BLOCK_DEM_DTC_DATA   ((uint16_t)1u)

/* Explicit byte packing (No padding bytes allowed in automotive flash blocks) */
#pragma pack(push, 1)
typedef struct {
    uint32_t dtc_id;
    uint8_t  status_mask;
    uint16_t occurrence_count;
    uint8_t  crc;
} NvM_DtcBlockType;
#pragma pack(pop)

/* Payload size over which CRC is calculated (all fields except crc itself) */
#define NVM_CRC_DATA_LEN (sizeof(NvM_DtcBlockType) - sizeof(uint8_t))

static NvM_DtcBlockType g_ram_mirror_block1;

/* Robust 8-bit checksum */
static uint8_t Calculate_CRC8(const uint8_t *data, size_t len)
{
    uint8_t crc = 0x00;
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
    }
    return crc;
}

static Std_ReturnType Fls_WritePhysical(const void *data, size_t size)
{
    FILE *fp = fopen(NVM_STORAGE_FILE, "wb");
    if (!fp) return E_NOT_OK;
    fwrite(data, 1, size, fp);
    fclose(fp);
    return E_OK;
}

static Std_ReturnType Fls_ReadPhysical(void *data, size_t size)
{
    FILE *fp = fopen(NVM_STORAGE_FILE, "rb");
    if (!fp) return E_NOT_OK;
    size_t read_bytes = fread(data, 1, size, fp);
    fclose(fp);
    return (read_bytes == size) ? E_OK : E_NOT_OK;
}

/* NvM_WriteBlock: Commits RAM mirror into Non-Volatile Flash */
Std_ReturnType NvM_WriteBlock(uint16_t BlockId, const void *SrcPtr)
{
    if (BlockId != NVM_BLOCK_DEM_DTC_DATA || SrcPtr == NULL) return E_NOT_OK;

    NvM_DtcBlockType *block = (NvM_DtcBlockType *)SrcPtr;
    block->crc = Calculate_CRC8((const uint8_t *)block, NVM_CRC_DATA_LEN);

    printf("  [NvM] Writing Block %u to Flash -> DTC: 0x%06X | Count: %u | CRC: 0x%02X\n",
           BlockId, block->dtc_id, block->occurrence_count, block->crc);

    return Fls_WritePhysical(block, sizeof(NvM_DtcBlockType));
}

/* NvM_ReadAll: Run during ECU Startup Phase */
Std_ReturnType NvM_ReadAll(void)
{
    printf("[ECU Boot Stage] Executing NvM_ReadAll()...\n");
    NvM_DtcBlockType disk_data;
    memset(&disk_data, 0, sizeof(disk_data));

    if (Fls_ReadPhysical(&disk_data, sizeof(NvM_DtcBlockType)) == E_OK) {
        uint8_t expected_crc = Calculate_CRC8((const uint8_t *)&disk_data, NVM_CRC_DATA_LEN);
        if (expected_crc == disk_data.crc) {
            memcpy(&g_ram_mirror_block1, &disk_data, sizeof(NvM_DtcBlockType));
            printf("  [NvM] Block 1 Restored -> DTC: 0x%06X | Count: %u | CRC OK (0x%02X)\n",
                   g_ram_mirror_block1.dtc_id, g_ram_mirror_block1.occurrence_count, disk_data.crc);
            return E_OK;
        } else {
            printf("  [NvM] CRC Mismatch! Memory corrupted.\n");
            return E_NOT_OK;
        }
    } else {
        printf("  [NvM] No existing flash file found. Initializing with default factory values.\n");
        memset(&g_ram_mirror_block1, 0, sizeof(NvM_DtcBlockType));
        return E_OK;
    }
}

int main(void)
{
    printf("=============================================================\n");
    printf(" AUTOSAR Memory Stack (NvM + Flash Emulation) Simulation\n");
    printf("=============================================================\n\n");

    /* 1. ECU Start / Reset phase */
    NvM_ReadAll();

    /* 2. Runtime Phase: Increment the occurrence count on persistent memory */
    printf("\n[Runtime Phase] Speed sensor detects overspeed condition...\n");
    g_ram_mirror_block1.dtc_id = 0xD01200;
    g_ram_mirror_block1.status_mask = 0x09;
    g_ram_mirror_block1.occurrence_count += 1;

    printf("  [DEM] RAM Mirror updated: Count is now %u\n", g_ram_mirror_block1.occurrence_count);

    /* 3. ECU Shutdown Phase: NvM writes modified blocks to flash */
    printf("\n[ECU Shutdown Phase] Ignition turned OFF. Committing RAM blocks to Flash...\n");
    NvM_WriteBlock(NVM_BLOCK_DEM_DTC_DATA, &g_ram_mirror_block1);

    printf("\nSimulation cycle completed.\n");
    return 0;
}
