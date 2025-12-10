# DMX Stepper Motor Controller (Arduino + DM860)

This project implements a **DMX-controlled stepper motor controller** using an **Arduino Uno** and a **DM860 stepper driver**.  
Motor **speed and direction** are controlled via two DMX channels.  
Stepper pulses are generated using **hardware Timer1** for precise, jitter-free timing.

Designed for **theatre and scenic motion control** applications.

---

## Features

- DMX512 control using the **Conceptinetics** library
- 2 DMX channels:
  - **Channel 1** – Direction
  - **Channel 2** – Speed
- Hardware-based pulse generation using **Timer1 (OC1A, Pin 9)**
- Jitter-free step pulses (no `delay()` / `delayMicroseconds()`)
- Compatible with **DM860 stepper driver**
- Stable operation independent of `loop()` execution time

---

## Hardware Requirements

- Arduino Uno (ATmega328P)
- DM860 stepper motor driver
- Stepper motor (compatible with DM860)
- RS485 → TTL DMX interface (e.g. MAX485)
- DMX512 source (lighting desk, software, etc.)
- External power supply for stepper motor
- Common ground between Arduino and DM860 logic

---

## Wiring

| Arduino Pin | DM860 Pin | Function |
|-------------|-----------|----------|
| D9          | PUL+ / PUL- | STEP (hardware toggle via OC1A) |
| D8          | DIR+ / DIR- | Direction |
| GND         | GND        | Common signal ground |

> Use the appropriate **PUL+/PUL- and DIR+/DIR- wiring** according to whether you are using **common-anode or common-cathode** logic on the DM860.

---

## DMX Mapping

| DMX Channel | Function  | Description |
|-------------|-----------|-------------|
| Channel 1   | Direction | `0–127` = CCW, `128–255` = CW |
| Channel 2   | Speed     | `0` = Stop, `1–255` = Speed control |

Speed is mapped to a **300–2000 µs half-period**, resulting in stable step frequencies suitable for scenic motion.

---

## Libraries

This project uses:

- **Conceptinetics DMX Library**  
  https://github.com/PaulStoffregen/Conceptinetics

Install via:

```
Arduino IDE → Library Manager → Search "Conceptinetics"
```

---

## How It Works

- **Timer1** is configured in **CTC mode** with **hardware toggle on OC1A (Pin 9)**.
- The timer automatically generates step pulses with **zero software jitter**.
- The main `loop()` only:
  - Reads DMX values
  - Updates motor direction
  - Updates Timer1 compare value to change motor speed

This ensures:

- Stable pulse timing
- No dependency on `loop()` execution time
- Reliable motion for stage use

---

## Safety Notes

- Always **power the DM860 and stepper motor from an external supply**.
- Never power the motor from the Arduino.
- Ensure **proper earthing and shielding** in stage environments.
- Start with **low speeds and torque limits** when testing.

---

## License

This project is licensed under the **MIT License**.  
See the `LICENSE` file for details.

---

## Author

João Miguel Ferreira  
Porto, Portugal  
2025

