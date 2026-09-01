# Robot-Kamp

ESP32 (Arduino/PlatformIO) firmware for an RC car with a 4-DOF arm bolted on top, controlled by a PS5 DualSense over Bluetooth via [Bluepad32](https://github.com/ricardoquesada/bluepad32).

## Architecture

Each concern lives in its own module. `main.cpp` is the only place that wires them together.

```
controller_input  ->  ControllerInput struct  ->  failsafe (gate) ->  drive / arm_base / arm_shoulder / arm_elbow / arm_wrist / arm_gripper
```

- **`controller/controller_input`** — the only module that talks to Bluepad32 directly. Reads the gamepad and returns a plain `ControllerInput` struct (booleans and ints). Every other module only ever sees this struct's fields, never Bluepad32 types — swapping controller libraries later means touching only this module.
- **`drive`** — Traxxas XL-5 ESC (throttle/brake) + Traxxas 2056 steering servo. Takes plain `driveForward`/`driveBackward`/`steerAxis` ints, knows nothing about triggers or sticks.
- **`arm/arm_base`** — Titan 12T base rotation motor via its own ESC. Continuous rotation, no position to track — just spin left / spin right / stop.
- **`util/stepped_servo`** — shared logic for a servo that holds an angle (0-180°) and steps it up/down while a direction is held. Used by the four joint modules below.
- **`arm/arm_shoulder`, `arm/arm_elbow`, `arm/arm_wrist`, `arm/arm_gripper`** — thin wrappers, each owning one `SteppedServo` instance. Take plain up/down-style booleans, expose `_update()` and `_angle()`.
- **`safety/failsafe`** — the gatekeeper. Every `loop()`, decides whether the current input is trustworthy (`failsafe_check`) and, if not, forces every actuator into a safe state (`failsafe_trigger`) instead of letting `main.cpp` act on it.
- **`config/pins.h`** — every pin number in the project, in one place. No other module hardcodes a pin.

None of the actuator modules (`drive`, `arm_base`, the joint modules) know where their input came from or whether it's safe — they just do what they're told. `main.cpp` and `failsafe` are the only places that make that judgment call.

## Controller mapping

| Input | ControllerInput field | Module |
|---|---|---|
| R2 (trigger, analog) | `driveForward` | drive (throttle) |
| L2 (trigger, analog) | `driveBackward` | drive (brake/reverse) |
| Left stick X | `steerAxis` | drive (steering) |
| L1 | `armBaseLeft` | arm_base (spin left) |
| R1 | `armBaseRight` | arm_base (spin right) |
| D-pad up / down | `shoulderUp` / `shoulderDown` | arm_shoulder |
| D-pad left / right | `gripperOpen` / `gripperClose` | arm_gripper |
| Triangle / Cross | `elbowUp` / `elbowDown` | arm_elbow |
| Square / Circle | `wristRotateA` / `wristRotateB` | arm_wrist |

Note: Bluepad32 names face buttons by pad position, not the PS glyph — `a()` = Cross, `b()` = Circle, `x()` = Square, `y()` = Triangle. See `controller_input.cpp` for the raw mapping.

## Pin map (`config/pins.h`)

| Pin | Assignment | Status |
|---|---|---|
| GPIO 26 | ESC (drive) | confirmed, tested |
| GPIO 27 | Steering servo (drive) | confirmed, tested |
| GPIO 13 | Arm base ESC (Titan 12T) | provisional |
| GPIO 14 | Shoulder servo (MG90S) | provisional |
| GPIO 16 | Elbow servo (MG90S) | provisional |
| GPIO 17 | Wrist servo (Traxxas 2056) | provisional |
| GPIO 18 | Gripper servo (MG90S) | provisional |

Provisional pins were picked from the ESP32-WROOM-32 safe GPIO list (no strapping/input-only/flash pins): G13, G14, G16, G17, G18, G19, G21, G22, G23, G25, G32, G33. G19/G21 are left free as spares.

## Building and flashing

Two PlatformIO environments, isolated via `build_src_filter` so they never compile against each other:

```bash
# Real robot firmware (src/main.cpp + all modules)
pio run -e esp32dev -t upload
pio device monitor

# Standalone single-servo bench test (src/servo_test/main.cpp)
pio run -e servo_test -t upload
pio device monitor
```

`servo_test` has no Bluepad32/controller dependency — it's plain Arduino + ESP32Servo, used to sanity-check a servo's range, direction, and smoothness before it's ever wired into the real arm modules. Set `SERVO_PIN` at the top of `src/servo_test/main.cpp` to whichever pin you're testing.

## Tuning the arm

**Speed (shoulder/elbow/wrist/gripper):** the `stepDegreesPerTick` argument to `.attach()` in each joint's `_init()` (currently `1.10f` in all four) — bigger = faster.

**Direction (shoulder/elbow/wrist/gripper):** if a joint moves opposite of what's expected, swap the two arguments passed into the internal `SteppedServo::update()` call in that joint's `_update()` function (e.g. `shoulderServo.update(down, up)` instead of `(up, down)`).

**Speed (arm base):** `ESC_SPIN_LEFT_US` / `ESC_SPIN_RIGHT_US` in `arm_base.cpp` (currently 1450/1550µs, ±50µs off the 1500µs neutral) — move further from neutral for more speed. Watch for the ESC's deadband near neutral.

**Direction (arm base):** swap `ESC_SPIN_LEFT_US`/`ESC_SPIN_RIGHT_US`, or swap which branch of `arm_base_update()` writes which value.

**Steering direction (drive):** sign of `steerAxis` in the pulse calculation in `drive.cpp`.

## Failsafe

`failsafe_check()` fails (returns unsafe) when the controller is disconnected, or when `input.lastUpdateMs` is older than `FAILSAFE_TIMEOUT_MS` (300ms). On trigger:

- **Velocity actuators** (drive, arm_base) are force-stopped — they have no position to preserve.
- **Position actuators** (the four joints) are held in place by calling their `_update()` with `(false, false)` — never forced to a hardcoded angle, since that could slam the arm into something.

## Status

- ✅ Controller input, pin config, drive, stepped-servo utility — built and tested.
- ✅ Four arm joint modules, arm_base, failsafe — built, wired into `main.cpp`, logic-tested via serial (no ControllerInput/Bluepad32 dependency issues).
- ⏳ Arm hardware (servos, Titan motor + ESC) still being physically wired/tuned — pin assignments and PWM speed constants are provisional until confirmed on real hardware.
