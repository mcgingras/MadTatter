//------------------------------------------------------------------------------
// 2 Axis CNC Controller
// T20 for Mad Tatter
// Modification of marginallycelver's demo.
//
//
// TODO:
// - add limiters
// - z axis
// - TCP protocol
//------------------------------------------------------------------------------

// ~~ Note: I've disabled all of the feedrate stuff.

#include "math.h"

//------------------------------------------------------------------------------
// CONFIG
// some of these I didn't use, but came from library. Might need later?
//------------------------------------------------------------------------------

#define VERSION        (1)      // firmware version
#define BAUD           (9600)  // How fast is the Arduino talking?
#define MAX_BUF        (64)     // What is the longest message Arduino can store?
#define STEPS_PER_TURN (400)    // depends on your stepper motor.  most are 200.
#define MIN_STEP_DELAY (50.0)
#define MAX_FEEDRATE   (1000000.0/MIN_STEP_DELAY)
#define MIN_FEEDRATE   (0.01)

// for arc directions
#define ARC_CW           (1)
#define ARC_CCW         (-1)

//------------------------------------------------------------------------------
// GLOBALS
//------------------------------------------------------------------------------

char  buffer[MAX_BUF];  // where we store the message until we get a newline
int   sofar;            // how much is in the buffer
float px, py;           // location
int LIFT_AMOUNT = 1000;   // how high to lift z on G00 rapid movement mode

float fr           =  0;
int MM_PER_SEGMENT =  50;
long  step_delay   =  10;
const int delay_between_steps_microsec = 10000;

// settings
char mode_abs=1;        // absolute mode?

// z_axis
const int z_max = 3500;
int z_plane;



// pins
const int step_pin_x = A1;
const int dir_pin_x = A0;
const int step_pin_y = D1;
const int dir_pin_y = D0;
const int step_pin_z = A3;
const int dir_pin_z = A2;
const int limit_x = D3;
const int limit_y = D2;
const int z_plane_pin = A4;
const int r_pin = A5;
const int g_pin = A6;
const int joystick_pin = A4;

//------------------------------------------------------------------------------
// METHODS
//------------------------------------------------------------------------------


void home(){
  while(digitalRead(limit_y) == LOW){
    xmstep(-1);
  }
  while(digitalRead(limit_x) == LOW){
    ymstep(-1);
  }
  position(0,0);
}


/**
 * sets the z axis plane at the beggining
 */
void setZ(){
    int STEP_AMT = 100;
    int btn = digitalRead(limit_x);
    int new_z, diff;
    int val = analogRead(z_plane_pin);
    z_plane = map(val, 0, 4095, 0, z_max);

    while(btn == LOW) {
      int joystick = analogRead(joystick_pin);
      int diff = joystick - 2920;
      int dir;
      if(abs(diff) > 100 and z_plane < z_max and z_plane > 0){
        if (diff > 100) { dir = -1; }
        else if (diff < -100) { dir = 1; }
        for (size_t i = 0; i < STEP_AMT; i++) {
          zmstep(dir);
        }
        z_plane += dir;
      }

      btn = digitalRead(limit_x);
      delay(100);
    }

}

/**
 * delay for the appropriate number of microseconds
 * @input ms how many milliseconds to wait
 */
void pause(long ms) {
  delay(ms/1000);
  delayMicroseconds(ms%1000);  // delayMicroseconds doesn't work for values > ~16k.
}

/**
 * Disables the motors
 */
void disable() {
  digitalWrite(step_pin_x, LOW);
  digitalWrite(step_pin_y, LOW);
}


/**
 * Set the feedrate (speed motors will move)
 * @input nfr the new speed in steps/second
 */
void feedrate(float nfr) {
  if(fr==nfr) return;  // same as last time?  quit now.

  if(nfr>MAX_FEEDRATE || nfr<MIN_FEEDRATE) {  // don't allow crazy feed rates
    Serial.print(F("New feedrate must be greater than "));
    Serial.print(MIN_FEEDRATE);
    Serial.print(F("steps/s and less than "));
    Serial.print(MAX_FEEDRATE);
    Serial.println(F("steps/s."));
    return;
  }
  step_delay = 1000000.0/nfr;
  fr = nfr;
}


/**
 * Set the logical position...
 * @input npx new position x
 * @input npy new position y
 */
void position(float npx,float npy) {
  // here is a good place to add sanity tests
  px=npx;
  py=npy;
}

/**
 * Steps the z axis motors
 * can be used to lift and lower z axis
 */
