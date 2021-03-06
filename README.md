# Project overview
1U rack mounted nixie display with IN-4 and INS-1 tubes.  
Uses an ESP32 and, optionally, a temperature sensor (such as the TMP36).  

## Description
The display, on its own, cannot actually do anything.  
It is designed to be used in tandem with its extensive REST API that allows complete control over the display:
* Controlling each individual tube (IN-4 & INS-1).
  * Both digits and brightness.
* Controlling the tube LEDs (red) *(not individually, as of PCB REV1)*.
* Controlling the onboard LED (blue).
* Basic authentication mechanism to ensure only select devices can talk to the display.
 
### Preview
Here is REV1 of a fully assembled unit mounted in a rack.  
![image](https://user-images.githubusercontent.com/13659371/150696203-9714aec0-d5d1-4025-a072-8ac5f2a038c3.png)

### Hardware
This project uses the following components:
* 6x `TLP627`
* 4x `IN-4`
* 4x `SN74141 (or clones)`
* 2x `INS-1`
* 1x `TMP36`
* 1x `ESP32`
* 1x `LDO 3.3V regulator`

*Exclusively SMD components are used where applicable.  
HV devices/traces use THT instead, as clearances and creepage distances would be violated otherwise.*

#### Power
The device runs on 5V **max** and gets fed using an onboard micro-USB socket.  
Additionally, it can be *fully* turned off using a dedicated slide switch.

The 170V regulator has been sourced externally and is not included in the component list.  
Virtually any regulator works, as long as it has similar dimensions to the popular `NCH8200HV`.

#### Notes
To save on vertical space (to stay within the 1U mounting constraint), the BCD-Decoders and opto-isolators reside on *extension boards*.  
These are connected using standard 2.54mm headers.  

It is recommended to mount them using *straight* 2.54mm headers, just to reduce the risk of accidential shocks.

#### Documentation
Please refer to the [wiki](https://github.com/ThisIsntTheWay/rack_nixie_display/wiki) for documentation.
