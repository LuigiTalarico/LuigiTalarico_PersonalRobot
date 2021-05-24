#ifndef MyRobotSAFETYPROPERTIES_HPP_
#define MyRobotSAFETYPROPERTIES_HPP_

#include <eeros/safety/SafetyProperties.hpp>
#include <eeros/hal/HAL.hpp>
#include "ControlSystem.hpp"
#include <eeros/sequencer/Sequencer.hpp>

class MyRobotSafetyProperties : public eeros::safety::SafetyProperties
{
public:
    MyRobotSafetyProperties(ControlSystem &cs, double dt);

    // Define all possible events
    eeros::safety::SafetyEvent abort;
    eeros::safety::SafetyEvent shutdown;
    eeros::safety::SafetyEvent doSystemOn;
    eeros::safety::SafetyEvent SystemStarted;
    eeros::safety::SafetyEvent emergency;
    eeros::safety::SafetyEvent resetEmergency;
    eeros::safety::SafetyEvent PowerOn;
    eeros::safety::SafetyEvent PowerOff;
    eeros::safety::SafetyEvent startMoving;
    eeros::safety::SafetyEvent stopMoving;
    eeros::safety::SafetyEvent motorsHalted;
    

    // Define all possible levels
    eeros::safety::SafetyLevel slSystemOff;
    eeros::safety::SafetyLevel slShuttingDown;
    eeros::safety::SafetyLevel slHalting;
    eeros::safety::SafetyLevel slStartingUp;
    eeros::safety::SafetyLevel slEmergency;
    eeros::safety::SafetyLevel slEmergencyBraking;
    eeros::safety::SafetyLevel slSystemOn;
    eeros::safety::SafetyLevel slMotorPowerOn;
    eeros::safety::SafetyLevel slSystemMoving;     

private:
    // Define all critical outputs
    // eeros::hal::Output<bool>* button;
    eeros::hal::Output<bool>* redLed;
    eeros::hal::Output<bool>* greenLed;

    // Define all critical inputs
    // eeros::hal::Input<bool>* ...;
    eeros::hal::Input<bool>* readyButton;

    ControlSystem &cs;
};

#endif // MyRobotSAFETYPROPERTIES_HPP_
