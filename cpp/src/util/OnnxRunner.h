#pragma once

#include <onnxruntime_cxx_api.h>

namespace Crawler {

class OnnxRunner {
private:

    std::string instanceName;
    std::string onnxPath;

    Ort::Env* env = nullptr;
    Ort::Session* session = nullptr;

    size_t numInputNodes;
    size_t numOutputNodes;
    std::string inputName;
    std::string outputName;
    
    size_t inputTensorSize;
    std::vector<float> inputTensorValues;

    size_t outputTensorSize;
    std::vector<float> outputTensorValues;

    std::vector<const char*> inputNames;
    std::vector<const char*> outputNames;
    std::vector<Ort::Value> inputTensors;
    std::vector<Ort::Value> outputTensors;


public:

    OnnxRunner(const std::string& modelName);
    ~OnnxRunner();

    void SetInputValues(std::vector<float>& values);
    void GetOutputValues(std::vector<float>& values);
    void Run();

    int GetInputSize();
    int GetOutputSize();

};

}