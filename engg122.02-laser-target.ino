//#include<ESP32Servo.h>
#include <Servo.h>
#include "eGizmo4Digit.h"

enum possibleStates {
  idle,
  timeRush,
  reflex,
  endlessStage1,
  endlessStage2,
  endlessStage3,
  endlessStage4,
};

enum targetServoPins {
  target1Servo = 3,
  target2Servo = 15,
  target3Servo = 16
};

enum targetSensorPins {
  target1Sensor = 14,
  target2Sensor = 5,
  target3Sensor = 6
};

const int timeRushPin = 17;
const int reflexPin = 18;
const int endlessPin = 19;

const int scoreStbPin = 11;
const int scoreClkPin = 12;
const int scoreDinPin = 13;

const int timeStbPin = 9;
const int timeClkPin = 8;
const int timeDinPin = 7;

eGizmo4Digit scoreDisplay(scoreStbPin, scoreClkPin, scoreDinPin);
eGizmo4Digit timeDisplay(timeStbPin, timeClkPin, timeDinPin);

Servo target1;
Servo target2;
Servo target3;

int baseMinDelay = 500;
int baseMaxDelay = 3500;

int fastMinDelay = 300;
int fastMaxDelay = 1500;

int reflexBaseMinDelay = 500;
int reflexBaseMaxDelay = 1500;

int reflexFastMinDelay = 200;
int reflexFastMaxDelay = 800;

int reflexMaxTime;
int reflexBaseMaxTime = 15 * 1000; // 15 seconds
int reflexFastMaxTime = 5 * 1000;

int deceptiveAngle = 45;
int targetAngle = 90;

int sensorThreshold = 50;

struct
{
  bool multiTarget;
  bool deceptionActive;
  int timeBeforeRound;
} stageParams;

bool gameBegun = false;
bool roundBegun = false;

bool targetsHit[3];
int targetsSet[3]; // 0 = unset, 1 = deceptive, 2 = set

int scoreForRound;
int score;
int level;

int numTargetsSet;
int numTargetsHit;

unsigned long currentTime;
int timeLimit;
unsigned long roundStartTime;
unsigned long roundEndTime;
unsigned long gameStartTime;

unsigned long preRoundDelay;

int reflexStage = 1;

possibleStates State = idle;

bool debugMode = true;

void setup() {
  Serial.begin(115200);
  target1.attach(target1Servo);
  target2.attach(target2Servo);
  target3.attach(target3Servo);

  pinMode(timeRushPin, INPUT_PULLUP);
  pinMode(reflexPin, INPUT_PULLUP);
  pinMode(endlessPin, INPUT_PULLUP);
  /*
  pinMode(timeRushPin, INPUT_PULLDOWN);
  pinMode(reflexPin, INPUT_PULLDOWN);
  pinMode(endlessPin, INPUT_PULLDOWN);
  */

  pinMode(target1Sensor, INPUT);

  if(debugMode)
  {
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
  }

  scoreDisplay.begin();
  timeDisplay.begin();

}

