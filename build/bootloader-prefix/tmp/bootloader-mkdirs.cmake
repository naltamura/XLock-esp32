# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/naltamura/esp/esp-idf/components/bootloader/subproject"
  "/home/naltamura/Documents/PHD3Y/ZKP_Xlock/Xlock_2v/build/bootloader"
  "/home/naltamura/Documents/PHD3Y/ZKP_Xlock/Xlock_2v/build/bootloader-prefix"
  "/home/naltamura/Documents/PHD3Y/ZKP_Xlock/Xlock_2v/build/bootloader-prefix/tmp"
  "/home/naltamura/Documents/PHD3Y/ZKP_Xlock/Xlock_2v/build/bootloader-prefix/src/bootloader-stamp"
  "/home/naltamura/Documents/PHD3Y/ZKP_Xlock/Xlock_2v/build/bootloader-prefix/src"
  "/home/naltamura/Documents/PHD3Y/ZKP_Xlock/Xlock_2v/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/naltamura/Documents/PHD3Y/ZKP_Xlock/Xlock_2v/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/naltamura/Documents/PHD3Y/ZKP_Xlock/Xlock_2v/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
