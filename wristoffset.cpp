#include "wristoffset.h"

WristOffset::WristOffset(double offset, double freq)
    :sec::Node(freq), offset(offset) {}

void WristOffset::refreshInputs() {
    armin.refreshData();
}

bool WristOffset::connected() const {
    return armin.isConnected();
}

void WristOffset::execute() {
    std::vector<double> v = armin.getData();
    v[6] -= offset;
    arm = v;
}

std::string WristOffset::parameters() const {
    return "Wrist offset";
}
