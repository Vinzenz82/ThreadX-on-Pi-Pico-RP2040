# Demo ThreadX
This project is a small demo of the high-performance [ThreadX](https://github.com/azure-rtos/threadx) kernel.  It includes examples of eight threads of different priorities, using a message queue, semaphore, mutex, event flags group, byte pool, and block pool.

In this demo, LED will blink once per second.

## Binary file
The built binary file is located at *build/demo_threadx/demo_threadx.uf2*.

## Output
```
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
           thread 1 messages sent:        445512
           thread 2 messages received:    445434
           thread 3 obtained semaphore:   26
           thread 4 obtained semaphore:   25
           thread 5 events received:      1
           thread 6 mutex obtained:       26
           thread 7 mutex obtained:       25

...
```
