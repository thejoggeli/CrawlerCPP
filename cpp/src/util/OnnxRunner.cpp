#include "OnnxRunner.h"
#include "core/Log.h"

using namespace std;

namespace Crawler {

OnnxRunner::OnnxRunner(const std::string& modelName){

    instanceName = "crawler-inference";
    onnxPath = string(CRAWLER_ONNX_PATH) + "/" + modelName;

    LogInfo("OnnxRunner", iLog << "loading model: " << onnxPath);

    // create ORT env
    env = new Ort::Env(OrtLoggingLevel::ORT_LOGGING_LEVEL_WARNING, instanceName.c_str());

    // create ORT session options
    Ort::SessionOptions sessionOptions;
    sessionOptions.SetIntraOpNumThreads(1);
    sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
    
    // create ORT session
    session = new Ort::Session(*env, onnxPath.c_str(), sessionOptions);

    numInputNodes = session->GetInputCount();
    numOutputNodes = session->GetOutputCount();

    inputName = "observations";
    outputName = "actions";

    Ort::TypeInfo inputTypeInfo = session->GetInputTypeInfo(0);
    auto inputTensorInfo = inputTypeInfo.GetTensorTypeAndShapeInfo();
    ONNXTensorElementDataType inputType = inputTensorInfo.GetElementType();
    std::vector<int64_t> inputDims = inputTensorInfo.GetShape();

    Ort::TypeInfo outputTypeInfo = session->GetOutputTypeInfo(0);
    auto outputTensorInfo = outputTypeInfo.GetTensorTypeAndShapeInfo();
    ONNXTensorElementDataType outputType = outputTensorInfo.GetElementType();
    std::vector<int64_t> outputDims = outputTensorInfo.GetShape();
    
    LogInfo("OnnxRunner", iLog << "Number of Input Nodes: " << numInputNodes);
    LogInfo("OnnxRunner", iLog << "Number of Output Nodes: " << numOutputNodes);
    LogInfo("OnnxRunner", iLog << "Input Name: " << inputName);
    LogInfo("OnnxRunner", iLog << "Input Type: " << inputType);
    LogInfo("OnnxRunner", iLog << "Input Dims Num: " << inputDims.size());
    LogInfo("OnnxRunner", iLog << "Input Dims [0]: " << inputDims[0]);
    LogInfo("OnnxRunner", iLog << "Output Name: " << outputName);
    LogInfo("OnnxRunner", iLog << "Output Type: " << outputType);
    LogInfo("OnnxRunner", iLog << "Output Type: " << outputType);
    LogInfo("OnnxRunner", iLog << "Output Dims Num: " << outputDims.size());
    LogInfo("OnnxRunner", iLog << "Output Dims [0]: " << outputDims[0]);

    // create input and output tensors
    inputTensorSize = inputDims[0];
    inputTensorValues = std::vector<float>(inputTensorSize);

    outputTensorSize = outputDims[0];
    outputTensorValues = std::vector<float>(outputTensorSize);

    inputNames = std::vector<const char*>{inputName.c_str()};
    outputNames = std::vector<const char*>{outputName.c_str()};

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

}

OnnxRunner::~OnnxRunner(){
    if(env){
        delete env;
    }
    if(session){
        delete session;
    }
}

int OnnxRunner::GetInputSize(){
    return inputTensorSize;
}

int OnnxRunner::GetOutputSize(){
    return outputTensorSize;
}

void OnnxRunner::SetInputValues(std::vector<float>& values){
    if(inputTensorValues.size() == values.size()){
        for(int i = 0; i < inputTensorValues.size(); i++){
            inputTensorValues[i] = values[i];
        }
    } else {
        LogError("OnnxRunner", iLog << "input size error: " << values.size() << " / " << inputTensorValues.size());
    }
}

void OnnxRunner::GetOutputValues(std::vector<float>& values){
    if(values.size() == 0){
        for(int i = 0; i < outputTensorValues.size(); i++){
            values.push_back(outputTensorValues[i]);
        }
    } else if(outputTensorValues.size() == values.size()){
        for(int i = 0; i < outputTensorValues.size(); i++){
            values[i] = outputTensorValues[i];
        }
    } else {
        LogError("OnnxRunner", iLog << "output sizes error: " << values.size() << " / " << outputTensorValues.size());
    }
}

void OnnxRunner::Run(){
    // LogDebug("OnnxRunner", iLog << "input size: " << inputTensorValues.size());
    // LogDebug("OnnxRunner", iLog << "output size: " << outputTensorValues.size());
    session->Run(
        Ort::RunOptions{nullptr}, 
        inputNames.data(),
        inputTensors.data(), 
        1 /*Number of inputs*/, 
        outputNames.data(),
        outputTensors.data(), 
        1 /*Number of outputs*/
    );
}


}