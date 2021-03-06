# C64 keys to modern bluetooth

Using a Arduino Pro Mini and UART Bluefriend with HID support to make an old C64 to a ble 4.0 keyboard.

I am sorry about my old 1982 commodore. the Mainboard is
still working. But I only want to use the high quality keyboard
for my modern notebook and desktop system.

Actually it is an improvement - but still usable.

*This project and keyboard layout has no focus on vice or mice c64 emulator!*

# to do

circuit

## parts

- [C64 keyboard only and not mainboard](https://en.wikipedia.org/wiki/Commodore_64)
- [Arduino Pro Mini](https://store.arduino.cc/arduino-pro-mini)
- [Bluefruit UART friend](https://learn.adafruit.com/introducing-the-adafruit-bluefruit-le-uart-friend)

# Bluefriend wireing

CTS to GND, GND to GND, arduino TX/RX to bluefruit RX/TX, and 3.7-5V power to VIN. I use the UART
and the AT commands to send HID keyboard keys via the Bluefriend Module. It is quite simple and
without any additional libary.

# Explain, explain

Again: it is a proof of concept and I did not spend to much brain power in it.
For example: `keypressed and holding that key` and `alt, alt+gr, F1..F12`
are not complete implemented. A combination of that keys and `shift` did not work..
if I (or you) spend more time: there are easy solutions for it.





## keymap

### ctrl

the ctrl will be implemented in commodore key with:

  g,r,f,l,y,z,s,c,v,a,x,0,plus,minus


### Caps/Shift Lock switch

The normal c64 keyboard connects this switch/button to the LEFT_SHIFT. But I want to identify
the Lock! Thus I cut the wire! If you press it, GND and A6 is connect (not connected? 10k resistor
between A6 and VCC makes it HIGH).

I use the us-layout on Linux. This `.Xmodmap` file in my home folder solves the "Umlaute" problem:

```
keycode 66 = Mode_switch
clear Lock
keycode 38 = a A adiaeresis Adiaeresis
keycode 30 = u U udiaeresis Udiaeresis
keycode 32 = o O odiaeresis Odiaeresis
keycode 39 = s S ssharp
```

With this change I have to press e.g. LEFT_SHIFT + SHIFT_LOCK + a to get a Ä!

### Restore is backslash and pipe

Similar to the Shift Lock: I add the 3rd connector (the 2nd wire from the c64)
to an A7 (analoge only) pin. Pressing RESTORE connects this wire to GND and
I add a 10k resistor to A7 and VCC to set it HIGH (analoge value>1000 is not pressed).

I did not realy need a RESTORE key, thus it is the \ key and with LEFT_SHIFT the |

### F1 and F3

... mark to the left and to the right.

### F5 and F7

... mark to up and to down.

### F6 and F8

... page up and page down.

### Cursor Keys and Mouse

It is different to the c64: with LEFT_SHIFT you get: cursor up / cursor left

Without shift: cursor down and cursor right.

Additionaly I add cursor left to the `<-` key and cursor up to the `up arrow` key.

With Commodore Key pressed: left shift is left-mouse-click and cursor keys are to move.

Commodore+SPACE is a right mouse click.

### Display brightness

LEFT_SHIFT and `<-` key makes brightness lower, LEFT_SHIFT and `up arrow` key makes
it brighter.

### Volume up/down

LEFT_SHIFT and + or - are mapped to volume keys.

### END HOME

Pressing CLR/HOME sets the cursor to the first position of the line.
In combination with LEFT_SHIFT the cursor is set to END.

### ESC and F11

The RUN/STOP key is ESC (with LEFT_SHIFT: F11).

### DEL and BACKSPACE

With LEFT_SHIFT and DEL you get the BACKSPACE.

### Missing keys

- the `^`: LEFT_SHIFT and `0`
- the ``` and `~`: `£` key (and LEFT_SHIFT)
- the TAB: use CTRL key
- the `_` underscore: LEFT_SHIFT and `=`
- the `{` and `}`: LEFT_SHIFT and `@` or `*`

Thats all folks! Some other chars are still lost.

## A4 A5 i2c

Unused! you may implement a mouse via MPU6050 gyroscope, poti or something like this.

# Wireing C64 Keyboard Plug

- 1: to GND
- 2: no wire, no hole
- 3: RESTORE key (to arduino A7)
- 4: hole, but no wire - not connected

Now, the "Matrix" wires:

- 5-12 (columns): to arduino Pin 9-2 
- 13-20 (rows): to arduino Pin 10, 11, 12, 13, A0, A1, A2, A3
