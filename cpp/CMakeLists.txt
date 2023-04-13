
include_directories(
    "${CMAKE_CURRENT_LIST_DIR}/src"
    "${CMAKE_CURRENT_LIST_DIR}/libs"
)

find_package(JetsonGPIO)

add_executable (
    TestTime
    test/TestTime.cpp
    src/core/Time.cpp
)
set_property(TARGET TestTime PROPERTY CXX_STANDARD 20)

add_executable (
    TestEigen
    test/TestEigen.cpp
    src/core/Time.cpp
)
set_property(TARGET TestEigen PROPERTY CXX_STANDARD 20)

add_executable (
    TestSerialCom
    test/TestSerialCom.cpp
    libs/serialib/Serialib.cpp
    src/comm/SerialStream.cpp
    src/parts/XYZServo.cpp
    src/core/Time.cpp
)
set_property(TARGET TestSerialCom PROPERTY CXX_STANDARD 20)

add_executable (
    CrawlerCPP
    src/CrawlerCPP.cpp
    libs/serialib/Serialib.cpp
    src/comm/SerialStream.cpp
    src/parts/XYZServo.cpp
    src/core/Time.cpp
    src/core/Log.cpp
    src/core/Config.cpp
    src/robot/Robot.cpp
    src/robot/Leg.cpp
    src/robot/Joint.cpp
    src/brain/Brain.cpp
    src/brain/SurferBrain.cpp
    src/brain/EmptyBrain.cpp
    src/math/Mathf.cpp
    src/util/HardwareButton.cpp
    src/util/Strings.cpp
)
set_property(TARGET CrawlerCPP PROPERTY CXX_STANDARD 20)
target_link_libraries(CrawlerCPP JetsonGPIO::JetsonGPIO)

