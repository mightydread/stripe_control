#include "main.h"


// NeopixelBus init
#ifdef SK6812
const int numberOfChannels = LED_NUMBER * 4; // Total number of channels you want to receive (1 led = 3 channels)
NeoPixelBus<NeoGrbwFeature, NeoSk6812Method> strip(LED_NUMBER, LED_PIN);
#endif
#ifdef WS2812
const int numberOfChannels = LED_NUMBER * 3; // Total number of channels you want to receive (1 led = 3 channels)
NeoPixelBus<NeoGrbFeature, NeoWs2812Method> strip(LED_NUMBER, LED_PIN);
#endif

// Artnet settings
ArtnetnodeWifi artnet;
const int startUniverse = 0;

// Calculate number of universes based on numer of leds
const int maxUniverses = numberOfChannels / 512 + ((numberOfChannels % 512) ? 1 : 0);
bool universesReceived[maxUniverses];
bool sendFrame = 1;
int previousDataLength = 0;

// connect to wifi – returns true if successful or false if not
boolean ConnectWifi(void)
{
  boolean state = true;
  int i = 0;
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(HOSTNAME);
  WiFi.begin(SSID, PSK);
  Serial.println("");
  Serial.println("Connecting to WiFi");
  
  // Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 20){
      state = false;
      break;
    }
    i++;
  }
  if (state){
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(SSID);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("");
    Serial.println("Connection failed.");
     ESP.restart();
  }
  
  return state;
}

void initTest()
{
  for (int i = 0 ; i < LED_NUMBER ; i++)
    strip.SetPixelColor(i, RgbwColor(127, 0, 0, 0));
  strip.Show();
  delay(300);
  for (int i = 0 ; i < LED_NUMBER ; i++)
    strip.SetPixelColor(i, RgbwColor(0, 127, 0, 0));
  strip.Show();
  delay(300);
  for (int i = 0 ; i < LED_NUMBER ; i++)
    strip.SetPixelColor(i, RgbwColor(0, 0, 127, 0));
  strip.Show();
  delay(300);
  for (int i = 0 ; i < LED_NUMBER ; i++)
    strip.SetPixelColor(i, RgbwColor(0, 0, 0, 127));
  strip.Show();
  delay(300);
  for (int i = 0 ; i < LED_NUMBER ; i++)
    strip.SetPixelColor(i, RgbwColor(0, 0, 0, 0));
  strip.Show();
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
  length = 512; 
  
  sendFrame = 1;

  // Store which universe has got in
  if ((universe - startUniverse) < maxUniverses)
    universesReceived[universe - startUniverse] = 1;

  for (int i = 0 ; i < maxUniverses ; i++)
  {
    if (universesReceived[i] == 0)
    {

      sendFrame = 0;
      break;
    }
  }

  // read universe and put into the right part of the display buffer
  for (int i = 0; i < length / 4; i++)
  {
    int led = i + (universe - startUniverse) * (previousDataLength / 4);
    if (led < LED_NUMBER)


      
      strip.SetPixelColor(led, RgbwColor(data[i * 4], data[i * 4 + 1], data[i * 4 + 2], data[i * 4 + 3]));
  }
  previousDataLength = length;     
  
  if (sendFrame)
  {
    strip.Show();
    //  Serial.println(previousDataLength);
    // Reset universeReceived to 0
    memset(universesReceived, 0, maxUniverses);
  }
}

void setup()
{
  Serial.begin(115200);
  artnet.setName(HOSTNAME);
  artnet.setNumPorts(1);
  artnet.enableDMXOutput(0);
  artnet.setStartingUniverse(startUniverse);
  ConnectWifi();
  artnet.begin();
  strip.Begin();
  initTest();

  // this will be called for each packet received
  artnet.setArtDmxCallback(onDmxFrame);
}

void loop()
{
  // we call the read function inside the loop
  artnet.read();

}