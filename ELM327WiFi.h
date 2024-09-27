
#include <ELMduino.h>

class ELM327WiFi {
  private:
    ELM327 myELM327;
    WiFiClient elmClient;
    const char* ssid;
    const char* password;
    const char* elmIP = "192.168.0.10";  // IP del ELM327 WiFi
    const int elmPort = 35000;           // Puerto del ELM327 WiFi

  public:
    ELM327WiFi(const char* wifiSSID, const char* wifiPassword) {
      ssid = wifiSSID;
      password = wifiPassword;
    }

    void begin() {
      // Conexión WiFi
      Serial.print("Conectando a la red WiFi: ");
      Serial.println(ssid);
      WiFi.begin(ssid, password);

      while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
      }
      Serial.println("\nConectado a la red WiFi");

      // Intentar conectar al ELM327
      if (!elmClient.connect(elmIP, elmPort)) {
        Serial.println("No se pudo conectar al ELM327 vía WiFi");
      } else {
        Serial.println("Conectado al ELM327 vía WiFi");
        myELM327.begin(elmClient);  // Iniciar ELM327 sobre la conexión WiFi
      }
    }

    float getRPM() {
      float rpm = myELM327.rpm();
      if (myELM327.status == ELM_SUCCESS) {
        return rpm;
      } else {
        Serial.println("Error al obtener RPM");
        return -1;
      }
    }

    float getMAP() {
      float map = myELM327.map();
      if (myELM327.status == ELM_SUCCESS) {
        return map;
      } else {
        Serial.println("Error al obtener MAP");
        return -1;
      }
    }

    float getCoolantTemp() {
      float temp = myELM327.coolantTemp();
      if (myELM327.status == ELM_SUCCESS) {
        return temp;
      } else {
        Serial.println("Error al obtener temperatura de refrigerante");
        return -1;
      }
    }
};

