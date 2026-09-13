#pragma once

// -----------------------------------------------------------------------------
// cannon_feeder
//
// Manually jogs the feeder servo between two fixed positions: pushPressed
// commands it toward the push (feed) position, returnPressed commands it
// toward rest. Holding neither (or both) just leaves it wherever it
// currently is -- there is no automatic timed cycle, the operator controls
// timing directly by how long each button is held.
//
// This module has no knowledge of Bluepad32 or ControllerInput beyond the
// two plain booleans passed into cannon_feeder_update() -- it has no idea
// what buttons those came from.
// -----------------------------------------------------------------------------

// Attaches the feeder servo on FEEDER_SERVO_PIN and moves it to the rest
// position. Call once from setup().
void cannon_feeder_init();

// Call once per loop() iteration. pushPressed moves the servo toward the
// push position, returnPressed moves it toward rest. If both or neither
// are pressed, the servo holds its current position.
void cannon_feeder_update(bool pushPressed, bool returnPressed);

// True while the feeder is at the rest position.
bool cannon_feeder_is_idle();

// Forces the feeder servo back to rest immediately, regardless of where it
// currently is. For the failsafe module to call.
void cannon_feeder_stop();
