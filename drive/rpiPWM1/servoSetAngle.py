import RPi.GPIO as GPIO
from time import sleep
GPIO.setmode(GPIO.BCM)
GPIO.setup(18,GPIO.OUT)
pwm=GPIO.PWM(18,50)
pwm.start(0)
sleep(1)
def SetAngle(angle):
    duty = angle / 18 + 2
    GPIO.output(18,True)
    pwm.ChangeDutyCycle(duty)
    sleep(2)
    GPIO.output(18,False)
    pwm.ChangeDutyCycle(0)
SetAngle(80)
SetAngle(30)
SetAngle(60)
pwm.stop()
GPIO.cleanup()
