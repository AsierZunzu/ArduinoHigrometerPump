# Arduino water planter with hygrometer & RTC
An Arduino project dedicated to managing a plant watering pump, based on an hygrometer's value, with a RTC clock to avoid noises late at night. 

## Hardware
### MK1
First try worked pretty well, but the hygrometer was of poor quality, so eventually it got rusty. A pin broke and the system took that reading as an absolute lack of humidity. It overflowed the plant until no water was left on the reservoir. The hardware was stored on a carboard box under the plant to reduce space usage, but when the plant got flooded most of the water ended up soaking the box and damaging the hardware. 
![MK1](https://raw.githubusercontent.com/AsierZunzu/ArduinoHygrometerPump/master/Images/Auto-Watering%20Plant%20%5BArduino%5D%20(MK1).jpg)

Due to lack of nearby power connections, it used to run on an USB battery pack, which lasted for a week or so.

TODO Add hardware and connections info.

### MK2
Lesson learned. I have added some hardware failure alerts on the code and encased the hardware on a plastic box. This time the plant will be near a power connection, so no battery is needed.

TODO Add images.

TODO Add hardware and connections info.