# Robot-Kamp

ESP32 (Arduino/PlatformIO) firmware for an RC car with a rotating base and a servo-actuated cannon (string-tensioned firing mechanism) bolted on top, controlled by a PS5 DualSense over Bluetooth via [Bluepad32](https://github.com/ricardoquesada/bluepad32).

## Architecture

Each concern lives in its own module. `main.cpp` is the only place that wires them together.

```
controller_input  ->  ControllerInput struct  ->  failsafe (gate) ->  drive / arm_base / cannon_fire
```

- **`controller/controller_input`** — the only module that talks to Bluepad32 directly. Reads the gamepad and returns a plain `ControllerInput` struct (booleans and ints). Every other module only ever sees this struct's fields, never Bluepad32 types — swapping controller libraries later means touching only this module.
- **`drive`** — Traxxas XL-5 ESC (throttle/brake) + Traxxas 2056 steering servo. Takes plain `driveForward`/`driveBackward`/`steerAxis` ints, knows nothing about triggers or sticks.
- **`base/arm_base`** — Titan 12T base rotation motor via its own ESC. Continuous rotation, no position to track — just spin left / spin right / stop.
- **`cannon/cannon_fire`** — the fire/reload state machine for the 4-servo firing mechanism. See [Cannon fire/reload sequence](#cannon-firereload-sequence) below.
- **`safety/failsafe`** — the gatekeeper. Every `loop()`, decides whether the current input is trustworthy (`failsafe_check`) and, if not, forces every actuator into a safe state (`failsafe_trigger`) instead of letting `main.cpp` act on it.
- **`config/pins.h`** — every pin number in the project, in one place. No other module hardcodes a pin.
- **`util/stepped_servo`** — leftover from a previous joint-arm version of this project. Nothing currently includes it; see [Known issues](#known-issues--cleanup-todo).

None of the actuator modules (`drive`, `arm_base`, `cannon_fire`) know where their input came from or whether it's safe — they just do what they're told. `main.cpp` and `failsafe` are the only places that make that judgment call.

## Controller mapping

| Input | ControllerInput field | Module |
|---|---|---|
| R2 (trigger, analog) | `driveForward` | drive (throttle) |
| L2 (trigger, analog) | `driveBackward` | drive (brake/reverse) |
| Left stick X | `steerAxis` | drive (steering) |
| L1 | `armBaseLeft` | arm_base (spin left) |
| R1 | `armBaseRight` | arm_base (spin right) |
| Cross / X | `fireRequested` | cannon_fire (fire) |
| Triangle | `reloadRequested` | cannon_fire (start reload sequence) |

Note: Bluepad32 names face buttons by pad position, not the PS glyph — `a()` = Cross, `b()` = Circle, `x()` = Square, `y()` = Triangle. See `controller_input.cpp` for the raw mapping.

## Pin map (`config/pins.h`)

| Pin | Assignment | Status |
|---|---|---|
| GPIO 26 | ESC (drive throttle/brake) | working |
| GPIO 27 | Steering servo (drive) | **broken — see Known issues** |
| GPIO 13 | Arm base ESC (Titan 12T) | wired, spin speeds still provisional |
| GPIO 14 | Servo 1 — firing pin (DOWN/UP) | tuned on real mechanism |
| GPIO 16 | Servo 2 — firing support (DOWN/UP) | tuned on real mechanism |
| GPIO 17 | Servo 3 — spool control (SET/RELEASED) | tuned on real mechanism |
| GPIO 25 | Servo 4 — LEGO motor direction switch (OFF/ON) | tuned on real mechanism |

## Cannon fire/reload sequence

`cannon_fire` runs a small state machine, driven from `loop()` via `cannon_fire_update(fireRequested, reloadRequested)`:

```
Loaded --fire()--> Fired --startReload()--> Reloading --...--> Loaded
```

While `Reloading`, `reloadStep` walks through, each step timed off `millis()` (never a blocking `delay()`, so the controller/failsafe keep being polled throughout):

```
SettingSpool -> SpoolSettling -> MotorRunning -> Locking -> MotorStopping -> MotorOffSettling -> ReleasingSpool
```

All calibration angles and step timings are tunable constants at the top of `cannon_fire.cpp`:

| Constant | Meaning |
|---|---|
| `SERVO1_DOWN` / `SERVO1_UP` | Firing pin: locked / fired position |
| `SERVO2_DOWN` / `SERVO2_UP` | Firing support: always moves in lockstep with Servo 1 |
| `SERVO3_SET` / `SERVO3_RELEASED` | Spool control positions |
| `SERVO4_OFF` / `SERVO4_ON` | LEGO motor direction-switch positions |
| `RELOAD_MOTOR_TIME_MS` | How long the motor pulls during `MotorRunning` |
| `SPOOL_SETTLE_TIME_MS` | Delay between setting the spool and switching the motor on |
| `LOCK_SETTLE_TIME_MS` | Delay between the firing pin locking and cutting motor power |
| `MOTOR_OFF_SETTLE_TIME_MS` | Delay between switching the motor off and releasing the spool |

Both `fireRequested` and `reloadRequested` are ignored while `Reloading` — the sequence always runs to completion uninterrupted.

## Building and flashing

Two PlatformIO environments, isolated via `build_src_filter` so they never compile against each other (PlatformIO 6.x only supports one project-wide `src_dir`, so a per-environment `src_dir` isn't an option — see the comments in `platformio.ini`):

```bash
# Real robot firmware (src/main.cpp + all modules)
pio run -e esp32dev -t upload
pio device monitor

# Per-servo controller jog test for the 4 cannon servos (src/servo_test/main.cpp)
pio run -e servo_test -t upload
pio device monitor
```

`servo_test` still needs the DualSense controller (it uses Bluepad32 just like the real firmware) but nothing else — no drive/arm_base/cannon_fire/failsafe dependency. Each face button toggles one servo between its two calibrated positions, so you can jog one joint at a time before trusting it in the automatic reload sequence:

| Button | Servo |
|---|---|
| Cross / X | Servo 1 — firing pin |
| Circle | Servo 2 — firing support |
| Square | Servo 3 — spool control |
| Triangle | Servo 4 — motor switch |

## Tuning

**Cannon servo angles/timings:** the constants block at the top of `cannon_fire.cpp` (see table above).

**Arm base speed:** `ESC_SPIN_LEFT_US` / `ESC_SPIN_RIGHT_US` in `arm_base.cpp` — still conservative placeholder values, never tuned against real load (see Known issues).

**Arm base direction:** swap `ESC_SPIN_LEFT_US`/`ESC_SPIN_RIGHT_US`, or swap which branch of `arm_base_update()` writes which value.

**Steering direction (drive):** sign of `steerAxis` in the pulse calculation in `drive.cpp`.

## Failsafe

`failsafe_check()` fails (returns unsafe) when the controller is disconnected, or when `input.lastUpdateMs` is older than `FAILSAFE_TIMEOUT_MS` (300ms). On trigger:

- **Drive and arm_base** are force-stopped to neutral — they have no position to preserve.
- **cannon_fire** is still ticked forward with `(false, false)` — an in-progress reload keeps running to completion rather than being aborted mid-sequence, since stopping partway could leave the mechanism in an unsafe half-locked state.

## Known issues / cleanup TODO

- **Dead code in the cannon module.** `cannon_fire_fire.cpp`, `cannon_fire_reload.cpp`, and `cannon_fire_internal.h` define a second copy of `fire()`/`startReload()`/`updateReload()` at global scope, matching an `extern` split that was never finished. The functions that actually run are private (anonymous-namespace) copies defined directly inside `cannon_fire.cpp`; the split files are never called and the build only stays green because the linker garbage-collects them. Any edit made only to the split files currently has zero effect on behavior. Needs a decision: finish the split (move the real implementation into them) or delete them.
- **`util/stepped_servo.{h,cpp}` is unused.** Leftover from a previous joint-arm version of this project — nothing currently includes it.
- **Steering (GPIO 27) is currently non-functional**, and was observed to make the ESP32 run hot while steering was unresponsive — suspected hardware fault (servo stall, short, or wiring issue) rather than a firmware bug, since `drive.cpp`'s steering logic hasn't changed. Do not run the car until this is resolved; investigate the steering servo and its wiring before assuming a code cause.
- **Firing pin (Servo 1) can stall/slip** when trying to lock against a fully-tensioned string with the push bar seated at the back of the barrel — a torque/leverage limitation on that joint rather than a sequencing bug (locking at full tension is required by the mechanical design, so the firmware can't avoid the load by reordering steps). See linkage geometry / servo torque / re-zeroing a slipped horn.

## Status

- ✅ Controller input, pin config, drive throttle, failsafe, cannon fire/reload state machine — built, tuned on real hardware where noted above.
- ✅ Per-servo controller jog tool (`servo_test` env) for bringing up/re-testing individual cannon servos.
- ⚠️ Drive steering — broken, suspected hardware fault (see Known issues).
- ⏳ Arm base spin speeds — wired but still provisional, not yet tuned against real load.
- ⚠️ Firing-pin lock-under-tension step is a known mechanical failure point (see Known issues).
