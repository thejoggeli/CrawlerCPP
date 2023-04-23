include_directories(
    "${CMAKE_CURRENT_LIST_DIR}/src"
    "${CMAKE_CURRENT_LIST_DIR}/libs"
)

find_package(JetsonGPIO)
find_package(Seasocks)

set(
    CRAWLER_SOURCE_FILES
    src/CrawlerCPP.cpp
    libs/serialib/Serialib.cpp
    src/comm/SerialStream.cpp
    src/parts/XYZServo.cpp
    src/brain/Brain.cpp
    src/brain/SurferBrain.cpp
    src/brain/GaitBrain.cpp
    src/brain/EmptyBrain.cpp
    src/core/Time.cpp
    src/core/Log.cpp
    src/core/Config.cpp
    src/robot/Robot.cpp
    src/robot/Leg.cpp
    src/robot/Joint.cpp
    src/math/Mathf.cpp
    src/util/HardwareButton.cpp
    src/util/MonoLED.cpp
    src/util/Strings.cpp
    src/util/Timer.cpp
    src/util/SmartBuffer.cpp
    src/util/ByteBufferReader.cpp
    src/util/Endian.cpp
    src/remote/SocketServer.cpp
    src/remote/SocketMessage.cpp
    src/remote/SeasocksHandler.cpp
    src/remote/GamepadKeys.cpp
    src/remote/Client.cpp 
    src/remote/ClientManager.cpp
    src/remote/PacketType.cpp
    src/remote/Packet.cpp
    src/crawler/Main.cpp
    src/gait/Gait.cpp
    src/gait/GaitFrame.cpp
)

# seasocks has to be built with:
# $ mkdir build && cd build
# $ cmake -D SEASOCKS_SHARED=ON ..
# $ sudo make install
find_library(
	libseasocks_location
	NAMES libseasocks.so
	PATHS /usr/local/lib/ /usr/lib
)
message("libseasocks_location: " ${libseasocks_location})
add_library(libseasocks SHARED IMPORTED)
set_property(
	TARGET libseasocks
	PROPERTY IMPORTED_LOCATION
	${libseasocks_location}
)

# make library
add_library(libCrawler SHARED ${CRAWLER_SOURCE_FILES})
set_property(TARGET libCrawler PROPERTY CXX_STANDARD 20)
set_property(TARGET libCrawler PROPERTY POSITION_INDEPENDENT_CODE TRUE)
target_link_libraries(libCrawler JetsonGPIO::JetsonGPIO)
target_link_libraries(libCrawler libseasocks)

function(CreateCrawlerExecutable ExecutableName ExecutableMain)
    add_executable (${ExecutableName} ${ExecutableMain})
    set_property(TARGET ${ExecutableName} PROPERTY CXX_STANDARD 20)
    target_link_libraries(${ExecutableName} libCrawler)
endfunction()

CreateCrawlerExecutable(TestBuffer "test/TestBuffer.cpp")
CreateCrawlerExecutable(TestServer "test/TestServer.cpp")
CreateCrawlerExecutable(TestTime "test/TestTime.cpp")
CreateCrawlerExecutable(TestEigen "test/TestEigen.cpp")
CreateCrawlerExecutable(TestSerialCom "test/TestSerialCom.cpp")
CreateCrawlerExecutable(TestGait "test/TestGait.cpp")
CreateCrawlerExecutable(CrawlerCPP "src/CrawlerCPP.cpp")


