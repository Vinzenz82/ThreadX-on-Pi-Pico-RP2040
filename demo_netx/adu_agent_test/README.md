# ADU agent test
This project validates [Azure Device Update](https://learn.microsoft.com/en-us/azure/iot-hub-device-update/device-update-azure-real-time-operating-system)(ADU) agent driver for Azure RTOS without connecting to Azure IoTHub.

The binary file of demo_threadx is placed in this project as const C array. During the test, it will be written to offset 1024K from beginning of flash. Once done, the device will reset. flash_loader will load the new application demo_threadx and start to run.

## Binary file
The built binary file is located at *build/demo_netx/adu_agent_test/adu_agent_test.combined.uf2*.

## Output
```
ADU driver testing 6.1!
Driver initialized successfully.
Driver flash erased successfully.
Driver firmware writing...
Driver firmware writing...
<repeat several times>
Driver firmware installed successfully.
Driver firmware apply successfully.
Version: 7.95.49 (2271bb6 CY) CRC: b7a28ef3 Date: Mon 2021-11-29 22:50:27 PST Ucode Ver: 1043.2162 FWID 01-c51d9400
cyw43 loaded ok, mac xx:xx:xx:xx:xx
**** ThreadX Demonstration on Raspberry Pi Pico **** 

           thread 0 events sent:          1
           thread 1 messages sent:        0
           thread 2 messages received:    0
           thread 3 obtained semaphore:   0
           thread 4 obtained semaphore:   0
           thread 5 events received:      0
           thread 6 mutex obtained:       0
           thread 7 mutex obtained:       0

**** ThreadX Demonstration on Raspberry Pi Pico **** 

           thread 0 events sent:          2
           thread 1 messages sent:        445535
           thread 2 messages received:    445499
           thread 3 obtained semaphore:   26
           thread 4 obtained semaphore:   25
           thread 5 events received:      1
           thread 6 mutex obtained:       26
           thread 7 mutex obtained:       25
```