#!/bin/bash
export PICO_SDK_PATH=../../../../pico-sdk/
openocd_install_location=$HOME"/applications/openocd_rp2040/openocd"

mkdir -p build && cd build
cmake ..
make -j$(cat /proc/cpuinfo | grep bogomips | wc -l)
retval=$?
cd ..


if [ "$1" = "flash" ] && [ $retval -eq 0 ];
then
    #$openocd_install_location/src/openocd -s $openocd_install_location/tcl -f openocd/flash.cfg
    picotool reboot -f -u
    sleep 1
    picotool load build/main.uf2
    picotool reboot
fi


