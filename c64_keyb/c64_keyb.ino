#include <SoftwareSerial.h>

#define NUM_ROWS 8
#define NUM_COLS 8

#define KEY_BACKSPACE     255 // DEL
#define KEY_TAB           254 // shift & SPACE
#define KEY_ESC           253 // RUN STOP
#define KEY_SHIFT         251 // LEFT
#define KEY_SPACE         234 //

#define KEY_DELETE        250 // shift & del = INST
#define KEY_HOME          249 // home (pos1)
#define KEY_RETURN        247 //

#define KEY_LEFT_ARROW    246 // <- or shift & CRSR->
#define KEY_DOWN_ARROW    245 // 
#define KEY_UP_ARROW      244 // arrow up key or shift CRSR down
#define KEY_RIGHT_ARROW   243 //

#define KEY_F1            242 // copy
#define KEY_F2            241 // mark left
#define KEY_F3            240 // paste
#define KEY_F4            239 // mark right
#define KEY_F5            238 // mouse up -----------------
#define KEY_F6            237 // mouse left --------------- (shift)
#define KEY_F7            236 // mouse down ---------------
#define KEY_F8            235 // mouse right -------------- (shift)

#define VOLUME_UP         232 // shift +
#define VOLUME_DOWN       231 // shift -
#define KEY_END           230 // CLR = shift home

#define KEY_COMMODORE     252 // mouse left click ------------------
#define KEY_CTRL          248 // mouse right click -----------------
#define KEY_SHIFT_RIGHT   233 // used as CTRL & Z (undo)

#define KEY_BRIGHTUP      229 // shift & arrow up key
#define KEY_BRIGHTDOWN    228 // shift & <- key

#define KEY_SHIFT_LOCK    A6 // I map auosAUO on Linux to äüößÄÜÖ
#define KEY_RESTORE       A7 // for \ and |

byte keyMap[NUM_ROWS][NUM_COLS] = {
{'1',KEY_LEFT_ARROW,KEY_CTRL,KEY_ESC,KEY_SPACE,KEY_COMMODORE,'q','2'},
{'3','w','a',KEY_SHIFT,'z','s','e','4'},
{'5','r','d','x','c','f','t','6'},
{'7','y','g','v','b','h','u','8'},
{'9','i','j','n','m','k','o','0'},
{'+','p','l',',','.',':','@','-'},
{'`'       ,'*'       ,';'            ,'/'           ,KEY_SHIFT_RIGHT,'='   ,KEY_UP_ARROW,KEY_HOME},
{KEY_DELETE,KEY_RETURN,KEY_RIGHT_ARROW,KEY_DOWN_ARROW,KEY_F1         ,KEY_F3,KEY_F5      ,KEY_F7}
};

byte keyMapShifted[NUM_ROWS][NUM_COLS] = {
{'!'          ,KEY_BRIGHTDOWN,KEY_CTRL      ,KEY_ESC     ,KEY_TAB         ,KEY_COMMODORE,'Q'         ,34},
{'#'          ,'W'           ,'A'           ,KEY_SHIFT   ,'Z'             ,'S'          ,'E'         ,'$'},
{'%'          ,'R'           ,'D'           ,'X'         ,'C'             ,'F'          ,'T'         ,'&'},
{'\''         ,'Y'           ,'G'           ,'V'         ,'B'             ,'H'          ,'U'         ,'('},
{')'          ,'I'           ,'J'           ,'N'         ,'M'             ,'K'          ,'O'         ,'^'},
{VOLUME_UP    ,'P'           ,'L'           ,'<'         ,'>'             ,'['          ,'{'         ,VOLUME_DOWN},
{'~'          ,'}'           ,']'           ,'?'         ,KEY_SHIFT_RIGHT,'_'          ,KEY_BRIGHTUP,KEY_END},
{KEY_BACKSPACE,KEY_RETURN    ,KEY_LEFT_ARROW,KEY_UP_ARROW,KEY_F2         ,KEY_F4       ,KEY_F6      ,KEY_F8}
};

int debounceCount[NUM_ROWS][NUM_COLS];

// define the row and column pins
byte rowPins[NUM_ROWS] = {A3, A2, A1, A0, 13, 12, 11, 10};
byte colPins[NUM_COLS] = { 2,  3,  4,  5,  6,  7,  8,  9};

// where is the shift key
#define SHIFT_COL 3
#define SHIFT_ROW 1

// how many times does a key need to register as pressed?
#define DEBOUNCE_VALUE 80
#define REPEAT_DELAY 300




// I am sorry: this is a hack for the cheap WAVGAT version of the Arduino pro mini
// normaly you do not need Softserial in 0 and 1 !! these are still hardware serial!

