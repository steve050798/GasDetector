#include <LiquidCrystal.h>
LiquidCrystal lcd(12,11,5,4,3,2);
//Resistances given in kilo ohm
float Rl=5;   
float Ro=10;
const int mq=A0;
float clean_air_factor=9.83; //The resistance of the sensor in clean air

//Sample points on resistance to concentration conversion curves
//Taken from characteristic curve in MQ-2 specification document
//data format:(logx,y,slope);
float LPGCurve[3]=  {2.3,0.21,-0.47}; 
float COCurve[3]=  {2.3,0.72,-0.34}; 
float SmokeCurve[3]={2.3,0.53,-0.44};                                                     

void setup() 
{
 pinMode(10,OUTPUT);
 pinMode(6,OUTPUT); 
 pinMode(7,OUTPUT);
 digitalWrite(7,LOW);
 pinMode(8,OUTPUT);
 pinMode(13,OUTPUT);
 digitalWrite(10,LOW);
 digitalWrite(13,HIGH);
 digitalWrite(8,LOW);
 
 Serial.begin(9600);
 Serial.print("Calibrating...\n");
 analogWrite(9,256);
 lcd.begin(16,2);
 lcd.setCursor(0,0);
 lcd.print("Calibrating...");
 delay(1000);
 digitalWrite(8,HIGH);
 Ro=MQcalibration(mq);
 digitalWrite(8,LOW);
 lcd.clear();
 lcd.setCursor(0,0);
 lcd.print("Done");
 lcd.setCursor(0,1);
 lcd.print("Ro=");
 lcd.print(Ro);
 lcd.print(" kohms");
 Serial.print("Done");
 Serial.print(Ro);
 Serial.print(" kohm\n");
 delay(1000); 
}
void beep()
{
  digitalWrite(10,HIGH);
    delay(100);
    digitalWrite(10,LOW);
    delay(100);

}
void blink()
{
    digitalWrite(6,HIGH);
    delay(100);
    digitalWrite(6,LOW);
    delay(100);
} 
void loop() 
{
  analogWrite(9,256);
  digitalWrite(8,HIGH);
  float lpg_ppm=ppm_calc(MQread(mq),Ro,LPGCurve);
  float co_ppm=ppm_calc(MQread(mq),Ro,COCurve);
  float smoke_ppm=ppm_calc(MQread(mq),Ro,SmokeCurve);
  digitalWrite(8,LOW);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Conc(ppm):");
  lcd.setCursor(0,1);
  lcd.print("LPG: ");
  lcd.print((int)lpg_ppm);
  lcd.print(" CO: ");
  lcd.print((int)co_ppm);
  
  Serial.print("\nConcentrations of gases:\n");
  Serial.print("LPG: ");Serial.print(lpg_ppm);Serial.print(" ppm\n");
  Serial.print("CO: ");Serial.print(co_ppm);Serial.print(" ppm\n");
  Serial.print("Smoke: ");Serial.print(smoke_ppm);Serial.print(" ppm\n");
  Serial.print("\n");
  delay(100);
  //CO monoxide upper bound taken from American industrial standard
  // 
  if(co_ppm>25||lpg_ppm>50||smoke_ppm>350)
  {
   blink();
   beep();
   } 
  }

float MQresistance(float Vs)
{
   return (Rl*(1023.0-Vs)/Vs);
}
//Calibration function that calculates Ro using 10 samples
float MQcalibration(int mq)
{
  int i;
  float val=0;
  for (i=0;i<10;i++) 
  { 
    val += MQresistance(analogRead(mq));
    delay(500);
  }
  val = val/2;                                      //calculate the average value
  val = val/clean_air_factor;                                                               
  return val;                                           
  }
float MQread(int mq)
{
  int i;
  float rs=0;
  for (i=0;i<50;i++) {
    rs += MQresistance(analogRead(mq));
    delay(5);
  }
  rs = rs/50;
  return rs;  
}
float ppm_calc(float rs,float ro,float *p)
{
  float ratio=rs/ro;
  //return pow(10,(((ratio-p[1])/p[2])+p[0]));
  return (pow(10,( ((log(ratio)-p[1])/p[2]) + p[0])));
}

