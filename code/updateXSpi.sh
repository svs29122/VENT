#/bin/sh

./tvt xspi_ctl.vent -o ~/Programming/Spi_Controller/ip_repo/XSpiCtl_1.0/hdl/

# seems like we sometimes need to touch the file again for vivado to pick it up
sleep 2
touch ~/Programming/Spi_Controller/ip_repo/XSpiCtl_1.0/hdl/xspi_ctl.vhdl
