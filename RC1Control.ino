/*
  Read the current and switch the IO
*/

#define EXTRA_BITS (4)

bool g_sendString = false;  // whether the string is complete
String data = "";

// pins for the LEDs:
const int interlockPin = 5;
const int startPin = 6;
const int pwmPinStart = 2;      // PWM to pin 2
const int fiberPin = 13;

void setup() {
  // initialize serial communications at 115200 bps:
  Serial.begin(115200);
  // set the time out to 10 mS.
  Serial.setTimeout(10);
  // set the iopins 
  // make the pins outputs:  
  pinMode(interlockPin, OUTPUT);
  pinMode(startPin, OUTPUT);
  pinMode(pwmPinStart, OUTPUT);  // sets the pin as output
  pinMode(pwmPinStart + 1, OUTPUT);  // sets the pin as output
  pinMode(pwmPinStart + 2, OUTPUT);  // sets the pin as output
  pinMode(fiberPin, OUTPUT);  // sets the pin as output
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
    
  //default no active
  digitalWrite(startPin, HIGH);
  digitalWrite(interlockPin, HIGH);
  analogWrite(pwmPinStart, 0);
  analogWrite(pwmPinStart + 1, 0);
  analogWrite(pwmPinStart + 2, 0);
  digitalWrite(fiberPin, LOW);

  
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
        Serial.println("RC1 Controller, 0.2.100");
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
      else if(data.indexOf("FIBER 0") >= 0)
      {
        digitalWrite(fiberPin, LOW);
        Serial.println("OK");
      }
      else if(data.indexOf("FIBER 1") >= 0)
      {
        digitalWrite(fiberPin, HIGH);
        Serial.println("OK");
      }
      
      else if(data.indexOf("PWM") >= 0)
      {
        int indexSpace = data.indexOf(" ");

        if(indexSpace >= 0)
        {
          data = data.substring(indexSpace + 1);

          char tmpBuffer[50];
          int port = 0;
          int setting = -1;
          data.toCharArray(tmpBuffer, 50);
          
          int convert = sscanf(tmpBuffer, "%i,%i", &port,&setting);
          // 168 is max to 3.3 volt
          if(setting > 169)
          {
            setting = 169;
          }
          else if(setting < 0)
          {
            setting = 0;
          }
          
          analogWrite(pwmPinStart + port - 1, setting);
          Serial.println("OK");
        }
        else
        {
          Serial.println("ERROR 1");
        }
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
