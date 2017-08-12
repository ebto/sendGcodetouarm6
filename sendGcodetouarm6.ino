/*
   Esplora Sends Gcode to uArm (Swift Pro via Serial Monitor)

   This sketch uses Esplora's keyboard emulation to control
   uArm Swfit Pro (4 DOF robot arm) via serial monitor.

   When the Esplora joystick or switch buttons are pressed,
   Esplora sends keybaord message in G-Gode -
   joystick controls x, y axes; switch left/right controls 4th motor angle;
   switch up/down controls z-axis; button down controls suction;
   slider controls movement speed; light sensor resets to a starting position.

   Step 1: Compile this sketch to Esplora.
   Keep it connected to the PC but don't touch buttons yet.
   If you have TFT LCD it displays which button is pressed.

   Step 2: Connect uArm Swift(Pro) to PC, power on.

   Step 3: From Arduino IDE, connect port to uArm (like /dev/cu.usbmodem!a13441)
   and select board as Mega 2560.

   Step 4: Open serial terminal from Arduino IDE.
   Check baud rate = 115200 and "Newline" is selected. uArm will
   echo text in terminal and is ready to receive commands

   Step 5: Put cursor on serial terminal input box and press a button on Esplora.
   A string like #25 G0 X180 Y-180 Z75 F10000

   Step 6: It moves! (Careful to move joystick only when cursor is in serial
   terminal input box, else you get gcode all over the place.

   Caution: closing serial terminal will make uArm go limp and lose all motor
   tension. Hold the uArm or put a cushion around to protect the effector.

   Created: 10 August 2017
   Author: Eric Ong (ebto on GitHub)
   Credits: sketch is based on sample provided on Arduino IDE - EsploraKart,
   written by Enrico Gueli. Original comments are left as-is,

*/

//#include <SoftwareSerial.h>
#include <Esplora.h>
#include <Keyboard.h>
#include <TFT.h>
#include <SPI.h>

//boolean buttonStates[4];
//
//const byte buttons[] = {
//  JOYSTICK_DOWN,    //X-axis down
//  JOYSTICK_LEFT,    //Y-axis left
//  JOYSTICK_UP,      //X-axis up
//  JOYSTICK_RIGHT   //Y-axis right
//  //SWITCH_RIGHT,     // 4
//  //SWITCH_LEFT,      // 2
//  //SWITCH_UP,        // 3
//  //SWITCH_DOWN       // 1
//};

//String rx;
int angle = 90;
int zVal = 0;
int toggle = LOW;
//int xyzVal[4] = {100, 6, 5, 10000};

void setup() {

  DisplaySplash(); // display sketch name

  // Open serial communications
  while (!Serial) {
    // wait for serial port to connect. Needed for native USB port only
    Serial.begin(115200);
  }

  //  if (Serial.available() > 0) {    // is a character available?
  //    rx = Serial.readString();       // get the character "echo:  M200 D0"
  //  }

  // Move to default resting position
  moveToXY(100, 2, 10000);
  moveToZ(20);
  moveWrist(0);
}

