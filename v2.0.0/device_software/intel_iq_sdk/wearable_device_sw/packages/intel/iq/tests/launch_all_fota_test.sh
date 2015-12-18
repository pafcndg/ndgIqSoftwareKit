#!/bin/sh

MAC_ADDR=$1
FILE_ROB=$2
FILE_NOMINAL=$3

echo "*******************************************"
echo "                 TEST 1                    "
echo "*******************************************"
echo
python fota_flash_cancel_req_during_flashing_rob.py ${MAC_ADDR}  ${FILE_ROB}
echo
echo "*******************************************"
echo "                 TEST 2                    "
echo "*******************************************"
echo
python fota_flash_start_req_during_flashing_rob.py ${MAC_ADDR} ${FILE_ROB}
echo
echo "*******************************************"
echo "                 TEST 3                    "
echo "*******************************************"
echo
python fota_flash_first_req_is_cancel_rob.py ${MAC_ADDR} ${FILE_ROB}
echo
echo "*******************************************"
echo "                 TEST 4                    "
echo "*******************************************"
echo
python fota_flash_first_req_is_bad_start_rob.py ${MAC_ADDR} ${FILE_ROB}
echo
echo "*******************************************"
echo "                 TEST 5                    "
echo "*******************************************"
echo
python fota_flash_first_req_is_continue_rob.py ${MAC_ADDR} ${FILE_ROB}
echo
echo "*******************************************"
echo "                 TEST 6                    "
echo "*******************************************"
echo
python fota_flash_recovery_iq.py ${MAC_ADDR} ${FILE_NOMINAL}