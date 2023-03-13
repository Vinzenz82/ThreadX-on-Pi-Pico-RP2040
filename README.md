# Azure RTOS on Raspberry Pi Pico W

This repository demonstrates [Azure RTOS](https://azure.com/rtos) on [Raspberry Pi Pico W](https://www.raspberrypi.com/documentation/microcontrollers/raspberry-pi-pico.html#raspberry-pi-pico-w-and-pico-wh).

## Contents

| File/folder       | Description                                |
|-------------------|--------------------------------------------|
| `demo_netx`       | Sample source code for NetX projects       |
| `demo_threadx`    | Sample source code for ThreadX projects    |
| `lib`             | Azure RTOS and other repositories.         |
| `main.c`          | Pico W user application entry point.       |

## Prerequisites

1. [Raspberry Pi Pico W](https://www.raspberrypi.org/products/raspberry-pi-pico/) board.

2. A terminal emulator (such as [Putty](https://www.chiark.greenend.org.uk/~sgtatham/putty/) or [Tera Term](https://ttssh2.osdn.jp/index.html.en) to display the output.

3. [Codespaces](https://github.com/features/codespaces)(**recommended**) or [VS Code](https://code.visualstudio.com/) + [Dev Container](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers).

## Build

### Codespaces
1. Go to root of this [repository](https://github.com/TiejunMS/Azure-RTOS-on-Raspberry-Pi-Pico-RP2040).
1. Choose `Code | Open with Codespaces`, to create and launch your Codespaces environment.
1. Open [/.vscode/settings.json](/.vscode/settings.json). Update `WIFI_SSID` and `WIFI_PASSWORD`.
1. To run *demo_netx/demo_azure_iot* sample, follow readme in this project to update configuration.
1. Press `F7` to build the application.
    > Choose the `GCC 9.2.1 arm-non-eabi` CMake kit if prompted.
1. Download binary file from `build` folder. See more details in readme of each project.

## Run

1. Hold the BOOTSEL button on Raspberry Pi Pico W and connect micro-USB cable, you will see a new USB drive `PRI-PR2` is mounted.

1. Drag and drop the binary file in build folder to the USB drive

## Observe the output

Open a terminal and connect to the `USB Serial Device (COMx)` just enumerated. You will see output. 

## License

This project is licensed under the [MIT](LICENSE) license.
 > Note, external libraries need commercial license.
   * Azure RTOS [licensed hardware](https://github.com/azure-rtos/threadx/blob/master/LICENSED-HARDWARE.txt)
   * [cyw43-driver](https://github.com/TiejunMS/cyw43-driver/blob/main/LICENSE)