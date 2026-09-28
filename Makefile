CC = gcc
CFLAGS = -Wall -O2 -Iapp

all: multi_swc can_sim diag_sim nvm_sim os_sim

multi_swc:
	$(CC) $(CFLAGS) app/main.c app/SpeedSensor_SWC.c app/Dashboard_SWC.c -o build/multi_swc_sim.exe

can_sim:
	$(CC) $(CFLAGS) bsw/com/can_bsw_sim.c -o build/can_bsw_sim.exe

diag_sim:
	$(CC) $(CFLAGS) bsw/diag/autosar_diag_sim.c -o build/diag_sim.exe

nvm_sim:
	$(CC) $(CFLAGS) bsw/nvm/autosar_nvm_sim.c -o build/nvm_sim.exe

os_sim:
	$(CC) $(CFLAGS) bsw/os_ecum/autosar_os_ecum_sim.c -o build/os_ecum_sim.exe

clean:
	rm -f build/*.exe ecu_nvm_storage.bin

.PHONY: all multi_swc can_sim diag_sim nvm_sim os_sim clean