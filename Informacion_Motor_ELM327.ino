#include "tft.h"
#include <ELMduino.h>
#include <WiFi.h>

//#include "ELM327WiFi.h"

typedef struct{
  char nombre_indicador[20];
  float valor_real;
  float valor_minimo_alarma;
  float valor_minimo_aviso;
  float valor_maximo_aviso;
  float valor_maximo_alarma;
  float bar_max;
  float bar_min;
}dato;

dato motor_1[11]={
//  name              real  min_alam min_alert  max_alert max_alarm  bar_max    bar_min
   {"RPM",            5600.0,    0.0,      1.0,    3000.0, 4000.0,    7000.0,    0.0},
   {"MAP",               0.0,    0.0,      1.0,    1400.0, 1850.0,    2000.0,    0.0},
   {"MAF",               1.0,    0.5,      1.0,      40.0,   56.0,      60.0,    0.0},
   {"P.COMBUSTIBLE",     0.3,    0.0,      0.1,       0.6,    0.7,       1.0,    0.0},
   {"T. AGUA",           0.0,    0.0,     10.0,     110.0,  118.0,     160.0,    0.0},
   {"T. ENGRASE",        0.0,    0.0,     10.0,     110.0,  118.0,     160.0,    0.0},
   {"EGT 1",          1000.0,    0.0,     10.0,    1200.0, 1250.0,    1300.0,    0.0},
   {"EGT 2",          1000.0,    0.0,     10.0,    1200.0, 1250.0,    1300.0,    0.0},
   {"EGT 3",          1000.0,    0.0,     10.0,    1200.0, 1250.0,    1300.0,    0.0},
   {"EGT 4",          1000.0,    0.0,     10.0,    1200.0, 1250.0,    1300.0,    0.0},
};
int numDatos = sizeof(motor_1) / sizeof(motor_1[0]);

// LovyanGFX
LGFX lcd; // Instancia de la clase LGFX

// ELM327
// Definir los datos de conexión Wi-Fi
const char* ssid = "WiFi-OBDII";
const char* password = "";
//IP Adress of your ELM327 Dongle
IPAddress server(192, 168, 0, 10);
WiFiClient client;
ELM327 myELM327;

int variable_a_leer;


