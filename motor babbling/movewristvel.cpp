#include "movewristvel.h"


MoveWristVel::MoveWristVel(HasLeftArm &robot, double freq)
    :sec::Node(freq), robot(robot) {}

void MoveWristVel::refreshInputs() {
    cmd.refreshData();
}

bool MoveWristVel::connected() const {
    return cmd.isConnected();
}

void MoveWristVel::execute() {
    robot.leftarm->moveVelJoint(5, cmd.getData());
}

std::string MoveWristVel::parameters() const {
    return "Velocity cmd";
}
