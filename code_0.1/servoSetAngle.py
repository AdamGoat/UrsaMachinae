def SetAngle(angle):
    duty = angle / 18 + 2
    GPIO.output(03,True)
    pwm.ChangeDutyCycle(duty)
    sleep(1)
    GPIO.output(03,False)
    pwm.ChangeDutyCycle(0)
