#pragma once

#include <stdint.h>

#include "controller/controller_input.h"

constexpr uint32_t FAILSAFE_TIMEOUT_MS = 300;

bool failsafe_check(const ControllerInput& input);

void failsafe_trigger();