void zmstep(int dirz){
  if(dirz > 0){
    digitalWrite(dir_pin_z, LOW);
  }
  else{
    digitalWrite(dir_pin_z, HIGH);
  }
  digitalWrite(step_pin_z, HIGH);
  digitalWrite(step_pin_z, LOW);
  delayMicroseconds(delay_between_steps_microsec);
}


/**
 * Steps the x axis motors
 */
void xmstep(int dirx){
  if(dirx > 0){
    digitalWrite(dir_pin_x, HIGH);
  }
  else{
    digitalWrite(dir_pin_x, LOW);
  }
  digitalWrite(step_pin_x, HIGH);
  digitalWrite(step_pin_x, LOW);
  delayMicroseconds(delay_between_steps_microsec);
}

/**
 * Steps the y axis motors
 */
void ymstep(int diry){
  if(diry > 0){
    digitalWrite(dir_pin_y, HIGH);
  }
  else{
    digitalWrite(dir_pin_y, LOW);
  }
  digitalWrite(step_pin_y, HIGH);
  digitalWrite(step_pin_y, LOW);
  delayMicroseconds(delay_between_steps_microsec);
}

/**
 * Uses bresenham's line algorithm to move both motors
 * @input newx the destination x position
 * @input newy the destination y position
 **/
void line(float newx,float newy) {
  long i;
  long over= 0;

  float dx  = newx-px;
  float dy  = newy-py;
  int dirx = dx>0?1:-1;
  int diry = dy>0?1:-1;
  dx = abs(dx);
  dy = abs(dy);

  if(dx>dy) {
    over = dx/2;
    for(i=0; i<dx; ++i) {
      xmstep(dirx);
      over += dy;
      if(over>=dx) {
        over -= dx;
        ymstep(diry);
      }
      pause(step_delay);
    }
  } else {
    over = dy/2;
    for(i=0; i<dy; ++i) {
      ymstep(diry);
      over += dx;
      if(over >= dy) {
        over -= dy;
        xmstep(dirx);
      }
      pause(step_delay);
    }
  }

  px = newx;
  py = newy;
}


// returns angle of dy/dx as a value from 0...2PI
float atan3(float dy,float dx) {
  float a = atan2(dy,dx);
  if(a<0) a = (PI*2.0)+a;
  return a;
}


// This method assumes the limits have already been checked.
// This method assumes the start and end radius match.
// This method assumes arcs are not >180 degrees (PI radians)
// cx/cy - center of circle
// x/y - end position
// dir - ARC_CW or ARC_CCW to control direction of arc
void arc(float x,float y,float cx,float cy,float dir) {
  // get radius
  float dx = px - cx;
  float dy = py - cy;
  float radius=sqrt(dx*dx+dy*dy);

  // find angle of arc (sweep)
  float angle1=atan3(dy,dx);
  float angle2=atan3(y-cy,x-cx);
  float theta=angle2-angle1;

  Serial.println(angle1);
  Serial.println(angle2);

  if(dir>0 && theta<0) angle2+=2*PI;
  else if(dir<0 && theta>0) angle1+=2*PI;

  theta=angle2-angle1;

  float len = abs(theta) * radius;

  float i, segments = ceil( len / MM_PER_SEGMENT );
  float nx, ny, angle3, scale;

  for(i=0;i<segments;++i) {
    // interpolate around the arc
    scale = (i)/(segments);

    angle3 = ( theta * scale ) + angle1;
    nx = cx + cos(angle3) * radius;
    ny = cy + sin(angle3) * radius;

    // send it to the planner
    line(nx,ny);
  }

  line(x,y);
}


/**
 * Look for character /code/ in the buffer and read the float that immediately follows it.
 * @return the value found.  If nothing is found, /val/ is returned.
 * @input code the character to look for.
 * @input val the return value if /code/ is not found.
 **/
float parseNumber(char code,float val) {
  char *ptr=buffer;  // start at the beginning of buffer
  while((long)ptr > 1 && (*ptr) && (long)ptr < (long)buffer+sofar) {  // walk to the end
    if(*ptr==code) {  // if you find code on your walk,
      return atof(ptr+1);  // convert the digits that follow into a float and return it
    }
    ptr=strchr(ptr,' ')+1;  // take a step from here to the letter after the next space
  }
  return val;  // end reached, nothing found, return default val.
}


/**
 * write a string followed by a float to the serial line.  Convenient for debugging.
 * @input code the string.
 * @input val the float.
 */
void output(const char *code,float val) {
  Serial.print(code);
  Serial.println(val);
}


/**
 * print the current position, feedrate, and absolute mode.
 */
void where() {
  output("X",px);
  output("Y",py);
  // output("F",fr);
  Serial.println(mode_abs?"ABS":"REL");
}


/**
 * display helpful information
 */
