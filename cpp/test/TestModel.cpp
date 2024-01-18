#include "core/Log.h"
#include <iostream>

#include <onnxruntime_cxx_api.h>
#include "math/Numbers.h"
#include "core/Time.h"

using namespace Crawler;
using namespace std;

// created from example
// https://github.com/leimao/ONNX-Runtime-Inference/blob/main/src/inference.cpp

int main(){

    string instanceName = "crawler-inference";
    string onnxPath = string(CRAWLER_ONNX_PATH) + "/Crawler2.onnx";

    cout << "loading model: " << onnxPath << endl;

    // create ORT env
    Ort::Env env(OrtLoggingLevel::ORT_LOGGING_LEVEL_WARNING, instanceName.c_str());

    // create ORT session options
    Ort::SessionOptions sessionOptions;
    sessionOptions.SetIntraOpNumThreads(1);
    sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
    
    // create ORT session
    Ort::Session session(env, onnxPath.c_str(), sessionOptions);
    
    // create ORT allocator
    Ort::AllocatorWithDefaultOptions allocator;

    size_t numInputNodes = session.GetInputCount();
    size_t numOutputNodes = session.GetOutputCount();

    string inputName = "observations";
    string outputName = "actions";

    Ort::TypeInfo inputTypeInfo = session.GetInputTypeInfo(0);
    auto inputTensorInfo = inputTypeInfo.GetTensorTypeAndShapeInfo();
    ONNXTensorElementDataType inputType = inputTensorInfo.GetElementType();
    std::vector<int64_t> inputDims = inputTensorInfo.GetShape();

    Ort::TypeInfo outputTypeInfo = session.GetOutputTypeInfo(0);
    auto outputTensorInfo = outputTypeInfo.GetTensorTypeAndShapeInfo();
    ONNXTensorElementDataType outputType = outputTensorInfo.GetElementType();
    std::vector<int64_t> outputDims = outputTensorInfo.GetShape();
    
    std::cout << "Number of Input Nodes: " << numInputNodes << std::endl;
    std::cout << "Number of Output Nodes: " << numOutputNodes << std::endl;
    std::cout << "Input Name: " << inputName << std::endl;
    std::cout << "Input Type: " << inputType << std::endl;
    std::cout << "Input Dims Num: " << inputDims.size() << std::endl;
    std::cout << "Input Dims [0]: " << inputDims[0] << std::endl;
    std::cout << "Output Name: " << outputName << std::endl;
    std::cout << "Output Type: " << outputType << std::endl;
    std::cout << "Output Type: " << outputType << std::endl;
    std::cout << "Output Dims Num: " << outputDims.size() << std::endl;
    std::cout << "Output Dims [0]: " << outputDims[0] << std::endl;

    // create input and output tensors
    size_t inputTensorSize = inputDims[0];
    std::vector<float> inputTensorValues(inputTensorSize);

    size_t outputTensorSize = outputDims[0];
    std::vector<float> outputTensorValues(outputTensorSize);

    std::vector<const char*> inputNames{inputName.c_str()};
    std::vector<const char*> outputNames{outputName.c_str()};
    std::vector<Ort::Value> inputTensors;
    std::vector<Ort::Value> outputTensors;

    Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(
        OrtAllocatorType::OrtArenaAllocator, 
        OrtMemType::OrtMemTypeDefault
    );
    
    inputTensors.push_back(Ort::Value::CreateTensor<float>(
        memoryInfo, 
        inputTensorValues.data(), 
        inputTensorSize, 
        inputDims.data(),
        inputDims.size()
    ));

    outputTensors.push_back(Ort::Value::CreateTensor<float>(
        memoryInfo, 
        outputTensorValues.data(), 
        outputTensorSize,
        outputDims.data(), 
        outputDims.size()
    ));

    int numTests = 100;
    uint64_t totalTimeMicros = 0.0f;

    std::cout << "i0: " << inputTensorValues[0] << endl;

    // for(int j = 0; j < inputTensorInfo.GetShape()[0]; j++){
    //     inputTensors[0] = Ort::Value::
    // }

    for(int i = 0; i < numTests; i++){
        
        for(int j = 0; j < inputTensorValues.size(); j++){
            inputTensorValues[j] = Numbers::Random(-1.0f, 1.0f);
        }

        uint64_t t_start = Time::GetSystemTimeMicros();

        session.Run(
            Ort::RunOptions{nullptr}, 
            inputNames.data(),
            inputTensors.data(), 
            1 /*Number of inputs*/, 
            outputNames.data(),
            outputTensors.data(), 
            1 /*Number of outputs*/
        );
        uint64_t t_end = Time::GetSystemTimeMicros();
        uint64_t t_run = t_end - t_start; 

        totalTimeMicros += t_run;

        // cout << "Run #" << i << " time: " << (t_run*1.0e-3) << " ms" << endl;

        std::cout << "o0: " << outputTensorValues[0] << endl;

    }

    float averageTimeMicros = (float)totalTimeMicros / (float)numTests;
    cout << "Total time: " << (averageTimeMicros * 1.0e-3) << " ms" << endl;
    cout << "Average time: " << (averageTimeMicros * 1.0e-3) << "ms" << endl;
    cout << "Average frequency: " << 1.0/(averageTimeMicros*1.0e-6) << " Hz" << endl; 

    return EXIT_SUCCESS;

}
