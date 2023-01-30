# this script is a lost resort for whenever the usb situation gets so messed up, you essentially have to reboot the host pc, or alternatively run this script. Taken from http://billauer.co.il/blog/2013/02/usb-reset-ehci-uhci-linux/

#!/bin/bash

if [[ $EUID != 0 ]] ; then
  echo This must be run as root!
  exit 1
fi

for xhci in /sys/bus/pci/drivers/?hci_hcd ; do

  if ! cd $xhci ; then
    echo Weird error. Failed to change directory to $xhci
    exit 1
  fi

  echo Resetting devices from $xhci...

  for i in ????:??:??.? ; do
    echo -n "$i" > unbind
    echo -n "$i" > bind
  done
done

