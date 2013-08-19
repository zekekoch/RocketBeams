#include <FastSPI_LED2.h>

//const int ledCount = 185;
const int ledCount =150;
const int NUM_STRIPS = 3;
const int tailLights = 2;
CRGB leds[NUM_STRIPS][ledCount];

int BOTTOM_INDEX = 0;
int TOP_INDEX = int(ledCount/2);
int EVENODD = ledCount%2;

//CRGB ledsX[ledCount]; //-ARRAY FOR COPYING WHATS IN THE LED STRIP CURRENTLY (FOR CELL-AUTOMATA, ETC)
//-PERISTENT VARS
byte idex = 0;        //-LED INDEX (0 to ledCount-1
byte ihue = 0;        //-HUE (0-360)
int ibright = 0;     //-BRIGHTNESS (0-255)
int isat = 0;        //-SATURATION (0-255)
bool bounceForward = true;  //-SWITCH FOR COLOR BOUNCE (0-1)
int bouncedirection = 0;
float tcount = 0.0;      //-INC VAR FOR SIN LOOPS
int lcount = 0;      //-ANOTHER COUNTING VAR


void setPixel(int adex, CRGB c) {
    for(int i = 0;i<NUM_STRIPS;i++)
    {
        leds[i][adex] = c;
    }
}

//-SET THE COLOR OF A SINGLE RGB LED
void setPixel(int adex, int cred, int cgrn, int cblu) {
    for(int i = 0;i<NUM_STRIPS;i++)
    {
        leds[i][adex] = CRGB(cred, cgrn, cblu);
    }
}

// fill_solid -   fill a range of LEDs with a solid color
void fillSolid(byte strand, const CRGB& color)
{
 fill_solid( leds[strand], ledCount, color);
}

void fillSolid(CRGB color)
{
    for(int i = 0;i<NUM_STRIPS;i++)
        fillSolid(i, color);
}

void fillSolid(int cred, int cgrn, int cblu) { //-SET ALL LEDS TO ONE COLOR
    fillSolid(0, CRGB(cred, cgrn, cblu));    
}

//-FIND INDEX OF HORIZONAL OPPOSITE LED
int horizontal_index(int i) {
    //-ONLY WORKS WITH INDEX < TOPINDEX
    if (i == BOTTOM_INDEX) {return BOTTOM_INDEX;}
    if (i == TOP_INDEX && EVENODD == 1) {return TOP_INDEX + 1;}
    if (i == TOP_INDEX && EVENODD == 0) {return TOP_INDEX;}
    return ledCount - i;
}


//-FIND INDEX OF ANTIPODAL OPPOSITE LED
int antipodal_index(int i) {
    //int N2 = int(ledCount/2);
    int iN = i + TOP_INDEX;
    if (i >= TOP_INDEX) {iN = ( i + TOP_INDEX ) % ledCount; }
    return iN;
}


//-FIND ADJACENT INDEX CLOCKWISE
int adjacent_cw(int i) {
    int r;
    if (i < ledCount - 1) {
        r = i + 1;
    }
    else {
        r = 0;
    }
    return r;
}


//-FIND ADJACENT INDEX COUNTER-CLOCKWISE
int adjacent_ccw(int i) {
    int r;
    if (i > 0 && i <= ledCount) {
        r = i - 1;
    }
    else {
        r = ledCount - 1;
    }
    return r;
}

void HSVtoRGB(int hue, int sat, int val, CRGB& c) {
    hsv2rgb_rainbow(CHSV(hue, sat, val), c);
}

CRGB HSVtoRGB(int hue, int sat, int val) {
    CRGB c;
    hsv2rgb_rainbow(CHSV(hue, sat, val), c);
    return c;
}

