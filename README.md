*Smart Fan System*

**Implementation & Design Sheet**
***1. Project Overview***

The Smart Fan is a temperature-controlled cooling system built using an Arduino microcontroller.
The system automatically turns the fan ON when the ambient temperature exceeds 23 °C and turns it OFF when the temperature falls below the threshold with hysteresis.

In addition to automatic control, the fan speed is manually adjustable using a potentiometer via PWM, allowing smooth and safe speed control.

The design demonstrates key embedded systems concepts, including:

- Sensor interfacing and ADC conversion
- PWM motor control
- Relay isolation for safe power switching
- Inductive load protection using flyback diodes
- Modular, readable firmware design

***2. System Architecture (High-Level)***
 Input → Processing → Output
 - Input:
  - TMP36 temperature sensor
  - Potentiometer (speed control)
- Processing:

Arduino reads sensor data

Compares temperature against threshold

Generates PWM signal for motor speed

Output:

DC fan motor (via motor driver)

Relay-controlled power switching

4-digit 7-segment display for temperature feedback

3. Components and Their Roles
3.1 Arduino Microcontroller

Acts as the central controller

Reads analog values from sensors

Makes control decisions

Generates PWM signals

Drives the relay and motor driver logic

3.2 TMP36 Temperature Sensor

Role: Measures ambient temperature

Connections:

Vout → Arduino A0

Vcc → Arduino +5V

GND → Arduino GND

Reason:

TMP36 provides an analog voltage proportional to temperature

Arduino’s ADC converts this voltage into a digital value

Used to determine when the fan should turn ON or OFF

3.3 Potentiometer (Speed Control)

Role: Manual fan speed control

Connections:

Wiper → Arduino A1

One end → +5V

Other end → GND

Reason:

Provides a variable analog input (0–5 V)

Arduino maps this to a PWM value (0–255)

Allows smooth real-time control of fan speed

3.4 L293D Motor Driver

Role: Controls DC motor direction and speed safely

Connections:

ENA → Arduino GPIO 2 (PWM)

IN1 → Arduino GPIO 5

IN2 → Arduino GPIO 4

OUT1 & OUT2 → DC motor terminals

All GND pins → Common GND

Reason:

MCU pins cannot supply enough current to drive a motor directly

Driver provides current amplification

Protects MCU from motor back-EMF

Allows PWM-based speed control

3.5 DC Motor (Fan)

Role: Provides cooling

Connections:

Motor terminals → L293D OUT1 & OUT2

Reason:

Converts electrical energy into airflow

Speed controlled using PWM

Direction controlled via IN1 / IN2 logic

3.6 Relay (SPDT)

Role: Safely switches motor power ON and OFF

Connections:

COM → +9 V external supply

NO → Motor driver supply input

Coil → Driven via NPN transistor

Flyback diode across relay coil

Reason:

Electrically isolates high-current motor supply from MCU

Prevents MCU damage

Allows full power cutoff when fan is OFF

3.7 NPN Transistor (Relay Driver)

Role: Drives relay coil safely

Connections:

Emitter → GND

Collector → Relay coil

Base → Arduino GPIO 7 through 1 kΩ resistor

Flyback diode across relay coil (reverse-biased)

Reason:

MCU GPIO cannot supply relay coil current directly

Transistor provides current amplification

Protects MCU I/O pins from overcurrent

3.8 Flyback Diode

Role: Protects against inductive voltage spikes

Placement & Orientation:

Connected across relay coil

Cathode → +V

Anode → Transistor collector

Reason:

Relay coil is an inductive load

When switched OFF, it generates a high-voltage spike

Diode provides a safe discharge path

Prevents damage to transistor and MCU

3.9 4-Digit 7-Segment Display (I²C)

Role: Displays temperature value

Connections:

SDA → Arduino A4

SCL → Arduino A5

Vcc → +5 V

GND → GND

Reason:

Provides real-time visual feedback

Uses I²C to reduce pin usage

Cleaner wiring and simpler firmware

4. Control Logic Summary

Arduino continuously reads temperature from TMP36

If temperature ≥ 23 °C:

Relay turns ON (powers motor driver)

Motor direction set

PWM enabled for speed control

Fan speed is adjusted using potentiometer

If temperature drops below threshold with hysteresis:

PWM stopped

Motor disabled

Relay turned OFF

Temperature is displayed on the 7-segment display

5. Key Design Considerations

Safety:

Relay isolation

Transistor current limiting

Flyback diode protection

Reliability:

No motor current drawn from MCU pins

Clean separation of sensing and actuation

Maintainability:

Modular functions

Clear pin naming

No magic numbers

Scalability:

Can be extended with Wi-Fi (ESP32), MQTT, or remote monitoring

6. Final Summary (Interview-Ready)

The Smart Fan system demonstrates practical embedded systems design by combining temperature sensing, PWM motor control, relay-based power isolation, and inductive load protection. The design prioritizes safety, reliability, and clean firmware structure while allowing both automatic and manual control of the fan.

“The design uses a PCB‑mount SPDT relay (Form C) with a 9 V DC coil (≈225 Ω, ≈0.36 W) and a 10 A contact rating at 24 VDC. The coil is driven from an Arduino digital output (conceptually via a transistor in real hardware). The relay’s COM contact is tied to the 9 V motor supply, the NO contact feeds the L293D motor driver VCC2, and the NC contact is left unconnected. The relay provides galvanic isolation and a master ON/OFF for the motor driver’s supply.”
