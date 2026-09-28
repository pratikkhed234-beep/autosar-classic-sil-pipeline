\# AUTOSAR Classic Architecture \& BSW Software-in-the-Loop (SiL) Simulation



A zero-cost, end-to-end AUTOSAR Classic Release 4.2.2 development and simulation pipeline implemented in C and Python. This project demonstrates SWC component modeling, RTE generation concepts, communication serialization, automotive diagnostics, non-volatile storage, and OS task management without proprietary tool dependencies (e.g., Vector DaVinci, EB Tresos).



\---



\## Architectural Stack Overview



| Layer / Module | Implementation Details |

|---|---|

| \*\*AUTOSAR ARXML\*\* | Python XML generator producing AUTOSAR 4.2.2 schema (`SpeedSensor\_Model.arxml`) |

| \*\*Application \& RTE\*\* | Producer (`SpeedSensor\_SWC`) \& Consumer (`Dashboard\_SWC`) communicating via `Rte\_Write` and `Rte\_Read` over VFB |

| \*\*COM / CanIf Stack\*\* | PDU signal serialization, little-endian byte ordering into CAN 2.0B 8-byte frames |

| \*\*DEM / DCM (UDS)\*\* | Fault qualification, DTC logging (`0xD01200`), and ISO 14229-1 UDS Service 0x19 (Read DTC) |

| \*\*NvM / Memory\*\* | RAM mirror buffering, 8-bit CRC data integrity, and persistent Flash/EEPROM emulation |

| \*\*OS \& EcuM\*\* | ECU State Machine (Startup 1/2, Run, Shutdown) \& Priority-Based Preemptive Scheduling |



\---



\## Build and Run Instructions



\### Prerequisites

\- GCC (MinGW-w64 / MSYS2 UCRT64)

\- GNU Make

\- Python 3.x



\### Build All Modules

```bash

make all

