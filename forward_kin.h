#ifndef FORWARD_KIN_H
#define FORWARD_KIN_H

#include <sec/node.h>
#include <sec/nodelink.h>
#include <robots/icub/icubpart.h>
#include <robots/icub/commons.h>
#include <vector>

class f_kin : public sec::Node {

public:
    f_kin(double freq = 100.0);

    void refreshInputs() override;

    bool connected() const override;

    void execute() override;

    std::string parameters() const override;

    sec::NodeIn<std::vector<double>> encs;

    sec::NodeOut<std::vector<double>> xyz;

private:
    iCubKin leftarm_kin;

};

#endif // FORWARD_KIN_H
