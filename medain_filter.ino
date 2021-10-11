// Arduino pin assignment
#define PIN_LED 9
#define PIN_TRIG 12
#define PIN_ECHO 13

// configurable parameters
#define SND_VEL 346.0 // sound velocity at 24 celsius degree (unit: m/s)
#define INTERVAL 25 // sampling interval (unit: ms)
#define _DIST_MIN 100 // minimum distance to be measured (unit: mm)
#define _DIST_MAX 300 // maximum distance to be measured (unit: mm)
#define _DIST_ALPHA 0.5 // EMA weight of new sample (range: 0 to 1). Setting this value to 1 effectively disables EMA filter.

// global variables
float timeout; // unit: us
float dist_min, dist_max, dist_raw, alpha; // unit: mm
unsigned long last_sampling_time; // unit: ms
float scale; // used for pulse duration to distance conversion

float Median[30]={0};
int sampleCount[3]={3,10,30};

void swap(int *a, int *b)
{
    int temp = *a; 
    *a = *b; 
    *b = temp;
}
 
// 퀵소트 함수 
void qSort(int arr[], int start, int end)
{
    if(start >= end) return;
    
    int pivot = arr[start];
    
    int i = start, j = end;
    
    while(i < j)
    {
        while(pivot < arr[j]) j--;
        while(i < j && pivot >= arr[i]) i++;
        swap(arr + i, arr + j);
    }
    
    swap(arr + start, arr + j);
    
    qSort(arr, start, j - 1);
    qSort(arr, j + 1, end);
}


void setup() {
// initialize GPIO pins
  pinMode(PIN_LED,OUTPUT);
  pinMode(PIN_TRIG,OUTPUT);
  digitalWrite(PIN_TRIG, LOW);
  pinMode(PIN_ECHO,INPUT);

// initialize USS related variables
  dist_min = _DIST_MIN; 
  dist_max = _DIST_MAX;
  alpha = _DIST_ALPHA;
  timeout = (INTERVAL / 2) * 1000.0; // precalculate pulseIn() timeout value. (unit: us)
  dist_raw = 0.0; // raw distance output from USS (unit: mm)
  scale = 0.001 * 0.5 * SND_VEL;

// initialize serial port
  Serial.begin(57600);

// initialize last sampling time
  last_sampling_time = 0;
}

void loop() {
// wait until next sampling time. 
// millis() returns the number of milliseconds since the program started. Will overflow after 50 days.
  if(millis() < last_sampling_time + INTERVAL) return;

// get a distance reading from the USS
  dist_raw = USS_measure(PIN_TRIG,PIN_ECHO);
  
  int i;
  for(i=1;i<30;i++){
    Median[i]=Median[i+1];
  }
  Median[i] = dist_raw;
  int resultMedian[3];
  for(i=0;i<3;i++){
    int* arr = (int*)malloc(sizeof(int) * sampleCount[i]); 
    int cnt=0;
    for(int j=30-sampleCount[i];j<30;j++,cnt++){
      arr[cnt]=Median[j];
    }
    qSort(arr,0,sampleCount[i]-1);
    resultMedian[i]=arr[sampleCount[i]/2];
    free(arr);
  }

// output the read value to the serial port
  Serial.print("Min:0,");
  Serial.print("raw:");
  Serial.print(map(dist_raw,0,400,100,500));
  Serial.print(",");
  Serial.print("median(N=3):");
  Serial.print(map(resultMedian[0],0,400,100,500));
  Serial.print(",");
  Serial.print("median(N=10):");
  Serial.print(map(resultMedian[1],0,400,100,500));
  Serial.print(",");
  Serial.print("median(N=30):");
  Serial.print(map(resultMedian[2],0,400,100,500));
  Serial.print(",");
  Serial.println("Max:500");

// turn on the LED if the distance is between dist_min and dist_max
  if(dist_raw < dist_min || dist_raw > dist_max) {
    analogWrite(PIN_LED, 255);
  }
  else {
    analogWrite(PIN_LED, 0);
  }

// update last sampling time
  last_sampling_time += INTERVAL;
}

// get a distance reading from USS. return value is in millimeter.
float USS_measure(int TRIG, int ECHO)
{
  float reading;
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  reading = pulseIn(ECHO, HIGH, timeout) * scale; // unit: mm
  if(reading < dist_min || reading > dist_max) reading = 0.0; // return 0 when out of range.
  return reading;
}
