#!/bin/bash


./mkspiffs -c ./spiffs_dir -b 4096 -p 256 -s 0x9F0000 spiffs_image.bin

python $IDF_PATH/components/esptool_py/esptool/esptool.py --chip esp32 --port $1 --baud 115200 write_flash -z 0x610000 spiffs_image.bin


