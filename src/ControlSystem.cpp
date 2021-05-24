#include "ControlSystem.hpp"

ControlSystem::ControlSystem(double dt)
    : Ewl("enc1"), Ewr("enc2"),
      myConstant(1.0), // myGain(2.0), (LT eliminate)
      Mwl("motor1"), Mwr("motor2"),
      timedomain("Main time domain", dt, true)
{
    // Name all blocks
    Ewl.setName("Ewl"); //Encoder wheel left
    Ewr.setName("Ewr");
    myConstant.setName("My constant");
    //(LT eliminate) myGain.setName("My gain");
    Mwl.setName("Mwl"); //Motor wheel left
    Mwr.setName("Mwr");

    // Name all signals
    Ewl.getOut().getSignal().setName("Position left wheel [m]");
    Ewr.getOut().getSignal().setName("Position right wheel [m]");
    myConstant.getOut().getSignal().setName("My constant value");
    //(LT eliminate) myGain.getOut().getSignal().setName("My constant value multiplied with my gain");

    // Connect signals
    Mwl.getIn().connect(myConstant.getOut());
    Mwr.getIn().connect(myConstant.getOut());
    //(LT eliminate) myGain.getIn().connect(myConstant.getOut());


    // Add blocks to timedomain
    timedomain.addblock(Ewl);
    timedomain.addblock(Ewr);
    timedomain.addBlock(myConstant);
    //(LT eliminate) timedomain.addBlock(myGain);

    // Add timedomain to executor
    eeros::Executor::instance().add(timedomain);
}