void dibujarIndicadoresNumericos(dato datos[]) {
  int numIndicadores = 0;
  int columnas = 2;
  int filas = 4;
  int margen = 1;

  int anchoPantalla = lcd.width();
  int altoPantalla = lcd.height(); // * 2 / 3;

  int anchoIndicador = (anchoPantalla - (margen * (columnas + 1))) / columnas;
  int altoIndicador = (altoPantalla - (margen * (filas + 1))) / filas;

  //  lcd.fillScreen(TFT_BLACK);             // Rellenar fondo de pantalla en negro
  lcd.setTextColor(TFT_WHITE);
  lcd.setTextSize(1);

  for (int i = 0; i < filas; i++) {
    for (int j = 0; j < columnas; j++) {
      if (numIndicadores >= filas * columnas) break;

      int x = margen + j * (anchoIndicador + margen);
      int y = margen + i * (altoIndicador + margen);

      // Dibujar el rectángulo para el indicador
      lcd.drawRect(x, y, anchoIndicador, altoIndicador, TFT_WHITE);

      // Dibujar el título del indicador
      lcd.setTextColor(TFT_WHITE);
      lcd.setTextSize(2);
      lcd.drawString(datos[numIndicadores].nombre_indicador, x + 5, y + 5);

      // Borrar el valor numérico anterior con un rectángulo negro
      int valorX = x + 5;
      int valorY = y + 30;
      lcd.fillRect(valorX, valorY, anchoIndicador - 10, 30, TFT_BLACK); // Rectángulo para borrar el valor anterior

      // Dibujar el valor numérico del indicador
      lcd.setTextSize(3);
      lcd.drawFloat(datos[numIndicadores].valor_real, 2, x + 5, y + 30);

      // Borrar la barra anterior
      int barX = x + 10;
      int barY = y + altoIndicador - 30;
      lcd.fillRect(barX, barY, anchoIndicador - 20, 20, TFT_BLACK);

      // Limitar valor_real a estar entre bar_min y bar_max
      if (datos[numIndicadores].valor_real > datos[numIndicadores].bar_max) {
        datos[numIndicadores].valor_real = datos[numIndicadores].bar_max;
      } else if (datos[numIndicadores].valor_real < datos[numIndicadores].bar_min) {
        datos[numIndicadores].valor_real = datos[numIndicadores].bar_min;
      }

      // Dibujar la barra indicadora horizontal
      float range = datos[numIndicadores].bar_max - datos[numIndicadores].bar_min;
      float normalizedValue = (datos[numIndicadores].valor_real - datos[numIndicadores].bar_min) / range;
      int barWidth = (int)(normalizedValue * (anchoIndicador - 20)); // Ancho de la barra

      // Determinar el color de la barra
      if (datos[numIndicadores].valor_real > datos[numIndicadores].valor_maximo_alarma || datos[numIndicadores].valor_real < datos[numIndicadores].valor_minimo_alarma) {
        lcd.fillRect(barX, barY, barWidth, 20, TFT_RED);
      } else if (datos[numIndicadores].valor_real > datos[numIndicadores].valor_maximo_aviso || datos[numIndicadores].valor_real < datos[numIndicadores].valor_minimo_aviso) {
        lcd.fillRect(barX, barY, barWidth, 20, TFT_YELLOW);
      } else {
        lcd.fillRect(barX, barY, barWidth, 20, TFT_GREEN);
      }

      numIndicadores++;
    }
  }
}

void dibujarGraficosDeBarras(dato datos[]) {
  int columnas = 4;
  int filas = 1;
  int margen = 2;
  int posicion_datos = 6;
  int numDatos = 10;  // Por ejemplo

  int anchoPantalla = lcd.width();
  int altoPantalla = lcd.height() / 3;

  int anchoGrafico = (anchoPantalla - (margen * (columnas + 1))) / columnas;
  int altoGrafico = (altoPantalla - (margen * (filas + 1))) / filas;

  for (int i = 0; i < filas; i++) {
    for (int j = 0; j < columnas; j++) {
      int indiceGrafico = i * columnas + j + posicion_datos;

      if (indiceGrafico < numDatos) {
        int x = margen + j * (anchoGrafico + margen);
        int y = altoPantalla + margen + (1 + i) * (altoGrafico + margen);

        // Dibujar el título del gráfico con el nombre del indicador
        lcd.setTextColor(TFT_WHITE);
        lcd.setTextSize(1);
        lcd.drawString(datos[indiceGrafico].nombre_indicador, x + 5, y + 5);

        // Borrar la barra anterior.
        lcd.fillRect(x + 5, y + 20, anchoGrafico - 10, altoGrafico - 25, TFT_BLACK);

        // Determinar el color de la barra según valor_real, valor_minimo_alarma, etc.
        uint32_t color = TFT_GREEN;  // Verde por defecto

        if (datos[indiceGrafico].valor_real >= datos[indiceGrafico].valor_maximo_alarma ||
            datos[indiceGrafico].valor_real <= datos[indiceGrafico].valor_minimo_alarma) {
          color = TFT_RED;  // Rojo para alarma
        } else if ((datos[indiceGrafico].valor_real >= datos[indiceGrafico].valor_maximo_aviso && 
                    datos[indiceGrafico].valor_real < datos[indiceGrafico].valor_maximo_alarma) ||
                   (datos[indiceGrafico].valor_real <= datos[indiceGrafico].valor_minimo_aviso && 
                    datos[indiceGrafico].valor_real > datos[indiceGrafico].valor_minimo_alarma)) {
          color = TFT_YELLOW;  // Amarillo para aviso
        }

        // Dibujar la barra con el color correspondiente
        int alturaBarra = map(datos[indiceGrafico].valor_real, 
                              datos[indiceGrafico].bar_min, 
                              datos[indiceGrafico].bar_max, 
                              0, altoGrafico - 20);  // Escalar valor_real a altura de la barra
        lcd.fillRect(x + 5, y + altoGrafico - 5 - alturaBarra, anchoGrafico - 10, alturaBarra, color);

        // Dibujar el contorno del gráfico
        lcd.drawRect(x, y, anchoGrafico, altoGrafico, TFT_WHITE);
      }
    }
  }
}


