openocd_install_location=$HOME"/applications/openocd_rp2040/openocd"

$openocd_install_location/src/openocd -s $openocd_install_location/tcl -f flash.cfg
