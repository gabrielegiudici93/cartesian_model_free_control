#ifndef WRISTOFFSET_H
#define WRISTOFFSET_H

#include <sec/node.h>
#include <sec/nodelink.h>

class WristOffset : public sec::Node {
public:
    WristOffset(double offset, double freq = 100.0);

    void refreshInputs() override;

    bool connected() const override;

    void execute() override;

    std::string parameters() const override;

    sec::NodeIn<std::vector<double>> armin;
    sec::NodeOut<std::vector<double>> arm;

private:
    double offset;
};

#endif // WRISTOFFSET_H
