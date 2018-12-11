// Example 45.1 - tronixstuff.com - CC by-sa-nc
// Connect Ping))) signal pin to Arduino digital 8
int signal=8;
int distance3; //3pin
int stop = 6;
int state = 0;
unsigned long pulseduration3=0;  //3pin
void setup()
{
 pinMode(signal, OUTPUT);
 pinMode(2, OUTPUT); //GPIO Pin that will be set high when the sensors senses a number <= the stop value
 Serial.begin(9600);
}
void measureDistance()
{
 digitalWrite(2, LOW);
 // set pin as output so we can send a pulse
 pinMode(signal, OUTPUT);
// set output to LOW
 digitalWrite(signal, LOW);
 delayMicroseconds(5);
 
 // now send the 5uS pulse out to activate Ping)))
 digitalWrite(signal, HIGH);
 delayMicroseconds(5);
 digitalWrite(signal, LOW);
 
 // now we need to change the digital pin
 // to input to read the incoming pulse
 pinMode(signal, INPUT);
 
 // finally, measure the length of the incoming pulse
 pulseduration3=pulseIn(signal, HIGH);
}
void loop()
{
 // get the raw measurement data from Ping)))
 measureDistance();
 
 // divide the pulse length by half
 pulseduration3=pulseduration3/2; 
 
 // now convert to centimetres. We're metric here people...
 distance3 = int(pulseduration3/29);
 
 // Display on serial monitor
// Serial.print("Distance for 3 pin - ");
// Serial.println(distance3);
// Serial.println(" cm");

 if(distance3 <= stop  && state != 1)
 {  
  // Serial.print(" less than or equal too stop value");
   // Serial.print(distance3);
  digitalWrite(2,HIGH);
  Serial.print("A");
  state = 1;

 }
 if(distance3 > stop && state == 1)
 {
  digitalWrite(2,LOW);
  state = 0; 
 }
 
 
 delay(500);
}