void help() {
  Serial.print(F("THE MAD TATTER version "));
  Serial.println(VERSION);
  Serial.println(F("Commands:"));
  Serial.println(F("G00 [X(steps)] [Y(steps)] [F(feedrate)]; - line"));
  Serial.println(F("G01 [X(steps)] [Y(steps)] [F(feedrate)]; - line"));
  Serial.println(F("G02 [X(steps)] [Y(steps)] [I(steps)] [J(steps)] [F(feedrate)]; - clockwise arc"));
  Serial.println(F("G03 [X(steps)] [Y(steps)] [I(steps)] [J(steps)] [F(feedrate)]; - counter-clockwise arc"));
  Serial.println(F("G04 P[seconds]; - delay"));
  Serial.println(F("G90; - absolute mode"));
  Serial.println(F("G91; - relative mode"));
  Serial.println(F("G92 [X(steps)] [Y(steps)]; - change logical position"));
  Serial.println(F("M18; - disable motors"));
  Serial.println(F("M100; - this help message"));
  Serial.println(F("M114; - report position and feedrate"));
  Serial.println(F("All commands must end with a newline."));
}


/**
 * Read the input buffer and find any recognized commands.  One G or M command per line.
 */
void processCommand() {
  int cmd = parseNumber('G',-1);
  switch(cmd) {
  case  0: {
    for (size_t i = 0; i < LIFT_AMOUNT; i++) { zmstep(1); }
    line( parseNumber('X',(mode_abs?px:0)) + (mode_abs?0:px),
          parseNumber('Y',(mode_abs?py:0)) + (mode_abs?0:py) );
    for (size_t i = 0; i < LIFT_AMOUNT; i++) { zmstep(-1); }
    break;
    }
  case  1: { // line
    feedrate(parseNumber('F',fr));
    line( parseNumber('X',(mode_abs?px:0)) + (mode_abs?0:px),
          parseNumber('Y',(mode_abs?py:0)) + (mode_abs?0:py) );
    break;
    }
  case 2:
  case 3: {  // arc
      feedrate(parseNumber('F',fr));
      arc(parseNumber('X',(mode_abs?px:0)) + (mode_abs?0:px),
          parseNumber('Y',(mode_abs?py:0)) + (mode_abs?0:py),
          parseNumber('I',(mode_abs?px:0)) + (mode_abs?0:px),
          parseNumber('J',(mode_abs?py:0)) + (mode_abs?0:py),
          (cmd==2) ? -1 : 1);
      break;
    }
  case  4:  pause(parseNumber('P',0)*1000);  break;  // dwell
  case 90:  mode_abs=1;  break;  // absolute mode
  case 91:  mode_abs=0;  break;  // relative mode
  case 92:  // set logical position
    position( parseNumber('X',0),
              parseNumber('Y',0) );
    break;
  default:  break;
  }

  cmd = parseNumber('M',-1);
  switch(cmd) {
  case 18:  // disable motors
    disable();
    break;
  case 100:  help();  break;
  case 114:  where();  break;
  default:  break;
  }
}


/**
 * prepares the input buffer to receive a new message and tells the serial connected device it is ready for more.
 */
void ready() {
  sofar=0;  // clear input buffer
  Serial.print(F(">"));  // signal ready to receive input
}


/**
 * First thing this machine does on startup.  Runs only once.
 */
void setup() {
  Serial.begin(BAUD);  // open coms

  pinMode(step_pin_x, OUTPUT);
  pinMode(dir_pin_x, OUTPUT);
  pinMode(step_pin_y, OUTPUT);
  pinMode(dir_pin_y, OUTPUT);
  pinMode(step_pin_z, OUTPUT);
  pinMode(dir_pin_z, OUTPUT);
  pinMode(r_pin, OUTPUT);
  pinMode(g_pin, OUTPUT);
  pinMode(limit_x, INPUT);
  pinMode(limit_y, INPUT);
  pinMode(joystick_pin, INPUT);

  position(0,0);  // set staring position... add this in when we have limiters

  // home();  // limit switches
  setZ();
  help();  // say hello
  ready();
}


/**
 * After setup() this machine will repeat loop() forever.
 */
void loop() {
  // listen for serial commands
  while(Serial.available() > 0) {  // if something is available
    char c=Serial.read();  // get it
    Serial.print(c);  // repeat it back so I know you got the message
    if(sofar<MAX_BUF-1) buffer[sofar++]=c;  // store it
    if((c=='\n') || (c == '\r')) {
      // entire message received
      buffer[sofar]=0;  // end the buffer so string functions work right
      Serial.print(F("\r\n"));  // echo a return character for humans
      processCommand();  // do something with the command
      ready();
    }
  }
}
