#################### MotorMagic is a copy of the DCTest.py example that has other junk added to it for
#################### learning how to control the motors
#!/usr/bin/python
from Adafruit_MotorHAT import Adafruit_MotorHAT, Adafruit_DCMotor

import time
import atexit

# create a default object, no changes to I2C address or frequency
mh = Adafruit_MotorHAT(addr=0x61)

# recommended for auto-disabling motors on shutdown!
def turnOffMotors():
    mh.getMotor(1).run(Adafruit_MotorHAT.RELEASE)
    mh.getMotor(2).run(Adafruit_MotorHAT.RELEASE)
    mh.getMotor(3).run(Adafruit_MotorHAT.RELEASE)
    mh.getMotor(4).run(Adafruit_MotorHAT.RELEASE)

atexit.register(turnOffMotors)

# Dillon: trying to create functions for pivoting
def pivotLeft():
    frontLeft.run(Adafruit_MotorHAT.FORWARD)
    frontRight.run(Adafruit_MotorHAT.FORWARD)
    backLeft.run(Adafruit_MotorHAT.FORWARD)
    backRight.run(Adafruit_MotorHAT.FORWARD)
    frontLeft.setSpeed(255)
    frontRight.setSpeed(255)
    backLeft.setSpeed(255)
    backRight.setSpeed(255)


def pivotRight():
    frontLeft.run(Adafruit_MotorHAT.BACKWARD)
    frontRight.run(Adafruit_MotorHAT.BACKWARD)
    backLeft.run(Adafruit_MotorHAT.BACKWARD)
    backRight.run(Adafruit_MotorHAT.BACKWARD)

################################# DC motor test!
#myMotor = mh.getMotor(3)
frontLeft = mh.getMotor(1)
frontRight = mh.getMotor(2)
backLeft = mh.getMotor(3)
backRight = mh.getMotor(4)

# set the speed to start, from 0 (off) to 255 (max speed)
#myMotor.setSpeed(150)
#myMotor.run(Adafruit_MotorHAT.FORWARD);
# turn on motor
#myMotor.run(Adafruit_MotorHAT.RELEASE);


#while (True):
#    print("Forward! ")
#    frontLeft.run(Adafruit_MotorHAT.BACKWARD)
#    frontRight.run(Adafruit_MotorHAT.FORWARD)
#    backLeft.run(Adafruit_MotorHAT.BACKWARD)
#    backRight.run(Adafruit_MotorHAT.FORWARD)

#    frontLeft.setSpeed(255)
#    frontRight.setSpeed(255)
#    backLeft.setSpeed(255)
 #   backRight.setSpeed(255)

#    time.sleep(1.0)
#    turnOffMotors()
#    time.sleep(1.0)

#    print("Pivot Left! ")
#    pivotLeft()
#    time.sleep(2.0)
#    turnOffMotors()
#    time.sleep(1.0)

#    print("Pivot Right! ")
#    pivotRight()
#    time.sleep(2.0)
#    turnOffMotors()

#    myMotor.run(Adafruit_MotorHAT.FORWARD)

#    myMotor.setSpeed(255)
#    print("\tSpeed up...")
#    for i in range(255):
#        myMotor.setSpeed(i)
#        time.sleep(0.01)

#    print("\tSlow down...")
#    for i in reversed(range(255)):
#        myMotor.setSpeed(i)
#        time.sleep(0.01)

 #   print("Backward! ")
#    myMotor.run(Adafruit_MotorHAT.BACKWARD)

 #   print("\tSpeed up...")
#    for i in range(255):
#        myMotor.setSpeed(i)
#        time.sleep(0.01)

 #   print("\tSlow down...")
#    for i in reversed(range(255)):
#        myMotor.setSpeed(i)
#        time.sleep(0.01)

  #  time.sleep(1.0)
#    print("Release")
  #  turnOffMotors()
#    myMotor.run(Adafruit_MotorHAT.RELEASE)
#    time.sleep(2.0)
