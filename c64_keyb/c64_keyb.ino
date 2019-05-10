#include <SoftwareSerial.h>

#define NUM_ROWS 8
#define NUM_COLS 8

#define KEY_BACKSPACE     255 // DEL
#define KEY_SHIFT_CTRL    254 // shift tab
#define KEY_STOP          253 // ESC
#define KEY_SHIFT         251 // LEFT
#define KEY_SPACE         234 //

#define KEY_DELETE        250 // shift del = INST
#define KEY_HOME          249 // home (pos1)
#define KEY_RETURN        247 //

#define KEY_LEFT_ARROW    246 // <- or shift & CRSR->
#define KEY_DOWN_ARROW    245 // 
#define KEY_UP_ARROW      244 // arrow up key or shift CRSR down
#define KEY_RIGHT_ARROW   243 //

#define KEY_F1            242 // mark left
#define KEY_F2            241 // mark right (shift)
#define KEY_F3            240 // mouse left click ---------
#define KEY_F4            239 // mouse right click -------- (shift) 
#define KEY_F5            238 // mouse up -----------------
#define KEY_F6            237 // mouse left --------------- (shift)
#define KEY_F7            236 // mouse down ---------------
#define KEY_F8            235 // mouse right -------------- (shift)

#define SHIFT_PLUS        232 // VOLUME_UP
#define SHIFT_MINUS       231 // VOLUME_down
#define KEY_END           230 // CLR = shift home

#define KEY_COMMODORE     252 
#define KEY_CTRL          248 // TAB
#define KEY_SHIFT_RIGHT   233

#define SHIFT_ARROWUP     229 // bright +
#define SHIFT_ARROWLEFT   228 // bright -
#define SHIFT_COMMODORE   227
// shift + run stop 
#define KEY_RUN           226 // F11

#define KEY_SHIFT_LOCK    A6 // I map auosAUO on Linux to äüößÄÜÖ
#define KEY_RESTORE       A7 // for \ and |

byte keyMap[NUM_ROWS][NUM_COLS] = {
{'1',       KEY_LEFT_ARROW, KEY_CTRL,        KEY_STOP,      KEY_SPACE,       KEY_COMMODORE, 'q',          '2'},
{'3',       'w',            'a',             KEY_SHIFT,     'z',             's',           'e',          '4'},
{'5',       'r',            'd',             'x',           'c',             'f',           't',          '6'},
{'7',       'y',            'g',             'v',           'b',             'h',           'u',          '8'},
{'9',       'i',            'j',             'n',           'm',             'k',           'o',          '0'},
{'+',       'p',            'l',             ',',           '.',             ':',           '@',          '-'},
{'`',       '*',            ';',             '/',            KEY_SHIFT_RIGHT, '=',          KEY_UP_ARROW, KEY_HOME},
{KEY_DELETE, KEY_RETURN,    KEY_RIGHT_ARROW, KEY_DOWN_ARROW, KEY_F1,          KEY_F3,       KEY_F5      , KEY_F7}
};

byte keyMapShifted[NUM_ROWS][NUM_COLS] = {
{'!'           ,SHIFT_ARROWLEFT ,KEY_SHIFT_CTRL ,KEY_RUN      ,KEY_SPACE       ,SHIFT_COMMODORE ,'Q'           ,34},
{'#'           ,'W'             ,'A'            ,KEY_SHIFT    ,'Z'             ,'S'             ,'E'           ,'$'},
{'%'           ,'R'             ,'D'            ,'X'          ,'C'             ,'F'             ,'T'           ,'&'},
{'\''          ,'Y'             ,'G'            ,'V'          ,'B'             ,'H'             ,'U'           ,'('},
{')'           ,'I'             ,'J'            ,'N'          ,'M'             ,'K'             ,'O'           ,'^'},
{SHIFT_PLUS    ,'P'             ,'L'            ,'<'          ,'>'             ,'['             ,'{'           ,SHIFT_MINUS},
{'~'           ,'}'             ,']'            ,'?'          ,KEY_SHIFT_RIGHT ,'_'             ,SHIFT_ARROWUP ,KEY_END},
{KEY_BACKSPACE ,KEY_RETURN      ,KEY_LEFT_ARROW ,KEY_UP_ARROW ,KEY_F2          ,KEY_F4          ,KEY_F6        ,KEY_F8}
};
  
int debounceCount[NUM_ROWS][NUM_COLS];

// define the row and column pins
byte rowPins[NUM_ROWS] = {A3, A2, A1, A0, 13, 12, 11, 10};
byte colPins[NUM_COLS] = { 2,  3,  4,  5,  6,  7,  8,  9};

// where is the shift key
#define SHIFT_COL 3
#define SHIFT_ROW 1
#define RSHIFT_COL 4
#define RSHIFT_ROW 6

// where is the commodore key
#define COMMO_COL 5
#define COMMO_ROW 0
bool commo = false;

