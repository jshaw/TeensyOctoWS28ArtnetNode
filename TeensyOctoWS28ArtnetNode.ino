/*
This example will receive multiple universes via Artnet and control a strip of ws2811 leds via 
Paul Stoffregen's excellent OctoWS2811 library: https://www.pjrc.com/teensy/td_libs_OctoWS2811.html
This example may be copied under the terms of the MIT license, see the LICENSE file for details
*/

#include <Artnet.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include <SPI.h>
#include <OctoWS2811.h>
#include <FastLED.h>

// OctoWS2811 settings
const int numPins = 8;
byte pinList[numPins] = { 2, 14, 7, 8, 6, 20, 21, 5};

// const int ledsPerStrip = 170; // change for your setup
// const int ledsPerStrip = 300; // change for your setup
// const int ledsPerStrip = 340; // change for your setup
const int ledsPerStrip = 300; // change for your setup
// const int ledsPerStrip = 600; // change for your setup
const byte numStrips= 8; // change for your setup

const int numLeds = ledsPerStrip * numStrips;
// Total number of channels you want to receive (1 led = 3 channels)
const int numberOfChannels = numLeds * 3;

const int config = WS2811_GRB | WS2811_800kHz;
// #define NUM_LEDS  2400
// #define NUM_STRIPS  8
// #define NUM_LEDS  170
// #define NUM_STRIPS  1
// #define BRIGHTNESS  150
#define BRIGHTNESS  150
#define FRAMES_PER_SECOND  30

#define DEBUG 0

CRGB rgbarray[numPins * ledsPerStrip];

DMAMEM int displayMemory[ledsPerStrip * numPins * 3 / 4];
int drawingMemory[ledsPerStrip * numPins * 3 / 4];

// OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);
OctoWS2811 octo(ledsPerStrip, displayMemory, drawingMemory, WS2811_GRB | WS2811_800kHz, numPins, pinList);

template <EOrder RGB_ORDER = GRB,
          uint8_t CHIP = WS2811_800kHz>
class CTeensy4Controller : public CPixelLEDController<RGB_ORDER, 8, 0xFF>
{
    OctoWS2811 *pocto;

public:
    CTeensy4Controller(OctoWS2811 *_pocto)
        : pocto(_pocto){};

    virtual void init() {}
    virtual void showPixels(PixelController<RGB_ORDER, 8, 0xFF> &pixels)
    {

        uint32_t i = 0;
        while (pixels.has(1))
        {
            uint8_t r = pixels.loadAndScale0();
            uint8_t g = pixels.loadAndScale1();
            uint8_t b = pixels.loadAndScale2();
            pocto->setPixel(i++, g, r, b);
            pixels.stepDithering();
            pixels.advanceData();
        }

        pocto->show();
    }
};

// Artnet settings
Artnet artnet;
const int startUniverse = 0; // CHANGE FOR YOUR SETUP most software this is 1, some software send out artnet first universe as 0.

// Check if we got all universes
const int maxUniverses = numberOfChannels / 512 + ((numberOfChannels % 512) ? 1 : 0);
bool universesReceived[maxUniverses];
bool sendFrame = 1;
int previousDataLength = 0;

// Change ip and mac address for your setup

// on macos, this mac address is for local host "ether" address
// for mac, it's en0
// MBP Mac address
byte mac[] = {0x3c, 0x22, 0xfb, 0x87, 0x16, 0x1b};

// RPi Mac address
// d8:3a:dd:33:ce:41
// byte mac[] = {0xd8, 0x3a, 0xdd, 0x33, 0xce, 0x41};


// IP of network.
// On mac using a ethernet dongle, this is configured in network
// On pi: {169, 254, 248, 133} - 169, 254, 248, 133

// on osx w/ dongle
byte ip[] = {192, 168, 1, 12};
// on Pi w/ dongle
// byte ip[] = {169, 254, 248, 133};

CTeensy4Controller<GRB, WS2811_800kHz> *pcontroller;

void setup()
{
  Serial.begin(115200);

  Serial.println("Studio Jordan Shaw");
  Serial.println("Same Material / Different Time");
  Serial.println("=================");
  Serial.println("Version: v-0.9.7");

  artnet.begin(mac, ip);

  octo.begin();
  pcontroller = new CTeensy4Controller<GRB, WS2811_800kHz>(&octo);

  FastLED.addLeds(pcontroller, rgbarray, numPins * ledsPerStrip).setCorrection(TypicalLEDStrip);
  FastLED.delay(1000/FRAMES_PER_SECOND);

  initTest();

  // this will be called for each packet received
  artnet.setArtDmxCallback(onDmxFrame);
}

void loop()
{

  // CHSV col = rgbToHsb(255, 0, 0);
  // Serial.println("col: " + col[0]);
  // Serial.println("col: " + col[1]);
  // Serial.println("col: " + col[2]);
  // Serial.println("--------------");

  // we call the read function inside the loop
  artnet.read();
}

