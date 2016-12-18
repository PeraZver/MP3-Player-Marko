# Troubleshooting Marko WAV Player 15.-17.12.2016

What happened:

 - device became unrecognized
 - 3V3 missing

 Solution:

  - 3V3 LDO regulator dead, replace it with a new one

  Result:

  - Device recognized only after the SD card is removed.
  - Can't be programmed, nor any R/W operations on memory
  - Programmer reports that "protection is set"

  ---------------------------------------------------------

What next:
   - SD card examined - works properly.
   - Maybe the fuse bits are misconfigured? 
   - Brown-out detection is set low for some reason?

 Check 3V3 again: it's low 3.0V. 
 Try adding changing capacitor and adding more capacitance - doesn't help.
 Set external power supply to 3V3 rails: DEVICE WORKS NORMALLY AGAIN!

 Conclusion:
   - bad power supply!

Cause:
  - Input voltage of 3V3 LDO regulator is about 3V. Voltage drop on D2 is 2V. This is way too much, maybe the diode is burnt?

 Bridge the diode: Device works normally again even with USB power supply. Probably the diode burned during the accidental shortening of Vin on LDO.