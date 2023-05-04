#!/bin/bash
export PICO_SDK_PATH=../../sdk/
openocd_install_location=$HOME"/applications/openocd_rp2040/openocd"

mkdir -p build && cd build
cmake ..
make -j$(cat /proc/cpuinfo | grep bogomips | wc -l)
retval=$?
cd ..


if [ "$1" = "flash" ] && [ $retval -eq 0 ];
then
    #$openocd_install_location/src/openocd -s $openocd_install_location/tcl -f openocd/flash.cfg
    #picotool reboot -f -u

    if picotool info | grep -q "No accessible RP2040 devices in BOOTSEL mode were found."; then
        echo -e "\nreset" > /dev/ttyACM0
    fi

    while picotool info | grep -q "No accessible RP2040 devices in BOOTSEL mode were found."; do
        echo "No rp2040 found"
        sleep 1
    done


    sleep 1
    picotool load build/main.uf2
    picotool reboot
fi