SoftwareSerial mSerial(0, 1); // RX, TX






bool shiftLockSpecial(byte r, byte c, bool shifted, int val) {
  char key = shifted ? keyMapShifted[r][c] : keyMap[r][c];
  if (val > 1000) return false;
  switch(key) {
    case 'a':
      mSerial.println(F("AT+BleKeyboardCode=00-00-39-04-00"));
      mSerial.println(F("AT+BleKeyboardCode=00-00"));
      return true;
    case 'o':
      mSerial.println(F("AT+BleKeyboardCode=00-00-39-12-00"));
      mSerial.println(F("AT+BleKeyboardCode=00-00"));
      return true;
    case 'u':
      mSerial.println(F("AT+BleKeyboardCode=00-00-39-18-00"));
      mSerial.println(F("AT+BleKeyboardCode=00-00"));
      return true;
    case 's':
      mSerial.println(F("AT+BleKeyboardCode=00-00-39-16-00"));
      mSerial.println(F("AT+BleKeyboardCode=00-00"));
      return true;

    case 'A':
      mSerial.println(F("AT+BleKeyboardCode=02-00-39-04-00"));
      mSerial.println(F("AT+BleKeyboardCode=00-00"));
      return true;
    case 'O':
      mSerial.println(F("AT+BleKeyboardCode=02-00-39-12-00"));
      mSerial.println(F("AT+BleKeyboardCode=00-00"));
      return true;
    case 'U':
      mSerial.println(F("AT+BleKeyboardCode=02-00-39-18-00"));
      mSerial.println(F("AT+BleKeyboardCode=00-00"));
      return true;
    default:
      return false;
  }
}

void setup() {
  mSerial.begin(9600);
  
  for (byte c = 0; c < NUM_COLS; c++) {
    pinMode(colPins[c], INPUT);
    digitalWrite(colPins[c], HIGH);
  
    for (byte r = 0; r < NUM_ROWS; r++) {
      debounceCount[r][c] = 0;
    }
  }

  // set all pins as inputs
  for (byte r = 0; r < NUM_ROWS; r++) {
    pinMode(rowPins[r], INPUT);
  }
   
  if (1==1) {
    mSerial.println("+++");
    delay(600);
    mSerial.println("AT+GAPDEVNAME=C64 Keyboard");
    delay(800);
    mSerial.println("AT+BLEHIDEN=1");
    delay(600);
    mSerial.println("AT+BAUDRATE=9600");
    delay(800);
    mSerial.println("ATZ");
    delay(600);
    mSerial.println("+++");
    delay(600);
  }
}

void loop() {
  // check for shift
  bool shifted = false;
  bool keyPressed = false;
  pinMode(rowPins[SHIFT_ROW], OUTPUT);

  if (digitalRead(colPins[SHIFT_COL]) == LOW) shifted = true;
  pinMode(rowPins[SHIFT_ROW], INPUT);
  int dummy = analogRead(KEY_SHIFT_LOCK);
            
  for (byte r = 0; r < NUM_ROWS; r++) {
    // turn the row on
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], LOW);
  
    for (byte c = 0; c < NUM_COLS; c++) {
      if (digitalRead(colPins[c]) == LOW) {
        // increase the debounce count
        debounceCount[r][c]++;
    
        // has the switch been pressed continually for long enough?
        int count = debounceCount[r][c];
        if (count == DEBOUNCE_VALUE) {
          // first press
          keyPressed = true;
          if (!shiftLockSpecial(r, c, shifted, dummy)) pressKey(r, c, shifted);
          
        } else if (count > DEBOUNCE_VALUE) {
          // check for repeats
          count -= DEBOUNCE_VALUE;
          if (count % REPEAT_DELAY == 0) {
            // send repeat
            keyPressed = true;
            if (!shiftLockSpecial(r, c, shifted, dummy)) pressKey(r, c, shifted);
          
          }
        }
      } else {
        // not pressed, reset debounce count
        debounceCount[r][c] = 0;
      }
    }
  
    // turn the row back off
    pinMode(rowPins[r], INPUT_PULLUP);
  }
  
  dummy = analogRead(KEY_RESTORE);
  
  if (dummy < 1000) {
    if (shifted) {
      // the | key
      mSerial.println(F("AT+BleKeyboardCode=02-00-31-00-00"));
      mSerial.println(F("AT+BleKeyboardCode=00-00"));            
    } else {
      // the \ key
      mSerial.println(F("AT+BleKeyboardCode=00-00-31-00-00"));
      mSerial.println(F("AT+BleKeyboardCode=00-00"));      
    }
  }

  digitalWrite(rowPins[SHIFT_ROW], LOW);
}