void loop() {
  currentTime = millis();

  bool timeRushInput = digitalRead(timeRushPin);
  bool reflexInput = digitalRead(reflexPin);
  bool endlessInput = digitalRead(endlessPin);

  timeRushInput = !timeRushInput;
  reflexInput = !reflexInput;
  endlessInput = !endlessInput;

  switch(State)
  {
    case idle:
      Serial.println("Idling");
      resetStage();
      scoreDisplay.DISP_OFF();
      timeDisplay.DISP_OFF();
      if(timeRushInput)
      {
        State = timeRush;
      }
      else if (reflexInput)
      {
        State = reflex;
    Serial.println("Hello!");
    
      }
      else if (endlessInput)
      {
        State = endlessStage1;
      }
      break;
    case timeRush:
      if(!gameBegun)
      {
        Serial.println("Starting Time Rush!");
        timeLimit = 180; // 3 minutes
        score = 0;
        scoreDisplay.begin();
        timeDisplay.begin();
        gameStartTime = currentTime;
        gameBegun = true;
      }
      else
      {
        if((currentTime - gameStartTime) >= (timeLimit * 1000))
        {
          Serial.println("Ending time rush!");
          resetStage();
          timeDisplay.DISP_OFF();
          flashScore(score, true);
          gameStartTime = currentTime;
          gameBegun = false;
          State = idle;
        }
        else
        {
          // Count down Time
          int seconds = (currentTime - gameStartTime) / 1000;
          seconds = timeLimit - seconds;
          if (seconds < 0)
          {
            seconds = 0;
          }
          displayTime(seconds);
          displayScore(score);

          if(!roundBegun)
          {
            if ((currentTime - roundEndTime) >= preRoundDelay)
            {              
              for (int i = 0; i < 3; i++)
              {
                targetsSet[i] = 2;
              }
              numTargetsSet = 3;
              numTargetsHit = 0;

              roundBegun = true;

              prepareTargets(targetsSet);
              resetTargetCounter(targetsHit);
            }
          }
          else
          {
            assessTargetsTimeRush(targetsHit, targetsSet, score, numTargetsHit);

            if(numTargetsHit >= 3) // Should never exceed 3
            {
              roundEndTime = currentTime;

              if (score >= 12)
              {
                preRoundDelay = (random(fastMinDelay,fastMaxDelay));
              }
              else
              {
                preRoundDelay = (random(baseMinDelay,baseMaxDelay));
              }
              roundBegun = false;
            }
          }
        }
      }
      break;
    case reflex:
      if (!gameBegun)
      {
        Serial.println("Starting Reflex Mode!");
        score = 0;
        level = 1;
        scoreDisplay.begin();
        timeDisplay.begin();
        gameStartTime = currentTime;
        reflexMaxTime = reflexBaseMaxTime;
        gameBegun = true;
        roundBegun = false;
      }
      else
      {
        if (level >= 7)
        {
          reflexMaxTime = reflexFastMaxTime;
        }
        
        if(roundBegun && ((currentTime - roundStartTime) >= reflexMaxTime))
        {
          Serial.println("Max duration reached!");
          resetStage();
          if (level >= 8)
          {
            timeDisplay.DISP_OFF();
            flashScore(score, true);
            gameBegun = false;
            State = idle;
          }
          else
          {
            roundBegun = false;
            if (level >= 6) // This is pre-iteration.
            {
              preRoundDelay = (random(fastMinDelay,fastMaxDelay));
            }
            else
            {
              preRoundDelay = (random(baseMinDelay, baseMaxDelay));
            }
            
            level++;
          }
        }

        if (!roundBegun)
        {
          if ((currentTime - roundEndTime) >= preRoundDelay)
          {
            Serial.println("Generating level: " + String(level));
            generateTargets(level, targetsSet);
            for (int i = 0; i < 3; i++)
            {
              Serial.println("Target " + String(i + 1) + ": " + targetsSet[i]);
            }
            prepareTargets(targetsSet);
            resetTargetCounter(targetsHit);
            roundBegun = true;
            roundStartTime = currentTime;
          }
        }
        else
        {
          if (assessTargetsReflex(targetsHit, targetsSet))
          {
            Serial.println("All valid targets hit!");
            scoreForRound = 0;
            for (int i = 0; i < 3; i++)
            {
              if (targetsSet[i] == 2 && targetsHit[i])
              {
                scoreForRound += 3;
              }
              if (targetsSet[i] == 1 && targetsHit[i])
              {
                scoreForRound -= 1;
              }
            }

            int timeDifference = currentTime - roundStartTime;

            if (timeDifference <= 2000)
            {
              scoreForRound += 1;
            }
            else if (timeDifference >= 3000)
            {
              scoreForRound -= 1;
            }
            else if (timeDifference >= 5000){
              scoreForRound -= 2;
            }

            score += scoreForRound;

            resetStage();
            if (level >= 6) // This is pre-iteration.
            {
              preRoundDelay = (random(fastMinDelay,fastMaxDelay));
            }
            else
            {
              preRoundDelay = (random(baseMinDelay, baseMaxDelay));
            }
            roundEndTime = currentTime;
            roundBegun = false;
            level++;
          }
        }

        int seconds = (currentTime - roundStartTime);
        seconds = reflexMaxTime - seconds;
        seconds /= 1000;
        if (seconds < 0)
        {
          seconds = 0;
        }
        displayTime(seconds);
        displayScore(score);
      }
      break;
    case endlessStage1:
      break;
    case endlessStage2:
      break;
    case endlessStage3:
      break;
    case endlessStage4:
      break;
  }

  delay(100);
}

void displayTime(int seconds)
{
  int minutes = seconds / 60;
  seconds = seconds - (minutes * 60);

  timeDisplay.SEND_DATA(0, 0, 0);
  timeDisplay.SEND_DATA(1, minutes, true);
  timeDisplay.SEND_DATA(2, seconds / 10, false);
  timeDisplay.SEND_DATA(3, seconds % 10, false);
}

void displayScore(int score)
{
  scoreDisplay.SEND_DATA(0, score / 1000, 0);
  scoreDisplay.SEND_DATA(1, score / 100, 0);
  scoreDisplay.SEND_DATA(2, score / 10, 0);
  scoreDisplay.SEND_DATA(3, score % 10, 0);
}

void flashScore(int score, bool finalScore)
{
  if (finalScore)
  {
    for (int i = 0; i < 3; i++)
    {
      displayScore(score);
      delay(1000);
      scoreDisplay.DISP_OFF();
      delay(500);
      scoreDisplay.begin();
    }
  }
  else
  {
    for (int i = 0; i < 2; i++)
    {
      displayScore(score);
      delay(800);
      scoreDisplay.DISP_OFF();
      delay(300);
      scoreDisplay.begin();

    }
  }
}

