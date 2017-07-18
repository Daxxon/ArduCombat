//Copyright 2015, Daniel Waggoner, All rights reserved.

//IR LED is on pin 3
//IR receiver1 is on pin 14
//IR receiver2 is on pin 15
//Pins 16, 17, 18, and 19 are motor cut-off pins
//#include <SPI.h>
//#include "printf.h"

int rocketSalvos = 8;
int playerID = 1;
unsigned long takingFirePulse;
int cooldown[2];       //[0] is gatling gun and [1] is rocket
uint8_t channel[] = {5, 6};
uint8_t channelPins[] = {PD5, PD6};
int numOfChannels = sizeof(channel);
uint8_t channelPPM[2];
int motor1[] = {16,17,18};
int motor2[] = {17,18,19};
int randNum = 0;
int ch5;
int ch6;

void setup() {

  DDRD |= (1 << DDD6);       //set pin 3 to OUTPUT
  DDRC = 60;                 //set pins 14 & 15 to INPUT and set pins 16-19 to OUTPUT
  PORTC = 60;                //set pins 14 & 15 to LOW and set pins 16-19 to HIGH

}

void loop() {

  takeFire();
  fireWeapons();
  applyCooldown();

}

void takeFire() {
  randomizer();
  takingFirePulse1 = pulseIn(PC0, HIGH, 500);   //measure IR pulse on pin 14
  takingFirePulse2 = pulseIn(PC1, HIGH, 500);   //measure IR pulse on pin 15
  if (takingFirePulse1 >= 19000 && takingFirePulse1 <= 21000) {
    onHitFromGatlingGun();
  } else if (takingFirePulse1 >= 15000 && takingFirePulse1 <= 17000) {
    onHitFromRocket();
  } else  if (takingFirePulse2 >= 19000 && takingFirePulse2 <= 21000) {
    onHitFromGatlingGun();
  } else if (takingFirePulse2 >= 15000 && takingFirePulse2 <= 17000) {
    onHitFromRocket();
  } else {
    return;
  }
}

void onHitFromGatlingGun() {
  digitalWrite(motor1[randNum], LOW);
  digitalWrite(motor2[randNum], LOW);
  delay(20);
  PORTC = 60;
}

void onHitFromRocket() {
  digitalWrite(motor1[randNum], LOW);
  digitalWrite(motor2[randNum], LOW);
  delay(40);
  PORTC = 60;
}

void setChannelPPM() {
  for (int i=0; i<2; i++) {
    channelPPM[i] = pulseIn(channelPins[i], HIGH, 15000);
  }
  ch5 = channelPPM[0];
  ch6 = channelPPM[1];
}

void fireWeapons() {
  setChannelPPM();
  if (ch6 > 1200 && cooldown[1] == 0 && rocketSalvos > 0) {
    for (int i = 0; i < 16; i++) {
      fireRocket();
    }
    rocketSalvos--;
    cooldown[1] += 1024;
  } else if (ch5 > 1200 && cooldown[0] < 256) {
    fireGatlingGun();
  } else if (ch5 > 1200 && cooldown[0] == 256) {
    fireGatlingGun();
    cooldown[0] += 256;
  } else if (cooldown[0] > 256) {
    return;
  }
}

void fireGatlingGun() {
  PORTD |= (1 << PD3);
  delay(20);
  PORTD &= ~(1 << PD3);
  cooldown[0] += 3;
}

void fireRocket() {
  PORTD |= (1 << PD3);
  delay(16);
  PORTD &= ~(1 << PD3);
}

void applyCooldown() {
  for (int i=0; i<2; i++) {
    if (cooldown[i] > 0) {
      cooldown[i]--;
    } else {
      return;
    }
  }
}

void randomizer() {
  (randNum < 3) ? randNum++ : randNum = 0;
}

/*
128.64.32.16.8.4.2.1
*/
