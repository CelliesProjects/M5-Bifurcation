//https://www.math.utah.edu/~jasonu/code/feigenbaum.cc
//https://transitionconsciousness.wordpress.com/2014/02/09/exploring-chaos/

//#include <M5Display.h>
//#include <TFT_eSPI.h>
#include <TFTShape.h>

#define backlight               32

TFT_eSPI    tft      = TFT_eSPI();
TFT_eSprite  doubleBuffer = TFT_eSprite(&tft);

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation( 1 );
  tft.fillScreen( TFT_BLACK );
  tft.setTextColor( TFT_WHITE, TFT_BLUE );

  ledcAttachPin( backlight, 0);
  ledcSetup( 0, 1300, 16 );
  ledcWrite( 0, 0xFFFF  / 16  ); //dimming the backlight will produce more base noise
}

inline float mapFloat( const double &x, const double &in_min, const double &in_max, const double &out_min, const double &out_max) {
  return ( x - in_min ) * ( out_max - out_min ) / ( in_max - in_min ) + out_min;
}

inline double f(const double r, const double x)
{
  return ( r * x * (1 - x) );
}

double rand_unit()
{
  return ( ((double)rand()) / RAND_MAX );
}

void loop() {
  const double r_min = 2.99;
  const double r_max = 4.0;

  const int num_intervals = tft.width();
  const int max_iterations = 600;

  srand(time(NULL));

  double r = r_min;
  double x_n;

  uint16_t xpos;
  uint16_t ypos;


  for (int i = 0; i < num_intervals; ++i) {
    r += (r_max - r_min) / num_intervals;
    x_n = f(r, rand_unit());
    //Serial.printf( "r=%f xpos=%i\n", r,xpos );
    for (int j = 0; j < max_iterations; ++j) {
      x_n = f(r, x_n);
      if (j > 500) {
        xpos = mapFloat( r, r_min, r_max, 0, tft.width());
        ypos = mapFloat( x_n, 0, 1, 0, tft.height());
      }
      tft.drawPixel( xpos, ypos, TFT_WHITE);
    }
  }

  pinMode(39,INPUT_PULLUP);
  while( digitalRead( 39 ) );
  tft.fillScreen(TFT_BLACK);

  uint16_t previous_x = 0;
  uint16_t previous_y = x_n;
  doubleBuffer.setRotation( 1 );
  doubleBuffer.createSprite( 200, 200);
  doubleBuffer.setColorDepth( 8 );
  doubleBuffer.setTextSize( 2 );
  double diff = 0.0001;
  for (r = 3.49999999; r < 4;)
  {
    if ( r > 3.5) diff = diff * 0.99999999999L;
    r += diff;
    x_n = 0.5;
    for (int i = 0; i < doubleBuffer.width(); i += 4) {
      x_n = f(r, x_n);
      ypos = mapFloat(x_n, 0, 1, 0, doubleBuffer.height());
      doubleBuffer.drawLine(previous_x, previous_y, i, ypos, TFT_WHITE);
      previous_x = i;
      previous_y = ypos;
    }
    char buff[20];
    snprintf(buff, sizeof(buff), "%f", r);
    doubleBuffer.drawString(buff, 10, 10);
    doubleBuffer.pushSprite( 10, 10 );
    doubleBuffer.fillScreen(TFT_BLUE);
    previous_x = 0;
  }
  while( digitalRead( 39 ) );
  tft.fillScreen(TFT_BLACK);
}
