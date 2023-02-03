cmake_minimum_required(VERSION 3.25)
Project(lime-cwtest)
Add_executable(lime-cwtest main.cpp)
Target_link_libraries(lime-cwtest "C:/Documents and Settings/User/Desktop/Lime/LimeSuite/builddir/src/Release/LimeSuite.lib")
# "C:/Program Files (x86)/Cypress/EZ-USB FX3 SDK/1.3/library/cpp/lib/x86/CyAPI.lib")
Target_include_directories(lime-cwtest PUBLIC "C:/Documents and Settings/User/Desktop/Lime/LimeSuite/src/")

