#include <SwitchControlLibrary.h>

#define WAIT_FOR_COMMAND 0
#define IN_PROGRESS      1
#define BTN_PUSH_TIME    50
#define BTN_RELEASE_TIME 70

bool queue[320];
int state = WAIT_FOR_COMMAND;
int receivedCount = 0;
int currentRow = 0;

void setup() {
  // put your setup code here, to run once:
  Serial1.begin(9600);
  SwitchControlLibrary(); // 初期化

  SwitchControlLibrary().pressButton(Button::RCLICK);
  SwitchControlLibrary().sendReport();
  delay(500);
  SwitchControlLibrary().releaseButton(Button::RCLICK);
  SwitchControlLibrary().sendReport();
  delay(500);
  
  SwitchControlLibrary().pressButton(Button::RCLICK);
  SwitchControlLibrary().sendReport();
  delay(500);
  SwitchControlLibrary().releaseButton(Button::RCLICK);
  SwitchControlLibrary().sendReport();
  delay(500);
  
  SwitchControlLibrary().pressButton(Button::RCLICK);
  SwitchControlLibrary().sendReport();
  delay(500);
  SwitchControlLibrary().releaseButton(Button::RCLICK);
  SwitchControlLibrary().sendReport();
  delay(500);

  SwitchControlLibrary().pressButton(Button::RCLICK);
  SwitchControlLibrary().sendReport();
  delay(500);
  SwitchControlLibrary().releaseButton(Button::RCLICK);
  SwitchControlLibrary().sendReport();
  delay(500);
}

void loop() {
  // put your main code here, to run repeatedly:

  switch (state) {
    case WAIT_FOR_COMMAND:
      waitForCommand();
      break;

    case IN_PROGRESS:
      inProgress();
      break;
  }
}

void waitForCommand() {
  if (Serial1.available() == 0) {
    return;
  }

  char pixel = Serial1.read();
  switch (pixel) {
    // white pixel
    case '0':
      queue[receivedCount] = false;
      receivedCount++;
      break;
      
    // black pixel
    case '1':
    queue[receivedCount] = true;
      receivedCount++;
      break;

    // end
    case '2':
      // validate
      if (receivedCount != 320) {
        // retry
        receivedCount = 0;
        return;
      }
      state = IN_PROGRESS;
      receivedCount = 0;
      break;
  }
}

void inProgress() {
  for (int completedCount = 0; completedCount < 320; completedCount++) {
    bool LTR = currentRow % 2 == 0;

    int currentIndex;
    if (LTR) {
      currentIndex = completedCount;
    } else {
      currentIndex = 319 - completedCount;
    }
    bool currentPixelValue = queue[currentIndex];
    
    // paint
    if (currentPixelValue) {
      SwitchControlLibrary().pressButton(Button::A);
      SwitchControlLibrary().sendReport();
      delay(BTN_PUSH_TIME);
      SwitchControlLibrary().releaseButton(Button::A);
      SwitchControlLibrary().sendReport();
      delay(BTN_RELEASE_TIME);
    } 
//    else {
//      SwitchControlLibrary().pressButton(Button::B);
//      SwitchControlLibrary().sendReport();
//      delay(BTN_PUSH_TIME);
//      SwitchControlLibrary().releaseButton(Button::B);
//      SwitchControlLibrary().sendReport();
//      delay(BTN_RELEASE_TIME);
//    }

    // move to next pixel
    if (completedCount != 319) {
      if (LTR) {
        SwitchControlLibrary().pressHatButton(HatButton::RIGHT);
      } else {
        SwitchControlLibrary().pressHatButton(HatButton::LEFT);
      }
      SwitchControlLibrary().sendReport();
      delay(BTN_PUSH_TIME);
      
      if (LTR) {
        SwitchControlLibrary().releaseHatButton(HatButton::RIGHT);
      } else {
        SwitchControlLibrary().releaseHatButton(HatButton::LEFT);
      }
      SwitchControlLibrary().sendReport();
      delay(BTN_RELEASE_TIME);
    }
    
    // last pixel
    else {
      SwitchControlLibrary().pressHatButton(HatButton::DOWN);
      SwitchControlLibrary().sendReport();
      delay(BTN_PUSH_TIME);
      SwitchControlLibrary().releaseHatButton(HatButton::DOWN);
      SwitchControlLibrary().sendReport();
      delay(BTN_RELEASE_TIME);
      
      Serial1.println("READY");
      currentRow++;
      state = WAIT_FOR_COMMAND;
    }
  }
}
