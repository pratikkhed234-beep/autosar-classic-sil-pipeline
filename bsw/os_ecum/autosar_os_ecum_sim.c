#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <windows.h>
#include "Std_Types.h"

/* ============================================================
 * AUTOSAR OS DEFINITIONS (OSEK / VDXX Standard)
 * ============================================================ */
typedef uint8_t TaskType;
typedef uint8_t TaskPriorityType;

#define TASK_HIGH_PRIO_10MS   ((TaskType)0) /* Priority 3 (Highest) */
#define TASK_MED_PRIO_40MS    ((TaskType)1) /* Priority 2           */
#define TASK_LOW_PRIO_100MS   ((TaskType)2) /* Priority 1 (Lowest)  */

typedef enum {
    TASK_SUSPENDED,
    TASK_READY,
    TASK_RUNNING
} TaskStateType;

typedef struct {
    const char*       name;
    TaskPriorityType  priority;
    TaskStateType     state;
    uint32_t          period_ms;
    void              (*entry_point)(void);
} OsTaskControlBlock;

/* ============================================================
 * MOCK RUNNABLES (The 3 Stacks Integrated)
 * ============================================================ */

/* High-priority runnable: Real-time Powertrain / CAN */
void Runnable_Task_10ms(void)
{
    printf("    [OS Prio 3 | 10ms] Executing SpeedSensor & CAN Transmission\n");
}

/* Medium-priority runnable: Dashboard display */
void Runnable_Task_40ms(void)
{
    printf("    [OS Prio 2 | 40ms] Executing Dashboard Display Refresh\n");
}

/* Low-priority runnable: DEM qualification and NvM mirror checks */
void Runnable_Task_100ms(void)
{
    printf("    [OS Prio 1 | 100ms] Executing Background DEM Diagnostics & NvM Maintenance\n");
}

/* Task Control Blocks configured statically (OIL / ARXML OS description) */
static OsTaskControlBlock g_os_tasks[] = {
    {"Task_10ms_High",   3, TASK_READY, 10,  Runnable_Task_10ms},
    {"Task_40ms_Med",    2, TASK_READY, 40,  Runnable_Task_40ms},
    {"Task_100ms_Low",   1, TASK_READY, 100, Runnable_Task_100ms}
};

#define TOTAL_TASKS (sizeof(g_os_tasks) / sizeof(g_os_tasks[0]))

/* ============================================================
 * AUTOSAR OS SCHEDULER (Priority-Based Preemptive Simulation)
 * ============================================================ */
void Os_Scheduler_Tick(uint32_t current_time_ms)
{
    /* Evaluate task activations based on OS alarms/periods */
    for (int p = 3; p >= 1; p--) {
        for (size_t i = 0; i < TOTAL_TASKS; i++) {
            if (g_os_tasks[i].priority == p) {
                if (current_time_ms % g_os_tasks[i].period_ms == 0) {
                    g_os_tasks[i].state = TASK_RUNNING;
                    g_os_tasks[i].entry_point();
                    g_os_tasks[i].state = TASK_READY;
                }
            }
        }
    }
}

/* ============================================================
 * AUTOSAR EcuM (ECU State Manager)
 * ============================================================ */
typedef enum {
    ECUM_STATE_OFF,
    ECUM_STATE_STARTUP_ONE,
    ECUM_STATE_STARTUP_TWO,
    ECUM_STATE_RUN,
    ECUM_STATE_SHUTDOWN
} EcuM_StateType;

static EcuM_StateType g_ecum_state = ECUM_STATE_OFF;

void EcuM_Init(void)
{
    printf("=============================================================\n");
    printf(" AUTOSAR EcuM State Machine & OS Scheduler Simulation\n");
    printf("=============================================================\n\n");

    /* Phase 1: Microcontroller Abstraction Layer (MCAL) initialization */
    g_ecum_state = ECUM_STATE_STARTUP_ONE;
    printf("[EcuM Phase 1] Initializing low-level MCU clocks, Port pins, Watchdog...\n");

    /* Phase 2: Basic Software (BSW) initialization */
    g_ecum_state = ECUM_STATE_STARTUP_TWO;
    printf("[EcuM Phase 2] Initializing BSW stacks (CanIf, PduR, Com, Dem)...\n");
    printf("[EcuM Phase 2] Calling NvM_ReadAll() to restore calibration & DTCs...\n");
    printf("[EcuM Phase 2] Calling StartOS()...\n\n");

    g_ecum_state = ECUM_STATE_RUN;
}

void EcuM_Shutdown(void)
{
    g_ecum_state = ECUM_STATE_SHUTDOWN;
    printf("\n[EcuM Shutdown] Terminal 15 (Ignition) OFF detected.\n");
    printf("[EcuM Shutdown] Terminating OS Tasks...\n");
    printf("[EcuM Shutdown] Calling NvM_WriteAll() to flush RAM mirrors to Flash...\n");
    printf("[EcuM Shutdown] MCU entering low-power Standby / Sleep mode.\n");
    g_ecum_state = ECUM_STATE_OFF;
}

/* ============================================================
 * MAIN SIMULATION RUNNER
 * ============================================================ */
int main(void)
{
    /* 1. Start ECU state machine */
    EcuM_Init();

    printf("--- [OS RUN STATE: Multi-Tasking Timeline Started] ---\n");

    /* Simulate 120 ms of real-time execution */
    for (uint32_t t = 10; t <= 120; t += 10) {
        printf("Timeline Tick: %3u ms\n", t);
        Os_Scheduler_Tick(t);
        Sleep(25);
    }

    /* 2. Execute shutdown sequence */
    EcuM_Shutdown();

    printf("\nSimulation completed successfully.\n");
    return 0;
}
