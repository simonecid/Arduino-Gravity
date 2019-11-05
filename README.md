# Gravity project

## Setting up the system

### Requirements
 
 * Power supply, 2.6V/0.8A is enough, we can survive even with 2V
 * "HV" (2V) cable from power supply to relay, male-male endings
 * Cable from ground to ground of magnet, male-male endings
 * Mechanic stand with 2 arms with clamps
 * Arduino kit

### Magnet and relay setup

 1) Connect relay red male plug to red magnet female.
 2) Connect relay red female to red "HV" male
 3) Connect other red "HV" male to power supply
 4) Connect ground cable to power supply ground and magnet ground
 5) Connect relay control bus to D4 (can change, configurable)

### IR readout setup

 1) Connect brown-red pair to - and + pins  (pin 1 and 2 from left, respectively) of readout chip, respectively
 2) Connect yellow cable to pin 4 starting from the left
 3) Take arduino bus and connect one ending to D3 (can change, configurable)
 4) Connect brown and red pair to ground (black) and vcc (red) pin, respectively
 5) Connect yellow to yellow

### Final steps

  1) Connect button to D4
  2) Connect LCD to I2C
  3) Connect Arduino to laptop via USB