include_directories(
    "${CMAKE_CURRENT_LIST_DIR}/src"
    "${CMAKE_CURRENT_LIST_DIR}/libs"
    "${CMAKE_CURRENT_LIST_DIR}/libs/adafruit/Adafruit_Sensor"
    "${CMAKE_CURRENT_LIST_DIR}/libs/adafruit/Adafruit_BusIO"
    "${CMAKE_CURRENT_LIST_DIR}/libs/adafruit/Adafruit_LSM6DS"
    "/home/crawler/onnxruntime/include"                        
    "/home/crawler/onnxruntime/include/onnxruntime"            
    "/home/crawler/onnxruntime/include/onnxruntime/core/session"
)

link_directories("/home/crawler/onnxruntime/build/Linux/Release")

set(ONNXRUNTIME_ROOTDIR "/home/crawler/onnxruntime/build/Linux/Release")
# set(ONNXRUNTIME_LIB "/home/crawler/.local/lib/python3.8/site-packages/onnxruntime/capi/libonnxruntime_providers_cuda.so")

find_package(JetsonGPIO)
find_package(Seasocks)
# find_package(Torch REQUIRED PATHS /home/crawler/.local/lib/python3.8/site-packages/torch)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

add_definitions(-DUSE_CUDA)

set(
    CRAWLER_SOURCE_FILES
    src/CrawlerCPP.cpp
    libs/serialib/Serialib.cpp 
    libs/libi2c/i2c.cpp
    src/app/App.cpp
    src/app/ServoThread.cpp
    src/app/PacketsComm.cpp
    src/comm/SerialStream.cpp
    src/comm/I2CBus.cpp
    src/parts/NAU7802/NAU7802.cpp
    src/parts/ISM330DHXC/ism330dhcx_reg.c
    src/parts/TCA9548A/TCA9548A.cpp
    src/parts/VCNL4010/VCNL4010.cpp
    src/parts/MuxI2C.cpp
    src/parts/XYZServo.cpp
    src/parts/HardwareButton.cpp
    src/parts/MonoLED.cpp
    src/parts/DistanceSensor.cpp
    src/parts/WeightSensor.cpp
    src/parts/IMU.cpp
    src/brain/Brain.cpp
    src/brain/SurferBrain.cpp
    src/brain/GaitBrain.cpp
    src/brain/WalkerBrain.cpp
    src/brain/EmptyBrain.cpp
    src/brain/CalibrationBrain.cpp
    src/core/Time.cpp
    src/core/Log.cpp
    src/core/Config.cpp
    src/robot/Robot.cpp
    src/robot/Leg.cpp
    src/robot/Joint.cpp
    src/math/Mathf.cpp
    src/math/Numbers.cpp
    src/util/Strings.cpp
    src/util/Timer.cpp
    src/util/Endian.cpp
    src/buffer/SmartBuffer.cpp
    src/buffer/ByteBufferReader.cpp
    src/buffer/ByteBufferWriter.cpp 
    src/threading/BufferedValue.cpp
    src/threading/Semaphore.cpp
    src/threading/Barrier.cpp
    src/threading/Signal.cpp
    src/remote/SocketServer.cpp
    src/remote/SocketMessage.cpp
    src/remote/SeasocksHandler.cpp
    src/remote/GamepadKeys.cpp
    src/remote/Client.cpp 
    src/remote/ClientManager.cpp
    src/remote/PacketType.cpp
    src/remote/Packet.cpp
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


find_library(
	libonnxruntime_location
	NAMES libonnxruntime.so
	PATHS "/home/crawler/onnxruntime/build/Linux/RelWithDebInfo"
)
message("libonnxruntime_location: " ${libonnxruntime_location})
add_library(libonnxruntime SHARED IMPORTED)
set_property(
	TARGET libonnxruntime
	PROPERTY IMPORTED_LOCATION
	${libonnxruntime_location}
)

# find_library(
# 	libtorch_location
# 	NAMES libtorch.so
# 	PATHS /home/crawler/.local/lib/python3.8/site-packages/torch/lib/
# )
# message("libtorch_location: " ${libtorch_location})
# add_library(libtorch SHARED IMPORTED)
# set_property(
# 	TARGET libtorch
# 	PROPERTY IMPORTED_LOCATION
# 	${libtorch_location}
# )


# make library
add_library(libCrawler SHARED ${CRAWLER_SOURCE_FILES})
set_property(TARGET libCrawler PROPERTY CXX_STANDARD 20)
set_property(TARGET libCrawler PROPERTY POSITION_INDEPENDENT_CODE TRUE)
target_link_libraries(libCrawler JetsonGPIO::JetsonGPIO)
target_link_libraries(libCrawler libseasocks)
target_link_libraries(libCrawler libonnxruntime)
# target_link_libraries(libCrawler "${TORCH_LIBRARIES}")
# set_property(TARGET libCrawler PROPERTY CXX_STANDARD 17)


# target_link_libraries(libCrawler libtorch)

function(CreateCrawlerExecutable ExecutableName ExecutableMain)
    add_executable (${ExecutableName} ${ExecutableMain})
    set_property(TARGET ${ExecutableName} PROPERTY CXX_STANDARD 20)
    target_link_libraries(${ExecutableName} libCrawler)
endfunction()

# CreateCrawlerExecutable(TestThreading "test/TestThreading.cpp")
# CreateCrawlerExecutable(TestBuffer "test/TestBuffer.cpp")
# CreateCrawlerExecutable(TestServer "test/TestServer.cpp")
# CreateCrawlerExecutable(TestTime "test/TestTime.cpp")
# CreateCrawlerExecutable(TestEigen "test/TestEigen.cpp")
# CreateCrawlerExecutable(TestSerialCom "test/TestSerialCom.cpp")
# CreateCrawlerExecutable(TestGait "test/TestGait.cpp")
# CreateCrawlerExecutable(TestI2C "test/TestI2C.cpp")
# CreateCrawlerExecutable(TestWeight "test/TestWeight.cpp")
# CreateCrawlerExecutable(TestDistance "test/TestDistance.cpp")
# CreateCrawlerExecutable(TestIMU "test/TestIMU.cpp")
CreateCrawlerExecutable(TestModel "test/TestModel.cpp")

CreateCrawlerExecutable(CrawlerCPP "src/CrawlerCPP.cpp")


