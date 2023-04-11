#include"Arduino.h"
#include"String.h"
#include<SoftwareSerial.h>
int button=2;
int X=A4;
int Y=A5;/*摇杆*/
int R = 4;/*LED*/
int G = 5;
int B = 6;
int color; /*1 = hong 2 = lv 3 = huang*/
bool Auto;
bool starton;
int state;
int w;
char output;
char oldoutput;
char input[10];
int k = 0;
bool zerotimeron;
int zerotimer;
int curtime;
int oldtime;
int zerotimes;
bool ownAuto;
bool newsend;
void setup() {
  // put your setup code here, to run once:
   pinMode(button,INPUT_PULLUP);
   pinMode(R,OUTPUT);
   pinMode(G,OUTPUT);
   pinMode(B,OUTPUT);
   Serial.begin(9600);/*蓝牙接硬串口连电脑*/
   digitalWrite(R,LOW);
   digitalWrite(G,LOW);
   digitalWrite(B,LOW);
   Auto = false;
   starton = false;
   attachInterrupt(0,isr,FALLING);
   w = 0;
   oldtime = 0;
   zerotimeron = false;
   zerotimer = 0;
   zerotimes = 3;
   newsend = true;
}
void isr()
{
  if(!starton)
  {
    output = '1';
    oldoutput = '0';
    
  }
 else
 {
   if(Auto) 
   {
    w = 0;
   }
   else
   {
    w = 1;
   }
 }
}
void loop() {
  // put your main code here, to run repeatedly:
    curtime =millis();
    if(zerotimeron){zerotimer = zerotimer + curtime - oldtime;}
    oldtime = curtime;
    int a = analogRead(X);
    int b = analogRead(Y);
    if(a < 100 && b < 100){state = 1;}/*在组装的时候，X轴正方向在左侧，Y轴正方向在上侧*/
    if(a > 450 && a < 600 && b < 100){state = 2;}
    if(a > 900 && b < 100){state = 3;}
    if(a < 100 && b > 400 && b < 600){state = 4;}
    if(a > 450 && a < 600 && b > 450 && b < 600){state = 0;}
    if(a > 900 && b > 450 && b < 600){state = 5;}
    if(a < 100 && b > 900){state = 6;}
    if(a > 450 && a < 600 && b > 900){state = 7;}
    if(a > 900 && b > 900){state = 8;}
    if(starton)
    {
      output = char(state + int('a'));
    }
    while(Serial.available())
    {
         input[k] = Serial.read();
         k++;
         if(input[k - 1] == '\n'){k = 0;break;}
    }
      if(input[0] == '0')
      {
        color = 2;
        starton = true;
        Auto = false;
        ownAuto = false;
      }
      if(input[0] == '1')
      {
         color = 1;
         starton = true;
         Auto = false;
         ownAuto =  false;
      }
      if(input[0] == '2')
      {
        color = 3;
        starton = true;
        Auto = true;
        ownAuto = false;
        
      }
      if(input[0] == '3')
      {
        color = 0;
        starton = false;
        Auto = false;
        ownAuto = false;
      }
      if(input[0] == '4')
      {
        color = 3;
        starton = true;
        Auto = true;
        ownAuto = true;
      
      }
    
    
    switch(color)
    {
      case 0:{digitalWrite(R,LOW);digitalWrite(G,LOW);digitalWrite(B,LOW);break;}
      case 1:{digitalWrite(R,HIGH);digitalWrite(G,LOW);digitalWrite(B,LOW);break;}
      case 2:{digitalWrite(R,LOW);digitalWrite(G,HIGH);digitalWrite(B,LOW);break;}
      case 3:{digitalWrite(R,LOW);digitalWrite(G,LOW);digitalWrite(B,HIGH);break;}
    }

      if(ownAuto)
      {
        w = 1;
      }
      if(w == 1 && starton)
      {
        output = char(int(output) - int('a') + int ('A'));
      }
      
      if( output != oldoutput && newsend)
      {
        zerotimeron = true;
        zerotimes = 0;
        zerotimer = 1000;
        oldoutput = output;
        newsend = false;
        
      }
      if( zerotimer > 500 && zerotimes < 3)
      {
        Serial.write(output);
        Serial.write('\n');
        zerotimer = 0;
        zerotimes += 1;
        delay(200);
      
      }
      if(zerotimes >= 3)
      {
        zerotimes = 3;
        newsend = true;
        zerotimer = 0;
        zerotimeron = false;
      
      }
    

 
    
} 
