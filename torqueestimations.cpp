#include "torqueestimations.h"

void TorqueEstimations::execute() {
    if (newvalue) {
        shoulder_pitch = last_reading.get(1).asDouble();
        elbow = last_reading.get(4).asDouble();
    }/* else {
        std::cout << "no new data" << std::endl;
    }*/
}
