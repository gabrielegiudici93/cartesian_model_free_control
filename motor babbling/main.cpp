#include <limits>

#include <sec/sec.h>
#include <sec/datalogger.h>
#include <sec/nodelink.h>

#include <utilities/signals.h>
#include <utilities/chrono.h>

#include <sec/plottingclient.h>
#include <sec/printer.h>

#include <robots/icub/commons.h>
#include <robots/icub/nodes.h>
#include <robots/icub/icubrobot.h>

#include <robots/icub/icubsim.h>
#include <robots/icub/icub.h>

#include "babblinggenerator.h"
#include "icubgazeborunner.h"
#include "movewristvel.h"
#include "forward_kin.h"
#include "safetystopper.h"
#include "wristoffset.h"
#include "torqueestimations.h"

//#define SIMULATOR

void merge_trials(const std::vector<std::string>& prefixes, const std::vector<std::string>& files) {

    for (auto& f : files) {
        for (auto& pre : prefixes) {
            std::string cmd = "cat ";
            cmd += pre + f;
            cmd += " | tail -n+2 >> ";
            size_t a = pre.find('/');
            cmd += pre.substr(0, a+1) + f;
            std::system(cmd.c_str());
        }
    }

}

void main_completo() {

    double wrist_offset = -12.6;//-12.60;                                                   //PARAMETERS
    const double trial_duration=5.0;
    const int num_trials = 300;
    const int max_num_sin=5;// 5 is the max number of sin
    const int num_sin_switch=num_trials/max_num_sin;
    sec::setDefaultFrequency(100.0);
    sec::setResultsMode(sec::SINGLE_FILES_MODE);
    sec::setResultsName("babbling-data");

#ifdef SIMULATOR
    sec::setSleeper(new sec::Barrier());

    iCubRobot<iCubSimLeftArm> robot("icubSim");
#else
    iCubRobot<iCubLeftArm> robot("icub");
#endif

    // encoders
    EncodersLeftArm wrongencs(robot);
    WristOffset encs(wrist_offset);
    sec::connect(wrongencs.arm, encs.armin);

    // read torque estimation
    TorqueEstimations torque_est("/wholeBodyDynamics/left_arm/Torques:o", 100.0);
//    TorqueEstimations torque_est("/icub/left_arm/state:o", 100.0);
    TorquesLeftArm torque_est2(robot);

    // forward kin
    f_kin kin;
    sec::connect(encs.arm, kin.encs);

    // safety
    std::vector<double> jointMin(7, std::numeric_limits<double>::lowest());
    std::vector<double> jointMax(7, std::numeric_limits<double>::max());
    std::vector<double> posMin(3, std::numeric_limits<double>::lowest());
    std::vector<double> posMax(3, std::numeric_limits<double>::max());
    jointMin[0] = -65;
    jointMax[0] =   5;
    jointMin[3] =  14;
    jointMax[3] =  90;
    jointMin[5] = -50;
    jointMax[5] =   0;
    posMin[1]=0.467;
    posMax[1]=0.8;

    SafetyStopper safetys(jointMin, jointMax, posMin, posMax);
    sec::connect(encs.arm, safetys.joints);
    sec::connect(kin.xyz, safetys.xyz);

    bool notsafe = false;
    auto stopper = [&safetys, &notsafe] () {
        notsafe = safetys.shouldStop();
        if (notsafe) {
            std::cout << "TRIAL NOT SAFE" << std::endl;
        }
        return notsafe;
    };

   // motion
    //BabblingGenerator generator({-2.5, 0.7, 0.0}, {3.0, 0.9, 40}, num_sin_switch ); //TORQUE03  //original -2.1, 0.5, 0.0   1.8,1.2,20
    //  BabblingGenerator generator({-2.5, 60, 0.0}, {3.0, 20, 40}, num_sin_switch ); //POSITION 3
    BabblingGenerator generator({-2.5, 0.0, 0.0}, {3.0, 90, 35}, num_sin_switch ); //velocity35

//    auto s0 = Signals::sin(0.1, 0.5)-2.1;
//    sec::SignalSource ss0(s0);
//    auto s1 = Signals::sin(0.2, 0.5) + 0.5;
//    sec::SignalSource ss1(s1);
//    auto s2 = Signals::sin(20.0, 0.5);
//    sec::SignalSource ss2(s2);

     //TORQUE
    LeftArmTorqueControl torqctrl(robot); //1
    sec::connect(generator.j0, torqctrl.shoulder_pitch);//shoulder torque

   // sec::connect(generator.j3, torqctrl.elbow); //elbow torque


    //POSITION
    //LeftArmPositionControl pos(robot);
    //sec::connect(generator.j3, pos.elbow);

    //VELOCITY CONTROL
    LeftArmVelocityControl velctrl(robot);
    sec::connect(generator.j3, velctrl.elbow);
    sec::connect(generator.j5, velctrl.wrist_pitch);

                    //    sec::connect(ss1.output, torqctrl.elbow);
//    sec::connect(ss0.output, torqctrl.shoulder_pitch);


                    // MoveWristVel velctrl(robot);
                    //sec::connect(generator.j5, velctrl.cmd); //wrist  vel

                    //    sec::connect(ss2.output, velctrl.cmd);

#ifdef SIMULATOR
    iCubGazeboRunner runner;
#endif

    // save file
    sec::DataLogger log_ref(",", "reference.csv");
//    sec::connect(ss0.output, log_ref, "r0");
//    sec::connect(ss1.output, log_ref, "r3");
//    sec::connect(ss2.output, log_ref, "r5");
    sec::connect(generator.j0, log_ref, "r0");
    sec::connect(generator.j3, log_ref, "r3");
    sec::connect(generator.j5, log_ref, "r5");


    sec::DataLogger log_encs(",", "encoders.csv");
    sec::connect(encs.arm, log_encs, "j0 j1 j2 j3 j4 j5 j6");
    sec::DataLogger log_xyz(",", "xyz.csv");
    sec::connect(kin.xyz, log_xyz, "x y z");

    sec::DataLogger log_torques(",", "torques.csv");
    sec::connect(torque_est.shoulder_pitch, log_torques, "et0");
    sec::connect(torque_est.elbow, log_torques, "et3");
    sec::connect(torque_est2.shoulder_pitch, log_torques, "jt0");
    sec::connect(torque_est2.elbow, log_torques, "jt3");

    sec::PlottingClient plot(50.0);
            //    sec::connect(generator.j0, plot, "j0");
            //    sec::connect(generator.j3, plot, "j3");
            //    sec::connect(generator.j5, plot, "j5");
    sec::connect(wrongencs.elbow, plot, "elbow");
    sec::connect(wrongencs.wrist_pitch, plot, "wrist");
    sec::connect(wrongencs.shoulder_pitch, plot, "shoulder");

    std::vector<std::string> saved_files;
    auto append_file = [&saved_files, &notsafe] () {
        if (!notsafe)
            saved_files.push_back(sec::getResultsPrefix());
    };


    auto setup = [&robot, wrist_offset] () {

        robot.leftarm->setControlMode({0, 3, 5}, {iCubControlMode::Position, iCubControlMode::Position, iCubControlMode::Position});

        robot.leftarm->setRefSpeeds(0.2);
        robot.leftarm->movePosJoints({0, 1, 2, 3, 4, 5, 6}, {-45, 8.0, 0.0, 20.0, -90, -40, wrist_offset}); // SETUP //vecchio valore elbow=60 nuovo=15.
        robot.leftarm->setRefSpeeds(0.7);
        sec::sleep(4000);

    };

    auto set_torque = [&robot] () {
      //  robot.leftarm->setControlMode({0, 3}, {iCubControlMode::Torque, iCubControlMode::Torque});
         //robot.leftarm->setControlMode({0}, { iCubControlMode::Torque});
         robot.leftarm->setControlMode({0}, { iCubControlMode::Torque});
         robot.leftarm->setControlMode({3,5}, {iCubControlMode::Velocity, iCubControlMode::Velocity});

    };

    auto set_pos = [&robot] () {
        robot.leftarm->setControlMode({0, 3, 5}, {iCubControlMode::Position, iCubControlMode::Position, iCubControlMode::Position});
    };

    int curr_trial = 1;
    Utils::Stopwatch stopwatch;
    stopwatch.start();
    auto print_trial = [&curr_trial, &stopwatch, num_trials] {
        std::cout << "Trial " << curr_trial++ << "/" << num_trials;
        std::cout << ", elapsed " << stopwatch.getTime() << std::endl;
    };

    sec::saveAllNodesParameters();

    sec::runTrials(num_trials, {setup, print_trial, set_torque}, {set_pos, append_file}, trial_duration, {stopper});

    set_pos();
    setup();

    merge_trials(saved_files, {"reference.csv", "encoders.csv", "xyz.csv", "torques.csv"});

    std::cout << "Good trials: " << saved_files.size() << std::endl;

}





