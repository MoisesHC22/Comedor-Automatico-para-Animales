
// Incorporación de librerias.
#include <Arduino_JSON.h>
#ifdef ESP32
  #include <WiFi.h>
  #include <HTTPClient.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  #include <WiFiClient.h>
#endif

#include <Servo.h>


// Configuración del WiFi.
const char* ssid = "Pas";
const char* password = "123456789";

// Configuración de los componentes.
int LedVerde = 14;
Servo miServo;

// Configuración de API del tiempo.
const char *host = "api.timezonedb.com";
const char *apiKey = "";
const char *timeZone = "America/Mexico_City";


void setup() {

// Configuraciones iniciales.
pinMode(LedVerde, OUTPUT);
miServo.attach(12);

Serial.begin(115200);

// Función para conectar el wemos.
WiFi.begin(ssid, password);

    while(WiFi.status() != WL_CONNECTED)
    {
      delay(1000);
      Serial.println("Buscando conexion de wifi...");
      digitalWrite(LedVerde, HIGH);
      delay(700);
      digitalWrite(LedVerde, LOW);
      delay(700);
      digitalWrite(LedVerde, HIGH);
      delay(700);
      digitalWrite(LedVerde, LOW);
      delay(700);
    }
    // Imprimir la ip de la wemos.
    Serial.println(WiFi.localIP());

}

void loop() {

// Mandamos a llamar la función para la hora.
OptenerTiempo();
delay(60000);
}


/* 
 Función para obtener un JSON de la plataforma timezonedb
 para obtener la hora desde la nube.
*/

void OptenerTiempo(){
  WiFiClient client;
  if(!client.connect(host, 80)){
    Serial.println("Error de conexión");
    return;
  }
  String url = "/v2.1/get-time-zone?key=" + String(apiKey)+ "&format=json&by=zone&zone=" + String(timeZone);
  Serial.print("Solicitando hora a TimezoneDB...");
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
                unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println("Error: tiempo de espera agotado");
      client.stop();
      return;
    }
  }
  Serial.println("Respuesta recibida:");
   bool httpResponseStarted = false;
  String response;
  while (client.available()) {
    char c = client.read();
    if (c == '{' || httpResponseStarted) {
      httpResponseStarted = true;
      response += c;
    }
  }

  JSONVar myObject = JSON.parse(response);

  if (JSON.typeof(myObject) == "undefined") {
    Serial.println("Parsing input failed!");
    return;
  }


/* 
  Solo obtenemos la linea "formatted" del JSON para obtener la hora
  en un string.
*/
  String ConvertirString = myObject["formatted"];
  Serial.println(ConvertirString);

// Extraemos solo la hora y los minutos del string "ConvertirString"
  String ExtraerHora = ConvertirString.substring(11,16);
  Serial.println(ExtraerHora);

/*
  Realizamos un if en donde si la variable "ExtraerHora" es igual a la hora
  que agreguemos se activara el servomotor.
*/
    if(ExtraerHora == "20:26")
    {
     miServo.write(180);
     delay(10000);
     miServo.write(0);
     delay(10000);
    }
}
