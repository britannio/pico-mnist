cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(pico_vision)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# There is another CMakeLists.txt in the src directory
add_subdirectory(src)

# We might want this
# add_compile_options(-Wall
#         -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
#         -Wno-unused-function # we have some for the docs that aren't called
#         -Wno-maybe-uninitialized
#         )