#include "Brain.h"

#include "robot/Robot.h"

namespace Crawler{

Brain::Brain() {

}

void Brain::SetRobot(Robot* robot){
    this->robot = robot;
}

}