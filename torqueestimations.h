#ifndef TORQUEESTIMATIONS_H
#define TORQUEESTIMATIONS_H

#include <robots/icub/commons.h>
#include <sec/nodelink.h>

class TorqueEstimations : public PortSource<yarp::os::Bottle> {

public:
    using PortSource::PortSource;

    void execute() override;

    sec::NodeOut<double> shoulder_pitch, elbow;

};

#endif // TORQUEESTIMATIONS_H
