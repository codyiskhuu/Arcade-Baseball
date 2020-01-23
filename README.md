# Aracade Baseball Machine

![alt text](https://github.com/codyiskhuu/Arcade-Baseball-Machine-/blob/master/Images/Copy%20of%20project.png "Project")

The Arcade Baseball Machine is build with 3 important concepts. (TLDR)

* Raspberrypi Zero W: that allowed me to control a solenoid that hits a ball through an accelerometer.
* An ATMEGA 1284 that communicated with the raspberrypi through bluetooth that took and processes the swings from the raspberrypi
  * Through SPI (Serial Peripheral Interface) the master ATMEGA takes the sensor inputs when the ball is shot into a one of the 3 holes and sends it over to the follower ATMEGA
* The follower ATMEGA 1284 recieves the data from the master ATMEGA and changes the baseball field and it properly keeps track of points, strikes, if there are players on the base and which section/sensor is good for points or not.
