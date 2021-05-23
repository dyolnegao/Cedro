#include <Servo.h> 

Servo myservo1;

int ir_s1 = 2;
int ir_s2 = 4;

int Total = 5;
int Space;

int flag1 = 0;
int flag2 = 0;

void setup() {
pinMode(ir_s1, INPUT);
pinMode(ir_s2, INPUT);
  
myservo1.attach(3);
myservo1.write(100);

Space = Total;
}

void loop(){ 

if(digitalRead (ir_s1) == LOW && flag1==0){
if(Space>0){flag1=1;
if(flag2==0){myservo1.write(0); Space = Space-1;}
}
}

if(digitalRead (ir_s2) == LOW && flag2==0){flag2=1;
if(flag1==0){myservo1.write(0); Space = Space+1;}
}

if(flag1==1 && flag2==1){
delay (1000);
myservo1.write(100);
flag1=0, flag2=0;
}

}
