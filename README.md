# Control a countertop ice maker with an arduino

This project is to replace the internal microcontroller on a NutriChef Ice Maker with an arduino. The original microcontroller was failing and this was cheeper than getting a replacement.

## Dependentcies

- LiquidCrystal_I2C library

## Hardware

- Arduino Mega
- 8 channel Relay (AC)
- 4 channel Relay (DC)
- 16x2 LCD with I2C chip
- 12v to 5v buck converter
- 12v 5a generic psu

## Existing Hardware

- 2 Water Pumps
- Solinoid
- Syncreonized AC Motor
- AC Motor
- Compressor w/ reverse valvc
- 5 Buttons
- IR line sensor
- Water float sensor
- 2 microswitches

## Ice Cycle

1) Home Tray to fill with water
2) Fill tray with water
3) Turn on the compressor and fan. Waits until ice is ready via timer
4) Move tray to other position
5) Reverse compressor for 10 seconds to drop the ice
6) Move tray to fill position to push ice into resivoir

---

### Notes

- The compressor has a built in cycle timer to prevent damage to the compressor components. Turning the compressor relay on right after it was turned off does nothing for 5 minutes.
- Ice Tray motor has no set direction.
