#include "core/Log.h"
#include <iostream>

#include "util/OnnxRunner.h"
#include "core/Time.h"
#include "core/Log.h"
#include "math/Numbers.h"

using namespace Crawler;
using namespace std;

int main(){

    OnnxRunner onnxRunner("Crawler2.onnx");

    std::vector<float> inputValues(onnxRunner.GetInputSize());
    std::vector<float> outputValues(onnxRunner.GetOutputSize());

    int numTests = 100;
    uint64_t totalTimeMicros = 0.0f;

    for(int i = 0; i < numTests; i++){

        // set input values
        // LogDebug("Test", "setting input values");
        for(int j = 0; j < inputValues.size(); j++){
            inputValues[j] = Numbers::Random(-1.0f, 1.0f);
        }
        onnxRunner.SetInputValues(inputValues);

        // run inference
        // LogDebug("Test", "running inference");
        uint64_t t_start = Time::GetSystemTimeMicros();
        onnxRunner.Run();
        uint64_t t_end = Time::GetSystemTimeMicros();
        uint64_t t_run = t_end - t_start; 
        totalTimeMicros += t_run;

        // get output values
        // LogDebug("Test", "getting output values");
        onnxRunner.GetOutputValues(outputValues);
        LogInfo("Test", iLog << "o0: " << outputValues[0]);

    }

    float averageTimeMicros = (float)totalTimeMicros / (float)numTests;
    LogInfo("Test", iLog << "Total time: " << (averageTimeMicros * 1.0e-3) << " ms");
    LogInfo("Test", iLog << "Average time: " << (averageTimeMicros * 1.0e-3) << "ms");
    LogInfo("Test", iLog << "Average frequency: " << 1.0/(averageTimeMicros*1.0e-6) << " Hz"); 

    return EXIT_SUCCESS;

}
