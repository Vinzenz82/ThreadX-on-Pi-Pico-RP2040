# Demo Azure IoT
TBD

## Binary file
* The built binary file is located at *build/demo_netx/demo_azure_iot/demo_azure_iot.combined.uf2*.
* Files for device update are:
    * *build/demo_netx/demo_azure_iot/demo_azure_iot_2.0.0.bin*
    * *build/demo_netx/demo_azure_iot/demo_azure_iot_2.0.0.importmanifest.json*

## Output
```
Version: 7.95.49 (2271bb6 CY) CRC: b7a28ef3 Date: Mon 2021-11-29 22:50:27 PST Ucode Ver: 1043.2162 FWID 01-c51d9400
cyw43 loaded ok, mac xx:xx:xx:xx:xx:xx
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
Gateway: x.x.x.x
DNS Server address: x.x.x.x
SNTP Time Sync...0.pool.ntp.org
SNTP Time Sync successfully.
[INFO] Azure IoT Security Module has been enabled, status=0
IoTHub Host Name: <host>.azure-devices.net; Device ID: <devie_id>.
Connected to IoTHub.
Driver initialized successfully.
[INFO] ADU agent started successfully!
Manufacturer: RaspberryPi, Model: PicoW, Installed Criteria: 1.0.0.
Sent properties request.
Telemetry message send: {"temperature":22}.
Received new update: Provider: RaspberryPi; Name: PicoW, Version: 2.0.0
Received all properties
[INFO] Updating firmware...
[INFO] Manufacturer: RaspberryPi
[INFO] Model: PicoW
[INFO] Firmware downloading...
Driver flash erased successfully.
Driver firmware writing...
[INFO] Getting download data... 0
Driver firmware writing...
[INFO] Getting download data... 1400
Driver firmware writing...
[INFO] Getting download data... 2800
Driver firmware writing...
<progress update>
[INFO] Getting download data... 648996
[INFO] Firmware downloaded
[INFO] Firmware installing...
Driver firmware installed successfully.
[INFO] Firmware installed
[INFO] Applying firmware...
[INFO] Manufacturer: RaspberryPi
[INFO] Model: PicoW
[INFO] Firmware applying...
Driver firmware apply successfully.
Version: 7.95.49 (2271bb6 CY) CRC: b7a28ef3 Date: Mon 2021-11-29 22:50:27 PST Ucode Ver: 1043.2162 FWID 01-c51d9400
cyw43 loaded ok, mac xx:xx:xx:xx:xx:xx
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
Gateway: x.x.x.x
DNS Server address: x.x.x.x
SNTP Time Sync...0.pool.ntp.org
SNTP Time Sync successfully.
[INFO] Azure IoT Security Module has been enabled, status=0
IoTHub Host Name: <host>.azure-devices.net; Device ID: <devie_id>.
Connected to IoTHub.
Driver initialized successfully.
[INFO] ADU agent started successfully!
Manufacturer: RaspberryPi, Model: PicoW, Installed Criteria: 2.0.0.
```