void setup() {
  
  Serial.begin(115200);
  
  lcd.init();                            // Inicializar la pantalla
  lcd.fillScreen(TFT_BLACK);             // Rellenar fondo de pantalla en negro
  lcd.setTextColor(TFT_WHITE);
  lcd.setTextSize(2);
  
  // Conectar al Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    lcd.drawString("Conectando Wi-Fi ELM327", 5, 5);
  }
  lcd.drawString("Conectado Wi-Fi ELM327", 5, 25);

  // Conectar al adaptador ELM327
  if (client.connect(server, 35000))  // Creamos un shocket.
    lcd.drawString("Conectado a ELM327", 5, 45);
  else
  {
    lcd.drawString("Fallo adaptador ELM327", 5, 45);
    while(1);
  }
  myELM327.begin(client, false, 2000);

  // Configurar ELM327
  myELM327.sendCommand("ATSP0"); // Establecer protocolo automático

  lcd.drawString("Conectado ECU", 5, 65);

  variable_a_leer=0;
  delay(1000);

}

void loop(){

  // Código del loop  
  switch (variable_a_leer) {

    case 0:{
          float RPM = myELM327.rpm();
          if (myELM327.nb_rx_state == ELM_SUCCESS){
            motor_1[0].valor_real=RPM;
            dibujarIndicadoresNumericos(motor_1);
            //dibujarGraficosDeBarras(motor_1);
            variable_a_leer=1;
          }else{
            if (myELM327.nb_rx_state != ELM_GETTING_MSG){
              myELM327.printError();
              variable_a_leer=1;
            };
          };
        break;
    };
    
    case 1:{
          float MAP = 10.0*(float)myELM327.manifoldPressure();
          if (myELM327.nb_rx_state == ELM_SUCCESS){
            motor_1[1].valor_real=MAP;
            dibujarIndicadoresNumericos(motor_1);
            //dibujarGraficosDeBarras(motor_1);
            variable_a_leer=2;
          }else{
            if (myELM327.nb_rx_state != ELM_GETTING_MSG){
              myELM327.printError();
              variable_a_leer=2;
            };
          };
        break;
    };

    case 2:{
          float MAF = (float)myELM327.mafRate();
          if (myELM327.nb_rx_state == ELM_SUCCESS){
            motor_1[2].valor_real=MAF;
            dibujarIndicadoresNumericos(motor_1);
            //dibujarGraficosDeBarras(motor_1);
            variable_a_leer=3;
          }else{
            if (myELM327.nb_rx_state != ELM_GETTING_MSG){
              myELM327.printError();
              variable_a_leer=3;
            };
          };
        break;
    };
    case 3:{
      variable_a_leer=4;
      break;
    };

    case 4:{
          float ECT = myELM327.engineCoolantTemp();
          if (myELM327.nb_rx_state == ELM_SUCCESS){
            motor_1[4].valor_real=ECT;
            dibujarIndicadoresNumericos(motor_1);
            //dibujarGraficosDeBarras(motor_1);
            variable_a_leer=0;
          }else{
            if (myELM327.nb_rx_state != ELM_GETTING_MSG){
              myELM327.printError();
              variable_a_leer=0;
            };
          };
        break;
    };

    default:{
          variable_a_leer=0;
          break;
    };
  };
  
};
