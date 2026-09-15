#include "cannon_fire_internal.h"

// Only valid from CannonState::Loaded (enforced by the switch in
// cannon_fire_update(), the sole caller of this function).
void fire() {
    setFiringServos(SERVO1_UP, SERVO2_UP);
    state = CannonState::Fired;
}
