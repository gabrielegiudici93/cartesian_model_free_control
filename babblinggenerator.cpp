#include "babblinggenerator.h"

#include <utilities/utilities.h>
#include <utilities/vector.h>

#include <iostream>

std::function<Utils::Vector(void)> randomgen = Utils::uniformGenerator(3);
std::function<Utils::Vector(void)> freqgen = Utils::uniformGenerator(3, 0.5, 1.0);


BabblingGenerator::BabblingGenerator(const std::vector<double>& refs, const std::vector<double>& amplmax, int signalswitch, double freq)
    :Source::Source(freq), refs(refs), amplmax(amplmax), signalswitch(signalswitch) {

    signalcount = 0;

    reset();

}

void BabblingGenerator::execute() {

    j0 = sigs[0]();
    j3 = sigs[1]();
    j5 = sigs[2]();

}

std::string BabblingGenerator::parameters() const {
    std::string ret = "Babbling generator";
    ret += "\n\trefs = " + Utils::make_string(refs, " ");
    ret += "\n\tamplmax = " + Utils::make_string(amplmax, " ");
    ret += "\n\tswitch = " + std::to_string(signalswitch);
    return ret;
}

void BabblingGenerator::reset() {

    sigs.clear();

    auto ampl = randomgen();
    auto freqs = randomgen();

    int nsin = signalcount / signalswitch + 1;

    for (unsigned int i = 0; i < refs.size(); i++) {
        sigs.push_back(Signals::sin(ampl[i]/nsin, freqs[i], 0.0, 100.0) * amplmax[i]);
    }

    std::cout << nsin << std::endl;

    for (int s = 1; s < nsin; s++) {
        ampl = randomgen();
        freqs = randomgen();
        for (unsigned int i = 0; i < refs.size(); i++) {
            sigs[i] = sigs[i] + Signals::sin(ampl[i]/nsin, freqs[i], 0.0, 100.0) * amplmax[i];
        }
    }

    for (unsigned int i = 0; i < refs.size(); i++) {
        sigs[i] = sigs[i] + refs[i];
    }

    signalcount++;

}

