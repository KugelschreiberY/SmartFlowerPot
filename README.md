# ( Pot )<sup>2</sup>

This repository offers instructions to build a smart flower pot. The final prototype will offer a userinterface to monitor the plants condition and controll watering. One can also built simply the flowerpot without the monitoring application (more information below).

Following these instructions allows you to build your own self-watering flower pot, which can look like this:

![Flower Pot Example](/images/FlowerPotExample.jpg)


## Hardware

- Microcontroller 
- Water Pump
- Soil Humidity Sensor
- Battery pack (3.3V and 6-9V)
- plastic pipe
- 

## Instructions

In this section we give a guideline how to build the flowerpot and set up the software. You don't have to stick to all steps necessarily, there are a lot of different implementations, especially concerning the circuit when you use another type of microcontroller. 

### Instructions/Recommendations for building the flower pot

TODO

Below you can find how the circuit can look like.

![Flower Pot Example](/images/circuit/circuitIncludingParts.jpg)

I am not really happy using two batteries, but since the power supply from the microcontroller doesn't suffice the water pump this is one possible solution.

### Instructions for setting up the micro controller

TODO

### Instructions for setting up the web application

In this section we explain how to set up the web interface to monitor the plant. Here we use a fork of anythingcontroller and epiccontrol by Menno van der Zee.

![Web Userinterface Example](/images/MonitoringSystem.png)

The server requires python and node.js, please note that it only runs on unix based operation systems i.e. MacOS or Linux. Note that the testing and most of the developemnt was performed on MacOS.

#### Using the flower pot without the monitoring system

You can also build the flower pot and use it without the monitoring system mentioned above. To do so, please comment out the wifi-initialization and http-requests in the respective sketch that should be uploaded to your microcontroller. (TODO offer a file without those connections).
