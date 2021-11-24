#include <Servo.h>
#define PIN_SERVO 10
#define PIN_IR A0
float EMA=0;
float w=0.1;
Servo myservo;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
myservo.attach(PIN_SERVO); 
myservo.writeMicroseconds(1500);
}
float ir_distance(void){ // return value unit: mm
  float val;
  float volt = float(analogRead(PIN_IR));
  val = ((6762.0/(volt-9.0))-4.0) * 10.0;
  return val;
}

void loop() {
  // put your main code here, to run repeatedly:
  float raw_dist = ir_distance();
  float volt=float(analogRead(PIN_IR));
  EMA=volt*w+(1-w)*EMA;
  Serial.print("min:0,max:500,dist:");
  Serial.println(raw_dist);
  Serial.println(EMA);
  if(390<EMA){
    myservo.writeMicroseconds(1700);
    }
    else{
      myservo.writeMicroseconds(1400);
    }
  delay(20);
}
