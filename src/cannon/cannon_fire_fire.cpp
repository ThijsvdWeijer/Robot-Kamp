#include "cannon_fire_internal.h"

void fire() {
    setFiringServos(SERVO1_UP, SERVO2_UP);
    state = CannonState::Fired;
}
