#include "safetystopper.h"

#include <iostream>

#include <utilities/utilities.h>

SafetyStopper::SafetyStopper(const std::vector<double> &jointMin, const std::vector<double> &jointMax, const std::vector<double> &posMin, const std::vector<double> &posMax, double freq)
    :sec::Node(freq), jointMin(jointMin), jointMax(jointMax), posMin(posMin), posMax(posMax) {}

void SafetyStopper::refreshInputs() {
    joints.refreshData();
    xyz.refreshData();
}

bool SafetyStopper::connected() const {
    return joints.isConnected() && xyz.isConnected();
}

void SafetyStopper::execute() {

    auto currj = joints.getData();
    for (unsigned int i = 0; i < currj.size(); i++) {
        if (currj[i] < jointMin[i] || currj[i] > jointMax[i]) {
            notsafe = true;
            std::cout << "joint " << i << std::endl;
        }
    }

    auto currp = xyz.getData();
    for (unsigned int i = 0; i < currp.size(); i++) {
        if (currp[i] < posMin[i] || currp[i] > posMax[i]) {
            notsafe = true;
            std::cout << "pos " << i << std::endl;
        }
    }

}

std::string SafetyStopper::parameters() const {
    std::string ret = "Safety stoppers with limits:\n\tjoints: min = [";
    ret += Utils::make_string(jointMin, ", ") + "], max = [";
    ret += Utils::make_string(jointMax, ", ") + "]\n\tpos: min = [";
    ret += Utils::make_string(posMin, ", ") + "], max = [";
    ret += Utils::make_string(posMax, ", ") + "]";
    return ret;
}

void SafetyStopper::reset() {
    notsafe = false;
}

bool SafetyStopper::shouldStop() const {
    return notsafe;
}
