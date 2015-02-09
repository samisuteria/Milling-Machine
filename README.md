# Milling-Machine
Frontend, Server, and Microcontroller code for a milling machine made for Lab IV at Texas Tech University. 

## Installing

The server and the front end are both handled by [Node.JS](http://nodejs.org) - follow directions for installing from their site. 

Depending on your choice of microcontroller either download and install [Arduino](http://arduino.cc/en/Main/Software) or [Energia](http://energia.nu/download/). The code was written for a [Tiva C Series Connected Launchpad](http://www.ti.com/tool/ek-tm4c1294xl) but should work on other microcontrollers that are Arduino compatible. Edit the top of the code to match the pins on your microcontroller. 

## Usage

Go to the MillingInterface folder in terminal and run using 
```
node index.js
```

## Creating G-Code Files

Download and follow the directions from [PCBGCode](http://pcbgcode.org/index.php?5). EagleCAD is required for this script. 

### G-Code Examples

- (\***) - Comments
- M02 - End of Program
- ^G01 X-(\d*.\d*) Y(\d*.\d*) $ - Linear Interpolation X Y
- ^G00 X-(\d*.\d*) Y(\d*.\d*) $ - Rapid Postioning -X Y
- ^G00 X(\d*.\d*) Y(\d*.\d*) $  - Rapid Postioning X Y
- ^G00 Z(\d*.\d*) $             - Rapid Postioning Z
- ^G01 Z-(\d*.\d*)$             - Linear Interpolation -Z (no feedrate change)
- ^G01 Z(\d*.\d*)$              - Linear Interpolation Z (no feedrate change)
- ^G01 Z-(\d*.\d*) F(\d*.\d*) $ - Linear Interpolation -Z (feedrate change)
- ^G01 X-(\d*.\d*) Y(\d*.\d*) F(\d*.\d*) $ - Linear Interpolation X Y (feedrate change)
- ^G04 P(\d*.\d*)$   						 - Dwell in milliseconds
- G90 							- Absolute Programming
- M03 - Spindle On (clockwise)
- M04 - Spindle On (counter) - not in code but good to have
- M05 - Spindle stop
- ^\n$ - empty line