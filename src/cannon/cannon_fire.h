#pragma once

enum class CannonState {
    Loaded,
    Fired,
    Reloading
};

// Sub-state while state == Reloading. Not meaningful in any other state.
enum class ReloadStep {
    SettingSpool,
    SpoolSettling,
    MotorRunning,
    Locking,
    MotorStopping,
    MotorOffSettling,
    ReleasingSpool
};

void cannon_fire_init();

void cannon_fire_update(bool fireRequested, bool reloadRequested);

CannonState cannon_fire_get_state();

// For debugging/status only (e.g. main.cpp's change-logging). Only
// meaningful while cannon_fire_get_state() == CannonState::Reloading.
ReloadStep cannon_fire_get_reload_step();
