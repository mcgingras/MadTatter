//------------------------------------------------------------------------------
// 2 Axis CNC Controller
// T20 for Mad Tatter
// Michael Gingras, Andrew Grossfeld, Anna Kamphampaty
// Modification of marginallycelver's demo.
//------------------------------------------------------------------------------

#include "config.h"

//------------------------------------------------------------------------------
// GLOBALS
//------------------------------------------------------------------------------

char  buffer[MAX_BUF];  // where we store the message until we get a newline
int   sofar;            // how much is in the buffer
float px, py;           // location

// speeds
float fr =     0;       // human version
long  step_delay;       // machine version

// settings
char mode_abs=1;        // absolute mode?

// pins
const int step_pin_x = A1;
const int dir_pin_x  = A0;
const int step_pin_y = D1;
const int dir_pin_y  = D0;


//------------------------------------------------------------------------------
// METHODS
//------------------------------------------------------------------------------


/**
 * delay for the appropriate number of microseconds
 * @input ms how many milliseconds to wait
 */
void pause(long ms) {
  delay(ms/1000);
  delayMicroseconds(ms%1000);
}



/**
 * Set the logical position...
 * @input npx new position x
 * @input npy new position y
 */
void position(float npx,float npy) {
  px=npx;
  py=npy;
}

/**
 * Steps the x axis motors
 */
void xmstep(){
  digitalWrite(step_pin_x, HIGH);
  digitalWrite(step_pin_x, LOW);
}

/**
 * Steps the y axis motors
 */
void ymstep(){
  digitalWrite(step_pin_y, HIGH);
  digitalWrite(step_pin_y, LOW);
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
  int diry = dy>0?-1:1;
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
      xmstep(diry);
      over += dx;
      if(over >= dy) {
        over -= dy;
        ymstep(dirx);
      }
      pause(step_delay);
    }
  }

  px = newx;
  py = newy;
}

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
void arc(float cx,float cy,float x,float y,float dir) {
  // get radius
  float dx = px - cx;
  float dy = py - cy;
  float radius=sqrt(dx*dx+dy*dy);

  float angle1=atan3(dy,dx);
  float angle2=atan3(y-cy,x-cx);
  float theta=angle2-angle1;

  if(dir>0 && theta<0) angle2+=2*PI;
  else if(dir<0 && theta>0) angle1+=2*PI;

  theta=angle2-angle1;
  float len = abs(theta) * radius;

  int i, segments = ceil( len * MM_PER_SEGMENT );

  float nx, ny, angle3, scale;

  for(i=0;i<segments;++i) {
    scale = ((float)i)/((float)segments);
    angle3 = ( theta * scale ) + angle1;
    nx = cx + cos(angle3) * radius;
    ny = cy + sin(angle3) * radius;
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
  char *ptr=serialBuffer;
  while((long)ptr > 1 && (*ptr) && (long)ptr < (long)serialBuffer+sofar) {
    if(*ptr==code) {
      return atof(ptr+1);
    }
    ptr=strchr(ptr,' ')+1;
  }
  return val;
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
  output("F",fr);
  Serial.println(mode_abs?"ABS":"REL");
}


/**
 * display helpful information
 */
void help() {
  Serial.print(F("GcodeCNCDemo2AxisV1 "));
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
  int cmd = parsenumber('G',-1);
  switch(cmd) {
  case  0:
  case  1: { // line
    feedrate(parsenumber('F',fr));
    line( parsenumber('X',(mode_abs?px:0)) + (mode_abs?0:px),
          parsenumber('Y',(mode_abs?py:0)) + (mode_abs?0:py) );
    break;
    }
  case 2:
  case 3: {  // arc
      feedrate(parsenumber('F',fr));
      arc(parsenumber('I',(mode_abs?px:0)) + (mode_abs?0:px),
          parsenumber('J',(mode_abs?py:0)) + (mode_abs?0:py),
          parsenumber('X',(mode_abs?px:0)) + (mode_abs?0:px),
          parsenumber('Y',(mode_abs?py:0)) + (mode_abs?0:py),
          (cmd==2) ? -1 : 1);
      break;
    }
  case  4:  pause(parsenumber('P',0)*1000);  break;  // dwell
  case 90:  mode_abs=1;  break;  // absolute mode
  case 91:  mode_abs=0;  break;  // relative mode
  case 92:  // set logical position
    position( parsenumber('X',0),
              parsenumber('Y',0) );
    break;
  default:  break;
  }

  cmd = parsenumber('M',-1);
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
  Serial.begin(BAUD);

  pinMode(step_pin_x, OUTPUT);
  pinMode(dir_pin_x, OUTPUT);
  pinMode(step_pin_y, OUTPUT);
  pinMode(dir_pin_y, OUTPUT);

  // position(0,0);  // set staring position... add this in when we have limiters

  help();
  ready();
}


/**
 * After setup() this machine will repeat loop() forever.
 */
void loop() {
  while(Serial.available() > 0) {
    char c=Serial.read();
    Serial.print(c); 
    if(sofar<MAX_BUF-1) buffer[sofar++]=c;
    if((c=='\n') || (c == '\r')) {
      buffer[sofar]=0;
      Serial.print(F("\r\n"));
      processCommand();
      ready();
    }
  }
}
