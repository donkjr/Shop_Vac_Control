// Remote Shop Vac Control Code
// Modified By DonKJr on 6/6/19
// Code & design modified from bytesized @ https://www.youtube.com/watch?v=VHgaBxTPEPM
// https://www.bytesizedengineering.com/projects/shop-vac-auto-switch
//This code controls a shop vac with the tool that its hose is connected to . 
//When the TOOL that is plugged into the tool AC socket is turned on ....
//the SHOP VAC that is plugged into the vacuumn socket turns on after a 1 second delay (reduces surge).
// When the TOOL is turned off the SHOP VAC turns off after a 2 second delay.
// This sketch is tuned for an Arduino Nano V3.0 buy at https://amzn.to/2WUfGue
// A manual on/off overide switch is provided that has the same on-off delays as in the automatic mode

//V1.2
#define CURRENT_SENSOR_PIN A0 // Current sensor is connected to analog pin A0
#define RELAY_PIN 2 // Relay module is connected to digital pin 2 (low true)
#define CURRENT_THRESHOLD 600 // The analog value above which the relay shall be triggered
                              // this value will depend on 
                              // the amperage rating of the ACS712 sensor being used (5,20,30amps)
                              // the current the tool draws 
                              // and the value of the sensors VCC. 
                              // Caution: every sensor has a slightly different sensitivity. You may have to initially adjust the CURRENT_THRESHOLD for each sensor
                              // Note: a pot could be put into the design to adjust this threshold
                              // For AC input the ACS712 sensor value will swing aroung VCC/2
                              //  Sensor Current Rating(A)  Sensitivity (mV/A) 
                              //  5                           185
                              //  20                          100
                              //  30                          60
#define CURRENT_SAMPLE_PERIOD 500 // The number of milliseconds to sample the current reading
#define RELAY_SHUTOFF_DELAY 2500 // The number of milliseconds to delay shutting off the relay after the tool has been shutoff
#define RELAY_ON_DELAY 1000      // Delay turning on Vacumn to reduce surge
#define OVERRIDE_SW_PIN 3 // Added a manual ON-OFF switch
#define DEBUG 1           //  General debug display. Comment out this line for non display
//#define DEBUG2 1           // Adds detailed output of tools current measurement. Comment this out to stop display
#define VER 1.3
//1.3 added debug functions adjusted threshold value up
// 1.3 Bug: relay turns on when board powers up without serial port plugged in.
int analogValue = 0; // Stores ADC values read in from the current sensor
unsigned long stopwatch = 0; // Used to keep track of elapsed time
boolean relayPinState = HIGH; // Stores the current state of the relay pin

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN,OUTPUT);
  pinMode(OVERRIDE_SW_PIN,INPUT_PULLUP);
  digitalWrite(RELAY_PIN, HIGH); // initialize the relay to off
  Serial.print("ShopVac Controller V");
  Serial.println(VER);
  Serial.println("by Don Kleinschnitz");
  Serial.println("...running");
}

void loop() 
{
  analogValue = 0; // Initialize analogValue to a known state
  stopwatch = millis(); // Store a snapshot of the current time since the program started executing
  // Check the manual override switch (added V1.2)
 
  // Collect the max ADC value from the tools current sensor for the predetermined sample period
  while(millis() - stopwatch < CURRENT_SAMPLE_PERIOD) 
    { 
      analogValue = max(analogValue,analogRead(CURRENT_SENSOR_PIN));
      #ifdef DEBUG2   
        Serial.print("MaxCurrent =");
        Serial.print(analogValue);
        Serial.print('\t');
        Serial.print("Setpoint =");
        Serial.println(CURRENT_THRESHOLD);
      #endif  
    }
  
  // If the max ADC value from the tools current sensor exceeds the threshold, set the relay state to LOW
  if(analogValue > CURRENT_THRESHOLD || digitalRead(OVERRIDE_SW_PIN) == LOW ) 
    {
      #ifdef DEBUG 
        if(digitalRead(OVERRIDE_SW_PIN) ==LOW)
          {
            Serial.println("Override SW ON");
          }
       #endif
      relayPinState=LOW; //set the vacuum relay to ON by changing the state
      #ifdef DEBUG 
        Serial.print("Threshold Exceeded with = ");
        Serial.println(analogValue);
        Serial.println("waiting ON delay ....");
      #endif
        delay(RELAY_ON_DELAY);
      #ifdef DEBUG 
        Serial.println("Relay ON");
      #endif
    }
 
  else  // turn off the vacuum if the current drops below setpoint
    {// Otherwise if the relay state was just LOW (ON) wait for the predetermined shutoff delay and set the state to HIGH
      if(relayPinState == LOW) 
      {
        #ifdef DEBUG
          Serial.println("waiting OFF delay....");
        #endif
        
        delay(RELAY_SHUTOFF_DELAY);
        relayPinState=HIGH;// turn off the vacuum relay
        #ifdef DEBUG
          Serial.println ("Relay off");
        #endif
      }
    }
  digitalWrite(RELAY_PIN,relayPinState); // Write the RELAY_PIN to the correct state
}
