#include "forward_kin.h"


f_kin::f_kin(double freq)
    :sec::Node(freq) {}

void f_kin::refreshInputs() {
    encs.refreshData();
}

bool f_kin::connected() const {
    return encs.isConnected();
}

void f_kin::execute() {

    xyz = leftarm_kin.getLeftHandPosition(encs.getData());

}

std::string f_kin::parameters() const {
    return "Forward kin";
}
