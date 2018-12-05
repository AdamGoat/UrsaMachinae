import MotorMagic
from MotorMagic import *
import encoder_attempt
from encoder_attempt import * 
import rotary_encoder

ENCODER1_PINA = 17
ENCODER1_PINB = 18
ENCODER2_PINA = 0
ENCODER2_PINB = 0
ENCODER3_PINA = 0
ENCODER3_PINB = 0
ENCODER4_PINA = 0
ENCODER4


global pos1

def callback1(way):
    #global pos1
    pos1 += way
    print("pos1={}".format(pos1))

def callback2(way):
    global pos2
    pos2 += way
    print("pos2{}".format(pos2))

def callback3(way):
    global pos3
    pos3 += way
    print("pos3{}".format(pos3))

def callback4(way):
    global pos4
    pos4 += way
    print("pos4{}".format(pos4))

atexit.register(MotorMagic.turnOffMotors)

pi = pigpio.pi()

pos1 = 0
pos2 = 0
pos3 = 0
pos4 = 0
decoder1 = rotary_encoder.decoder(pi,17,18,callback1)

while(pos1 > -231*3*4):
    MotorMagic.pivotLeft()

MotorMagic.turnOffMotors()
GPIO.cleanup()
