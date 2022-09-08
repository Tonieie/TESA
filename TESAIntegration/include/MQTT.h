#include <Arduino.h>
#include "PubSubClient.h"
#include "WiFi.h"

#define WIFI_SSID "1111111111"
#define WIFI_PASSWD "123456789"
#define MQTT_SERVER "192.168.217.124"

class MQTT
{
    public:
        MQTT()
        {
			client = new PubSubClient(espClient);
        }   

        ~MQTT()
        {
			delete(client);
        }   

		//0 : gyro, 1 : acceleration
		void publish(bool mux, const char *msg)
		{
			if (!client->connected())
				reconnect();
			client->loop();
			if(mux)
				client->publish("topgun/shabu/acc", msg);
			else
				client->publish("topgun/shabu/gyro", msg);
		}

		void begin()
		{
			connectWifi();
			setupClient();
		}

	private:
		WiFiClient espClient;
		PubSubClient *client;

		void connectWifi()
		{
			Serial.print("Connecting to ");
			Serial.println(WIFI_SSID);

			WiFi.mode(WIFI_STA);
			WiFi.begin(WIFI_SSID, WIFI_PASSWD);

			while (WiFi.status() != WL_CONNECTED)
			{
				delay(500);
				Serial.print(".");
			}

			Serial.println("");
			Serial.println("WiFi connected");
			Serial.println("IP address: ");
			Serial.println(WiFi.localIP());
		}

		void setupClient()
		{
			client->setServer(MQTT_SERVER, 1883);
			client->setCallback(callback);
			client->setBufferSize(2048);
		}

		static void callback(char *topic, byte *payload, unsigned int length)
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
			while (!client->connected())
			{

				Serial.print("Attempting MQTT connection...");
				// Attempt to connect
				if (client->connect("Tonii","tonii","1234"))
				{
					Serial.println("connected");
				}
				else
				{
					Serial.print("failed, rc=");
					Serial.print(client->state());
					Serial.println(" try again in 5 seconds");
					delay(5000);
				}
			}
		}

};