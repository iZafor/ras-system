#include <OneWire.h>
#include <DallasTemperature.h>

#define PH_SENSOR_PIN A0          
#define TURBIDITY_SENSOR_PIN A1
#define TEMPERATURE_SENSOR_PIN 8

String str;

float b, phOffset= 4.6;

OneWire oneWire(TEMPERATURE_SENSOR_PIN);
DallasTemperature sensors(&oneWire);	

float get_ph();
float get_ntu();
float get_temp();

void setup() {
  Serial3.begin(115200);  
}

void loop() {
  float phValue = get_ph();
  float ntu = get_ntu();
  float temp = get_temp();

  str = "";
  str += phValue;
  str += ",";
  str += ntu;
  str += ",";
  str += temp;

  Serial3.println(str);
  delay(5000);
}

float get_ph() {
  float phAvgValue = 0.0;
  int ph_buf[10], temp;

  for(int i=0;i<10;i++) {      //Get 10 sample value from the sensor for smooth the value
    ph_buf[i]=analogRead(PH_SENSOR_PIN);
    delay(10);
  }
  
  for(int i=0;i<9;i++) {      //sort the analog from small to large
    for(int j=i+1;j<10;j++) {
      if(ph_buf[i]>ph_buf[j]) {
        temp=ph_buf[i];
        ph_buf[i]=ph_buf[j];
        ph_buf[j]=temp;
      }
    }
  }

  for(int i=2;i<8;i++) {                     
    phAvgValue+=ph_buf[i];
  }

  float phValue=(float)phAvgValue*5.0/1024.0/6; 
  phValue=phValue+phOffset;                      

  if(phValue<0) {
    phValue=0;
  }else if (phValue > 14) {
    phValue=14;
  }

  return phValue;
}

float get_ntu() {
  int read_adc = analogRead(TURBIDITY_SENSOR_PIN);
  read_adc = read_adc > 208 ? 208 : read_adc;
  return map(read_adc, 0, 208, 300, 0);
}

float get_temp() {
  sensors.requestTemperatures(); 
  return sensors.getTempCByIndex(0);
}

float round_to_dp( float in_value, int decimal_place ) {
  float multiplier = powf( 10.0f, decimal_place );
  in_value = roundf( in_value * multiplier ) / multiplier;
  return in_value;
}