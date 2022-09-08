#include <Arduino.h>
#include "M5StickCPlus.h"
#include "Wire.h"
#include "Adafruit_PWMServoDriver.h"
#include "Adafruit_ADS1X15.h"
#include "WiFi.h"
#include "PubSubClient.h"

#define SDA_PIN 32
#define SCL_PIN 33
#define ADC_ADDR1 0x48 // GND  /**/

Adafruit_ADS1115 ads;
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(); // Initiates library.

uint16_t servo_min[] = {60, 80, 80, 80};
uint16_t servo_max[] = {540, 540, 550, 540};
uint16_t max_pos[] = {23580, 23045, 23570, 26400};

struct IMU
{
  float ax;
  float ay;
  float az;
  float gx;
  float gy;
  float gz;
} imu;

#define WIFI_SSID "Tonii.iphone"
#define WIFI_PASSWD "123456789"
#define MQTT_SERVER "mqtt3.thingspeak.com"
#define SECRET_MQTT_CLIENT_ID "ESQ7HRAfGxIuHjEMNgYBJgQ"
#define SECRET_MQTT_USERNAME "ESQ7HRAfGxIuHjEMNgYBJgQ"
#define SECRET_MQTT_PASSWORD "Q2x+1cZbSltrux03aa+FZjrQ"
#define MQTT_CHANNEL "1850062"

// WIFI
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWD;
const char *mqtt_server = MQTT_SERVER;

// MQTT client
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi()
{

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect()
{
  // Loop until it is connected
  while (!client.connected())
  {

    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(SECRET_MQTT_CLIENT_ID, SECRET_MQTT_USERNAME, SECRET_MQTT_PASSWORD))
    {
      Serial.println("connected");

      char subscribe_topic[100];
      sprintf(subscribe_topic, "channels/%s/subscribe", MQTT_CHANNEL);
      client.subscribe(subscribe_topic);
      sprintf(subscribe_topic, "channels/%s/subscribe/fields/%s", MQTT_CHANNEL, "field1");
      client.subscribe(subscribe_topic);

      Serial.println("subscribed");
      M5.Lcd.fillScreen(GREEN);
      M5.Lcd.setCursor(15, 10);
      M5.Lcd.setTextColor(WHITE);
      M5.Lcd.setTextSize(4);
      M5.Lcd.printf("READY");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void mqttPublish(uint16_t *finger_respond, float acc_z)
{

  char topic[40];
  char payload[100];
  sprintf(topic, "channels/%s/publish", MQTT_CHANNEL);
  sprintf(payload, "field1=%d&field2=%d&field3=%d&field4=%d&field5=%.2f&stats=MQTTPUBLISH",
          finger_respond[0], finger_respond[1], finger_respond[2], finger_respond[3], acc_z);
  client.publish(topic, payload);
  Serial.println("MQTT Publish");
  delay(500);
}

void setup()
{
  Wire.begin(SDA_PIN, SCL_PIN);
  ads.begin(ADC_ADDR1, &Wire);
  pwm.begin();
  pwm.setPWMFreq(50);
  delay(20);
  M5.begin();
  M5.Lcd.print("Setup done!");
  M5.Imu.Init();

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  client.setBufferSize(2048);
}

void testOffset(uint8_t test_no)
{

  for (int angle = 0; angle <= 150; angle += 10)
  {
    pwm.setPWM(test_no, 0, angle);
    Serial.printf("val : %d\n", angle);
    delay(500);
  }
  for (int angle = 450; angle <= 600; angle += 10)
  {
    pwm.setPWM(test_no, 0, angle);
    Serial.printf("val : %d\n", angle);
    delay(500);
  }
}

void writeServo(uint8_t servo_no, float angle)
{
  uint16_t pwm_val = map(angle, 0, 180, servo_min[servo_no - 3], servo_max[servo_no - 3]);
  pwm_val = constrain(pwm_val, servo_min[servo_no - 3], servo_max[servo_no - 3]);
  pwm.setPWM(servo_no, 0, pwm_val);
  delay(5);
}

void free()
{
  for (uint8_t no = 3; no < 7; no++)
  {
    pwm.setPWM(no, 0, servo_max[no - 3]);
    delay(5);
  }
}

uint16_t finger_pos[5] = {0};
void readPos()
{
  for (size_t i = 0; i < 4; ++i)
  {
    finger_pos[i] = ads.readADC_SingleEnded(i);
    if (finger_pos[i] < 0)
      finger_pos[i] = 0;
  }
}

void lock()
{
  for (uint8_t no = 3; no < 7; no++)
  {
    uint16_t pwm_val = map(finger_pos[no - 3], 0, max_pos[no - 3], servo_min[no - 3], servo_max[no - 3]);
    pwm.setPWM(no, 0, pwm_val);
    delay(5);
  }
}

void loopServo()
{
  for (uint8_t no = 3; no < 7; no++)
  {
    pwm.setPWM(no, 0, servo_min[no - 3]);
    delay(5);
  }
  delay(2000);
  for (uint8_t no = 3; no < 7; no++)
  {
    pwm.setPWM(no, 0, servo_max[no - 3]);
    delay(5);
  }
  delay(2000);
  free();
}

void loop()
{

  static uint32_t last_time = millis();
  if (millis() - last_time > 20)
  {
    M5.Imu.getAccelData(&imu.ax, &imu.ay, &imu.az);
    readPos();
    if (imu.az > 1.2)
      free();
    else if (imu.az < 0.8)
      lock();

    if (!client.connected())
      reconnect();
    client.loop();

    mqttPublish(finger_pos,imu.az);
  }
}