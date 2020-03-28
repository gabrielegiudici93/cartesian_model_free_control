#ifndef BABBLINGGENERATOR_H
#define BABBLINGGENERATOR_H

#include <string>

#include <sec/source.h>
#include <sec/nodelink.h>

#include <utilities/signals.h>


class BabblingGenerator : public sec::Source {

public:
    BabblingGenerator(const std::vector<double>& refs, const std::vector<double>& amplmax, int signalswitch, double freq = 0.0);

    virtual void execute() override;

    virtual std::string parameters() const override;

    sec::NodeOut<double> j0, j3, j5;

    void reset() override;

private:
    std::vector<double> refs, amplmax;
    std::vector<Signals::Signal> sigs;
    int signalswitch, signalcount;

};

#endif // BABBLINGGENERATOR_H
