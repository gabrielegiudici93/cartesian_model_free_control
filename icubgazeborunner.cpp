#include "icubgazeborunner.h"

#include <yarp/os/Bottle.h>

#include <robots/icub/commons.h>

iCubGazeboRunner::iCubGazeboRunner(double freq)
    :Node::Node(freq) {

    rpc.open("/local/icubgazebo");
    rpc.addOutput("/clock/rpc");

    // stop the simulation
    yarp::os::Bottle req, res;
    req.addString("pauseSimulation");
    rpc.write(req, res);

}

iCubGazeboRunner::~iCubGazeboRunner() {
    rpc.close();
}

void iCubGazeboRunner::refreshInputs() {

}

bool iCubGazeboRunner::connected() const {
    return true;
}

void iCubGazeboRunner::execute() {

    yarp::os::Bottle req, res;
    req.addString("stepSimulationAndWait");
    req.addInt(1000/freq);
    rpc.write(req, res);

}

std::string iCubGazeboRunner::parameters() const {
    return "";
}

void iCubGazeboRunner::resetGazeboTime() const {
    yarp::os::Bottle req, res;
    req.addString("resetSimulationTime");
    rpc.write(req, res);
}