// Send the keypress
void pressKey(byte r, byte c, bool shifted) {
  byte key = shifted ? keyMapShifted[r][c] : keyMap[r][c];
  
  if (key < 228)  {
    if ((char)key=='?') {
      mSerial.println(F("AT+BleKeyboardCode=02-00-38-00-00"));
      mSerial.println(F("AT+BleKeyboardCode=00-00"));
    } else if ((char)key=='~') {
      mSerial.println(F("AT+BleKeyboardCode=02-00-35-00-00"));
      mSerial.println(F("AT+BleKeyboardCode=00-00"));
    } else if ((char)key=='`') {
      mSerial.println(F("AT+BleKeyboardCode=00-00-35-00-00"));
      mSerial.println(F("AT+BleKeyboardCode=00-00"));      
    } else if ((char)key=='{') {
      mSerial.println(F("AT+BleKeyboardCode=02-00-2F-00-00"));
      mSerial.println(F("AT+BleKeyboardCode=00-00"));
    } else if ((char)key=='}') {
      mSerial.println(F("AT+BleKeyboardCode=02-00-30-00-00"));
      mSerial.println(F("AT+BleKeyboardCode=00-00"));
    } else if ((char)key=='^') {
      mSerial.println(F("AT+BleKeyboardCode=02-00-23-00-00"));
      mSerial.println(F("AT+BleKeyboardCode=00-00"));
    } else {
      mSerial.print(F("AT+BleKeyboard="));
      mSerial.println((char)key);
    }    
  } else {
    switch (key) {
      case KEY_SPACE:
        mSerial.println(F("AT+BleKeyboardCode=00-00-2C-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case KEY_LEFT_ARROW:
        mSerial.println(F("AT+BleKeyboardCode=00-00-50-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case KEY_RIGHT_ARROW:
        mSerial.println(F("AT+BleKeyboardCode=00-00-4F-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case KEY_UP_ARROW:
        mSerial.println(F("AT+BleKeyboardCode=00-00-52-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case KEY_DOWN_ARROW:
        mSerial.println(F("AT+BleKeyboardCode=00-00-51-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
        
      case KEY_RETURN:
        mSerial.println(F("AT+BleKeyboardCode=00-00-28-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case KEY_ESC:
        mSerial.println(F("AT+BleKeyboardCode=00-00-29-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case KEY_DELETE:
        mSerial.println(F("AT+BleKeyboardCode=00-00-2A-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;

      case KEY_TAB:
        mSerial.println(F("AT+BleKeyboardCode=00-00-2B-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case KEY_HOME:
        mSerial.println(F("AT+BleKeyboardCode=00-00-4A-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case KEY_END:
        mSerial.println(F("AT+BleKeyboardCode=00-00-4D-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case KEY_BACKSPACE: // delete forward
        mSerial.println(F("AT+BleKeyboardCode=00-00-4C-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;

      case VOLUME_UP:
        mSerial.println(F("AT+BleHidControlKey=VOLUME+"));
        break;
      case VOLUME_DOWN:
        mSerial.println(F("AT+BleHidControlKey=VOLUME-"));
        break;

      case KEY_F2:
        mSerial.println(F("AT+BleKeyboardCode=02-00-50-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case KEY_F4:
        mSerial.println(F("AT+BleKeyboardCode=02-00-4F-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case KEY_F1:
        mSerial.println(F("AT+BleKeyboardCode=00-00-7C-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case KEY_F3:
        mSerial.println(F("AT+BleKeyboardCode=00-00-7D-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;

      case KEY_BRIGHTDOWN:
        mSerial.println(F("AT+BleHidControlKey=BRIGHTNESS-"));
        break;
      case KEY_BRIGHTUP:
        mSerial.println(F("AT+BleHidControlKey=BRIGHTNESS+"));
        break;

      case KEY_SHIFT_RIGHT:
        mSerial.println(F("AT+BleKeyboardCode=04-00-7A-00-00")); // undo
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;


      case KEY_COMMODORE:
        mSerial.println(F("AT+BleHidMouseButton=L,click"));
        break;
      case KEY_CTRL:
        mSerial.println(F("AT+BleHidMouseButton=R,click"));
        break;
      case KEY_F5:
        mSerial.println(F("AT+BleHidMouseMove=0,-20"));
        break;
      case KEY_F7:
        mSerial.println(F("AT+BleHidMouseMove=0,20"));
        break;
      case KEY_F6:
        mSerial.println(F("AT+BleHidMouseMove=-20,0"));
        break;
      case KEY_F8:
        mSerial.println(F("AT+BleHidMouseMove=20,0"));
        break;
      default:
        ;
    }
  }
}

