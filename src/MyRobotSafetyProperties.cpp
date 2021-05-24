#include "MyRobotSafetyProperties.hpp"

MyRobotSafetyProperties::MyRobotSafetyProperties(ControlSystem &cs, double dt)
    : cs(cs),
    
      //Name the Events

      abort("abort");
      shutdown("Shutdown");
      doSystemOn("Do system on");
      SystemStarted("System started");
      emergency("Emergency");
      resetEmergency("Reset Emergency");
      PowerOn("Power On");
      PowerOff("Power Off");
      startMoving("Start moving");
      stopMoving("Stop moving");
      motorsHalted("Motors halted");

      //Name the Levels

      slSystemOff("System is offline"),
      slShuttingDown("System shutting down");
      slHalting("System halting");
      slStartingUp("System starting up");
      slEmergency("Emergency");
      slEmergencyBraking("System halting");
      slSystemOn("System is online");
      slMotorPowerOn("Motors powered");
      slSystemMoving("System moving");

{
    eeros::hal::HAL &hal = eeros::hal::HAL::instance();

    // Declare and add critical outputs
    // ... = hal.getLogicOutput("...");
    redLed = hal.getLogicOutput("onBoardLEDred");
    greenLed = hal.getLogicOutput("onBoardLEDgreen");

    // criticalOutputs = { ... };
    criticalOutputs = { redLed, greenLed };

    // Declare and add critical inputs
    // ... = eeros::hal::HAL::instance().getLogicInput("...", ...);
    readyButton = hal.getLogicInput("onBoardButtonPause", true);

    // criticalInputs = { ... };
    criticalInputs = { readyButton };

    // Add all safety levels to the safety system
    addLevel(slSystemOff);
    addLevel(slShuttingDown);
    addLevel(slHalting);
    addLevel(slStartingUp);
    addLevel(slEmergency);
    addLevel(slEmergencyBraking);
    addLevel(slSystemOn);
    addLevel(slMotorPowerOn);
    addLevel(slSystemMoving);

    // Add events to individual safety levels
    slSystemOff.addEvent(doSystemOn, slStartingUp, kPublicEvent);
    slShuttingDown.addEvent(shutdown, slSystemOff, kPrivateEvent);
    slHalting.addEvent(motorsHalted, slShuttingDown, kPrivateEvent);
    slStartingUp.addEvent(SystemStarted, slSystemOn, kPrivateEvent);
    slEmergency.addEvent(resetEmergency, slSystemOn, kPrivateEvent);
    slEmergencyBraking.addEvent(motorsHalted, slEmergency, kPublicEvent);
    slSystemOn.addEvent(PowerOn, slMotorPowerOn, kPublicEvent);
    slMotorPowerOn.addEvent(startMoving, slSystemMoving, kPublicEvent);
    slMotorPowerOn.addEvent(PowerOff, slSystemOn, kPublicEvent);
    slSystemMoving.addEvent(stopMoving, slSystemMoving, kPublicEvent);
    slSystemMoving.addEvent(emergency, slEmergencyBraking, kPublicEvent);
    slSystemMoving.addEvent(abort, slHalting, kPublicEvent);

    // Add events to multiple safety levels
    // addEventToAllLevelsBetween(lowerLevel, upperLevel, event, targetLevel, kPublicEvent/kPrivateEvent);
    addEventToAllLevelsBetween(slSystemOn, slMotorPowerOn, emergency, slEmergency, kPublicEvent);
    addEventToAllLevelsBetween(slSystemOn, slMotorPowerOn, abort, slShuttingDown, kPublicEvent);

    // Define input actions for all levels
    // level.setInputActions({ ... });
    slSystemOff.setInputActions({ ignore(readyButton) });
    slShuttingDown.setInputActions({ ignore(readyButton) });
    slHalting.setInputActions({ ignore(readyButton) });
    slStartingUp.setInputActions({ ignore(readyButton) });
    slEmergency.setInputActions({ ignore(readyButton) });
    slEmergencyBraking.setInputActions({ ignore(readyButton) });
    slSystemOn.setInputActions({ check(readyButton, false, PowerOn)} );
    slMotorPowerOn.setInputActions({ ignore(readyButton) });
    slSystemMoving.setInputActions({ ignore(readyButton) });

    // Define output actions for all levels
    // level.setOutputActions({ ... });
    slSystemOff.setOutputActions({ set(redLed, true), set(green, false)});
    slShuttingDown.setOutputActions({ set(redLed, true), set(green, false)});
    slHalting.setOutputActions({ set(redLed, true), set(green, false)});
    slStartingUp.setOutputActions({ set(redLed, true), set(green, false)});
    slEmergency.setOutputActions({ set(redLed, true), set(green, false)});
    slEmergencyBraking.setOutputActions({ set(redLed, true), set(green, false)});
    slSystemOn.setOutputActions({ set(redLed, true), set(green, true)});
    slMotorPowerOn.setOutputActions({ set(redLed, false), set(green, true)});
    slSystemMoving.setOutputActions({ set(redLed, false), set(green, true)});
    
    // Define and add level actions
    slSystemOff.setLevelAction([&](SafetyContext *privateContext) {
        eeros::Executor::stop();
        eeros::sequencer::Sequencer::instance().abort();
    });

    slShuttingDown.setLevelAction([&](SafetyContext *privateContext) {
        cs.timedomain.stop();
        privateContext->triggerEvent(shutdown)
    });

    slHalting.SetLevelAction([&] (SafetyContext * privateContext)){
        // Check if motors are standing still (to do...)
        privateContext->triggerEvent(motorsHalted)
    });

    slStartingUp.SetLevelAction([&] (SafetyContext * privateContext)){
        cs.timedomain.start() // inverse mirror of the 'slShuttingDown' level
        privateContext->triggerEvent(SystemStarted)
    });

    slEmergency.setLevelAction([& dt](SafetyContext *privateContext) {
        if (slEmergency.getNofActivations()*dt == 1) //wait 1 sec
        {
            static int counter = 0
            if (counter++ == 3) privateContext->triggerEvent(abort) // abort after entering emergency sequence 4 times
            privateContext->triggerEvent(resetEmergency)
        }
    });

    slEmergencyBraking.setLevelAction([&](SafetyContext *privateContext) {
        // Check if motors are standing still if they to attivate 'motorsHalted' even(to do...)
        privateContext->triggerEvent(motorsHalted)
    });

    slSystemOn.setLevelAction([&](SafetyContext *privateContext) {
        // to create a bottom functionality which if press attivate the 'PowerOn' event (to do...)
    });

    slMotorPowerOn.setLevelAction([&](SafetyContext *privateContext) {
        // check if the motor are moving, and if they are call the event 'startMoving'(to do)
        privateContext->triggerEvent(startMoving)
    });

    slSystemMoving.setLevelAction([&](SafetyContext *privateContext) {
        // check if the motor are standing still and move to slMotorPowerOn
        // -> slMotorPowerOn
        //privateContext->triggerEvent(emergency)
    });

    // Define entry level
    setEntryLevel(slSystemOff);

    // Define exit function
    exitFunction = ([&](SafetyContext *privateContext) {
        privateContext->triggerEvent(abort);
    });
}
