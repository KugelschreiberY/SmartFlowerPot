# ( Pot )<sup>2</sup>

This repository offers instructions to build a smart flower pot. The final prototype will offer a user interface to monitor the plants condition and control watering. One can also built simply the flowerpot without the monitoring application (more information below).

Following these instructions allows you to build your own self-watering flower pot, which can look like this:

![Flower Pot Example](/images/FlowerPotExample.jpg)

## Suggested Hardware

- Adafruit Feather M0 Wifi
- waterproof water pump DC 3V
- soil moisture sensor
- water level sensor
- battery pack (1x 3.3V for Feather and 1x 6-9V for the water pump)
- plastic pipe 

- transistor (NPN BC547)
- base resistor 220 Ohm
- jumper wires

## Instructions

In this section we give a guideline how to build the flowerpot and set up the software. You don't have to stick to all steps necessarily. There are a lot of different implementations, especially concerning the circuit when using another type of micro controller. 

### Instructions for building the flower pot

Below you can find how the circuit can look like.

![Flower Pot Example](/images/circuit/circuitIncludingParts.jpg)

Mapping to the pins of the sensors and actuators is as follows:
- A2: water level sensor
- A1: soil moisture sensor
- 10: collector of transistor

Although I am not happy using two batteries, this is one solution to the problem that the power supply of the micro controller (3.3V) does not suffice for the water pump.
Please note as well that instead of the Adafruit Feather you can use any other micro controller. An Arduino Uno for example has the advantage of now requiring another power source (but the lack of no wifi: see section "Using the flower pot without the monitoring system").  

To offer a nicely distributed water supply for your plant, you can create a plastic pipe circuit, which is connected to the water pump.

TODO: offer picture of water pump circuit

### Instructions for setting up the micro controller

First you need to enter the wifi network you are using into the first lines of the sketch
```c
char ssid[] = "your-ssid";      // your network SSID (name)
char pass[] = "your-password";  // your network password (use for WPA, or use as key for WEP)
```

To upload the sketch, simply start the Arduino IDE and connect your micro controller.

### Instructions for setting up the web application

Now we set up the web interface to monitor the plant, we use the anythingcontroller and epiccontrol by Menno van der Zee. (Todo add reference)

![Web Userinterface Example](/images/MonitoringSystem.png)

The server requires python and node.js, please note that it only runs on unix based operation systems i.e. MacOS or Linux. The testing and most of the development was performed on MacOS.

#### Using the flower pot without the monitoring system

You can also build the flower pot and use it without the monitoring system mentioned previously e.g. if you are using a micro controller without a wifi module. To do so, please comment out the wifi initialization and http-requests in the respective sketch that should be uploaded to your micro controller. 

If you have any questions or suggestions for optimisation feel free to open an issue or contact me via email.