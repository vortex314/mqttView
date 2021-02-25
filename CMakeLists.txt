cmake_minimum_required(VERSION 3.1.0)
# set(CMAKE_C_COMPILER /usr/bin/x86_64-linux-gnu-gcc-9)
# set(CMAKE_CXX_COMPILER /usr/bin/x86_64-linux-gnu-gcc-9)
project(mqttView VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(mqttView  src/main.cpp src/mqttmodel.cpp ) 

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(GCC_FLAGS "-DLINUX")
include(CPack)

add_definitions(${GCC_FLAGS})
include_directories(src /usr/include/x86_64-linux-gnu/qt5/QtWidgets /usr/include/x86_64-linux-gnu/qt5 ../limero/inc ../limero/linux ../ArduinoJson/src ../paho.mqtt.c/src  )
target_link_libraries(mqttView -L/home/lieven/workspace/limero/linux/build -llimero -L/home/lieven/workspace/paho.mqtt.c/build/output/ -l:libpaho-mqtt3c.a -lpthread  -lrt  -latomic Qt5::Widgets)
