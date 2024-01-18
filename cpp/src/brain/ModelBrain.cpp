
#include "ModelBrain.h"
#include "robot/Robot.h"
#include "robot/Leg.h"
#include "robot/Joint.h"
#include <cmath>
#include "math/Mathf.h"
#include <cassert>
#include "core/Log.h"
#include "core/Time.h"
#include "remote/ClientManager.h"
#include "remote/Client.h"
#include "util/OnnxRunner.h"

using namespace std;

namespace Crawler {

ModelBrain::ModelBrain() : Brain() {

}

ModelBrain::~ModelBrain() {

    if(onnxRunner){
        delete onnxRunner;
    }

}

void ModelBrain::Init(){

}

void ModelBrain::Update(){

}

void ModelBrain::FixedUpdate(){

    onnxRunner->SetInputValues(inputValues);
    onnxRunner->Run();
    onnxRunner->GetOutputValues(outputValues);

}

void ModelBrain::Destroy(){

}

}