void nonReactiveFade() { //-BOUNCE COLOR (SIMPLE MULTI-LED FADE)
    static long lastBounceTime;
    const int bounceInterval = 250;
    
    static byte phase = 0;
    phase++;
    static byte trailLength = 5;
    if (phase % 8 == 0)
        trailLength = random(4,12);
    //Serial.print("trailLength: ");Serial.println(trailLength);
    byte trailDecay = (255-64)/trailLength;
    //Serial.print("trailDecay: ");Serial.println(trailDecay);
    static byte hue = random(0,10);

    fillSolid(HSVtoRGB(200, 255, 128));
    
    if (bounceForward) {
        //Serial.print("bouncing forward");Serial.println(idex);
        for(int i = 0;i<trailLength;i++)
        {
            setPixel(adjacent_cw(idex-i), HSVtoRGB(hue, 255, 255 - trailDecay*i));
        }
        idex++;
        if (idex == ledCount) {
            bounceForward = !bounceForward;
            idex--;
        }
    } else {
        //Serial.print("bouncing backwards idex:");Serial.print(idex);
        //todo: the trail is running off the array
        for(int i = 0;i<trailLength;i++)
        {
            setPixel(adjacent_ccw(idex+i), HSVtoRGB(hue, 255, 255 - trailDecay*i));
        }
        idex--;
        if (idex == 0) {
            bounceForward = !bounceForward;
        }
    }
    
}

void rotatingRainbow()
{
    static byte hue = 0;
    for(int i = 0;i < NUM_STRIPS;i++)
    {
        fill_rainbow(leds[i], ledCount, hue++, 1);
    }
}

void testStrips()
{
    fill_solid(leds[0], ledCount, CRGB::Purple);
    fill_solid(leds[tailLights], ledCount, CRGB::Red);
    fill_solid(leds[2], ledCount, CRGB::Purple);

}

void rotatingColors()
{
    static byte color = 0;
    for(int i = 0;i<3;i++)
    {
        fill_solid(leds[i], ledCount, (CRGB::HTMLColorCode)color++);
    }
}

void setup()
{
  // For safety (to prevent too high of a power draw), the test case defaults to
  // setting brightness to 25% brightness
  LEDS.setBrightness(55);
  
  LEDS.addLeds<WS2811,4, GRB>(leds[0], ledCount);
  LEDS.addLeds<WS2811,7, RGB>(leds[tailLights], ledCount);
  LEDS.addLeds<WS2811,10, GRB>(leds[2], ledCount);
  /*
  LEDS.addLeds<WS2811,7, GRB>(leds[2], ledCount);
  LEDS.addLeds<WS2811,8, GRB>(leds[3], ledCount);
  LEDS.addLeds<WS2811,6, GRB>(leds[4], ledCount);
  LEDS.addLeds<WS2811,20, GRB>(leds[5], ledCount);
  LEDS.addLeds<WS2811,21, GRB>(leds[6], ledCount);
  */

  Serial.begin(9600);

  fillSolid(0,0,0); //-BLANK STRIP  
  LEDS.show();
}

void pulseJets()
{
  static bool increment = true;
  static byte hue = 0;
  static byte redShift = 0;

  for (int i = 0;i<50;i++)
  {
    leds[tailLights][i].setHue(random(0,40));
    leds[tailLights][99-i].setHue(random(0,40));

    leds[tailLights][i] += CRGB(redShift,0,0);
    leds[tailLights][99-i]+= CRGB(redShift,0,0);
  }
    redShift+=5;
    delay(10);
  LEDS.show();
}

void smile()
{
  fill_solid(leds[tailLights], ledCount, CRGB::Black);
  //for (int i = 0; i<32;i++)
  //  leds[tailLights][i] = CRGB::Red;

  for (int i = 36; i<43;i++)
    leds[tailLights][i] = CRGB::Red;

  leds[tailLights][44] = CRGB::Blue;
  leds[tailLights][34] = CRGB::Blue;

  leds[tailLights][49] = CRGB::Purple;
}

void bigSmile()
{
  fill_solid(leds[tailLights], ledCount, CRGB::Black);

  for (int i = 8; i<23;i++)
    leds[tailLights][i] = CRGB::Red;

  leds[tailLights][3] = CRGB::Blue;
  leds[tailLights][27] = CRGB::Blue;

  leds[tailLights][49] = CRGB::HotPink;

  LEDS.show();
}


void loop()
{
  //rotatingRainbow();
  if(random(0,5000) == 0)
  {
      smile();
      LEDS.show();
      delay(1000);
  }
  else
  {
    pulseJets();
  }//wink();
  //rotatingColors();
  //nonReactiveFade();
  //fillSolid(CRGB::Blue);
  //nonReactiveFade();
  //fillSolid(CRGB::HotPink);
  LEDS.show();
  //fillSolid(CRGB::Pink);
    // delay(delayTime);
}

