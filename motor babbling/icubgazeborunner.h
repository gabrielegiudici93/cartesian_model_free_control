#ifndef ICUBGAZEBORUNNER_H
#define ICUBGAZEBORUNNER_H

#include <string>

#include <yarp/os/RpcClient.h>

#include <sec/node.h>
#include <sec/nodelink.h>


class iCubGazeboRunner : public sec::Node {

public:
    iCubGazeboRunner(double freq = 0.0);

    virtual ~iCubGazeboRunner();

    virtual void refreshInputs() override;

    virtual bool connected() const override;

    virtual void execute() override;

    virtual std::string parameters() const override;

    // this is likely the cause of the HW_FAULTS
    void resetGazeboTime() const;

private:
    yarp::os::RpcClient rpc;

};

#endif // ICUBGAZEBORUNNER_H
