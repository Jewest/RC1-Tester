/*
  Read the current and switch the IO
*/

#define EXTRA_BITS (4)

bool g_sendString = false;  // whether the string is complete
String data = "";

// pins for the LEDs:
const int interlockPin = 5;
const int startPin = 6;

void setup() {
  // initialize serial communications at 115200 bps:
  Serial.begin(115200);
  // set the time out to 10 mS.
  Serial.setTimeout(10);
  // set the iopins 
  // make the pins outputs:  
  pinMode(interlockPin, OUTPUT);
  pinMode(startPin, OUTPUT);
  
  //default no active
  digitalWrite(startPin, HIGH);
  digitalWrite(interlockPin, HIGH);
}


void loop()
{
   
   // look for the newline. That's the end of your sentence:
   while (Serial.available() > 0) 
   {
    data += Serial.readString();// read the incoming data as string

    if(data.indexOf('\n') > 0)
    {
      //convert to uppercase
      data.toUpperCase();
      
      if(data.indexOf("*IDN?") >= 0)
      {
        Serial.println("RC1 Controller, 0.1.0");
      }
      else if(data.indexOf("CURRENT?") >= 0)      
      {
        SendCurrentMeasurement();
      }
      else if(data.indexOf("LSR 1") >= 0)
      {
        // the pin is low active 
       digitalWrite(startPin, LOW);
        
        Serial.println("OK");
      }
      else if(data.indexOf("LSR 0") >= 0)
      {
        // the pin is low active 
        digitalWrite(startPin, HIGH);
        Serial.println("OK");
      }
      else if(data.indexOf("INTL 1") >= 0)
      {
        // the pin is low active 
        digitalWrite(interlockPin, LOW);
        Serial.println("OK");
      }
      else if(data.indexOf("INTL 0") >= 0)
      {
        // the pin is low active 
        digitalWrite(interlockPin, HIGH);
        Serial.println("OK");
      }
      else
      {
        Serial.println("ERROR 1");
      }
      //clear the data
      data = "";
    }



   }
}


void SendCurrentMeasurement() 
{

  uint32_t sensorValue1 = 0;
  uint32_t sensorValue2 = 0;
  uint32_t sensorValue3 = 0;

  for(int counter = 0; counter < (1 << EXTRA_BITS); ++counter)
  {
    // read the analog in value:
    sensorValue1 += analogRead(A0);
    sensorValue2 += analogRead(A1);
    sensorValue3 += analogRead(A2);
  }
  // map it to the range of the analog out:
  Serial.print(map(sensorValue1, 0, ((1 << (10 + EXTRA_BITS))-1), 0, 5000));
  Serial.print(";");
  Serial.print(map(sensorValue2, 0, ((1 << (10 + EXTRA_BITS))-1), 0, 5000));
  Serial.print(";");
  Serial.println(map(sensorValue3, 0, ((1 << (10 + EXTRA_BITS))-1), 0, 5000));
  
}
