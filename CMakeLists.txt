cmake_minimum_required(VERSION 3.10)
project(dmx C)

set(CMAKE_C_FLAGS "--std-c99 --model-small --opt-code-size")

include_directories(/usr/share/sdcc/include)

add_executable(dmx main.c uart.c dip.c dmx.c led.c pwm.c color.c)
