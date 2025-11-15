AVR Basic I/O Interfacing -- Stepper Motor Control (Lab 2)

This project implements an AVR-based control system using an ATmega328P
(Arduino UNO) to drive a unipolar stepper motor while managing multiple
input buttons, LEDs, and a 7-segment display. The system was developed
and simulated in SimulIDE, with code written in the Arduino IDE using
register-level AVR operations.

Overview

The project focused on configuring GPIO pins for both input and output,
implementing stepper motor sequences (half-step and full-step), and
developing a mode-selection state machine. Additional features included
speed control, direction control, simple debouncing, and emergency-stop
handling.

Hardware Components (SimulIDE)

-   Arduino UNO (ATmega328P)
-   5 push buttons
-   2 LEDs with 330Ω resistors
-   7-segment display
-   ULN2003 driver
-   Unipolar stepper motor
-   5V power supply and ground rails

System Behavior

Buttons: - MODE: Cycled through the available operation modes. -
START/STOP: Started or stopped the stepper motor depending on the
current mode. - DIR: Reversed the stepper rotation direction. - SPEED UP
/ SPEED DOWN: Adjusted motor speed in automatic modes.

LEDs: - STATUS LED: Indicated whether the motor was running. - DIR LED:
Reflected the current direction (forward or reverse).

7-Segment Display: Displayed the active mode code: 0: Stop 1: Manual --
Half Step 2: Manual -- Full Step 3: Automatic -- Half Step 4: Automatic
-- Full Step E: Emergency Stop

Stepper Motor Control: The motor was driven using full-step and
half-step sequences. In automatic modes, it ran continuously. In manual
modes, each press of START triggered a single step or a short predefined
burst.

Safety / Emergency Function: Holding the START/STOP button for more than
2 seconds activated the emergency-stop state. All outputs were set low,
and the 7-segment display blinked the letter E. The same long press
cleared the emergency state.

Project Structure

/code Source code implemented in Arduino IDE /simulation SimulIDE
schematic README.md Project documentation

Running the Project

1.  Open the source code in the Arduino IDE.
2.  Upload it to the microcontroller or run it through SimulIDE.
3.  Load the SimulIDE schematic to test the system behavior, modes, and
    stepper operation.
