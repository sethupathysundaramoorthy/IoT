#include <ESP8266WiFi.h>
#include <DHT.h>

#define DHTPIN D1         // D1 on NodeMCU (GPIO5)
#define DHTTYPE DHT11     // DHT11 sensor type

const char* ssid = "YOUR_WIFI_SSID";      // Wi-Fi SSID
const char* password = "YOUR_WIFI_PASSWORD"; // Wi-Fi Password
const char* server = "api.thingspeak.com"; // ThingSpeak server

String apiKey = "YOUR_THINGSPEAK_API_KEY"; // ThingSpeak Write API Key

DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(10);
  dht.begin();

  // Connect to Wi-Fi
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");
}

void loop() {
  // Read temperature and humidity from DHT11
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(); // Celsius by default

  // Check if readings failed
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Print readings to Serial Monitor
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %, Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C");

  // Connect to ThingSpeak and send data
  if (client.connect(server, 80)) {
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(temperature);
    postStr += "&field2=";
    postStr += String(humidity);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: " + String(server) + "\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: " + String(postStr.length()) + "\n\n");
    client.print(postStr);

    Serial.println("Data sent to ThingSpeak");
  }

  client.stop();
  delay(20000); // ThingSpeak allows updates every 15 seconds
}
