#pragma once

enum class CannonState {
    Loaded,
    Fired,
    Reloading
};

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

ReloadStep cannon_fire_get_reload_step();
