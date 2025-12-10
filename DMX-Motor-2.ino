#include <Conceptinetics.h>

#define DMX_SLAVE_CHANNELS 2
#define RXEN_PIN 2

#define DIR_PIN    8          // Direction for DM860
#define PULSE_PIN  9          // STEP for DM860 (OC1A pin on Uno!)

#define DIR_CHANNEL    1
#define SPEED_CHANNEL  2

DMX_Slave dmx_slave(DMX_SLAVE_CHANNELS, RXEN_PIN);

// --- Timer1 setup for hardware toggle on OC1A (pin 9) ---

void timer1_init()
{
  pinMode(PULSE_PIN, OUTPUT);
  digitalWrite(PULSE_PIN, LOW);

  // Stop Timer1 and reset registers
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  // CTC mode (Clear Timer on Compare Match)
  // WGM12 = 1, others 0 → Mode 4
  TCCR1B |= (1 << WGM12);

  // Prescaler = 8 → 16 MHz / 8 = 2 MHz → 0.5 µs per tick
  TCCR1B |= (1 << CS11);

  // Do NOT connect OC1A yet (we'll do it when we want motion)
  // COM1A0 will be set in updateStepperSpeed()
}

// Disconnect OC1A so pin 9 stops toggling
void stopStepperPulses()
{
  // Clear COM1A0 bit: disconnect OC1A from Timer1
  TCCR1A &= ~(1 << COM1A0);
  digitalWrite(PULSE_PIN, LOW);
}

// DMX speed 0..255 -> step pulse half-period (µs) -> Timer1 OCR1A
void updateStepperSpeed(uint8_t dmxSpeed)
{
  if (dmxSpeed == 0)
  {
    // Stop motor
    stopStepperPulses();
    return;
  }

  // Map DMX 1..255 to half-period in microseconds
  // Lower DMX -> slower (larger period)
  // Higher DMX -> faster (smaller period)
  uint16_t half_period_us = map(dmxSpeed, 1, 255, 2000, 300);

  // DM860 needs a few microseconds min pulse width; 300 µs is very safe.
  // If later you want more speed, you can reduce this lower limit
  // but keep it > ~5–10 µs to respect the driver timing.

  // Convert half-period_us to OCR1A:
  // tick = 0.5 µs → half_period_us = (OCR1A + 1) * 0.5
  // => OCR1A = half_period_us * 2 - 1
  uint16_t ocr = (uint16_t)(half_period_us * 2UL - 1UL);

  noInterrupts();
  OCR1A = ocr;

  // Enable toggle on OC1A (pin 9)
  // COM1A0 = 1, COM1A1 = 0 -> toggle OC1A on compare
  TCCR1A |= (1 << COM1A0);
  interrupts();
}

void setup()
{
  dmx_slave.enable();
  dmx_slave.setStartAddress(1);

  pinMode(DIR_PIN, OUTPUT);
  digitalWrite(DIR_PIN, LOW);

  timer1_init();
  stopStepperPulses();  // ensure stopped at start
}

void loop()
{
  // Read DMX channels
  uint8_t newDmxDir   = dmx_slave.getChannelValue(DIR_CHANNEL);
  uint8_t newDmxSpeed = dmx_slave.getChannelValue(SPEED_CHANNEL);

  // Direction channel: <=127 one way, >127 the other
  if (newDmxDir > 127) {
    digitalWrite(DIR_PIN, HIGH);
  } else {
    digitalWrite(DIR_PIN, LOW);
  }

  // Speed channel: 0 = stop, 1..255 = step frequency
  updateStepperSpeed(newDmxSpeed);

  // loop() is now free — timing is handled entirely in hardware
}