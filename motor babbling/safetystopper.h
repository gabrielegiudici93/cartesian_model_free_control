#ifndef SAFETYSTOPPER_H
#define SAFETYSTOPPER_H

#include <sec/node.h>
#include <sec/nodelink.h>
#include <vector>


class SafetyStopper : public sec::Node {

public:
    SafetyStopper(const std::vector<double>& jointMin,const std::vector<double>& jointMax,
                  const std::vector<double>& posMin, const std::vector<double>& posMax,
                  double freq = 100.0);

    void refreshInputs() override;

    bool connected() const override;

    void execute() override;

    std::string parameters() const override;

    void reset() override;

    bool shouldStop() const;

    sec::NodeIn<std::vector<double>> joints, xyz;

private:
    std::vector<double> jointMin, jointMax, posMin, posMax;
    bool notsafe = false;
};

#endif // SAFETYSTOPPER_H