void loop() {

  // reset uarm to predefined coordinate when light sensor is covered
  if (Esplora.readLightSensor() <= 250) {
    moveToXY(100, 5, 10000);
    moveToZ(0);
    delay(1000);
    moveWrist(0);
    delay(1000);
    readEffector(RELEASED);
    //Play tune to notify reset command
    //PlayCoin();
  }

  // toggle end effector on/off
  if (Esplora.readJoystickButton() == PRESSED) {
    if (toggle == PRESSED) {
      readEffector(toggle);
      toggle = RELEASED;
    } else {
      readEffector(toggle);
      toggle = PRESSED;
    }
  }

  // control wrist with R/L switch button
  while (Esplora.readButton(SWITCH_LEFT) == PRESSED) {
    moveWrist(-5);
  }

  while (Esplora.readButton(SWITCH_RIGHT) == PRESSED) {
    moveWrist(5);
  }

  while (Esplora.readButton(SWITCH_UP) == PRESSED) {
    budgeBy(0, 0, 5);
  }

  while (Esplora.readButton(SWITCH_DOWN) == PRESSED) {
    budgeBy(0, 0, -5);
  }

  while (Esplora.readButton(JOYSTICK_LEFT) == PRESSED) {
    budgeBy(0, 3, 0);
  }

  while (Esplora.readButton(JOYSTICK_RIGHT) == PRESSED) {
    budgeBy(0, -3, 0);
  }

  while (Esplora.readButton(JOYSTICK_UP) == PRESSED) {
    budgeBy(3, 0, 0);
  }

  while (Esplora.readButton(JOYSTICK_DOWN) == PRESSED) {
    budgeBy(-3, 0, 0);
  }

//  Keyboard.releaseAll();

  //  // Iterate through all the buttons:
  //  for (byte thisButton = 0; thisButton < 4; thisButton++) {
  //
  //    boolean lastState = buttonStates[thisButton];
  //    boolean newState = Esplora.readButton(buttons[thisButton]);
  //
  //    if (lastState != newState) { // Something changed!
  //      /*
  //        The Keyboard library allows you to "press" and "release" the
  //        keys as two distinct actions. These actions can be
  //        linked to the buttons we're handling.
  //
  //      */
  //      if (newState == PRESSED) {
  //
  //        EsploraTFT.stroke(ST7735_GREEN);
  //
  //        //        if (buttons[thisButton] == JOYSTICK_DOWN || JOYSTICK_LEFT || JOYSTICK_RIGHT || JOYSTICK_UP) {
  //        //          int xyVal[3] = {Esplora.readJoystickX(), Esplora.readJoystickY(), Esplora.readSlider()};
  //        //
  //        //          //budgeBy(xyVal[1],xyVal[0],0,xyVal[2]);
  //        //
  //        //          moveToXY(
  //        //            map(xyVal[1], 511, -512, 5, 356),  // joystick y axis (left/right) mapped to uArm x axis (forward/backward)
  //        //            map(xyVal[0], -512, 511, -250, 250), // joystick x axis (up/down) mapped to uArm y axis (left/right)
  //        //            map(xyVal[2], 1023, 0, 1000, 30000)
  //        //          );
  //        //        }else {
  //        //          // nothing here
  //        //        }
  //
  //      } else if (newState == RELEASED) {
  //        Keyboard.releaseAll();
  //      }
  //
  //      // Store the new button state, so you can sense a difference later:
  //      buttonStates[thisButton] = newState;
  //
  //    } // if laststate != newstate
  //  } // for..loop end

}  // void loop end

void DisplaySplash() {   // display first screen


  // initialize TFT LCD display
  EsploraTFT.begin();
  // clear the screen with a black background
  EsploraTFT.background(ST7735_BLACK);

  EsploraTFT.stroke(ST7735_WHITE);
  EsploraTFT.setTextSize(2);
  EsploraTFT.setTextWrap(true);
  EsploraTFT.text("ESPLORA", 0, 0);
  EsploraTFT.text("SENDS GCODE", 0, 20);
  EsploraTFT.text("ALL BTNS", 0, 40);
  EsploraTFT.text("x, y, z", 0, 60);
  EsploraTFT.text("wrist, effectr", 0, 80);
  EsploraTFT.text("slider", 0, 100);
  delay(3000);
  EsploraTFT.background(ST7735_BLACK);

  // set the font size for the loop
  EsploraTFT.setTextSize(2);
  // set the font color
  EsploraTFT.stroke(ST7735_GREEN);
  EsploraTFT.text("X-axis:", 0, 0);
  EsploraTFT.text("Y-axis:", 0, 20);
  EsploraTFT.text("Z-axis:", 0, 40);
  EsploraTFT.text("Wrist:", 0, 60);
  EsploraTFT.text("Speed:", 0, 80);
  EsploraTFT.text("Suction:", 0, 100);

}

void moveToXY(int xval, int yval, int fval) {
  const String DEBUG = "#25 ";
  const String moveToXY = "G0 ";
  char xPrintout[5] = {xval};
  char yPrintout[5] = {yval};
  //char zPrintout[5] = {zval};
  char fPrintout[6] = {fval};
  String str[3];

  str[0] = String(xval);
  str[0].toCharArray(xPrintout, 5);
  str[1] = String(yval);
  str[1].toCharArray(yPrintout, 5);
  //str[2] = String(zval);
  //str[2].toCharArray(zPrintout, 5);
  str[2] = String(fval);
  str[2].toCharArray(fPrintout, 6);

  Keyboard.print(DEBUG + moveToXY + "X" + xval + " Y" + yval + " F" + fval + "\n");

  // show values on TFT LCD and refresh
  EsploraTFT.stroke(ST7735_WHITE);
  EsploraTFT.text(xPrintout, 100,  0);
  EsploraTFT.text(yPrintout, 100, 20);
  //EsploraTFT.text(zPrintout, 100, 40);
  EsploraTFT.text(fPrintout, 100, 80);
  delay(250);
  EsploraTFT.stroke(ST7735_BLACK);
  EsploraTFT.text(xPrintout, 100, 0);
  EsploraTFT.text(yPrintout, 100, 20);
  //EsploraTFT.text(zPrintout, 100, 40);
  EsploraTFT.text(fPrintout, 100, 80);
}

