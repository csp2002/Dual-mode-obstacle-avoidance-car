#include"Arduino.h"
#include"String.h"
#include<SoftwareSerial.h>
unsigned long curtime, oldtime;
int Eco2 = 2,Trig2 = 3;
int Eco1 = 4,Trig1 = 7;
int ENAL = 6,ENAR = 5;
int Trigpin=8;
int Ecopin=9;
float distance;
int IN1L = 10, IN1R = 12;
int IN2L = 11, IN2R = 13;
int countL = 0, countR = 0;
double goalspeedL,goalspeedR;
double outspeedL,outspeedR;
double inspeedL,inspeedR;
double rpmL,rpmR;
double pre_errL = 0.0,pre_errR = 0.0;
double pre_derrL = 0.0,pre_derrR = 0.0;
double Kp = 0.5;
double Ki = 0.05;
double Kd = 0.6;
double outpwmL = 0,outpwmR = 0;
char input[10];
int stoptimer = 0;
int noticetimer = 0;
int turntimer = 0;
bool stopon = false;
bool noticeon = false;
bool turnon = false;
int timertime;
int state = 0;
bool Auto = true;
int turn = 0;
bool leftok = true;
int stoptime = 1000;
int continuetime = 500;
int stopdistance = 15;
int continuedistance = 40;
bool left = true;
bool right = true;
bool starton = false;
String ret;
int k;
char output;
char oldoutput;
float ll=5;
float rr=5;
bool ownAuto = false;
int ownAutotimer = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(IN1L,OUTPUT);
  pinMode(IN2L,OUTPUT);
  pinMode(ENAL,OUTPUT);
  
  pinMode(IN1R,OUTPUT);
  pinMode(IN2R,OUTPUT);
  pinMode(ENAR,OUTPUT);
  pinMode(Trigpin,OUTPUT);
  pinMode(Trig2,OUTPUT);
  pinMode(Ecopin,INPUT);
  pinMode(Eco2,INPUT);
  pinMode(Trig1,OUTPUT);
  pinMode(Eco1,INPUT);
  digitalWrite(IN1L,HIGH);
  digitalWrite(IN2L,LOW);
  digitalWrite(IN1R,HIGH);
  digitalWrite(IN2R,LOW);
  /*attachInterrupt(0,CodeL,FALLING);
  attachInterrupt(1,CodeR,FALLING);*/
  curtime = 0;
  Auto = false;
  k = 0;
}
void leftcheck()
{
  digitalWrite(Trig1,LOW);
  delayMicroseconds(8);
  digitalWrite(Trig1,HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig1,LOW);
  ll = pulseIn(Eco1,HIGH)/58.00;
  
  return;
  
}
void rightcheck()
{
  digitalWrite(Trig2,LOW);
  delayMicroseconds(8);
  digitalWrite(Trig2,HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig2,LOW);
  rr = pulseIn(Eco2,HIGH)/58.00;
  
  return;
  
}
void dist()
{
  digitalWrite(Trigpin,LOW);
  delayMicroseconds(8);
  digitalWrite(Trigpin,HIGH);
  delayMicroseconds(10);
  digitalWrite(Trigpin,LOW);
  distance = pulseIn(Ecopin,HIGH)/58.00;
}

void loop() {
  // put your main code here, to run repeatedly:
  curtime = millis();
  if(stopon){stoptimer += curtime - timertime;}
  if(turnon){turntimer += curtime - timertime;}
  if(ownAuto){ownAutotimer += curtime - timertime;}  
  
  while(Serial.available())
  {
    input[k] = Serial.read();
    k++;
    if(input[k - 1] == '\n'){k = 0;break;}
  }
  if(input[0] == '1') {starton = true;delay(100);}
  else
  {
    int w =  int(input[0]);
    if( w >= int('a') && w < int('z'))
    {
      Auto = false;
      state = w - int('a');
    }
    if( w >= int('A') && w < int('Z') )   
    {
      Auto = true;
      state = w - int('A');
    }
  }
  if(ownAuto && ownAutotimer < 5000)
  {Auto = true;}
  if(ownAutotimer > 5000 )
  {ownAutotimer = 0;ownAuto = false;}
  if(!starton){Auto = false;}
  if(starton)
  {
    
    dist();
    
    if(distance < stopdistance)
    {
      noticetimer += curtime - timertime;
    }
    else
    {
      noticetimer = 0;
    }
    if(noticetimer > 5000 && !Auto){Auto = true;ownAuto = true;}
    if(noticetimer > 10000){starton = false;}
    if(Auto)
    {
      if(distance <= stopdistance && turn == 0)
      {
         leftcheck();
         rightcheck();


        if(ll > rr){ leftok  = true;}
        else { leftok  = false;}
        turn = 1;
        stopon = true;
        
      }
      if(turn == 1 && stoptimer > stoptime)
      {
        stopon = false;
        stoptimer = 0;
        turnon = true;
        if(leftok)
        {turn = 2;}
        else
        {turn = 3;}
  
      }
      if((turn == 2 || turn == 3) && turntimer > continuetime)
      {
        turn = 0;
        turnon = false;
        turntimer = 0;
      }
      switch(turn)
      {
        case 0:{state = 2;break;}/*前进*/
        case 1:{state = 7;break;}/*后退*/
        case 2:{state = 1;break;}/*左前转*/
        case 3:{state = 3;break;}/*右前转*/
      }
    } 
   
    
    switch(state)
    {
      case 1:{outpwmL = 0; outpwmR = 70;left = true;right = true;break;}/*前进左转*/
      case 0:{outpwmL = 0; outpwmR = 0;left = true;right = true;break;}/*停止*/
      case 2:{outpwmL = 83; outpwmR = 70;left = true;right = true;break;}/*直行*/
      case 3:{outpwmL = 80; outpwmR = 0;left = true;right = true;break;}/*前进右转*/
      case 4:{outpwmL = 50; outpwmR = 50;left = false;right = true;break;}/*原地左转*/
      case 5:{outpwmL = 50; outpwmR = 50;left = true;right = false;break;}/*原地右转*/
      case 6:{outpwmL = 0; outpwmR = 70;left = false;right = false;break;}/*向左后退*/
      case 7:{outpwmL = 83; outpwmR = 70;left = false;right = false;break;}/*直线后退*/
      case 8:{outpwmL = 80; outpwmR = 0;left = false;right = false;break;}/*向右后退*/
    }
    
    if(state == 0) 
    {    
      outpwmL = outpwmR = 0;
    }
    if(!left)
    {
      digitalWrite(IN1L,LOW);
      digitalWrite(IN2L,HIGH);
    }
    else
    {
      digitalWrite(IN1L,HIGH);
      digitalWrite(IN2L,LOW);
    }
    if(!right)
    {
      digitalWrite(IN1R,LOW);
      digitalWrite(IN2R,HIGH);
    }
    else
    {
      digitalWrite(IN1R,HIGH);
      digitalWrite(IN2R,LOW);
    }
  
    analogWrite(ENAL,outpwmL);
    analogWrite(ENAR,outpwmR);
    
  }
  if(!starton){output = '3';oldoutput = 'a';analogWrite(ENAL,0);analogWrite(ENAR,0);}
  else
  {
    
    if(Auto)
    {
      if(ownAuto){output = '4';}
      else{output = '2';}
    }
    else
    {
      if(distance < stopdistance){output = '1';}
      else{output = '0';}
    }
  }
  if(output != oldoutput)
  {
    Serial.write(output);
    Serial.write('\n');
    oldoutput = output;
  }
  timertime = curtime; 

}
