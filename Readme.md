# Arduino Gravity Recorder

## Dependencies

* Arduino SPI library
* Arduino SD library
* Arduino Wire library
* arduino-cli
* arduino:avr core
* avr-gcc

## Compilation

This step will automatically fetch the [Arduino-CMake-Toolchain](https://github.com/a9183756-gh/Arduino-CMake-Toolchain), compile the code, and upload it to your Arduino Nano.

```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
make upload-arduino_gravity_recorder SERIAL_PORT=/dev/ttyUSB0
```