void kinonly() {

    sec::setDefaultFrequency(10.0);

#ifdef SIMULATOR
    sec::setSleeper(new sec::Barrier());

    iCubRobot<iCubSimLeftArm> robot("icubSim");
#else
    iCubRobot<iCubLeftArm> robot("icub");
#endif


    EncodersLeftArm encs(robot);

    f_kin kin;
    sec::connect(encs.arm, kin.encs);

    sec::Printer printer;
    sec::connect(kin.xyz, printer, " ");

    sec::run();

}



void test_safety() {

    sec::setDefaultFrequency(100.0);

#ifdef SIMULATOR
    sec::setSleeper(new sec::Barrier());

    iCubRobot<iCubSimLeftArm> robot("icubSim");
#else
    iCubRobot<iCubLeftArm> robot("icub");
#endif


    EncodersLeftArm encs(robot);

    f_kin kin;
    sec::connect(encs.arm, kin.encs);


    std::vector<double> jointMin(7, std::numeric_limits<double>::lowest());
    std::vector<double> jointMax(7, std::numeric_limits<double>::max());
    std::vector<double> posMin(3, std::numeric_limits<double>::lowest());
    std::vector<double> posMax(3, std::numeric_limits<double>::max());
    jointMax[3] = 50;

    SafetyStopper safetys(jointMin, jointMax, posMin, posMax);
    sec::connect(encs.arm, safetys.joints);
    sec::connect(kin.xyz, safetys.xyz);

    auto stopper = [&safetys] () {
        return safetys.shouldStop();
    };

    auto s = Signals::ramp(1, 45);
    sec::SignalSource ref(s);
    LeftArmPositionControl ctrl(robot);
    sec::connect(ref.output, ctrl.elbow);

    sec::PlottingClient plotter;
    sec::connect(ref.output, plotter, "ref");

    sec::run(0.0, {stopper});

    if (safetys.shouldStop()) {
        std::cout << "Safety boundaries reached" << std::endl;
    } else {
        std::cout << "OK" << std::endl;
    }

}

int main() {

//    kinonly();
//   test_safety();
   main_completo();

    return 0;
}