const uint8_t PROGMEM gamma8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
  sendFrame = 1;

  // Store which universe has got in
  if ((universe - startUniverse) < maxUniverses)
    universesReceived[universe - startUniverse] = 1;

  for (int i = 0 ; i < maxUniverses ; i++)
  {
    if(DEBUG){
      Serial.print("universesReceived[i] \t");
      Serial.println(i);
      Serial.print("\t");
      Serial.println(universesReceived[i]);
    }

    if (universesReceived[i] == 0)
    {
      sendFrame = 0;
      break;
    }
  }

  if(DEBUG){
    // print out our data
    Serial.print("universe number = ");
    Serial.print(artnet.getUniverse());
    Serial.print("\tdata length = ");
    Serial.print(artnet.getLength());
    Serial.print("\tsequence n0. = ");
    Serial.println(artnet.getSequence());
    Serial.print("DMX data: ");
    // print out MORE data:
    Serial.print("ledsPerStrip = ");
    Serial.print(ledsPerStrip);
    Serial.print("\tmaxUniverses = ");
    Serial.print(maxUniverses);
    Serial.print("\tsendFrame = ");
    Serial.println(sendFrame);
  }

  // read universe and put into the right part of the display buffer
  for (int i = 0; i < length / 3; i++)
  {
    int led = i + (universe - startUniverse) * (previousDataLength / 3);
    if (led < numLeds){

      // if(DEBUG){
      //   Serial.print(artnet.getDmxFrame()[i]);
      //   Serial.println("  ");
      // }
      // rgbarray[led] = CRGB( data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);

      rgbarray[led] = CRGB( gamma8[data[i * 3]], gamma8[data[i * 3 + 1]], gamma8[data[i * 3 + 2]] );
      // rgbarray[led] = byte(artnet.getDmxFrame()[i]);
    }
  }
  previousDataLength = length;

  if (sendFrame)
  {
    if (DEBUG) {
      Serial.println("\t DRAW LEDs");
    }
    
    FastLED.show(); 
    // Reset universeReceived to 0
    memset(universesReceived, 0, maxUniverses);
  }
}

void initTest()
{
  for (int i = 0 ; i < numLeds ; i++)
    rgbarray[i] = CRGB::Red;
    FastLED.show();
    delay(500);
  for (int i = 0 ; i < numLeds  ; i++)
    rgbarray[i] = CRGB::Green;
    FastLED.show();
    delay(500);
  for (int i = 0 ; i < numLeds  ; i++)
    rgbarray[i] = CRGB::Blue;
    FastLED.show();
    delay(500);
  for (int i = 0 ; i < numLeds  ; i++)
    rgbarray[i] = CRGB::Black;
    FastLED.show();
}

  /**
   * Converts an RGB color value to HSB / HSV. Conversion formula
   * adapted from http://en.wikipedia.org/wiki/HSV_color_space.
   * ported from: https://github.com/carloscabo/colz/blob/master/public/js/colz.class.js
   *
   * @param   Number  r       The red color value
   * @param   Number  g       The green color value
   * @param   Number  b       The blue color value
   * @return  Array           The HSB representation
   */
   
//   CHSV rgbToHsb (int r, int g, int b) {
//     int max; 
//     int min;
//     float h = 0;
//     int s;
//     int v;
//     int d;

//     r = r / 255;
//     g = g / 255;
//     b = b / 255;

//     max = largest(r, g, b);
//     min = smallest(r, g, b);
//     v = max;

//     d = max - min;
//     if(max == 0){
//       max = 0;
//     } else {
//       max = d / max;
//     }
//     // s = max === 0 ? 0 : d / max;
//     s = max;

//     if (max == min) {
//       h = 0; // achromatic
//     } else {
//       if(max == r){
//         h = (g - b) / d + (g < b ? 6 : 0); 
//       } else if(max == g){
//         h = (b - r) / d + 2; 
//       } else if (max == b){
//         h = (r - g) / d + 4; 
//       }
//       // switch (max) {
//       //   case r: h = (g - b) / d + (g < b ? 6 : 0); 
//       //     break;
//       //   case g: h = (b - r) / d + 2; 
//       //     break;
//       //   case b: h = (r - g) / d + 4; 
//       //     break;
//       // }
//       h = h / 6;
//     }

//     // map top 360,100,100
//     h = round(h * 360);
//     s = round(s * 100);
//     v = round(v * 100);

//     return CHSV(h, s, v);
//   };

// int largest(int r, int g, int b){
//     unsigned int i; 

//     int val[] = {r,g,b};

//     // int[] val = new int[3];
//     // val[0] = r;  // Assign value to first element in the array
//     // val[1] = g; // Assign value to second element in the array
//     // val[2] = b;  // Assign value to third element in the array
    
//   // Initialize maximum element 
//   int max = val[0]; 
    
//   // Traverse array elements from second and 
//   // compare every element with current max 
//   for (i = 1; i < (sizeof(val) / sizeof(val[0])); i++) 
//       if (val[i] > max) 
//           max = val[i]; 
    
//   return max;
// }

// int smallest(int r, int g, int b){
//     unsigned int i; 
//     int val[] = {r,g,b};

//     // int[] val = new int[3];
//     // val[0] = r;  // Assign value to first element in the array
//     // val[1] = g; // Assign value to second element in the array
//     // val[2] = b;  // Assign value to third element in the array
    
//   // Initialize maximum element 
//   int min = val[0]; 
    
//   // Traverse array elements from second and 
//   // compare every element with current max 
//   for (i = 1; i < (sizeof(val) / sizeof(val[0])); i++) 
//       if (val[i] < min) 
//           min = val[i]; 
    
//   return min;
// }

