
#include <Servo.h>
#define PIN_SERVO 10
#define PIN_IR A0
#define S 380
#define E 200
float EMA=0;
float w=0.1;
Servo myservo;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
myservo.attach(PIN_SERVO); 
myservo.writeMicroseconds(1600);
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
  EMA=raw_dist*w+(1-w)*EMA;
  Serial.print("min:0,max:500,dist:");
  Serial.print(raw_dist);
  if(EMA<E){
    Serial.print("r:");
  Serial.println(2100-(EMA-E)/E*200);
    myservo.writeMicroseconds(2100-(EMA-E)/280*200);
    }
    else if (EMA>S){
      Serial.print("r:");
  Serial.println(2100+(EMA-S)/S*400);
      myservo.writeMicroseconds(2100+(EMA-S)/S*400);
      }
    else{
      Serial.println("r:2150");
      myservo.writeMicroseconds(2150);
      }
  delay(10);
  
  }