void moveToZ(int zincrement) {
  const String DEBUG = "#25 ";
  const String moveToZ = "G0 ";
  zVal = constrain(zVal + zincrement, 5, 175);
  char zPrintout[5] = {zVal};
  String str[1];

  str[0] = String(zVal);
  str[0].toCharArray(zPrintout, 5);

  Keyboard.print(DEBUG + moveToZ + "Z" + zVal + "\n");

  // show values on TFT LCD and refresh
  EsploraTFT.stroke(ST7735_WHITE);
  EsploraTFT.text(zPrintout, 100, 40);
  delay(250);
  EsploraTFT.stroke(ST7735_BLACK);
  EsploraTFT.text(zPrintout, 100, 40);
}

void budgeBy(int x, int y, int z) {
  const String COMMAND = "#25 G2204 ";

  int coord[4] = {x, y, z, Esplora.readSlider()};
  coord[0] = constrain(coord[0] + x, -250, 250);
  coord[1] = constrain(coord[1] + y, 50, 356);
  coord[2] = constrain(coord[2] + z, 5, 175);
  coord[3] = map(coord[3], 1023, 0, 1000, 20000);

  char xPrintout[5] = {x};
  char yPrintout[5] = {y};
  char zPrintout[5] = {z};
  char fPrintout[6] = {coord[3]};

  String str[3];
  str[0] = String(x);
  str[0].toCharArray(xPrintout, 5);
  str[1] = String(y);
  str[1].toCharArray(yPrintout, 5);
  str[2] = String(z);
  str[2].toCharArray(zPrintout, 5);
  str[2] = String(coord[3]);
  str[2].toCharArray(fPrintout, 6);

  Keyboard.print(COMMAND + "X" + x + " Y" + y + " Z" + z + " F" + coord[3] + "\n");

  // show values on TFT LCD and refresh
  EsploraTFT.stroke(ST7735_WHITE);
  EsploraTFT.text(xPrintout, 100,  0);
  EsploraTFT.text(yPrintout, 100, 20);
  EsploraTFT.text(zPrintout, 100, 40);
  EsploraTFT.text(fPrintout, 100, 80);
  delay(50);
  EsploraTFT.stroke(ST7735_BLACK);
  EsploraTFT.text(xPrintout, 100, 0);
  EsploraTFT.text(yPrintout, 100, 20);
  EsploraTFT.text(zPrintout, 100, 40);
  EsploraTFT.text(fPrintout, 100, 80);

}

void readEffector(boolean btn) {
  const String COMMAND = "#25 M2231 ";
  const String PUMP_ON = "V1\n";
  const String PUMP_OFF = "V0\n";

  if (btn == LOW) {
    Keyboard.println(COMMAND + PUMP_ON);
    EsploraTFT.stroke(ST7735_WHITE);
    EsploraTFT.text("ON", 100,  100);
    delay(100);
    EsploraTFT.stroke(ST7735_BLACK);
    EsploraTFT.text("ON", 100, 100);
  } else if (btn == HIGH) {
    Keyboard.println(COMMAND + PUMP_OFF);
    EsploraTFT.stroke(ST7735_WHITE);
    EsploraTFT.text("OFF", 100,  100);
    delay(100);
    EsploraTFT.stroke(ST7735_BLACK);
    EsploraTFT.text("OFF", 100, 100);
  }
}

void moveWrist(int offset) {
  const String CMD = "#25 G2202 N3 V";
  angle = constrain(angle + offset, 0, 180);
  char Printout[4] = {angle};
  String str[1];
  str[0] = String(angle);
  str[0].toCharArray(Printout, 4);

  Keyboard.println(CMD + angle + "\n");

  EsploraTFT.noStroke();
  EsploraTFT.stroke(ST7735_WHITE);
  EsploraTFT.text(Printout, 100,  60);
  delay(50);
  EsploraTFT.stroke(ST7735_BLACK);
  EsploraTFT.text(Printout, 100,  60);
}

void PlayCoin() {
  //play a tune on uArm
  Keyboard.println("M2210 F988 T100\n" );
  delay(100);
  Keyboard.println("M2210 F1319 T850\n" );
  delay(850);
}


