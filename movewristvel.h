#ifndef MOVEWRISTVEL_H
#define MOVEWRISTVEL_H

#include <sec/node.h>
#include <sec/nodelink.h>

#include <robots/icub/icubpart.h>

class MoveWristVel : public sec::Node {

public:
    MoveWristVel(HasLeftArm& robot, double freq = 100.0);

    void refreshInputs() override;

    bool connected() const override;

    void execute() override;

    std::string parameters() const override;

    sec::NodeIn<double> cmd;

private:
    HasLeftArm& robot;

};

#endif // MOVEWRISTVEL_H
