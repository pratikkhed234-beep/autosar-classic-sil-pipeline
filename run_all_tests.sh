#!/bin/bash
set -e

echo "======================================================"
echo " Starting Full AUTOSAR SiL Test Suite Execution"
echo "======================================================"

# 1. Clean and Rebuild All Binaries
make clean
make all

echo ""
echo ">>> [TEST 1/5] Intra-ECU Multi-SWC RTE Simulation"
./build/multi_swc_sim.exe

echo ""
echo ">>> [TEST 2/5] Inter-ECU CAN Bus Communication"
./build/can_bsw_sim.exe

echo ""
echo ">>> [TEST 3/5] Diagnostics Stack (DEM/DCM UDS Service 0x19)"
./build/autosar_diag_sim.exe

echo ""
echo ">>> [TEST 4/5] Memory Stack (NvM Persistence & CRC Verification)"
./build/autosar_nvm_sim.exe

echo ""
echo ">>> [TEST 5/5] System Management (EcuM State Machine & OS Scheduler)"
./build/autosar_os_ecum_sim.exe

echo ""
echo "======================================================"
echo " [PASSED] All 5 AUTOSAR Modules Verified Successfully!"
echo "======================================================"
