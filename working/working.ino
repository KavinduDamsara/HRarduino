
#define USE_ARDUINO_INTERRUPTS false
#include <PulseSensorPlayground.h>
#include <SoftwareSerial.h>

const int OUTPUT_TYPE = SERIAL_PLOTTER;
int myBPM;

const int PULSE_INPUT = A1;
const int PULSE_BLINK = 13;    // Pin 13 is the on-board LED
const int PULSE_FADE = 5;
const int THRESHOLD = 550;   // Adjust this number to avoid noise when idle
const int maxRateReal = 160;
const int minRateReal = 50;
const int arraySize = 10;
int arrayIndex = 0;
int HRarray[arraySize];

byte samplesUntilReport;
const byte SAMPLES_PER_SERIAL_SAMPLE = 10;

PulseSensorPlayground pulseSensor;
//SoftwareSerial mySerial(0, 1);

void setup() {
  
  Serial.begin(9600);
  //mySerial.begin(9600);

  // Configurations....
  pulseSensor.analogInput(PULSE_INPUT);
  pulseSensor.blinkOnPulse(PULSE_BLINK);
  pulseSensor.fadeOnPulse(PULSE_FADE);

  //pulseSensor.setSerial(Serial);
  pulseSensor.setOutputType(OUTPUT_TYPE);
  pulseSensor.setThreshold(THRESHOLD);
  
  samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;
  
//  if (!pulseSensor.begin()) {
//    for(;;) {
//      // Flash the led to show things didn't work.
//      digitalWrite(PULSE_BLINK, LOW);
//      delay(50);
//      digitalWrite(PULSE_BLINK, HIGH);
//      delay(50);
//    }
//  }
}

void loop() {

 
  if (pulseSensor.sawNewSample()) {
    
    if (--samplesUntilReport == (byte) 0) {
      samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;

      pulseSensor.outputSample();

      if (pulseSensor.sawStartOfBeat()) { 
      myBPM = pulseSensor.getBeatsPerMinute(); 
      
      //Serial.println(myBPM); 
      if(myBPM >= minRateReal && myBPM <= maxRateReal){
        //Serial.print("BPM:mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm "); 
        getAverage(myBPM);
      }
      else{
      //  Serial.println("unReal");
      }
      }
    }
  }
}
void getAverage(int rate){
  HRarray[arrayIndex] = rate;
  arrayIndex ++;
  if(arrayIndex == arraySize){
    arrayIndex = 0;
    sendAverageRate();
  }
}

//filtering function to refuse ab normal values.
void sendAverageRate(){
  int sum = 0;
  for(int x=0; x<arraySize; x++){
    sum += HRarray[x]; 
  }
  int avgBPM = sum/arraySize;
  //Serial.print("BPM:mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm "); 
  //Serial.println(avgBPM);
  sendAndroidValues(avgBPM);
}

void sendAndroidValues(int BPM)
 {
  float BPMF = BPM;
  
  Serial.print('#');
  Serial.print(BPMF);
  Serial.print('~'); // end of transmission
  Serial.println();
 //delay(10);       
}

