#include "pitches.h"
const int buzzerPin = 1;

float song[] = {
  DO1, MI, RE, FA, MI, SOL, FA, LA, SOL, SI, LA, DO2
};

const int songLength = 12;

int noteDurations[] = {
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4
};

unsigned long lastTone = 0;
int toneInterval = (1000 / 4) * 1.30;
int note = 0;

int musicChoiceAddress = 19;
int playMusic = 1;

void playMelody() {
  int noteDuration = 1000 / noteDurations[note];
  tone(buzzerPin, song[note], noteDuration);
  note++;

}

void playIntroSong() {

}
