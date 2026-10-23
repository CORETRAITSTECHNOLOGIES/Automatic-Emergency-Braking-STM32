# Automatic Emergency Braking & Parking Assist — STM32 Nucleo-F401RE

An educational embedded prototype using **STM32 Nucleo-F401RE**, **Mbed OS** and **PlatformIO**, reconstructed from the owner's uploaded 2024 academic source project.

**Safety notice:** Experimental laboratory code only. Not automotive-grade or validated for real braking/propulsion; never connect to a road vehicle.

## Architecture
- Mbed threads for sensing, control, engine/ignition, and event dispatch
- Mail-based inter-thread communication
- Analog inputs emulate speed and obstacle positions
- Partial/hard brake decisions and parking mode based on configurable thresholds
- LEDs, PWM indication, serial diagnostics

The firmware uses proxy values rather than calibrated physical distance measurements. It has not been compiled or hardware-validated in this environment.

## Quick start
Install PlatformIO Core and your STM32/Mbed toolchain, then:

```bash
pio run -e nucleo_f401re
pio run -e nucleo_f401re -t upload
pio device monitor
```

For desktop-only tests:

```bash
python3 simulation/braking_demo.py
python3 -m unittest discover -s tests -v
```

## Project structure
- `src/main.cpp`: historical Mbed application
- `include/`: project constants and pin mappings
- `platformio.ini`: build environment
- `simulation/`: separate non-actuating braking decision model
- `tests/`: desktop tests
- `docs/`: architecture and limitations
- `test/`: original PlatformIO tests

## Known limitations
No validated stopping-distance model, no reliable fault handling, race-prone shared flags and ambiguous sensor units. A build with a current Mbed dependency may require compatibility updates.

**Keywords:** Embedded C++, STM32, RTOS, Mbed OS, PlatformIO, AEB, Collision Avoidance, Parking Assist, IoT Prototyping.
