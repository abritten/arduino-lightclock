#include <IRremote.h>


int IRpin = 11;
IRrecv irrecv(IRpin);
decode_results results;


void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn();
  pinMode(13,OUTPUT);

}

  
void loop() 

{   
  if (irrecv.decode(&results)) 
    {
     Serial.println(results.value, HEX);
      delay(10);
      if(results.value==0xCxxxxO)
      
      digitalWrite(13,HIGH); 
      delay(500);
      digitalWrite(13,LOW);
     irrecv.resume(); 
    }
}    