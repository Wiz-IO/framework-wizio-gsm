```cpp

#include <Arduino.h>

#include <DHT.h>
DHT dht(D3, DHT11); // arduino pin, sensor type

void dht_loop()
{
  // Wait a few seconds between measurements.
  delay(2000);
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F(" C, "));
  Serial.print(f);
  Serial.print(F(" F,  Heat index: "));
  Serial.print(hic);
  Serial.print(F(" C, "));
  Serial.print(hif);
  Serial.println(F(" F"));
}

void setup()
{
  Serial.begin(115200, true);
  printf("\n[APP] Comet 2021 Georgi Angelov\n");
  pinMode(LED_NET, OUTPUT);
  dht.begin();
}

void loop()
{
  led_blink(LED_NET, 500);
  dht_loop();
}

/*
Humidity: 34.00%  Temperature: 26.00 C, 78.80 F,  Heat index: 25.54 C, 77.98 F
Humidity: 34.00%  Temperature: 26.00 C, 78.80 F,  Heat index: 25.54 C, 77.98 F
Humidity: 34.00%  Temperature: 26.00 C, 78.80 F,  Heat index: 25.54 C, 77.98 F
Humidity: 34.00%  Temperature: 26.00 C, 78.80 F,  Heat index: 25.54 C, 77.98 F
*/

```
