# Demo NetX Ping
This is a small ping demo of the high-performance [NetX Duo](https://github.com/azure-rtos/netxduo) TCP/IP stack. Once the device is connected to WIFI AP and IP address is acquired, open a terminal from PC and ping address of device.

## Binary file
The built binary file is located at *build/demo_netx/demo_netx_ping/demo_netx_ping.uf2*.

## Output
```
Version: 7.95.49 (2271bb6 CY) CRC: b7a28ef3 Date: Mon 2021-11-29 22:50:27 PST Ucode Ver: 1043.2162 FWID 01-c51d9400
cyw43 loaded ok, mac xx:xx:xx:xx:xx
API: 12.2
Data: RaspberryPi.PicoW
Compiler: 1.29.4
ClmImport: 1.47.1
Customization: v5 22/06/24
Creation: 2022-06-24 06:55:08
Connecting to Wi-Fi...
connect status: joining
connect status: link up
Connected to onetime.
DHCP In Progress...
IP address: x.x.x.x
Mask: 255.255.255.0
```