void resetStage()
{
  if (debugMode)
  {
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
  }

  target1.write(0);
  target2.write(0);
  target3.write(0);
}

void resetTargetCounter(bool targetsHit[3])
{
  for (int i = 0; i < 3; i++)
  {
    targetsHit[i] = false;
  }
}

void prepareTargets(int targetsSet[3])
{
  if(debugMode)
  {
    digitalWrite(3, targetsSet[0]);
    digitalWrite(4, targetsSet[1]);
    digitalWrite(5, targetsSet[2]);
  }

  target1.write(targetsSet[0] * 45);
  target2.write(targetsSet[1] * 45);
  target3.write(targetsSet[2] * 45);
}

void generateTargets(
  int level,
  int targetsSet[3]
)
{
  // Level 1-2 = single target mode
  // Level 3-4 = multi-target mode
  // Level 5-6 = multi w/ deception
  // Level 7-8 = even less time

  if (level < 3)
  {
    int target = random(0,3);
    Serial.println("Target chosen: " + String(target));
    for (int i = 0; i < 3; i++)
    {
      targetsSet[i] = 0;
      if (i == target)
      {
        targetsSet[i] = 2;
      }
    }
  }
  else
  {
    int rng = random(1,101);
    rng = 30;
    
    // Game weighted to prefer two targets.
    int remainingIndices[2];
    int j = 0;
    if (rng <= 60)
    {
      Serial.println("Picking two targets...");
      // pick a target and make that the only NON-target
      int target = random(0,3);
      for(int i = 0; i < 3; i++)
      {
        targetsSet[i] = 2;
        if(target == i)
        {
          targetsSet[i] = 0;
        }
        else
        {
          remainingIndices[j] = i;
          j++;
        }
      }
    }
    else {
      for (int i = 0; i < 3; i++)
      {
        targetsSet[i] = 2;
      }
    }

    bool addDeception = random(0,2);

    // Modify to have deception
    if (level > 4 && addDeception)
    {
      int sus;
      if (rng > 60)
      {
        sus = random(0,3);
        targetsSet[sus] = 1;
      }
      else {
        sus = random(0,2);
        targetsSet[remainingIndices[sus]] = 1;
      }
    }
  }
}

bool assessTargetsReflex(
  bool targetsHit[3],
  int targetsSet[3]
)
{
  // a target will remain "hit" if it was already hit before or if it just got hit now.
  targetsHit[0] = targetsHit[0] || assessOneTarget(target1Sensor);
  targetsHit[1] = targetsHit[1] || assessOneTarget(target2Sensor);
  targetsHit[2] = targetsHit[2] || assessOneTarget(target3Sensor);

  Serial.println("Current target matrix: ");
  for (int i = 0; i < 3; i++)
  {
    Serial.println("Target hit at: " + String(targetsHit[i]));
  }

  bool allTargetsHit = true;
  for (int i = 0; i < 3; i++)
  {
    if (targetsSet[i] == 2 && !targetsHit[i])
    {
      allTargetsHit = false;
    }
  }

  return allTargetsHit;
}

bool assessTargetsTimeRush(
  bool targetsHit[3],
  int targetsSet[3],
  int &score,
  int &numTargetsHit
  )
{
  bool pastTargetsHit[3];
  for(int i = 0; i < 3; i++)
  {
    pastTargetsHit[i] = targetsHit[i];
  }

  // a target will remain "hit" if it was already hit before or if it just got hit now
  targetsHit[0] = targetsHit[0] || assessOneTarget(target1Sensor);
  targetsHit[1] = targetsHit[1] || assessOneTarget(target2Sensor);
  targetsHit[2] = targetsHit[2] || assessOneTarget(target3Sensor);

  bool newHits[3] = {false, false, false};
  for (int i = 0; i < 3; i++)
  {
    // It will only be true if the target was not hit before.
    if (!(pastTargetsHit[i]) && targetsHit[i])
    {
      newHits[i] = true;
      numTargetsHit++;
      score++;
    }
  }

  if(newHits[0])
  {
    target1.write(0);
    if(debugMode)
    {
      digitalWrite(3, LOW);
    }
  }
  if(newHits[1])
  {
    target2.write(0);
    if(debugMode)
    {
      digitalWrite(4, LOW);
    }
  }
  if(newHits[2])
  {
    target3.write(0);
    if(debugMode)
    {
      digitalWrite(5, LOW);
    }
  }
}

bool assessOneTarget(int n)
{
  int value = analogRead(n);

  if(debugMode)
  {
    return !(digitalRead(n + 13));
  }
  return (value <= sensorThreshold);
}