// how many times does a key need to register as pressed?
#define DEBOUNCE_VALUE 80
#define REPEAT_DELAY 260




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
    pinMode(rowPins[r], INPUT_PULLUP);
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
  bool keyPressed = false;

  // check for shift ------------------------------------
  bool shifted = false;
  // initial check the shift key
  pinMode(rowPins[SHIFT_ROW], OUTPUT); // default is high?!
  if (digitalRead(colPins[SHIFT_COL]) == LOW) shifted = true;
  // set it back
  pinMode(rowPins[SHIFT_ROW], INPUT_PULLUP);

  // some for right shift key
  pinMode(rowPins[RSHIFT_ROW], OUTPUT); // default is high?!
  if (digitalRead(colPins[RSHIFT_COL]) == LOW) shifted = true;
  // set it back
  pinMode(rowPins[RSHIFT_ROW], INPUT_PULLUP);
  
  // check for commodore ----------------------------------
  commo = false;
  // initial check the commo key
  pinMode(rowPins[COMMO_ROW], OUTPUT); // default is hight?!
  if (digitalRead(colPins[COMMO_COL]) == LOW) commo = true;
  // set it back
  pinMode(rowPins[COMMO_ROW], INPUT_PULLUP);
  // -----------------------------------------------------
  
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

  digitalWrite(rowPins[SHIFT_ROW],  LOW);
  digitalWrite(rowPins[RSHIFT_ROW], LOW);
  digitalWrite(rowPins[COMMO_ROW],  LOW);
}

// Send the keypress
void pressKey(byte r, byte c, bool shifted) {
  byte key = shifted ? keyMapShifted[r][c] : keyMap[r][c];

  // commodore key as ctrl (shift+ctrl)
  if (commo) {
    switch ((char)key) {
      case '0':
        mSerial.println(F("AT+BleKeyboardCode=01-00-27-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case 'a':
        mSerial.println(F("AT+BleKeyboardCode=01-00-04-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case 'c':
        mSerial.println(F("AT+BleKeyboardCode=01-00-06-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case 'f':
        mSerial.println(F("AT+BleKeyboardCode=01-00-09-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case 'g':
        mSerial.println(F("AT+BleKeyboardCode=01-00-0A-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case 'l':
        mSerial.println(F("AT+BleKeyboardCode=01-00-0F-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case 'r':
        mSerial.println(F("AT+BleKeyboardCode=01-00-15-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case 's':
        mSerial.println(F("AT+BleKeyboardCode=01-00-16-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case 'v':
        mSerial.println(F("AT+BleKeyboardCode=01-00-19-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case 'y':
        mSerial.println(F("AT+BleKeyboardCode=01-00-1C-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case 'x':
        mSerial.println(F("AT+BleKeyboardCode=01-00-1B-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case 'z':
        mSerial.println(F("AT+BleKeyboardCode=01-00-1D-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;

      case 'A':
        mSerial.println(F("AT+BleKeyboardCode=03-00-04-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case 'C':
        mSerial.println(F("AT+BleKeyboardCode=03-00-06-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case 'F':
        mSerial.println(F("AT+BleKeyboardCode=03-00-09-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case 'G':
        mSerial.println(F("AT+BleKeyboardCode=03-00-0A-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case 'L':
        mSerial.println(F("AT+BleKeyboardCode=03-00-0F-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case 'R':
        mSerial.println(F("AT+BleKeyboardCode=03-00-15-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case 'S':
        mSerial.println(F("AT+BleKeyboardCode=03-00-16-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case 'V':
        mSerial.println(F("AT+BleKeyboardCode=03-00-19-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case 'Y':
        mSerial.println(F("AT+BleKeyboardCode=03-00-1C-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case 'X':
        mSerial.println(F("AT+BleKeyboardCode=03-00-1B-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case 'Z':
        mSerial.println(F("AT+BleKeyboardCode=03-00-1D-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      
      
      case '+':
        mSerial.println(F("AT+BleKeyboardCode=01-00-2E-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case '-':
        mSerial.println(F("AT+BleKeyboardCode=01-00-2D-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      default:
        ;
    }
    return;
  }


  
  if (key < 226)  {
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
      case KEY_STOP:
        mSerial.println(F("AT+BleKeyboardCode=00-00-29-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case KEY_RUN:
        mSerial.println(F("AT+BleKeyboardCode=00-00-44-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case KEY_DELETE:
        mSerial.println(F("AT+BleKeyboardCode=00-00-2A-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case KEY_CTRL:
        mSerial.println(F("AT+BleKeyboardCode=00-00-2B-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;
      case KEY_SHIFT_CTRL:
        mSerial.println(F("AT+BleKeyboardCode=02-00-2B-00-00"));
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

      case SHIFT_PLUS:
        mSerial.println(F("AT+BleHidControlKey=VOLUME+"));
        break;
      case SHIFT_MINUS:
        mSerial.println(F("AT+BleHidControlKey=VOLUME-"));
        break;


      case SHIFT_ARROWLEFT:
        mSerial.println(F("AT+BleHidControlKey=BRIGHTNESS-"));
        break;
      case SHIFT_ARROWUP:
        mSerial.println(F("AT+BleHidControlKey=BRIGHTNESS+"));
        break;


      case KEY_F1:
        mSerial.println(F("AT+BleKeyboardCode=02-00-50-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));
        break;

      case KEY_F2:
        mSerial.println(F("AT+BleKeyboardCode=02-00-4F-00-00"));
        mSerial.println(F("AT+BleKeyboardCode=00-00"));  
        break;


      case KEY_F3:
        mSerial.println(F("AT+BleHidMouseButton=L,click"));
        break;
      case KEY_F4:
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

