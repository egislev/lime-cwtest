cmake_minimum_required(VERSION 3.0)
Project(lime-cwtest)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
Add_executable(lime-cwtest main.cpp)
Target_link_libraries(lime-cwtest "C:/Documents and Settings/User/Desktop/Lime/LimeSuite/builddir/src/Release/LimeSuite.lib" "C:/Program Files (x86)/Cypress/EZ-USB FX3 SDK/1.3/library/cpp/lib/x64/CyAPI.lib" "C:/Documents and Settings/User/Desktop/Lime/LimeSuite/src/ConnectionFTDI/FTD3XXLibrary/x64/Static/FTD3XX.lib" "msvcrt.lib" "vcruntime.lib")
Target_include_directories(lime-cwtest PUBLIC "C:/Documents and Settings/User/Desktop/Lime/LimeSuite/src/")

