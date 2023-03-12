# Flash loader
This folder contains modified linker scripts from [flash_loader](https://github.com/rhulme/pico-flashloader). The 2048K flash is divided into three parts.
* 0-4K, programed with flash loader.
* 4-1024K, flashed with user application.
* 1024-2048K, used to store updated user application.