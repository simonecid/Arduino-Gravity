#include <rgb_lcd.h>

// Libraries used to control the LCD Display
#include <Wire.h>

// Instantiating a "LCD" object
rgb_lcd lcd;

const unsigned char relayPin = 4; // relay pin
const unsigned char buttonPin = 2; // button pin
const unsigned char irSensorPin = 3; // ir sensor pin
bool wasButtonPreviouslyPressed; // keep status of button at previous cycle

#define TOTAL_NUMBER_OF_SLITS 53
#define CLOCK_SPEED 16e6

#define USED_NUMBER_OF_SLITS 47
#define OFFSET_NUMBER_OF_SLITS 6

float x[TOTAL_NUMBER_OF_SLITS] = { 0, 0.007196666667, 0.01439333333, 0.02161116667, 0.0287655, 0.03595158333, 0.043307, 0.05050366667, 0.057658, 0.06485466667, 0.07206191667, 0.07927975, 0.08645525, 0.09363075, 0.100838, 0.108077, 0.1152525, 0.122428, 0.1296140833, 0.1368001667, 0.1440074167, 0.1511723333, 0.15840075, 0.1656291667, 0.1728258333, 0.179959, 0.1871768333, 0.1943840833, 0.2016019167, 0.2087668333, 0.2159740833, 0.2231601667, 0.2303356667, 0.2375640833, 0.2447713333, 0.2519468333, 0.2591223333, 0.2663401667, 0.2735474167, 0.2807440833, 0.287909, 0.295148, 0.3023658333, 0.3095519167, 0.3167274167, 0.32394525, 0.3311525, 0.3383703333, 0.34553525, 0.3527425, 0.35994975, 0.3671675833, 0.3743219167};

float times[TOTAL_NUMBER_OF_SLITS];
unsigned int slitIdx = 0;

unsigned long overflowCounter = 0;

// should be declared like that to optimize for the microcontroller core
float fit_G(int N_points, int offset, float px[], float py[]) __attribute__((__optimize__("O2")));

// if the button is pressed, we enable interrupts on the sensor and disable the magnet
void disableRelay()
{
  attachInterrupt( digitalPinToInterrupt( irSensorPin ), storePositionAndTime, FALLING );
  digitalWrite(relayPin, false);
  return;
}

void storePositionAndTime()
{
  //storing data
  if (slitIdx < TOTAL_NUMBER_OF_SLITS)
  {
    times[slitIdx] = micros()/1e6;
    
    slitIdx++;
  }

  //once we have stored all the data, re-enabling magnet and disabling data acquisition interrupt
  if (slitIdx == TOTAL_NUMBER_OF_SLITS) 
  {
    detachInterrupt( digitalPinToInterrupt( irSensorPin ) );
    digitalWrite(relayPin, true);
  }
}

// the setup function runs once when you press reset or power the board
void setup() {
  // put your setup code here, to run once:
  Serial.begin( 9600 );  // (9600 is the baudrate).
  
  // initialize digital pin 3 as an output.
  pinMode(relayPin, OUTPUT);
  // initialize digital pin 2 as an input.
  pinMode(buttonPin, INPUT);
  // initialise ir sensor pin
  pinMode(irSensorPin, INPUT);
  // initialise button status
  // Initialize the lcd, 2 rows of 16 chars
  lcd.begin(16, 2);
  // Set the background LED color
  lcd.setRGB(0, 255, 255);
  lcd.clear();

  // printing setup
  lcd.setCursor(0, 1);
  lcd.print("OFF: ");
  lcd.print(OFFSET_NUMBER_OF_SLITS);
  lcd.print(" SLITS: ");
  lcd.print(USED_NUMBER_OF_SLITS);

  digitalWrite(relayPin, true);
  attachInterrupt( digitalPinToInterrupt( buttonPin ), disableRelay, CHANGE );

}

// the loop function runs over and over again forever
void loop() {
  if (slitIdx == TOTAL_NUMBER_OF_SLITS)
  {
    for (char x = TOTAL_NUMBER_OF_SLITS - 1; x >= 0; x--)
    {
      times[x] -= times[OFFSET_NUMBER_OF_SLITS];
    }
    

    float g = fit_G(USED_NUMBER_OF_SLITS, OFFSET_NUMBER_OF_SLITS, times, x);

    float T0 = times[10];
    float T1 = times[30];
    float T2 = times[50]; // time in [seconds]
    
    float X0 = x[10];
    float X1 = x[30];
    float X2 = x[50]; // position in [meters]
    
    float v1 = (X1-X0)/(T1-T0); // [m/s]
    float v2 = (X2-X1)/(T2-T1);
//    float g = 2*(v2-v1)/(T2-T0); //[m/s²]
    
    lcd.setCursor(0, 0);
    lcd.print("g: ");
    lcd.print(g, 6);
    lcd.print("                        ");
    
    // Serial.println("Times are:");
    // for (unsigned char i = OFFSET_NUMBER_OF_SLITS; i < OFFSET_NUMBER_OF_SLITS + USED_NUMBER_OF_SLITS; i++) 
    // {
    //   Serial.print(times[i], 8);
    //   Serial.print("\t");
    //   Serial.println(x[i], 8);
    // }
    Serial.print("g is ");
    Serial.println(g, 6);
    slitIdx = 0;
  }
}

// The correct array of spaces between each of the holes are in the following array:

// USAGE: float g = fit_G( sizeof(x)/sizeof(float), t, x);

float fit_G( int N_points, int offset, float px[], float py[] ) {

  int i;
  float S00, S10, S20, S30, S40, S01, S11, S21;
  float denom, x, y, a, b, c;
  S00=S10=S20=S30=S40=S01=S11=S21=0;

  Serial.println("Fitting:");
  for (i=offset; i<N_points + offset; i++) {

    Serial.print(px[i], 8);
    Serial.print("\t");
    Serial.println(py[i], 8);
    x = px[i];
    y = py[i];
    //S00 += 1; // x^0+y^0
    S10 += x;
    S20 += x * x;
    S30 += x * x * x;
    S40 += x * x * x * x;
    S01 += y;
    S11 += x * y;
    S21 += x * x * y;
  }
  S00 = N_points;

  denom = S00*(S20*S40 - S30*S30) - S10*(S10*S40 - S20*S30) + S20*(S10*S30 - S20*S20);

  /* c = (S01*(S20*S40-S30*S30)-S11*(S10*S40-S20*S30)+S21*(S10*S30-S20*S20))/denom;
  b = (S00*(S11*S40-S30*S21)-S10*(S01*S40-S21*S20)+S20*(S01*S30-S11*S20))/denom;*/
  a = ( S00*(S20*S21 - S11*S30) - S10*(S10*S21 - S01*S30) + S20*(S10*S11 - S01*S20) )/denom;

  float g = a*2;
  return g;
}
