#include <sec/sec.h>

#include <utilities/signals.h>

#include <sec/plottingclient.h>

#include <robots/icub/commons.h>
#include <robots/icub/nodes.h>
#include <robots/icub/icubrobot.h>

#include <robots/icub/icubsim.h>
#include <robots/icub/icub.h>


//#define SIMULATOR
int main(int argc, char *argv[]) {

    sec::setDefaultFrequency(100.0);

#ifdef SIMULATOR
    sec::setSleeper(new sec::Barrier());

    iCubRobot<iCubSimLeftArm> robot("icubSim");
#else
    iCubRobot<iCubLeftArm> robot("icub");
#endif


EncodersLeftArm encs(robot);
f_kin kin;

// encoder
//auto e0 = EncodersLeftArm(robot).arm

sec::PlottingClient plot;
sec::connect(encs.shoulder_pitch, plot, "shoulder_pitch");
sec::connect(encs.shoulder_roll, plot, "shoulder_roll");
sec::connect(encs.shoudler_yaw, plot, "shoulder_yaw"); //sbagliato SHOUdlER
sec::connect(encs.elbow, plot, "elbow");
sec::connect(encs.wrist_prosup, plot, "wrist_prosup");
sec::connect(encs.wrist_pitch, plot, "wrist_pitch");
sec::connect(encs.wrist_yaw, plot, "wrist_yaw");


sec::connect(encs.arm , f_kin );

MoveWristVel velctrl(robot);
sec::connect(ss2.output, velctrl.cmd);
