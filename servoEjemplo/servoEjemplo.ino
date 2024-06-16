#include <WiFi.h>
#include <ESP32Servo.h>
#include <WebSocketsServer.h>

// Configuración del Wi-Fi
const char* ssid = "INFINITUM57C7_2.4";
const char* password = "4ApgSFasu4";

// Configuración de los Servomotores
Servo servo1;
Servo servo2;
int servoPin1 = 18;
int servoPin2 = 19;

// WebSocket server
WebSocketsServer webSocket = WebSocketsServer(81);
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
  servo1.write(0);
  servo2.write(0); 

  
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado.");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  // Iniciar el servidor HTTP
  server.begin();

  // Iniciar el servidor WebSocket
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Nuevo cliente conectado.");
    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();

    // Página web
    if (request.indexOf("GET / ") >= 0) {
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("Connection: close");
      client.println();
      client.println("<!DOCTYPE HTML>");
      client.println("<html>");
      client.println("<head>");
      client.println("<style>");
      client.println("body { font-family: Arial, sans-serif; text-align: center; }");
      client.println(".slider { width: 80%; margin: auto; }");
      client.println("</style>");
      client.println("</head>");
      client.println("<body><h1>Control de Servos</h1>");
      client.println("<p>Mover Servo 1 a Ángulo:</p>");
      client.println("<input type=\"range\" min=\"0\" max=\"180\" value=\"0\" class=\"slider\" id=\"servo1Range\">");
      client.println("<p>Ángulo: <span id=\"angle1Value\">90</span>°</p>");
      client.println("<p>Mover Servo 2 a Ángulo:</p>");
      client.println("<input type=\"range\" min=\"0\" max=\"180\" value=\"0\" class=\"slider\" id=\"servo2Range\">");
      client.println("<p>Ángulo: <span id=\"angle2Value\">90</span>°</p>");
      client.println("<p>Mover Ambos Servos a Ángulo:</p>");
      client.println("<input type=\"range\" min=\"0\" max=\"180\" value=\"0\" class=\"slider\" id=\"bothServoRange\">");
      client.println("<p>Ángulo: <span id=\"bothAngleValue\">90</span>°</p>");
      client.println("<script>");
      client.println("var slider1 = document.getElementById('servo1Range');");
      client.println("var angle1 = document.getElementById('angle1Value');");
      client.println("var slider2 = document.getElementById('servo2Range');");
      client.println("var angle2 = document.getElementById('angle2Value');");
      client.println("var bothSlider = document.getElementById('bothServoRange');");
      client.println("var bothAngle = document.getElementById('bothAngleValue');");
      client.println("var websocket = new WebSocket('ws://' + window.location.hostname + ':81/');");
      client.println("websocket.onopen = function(event) { console.log('Conectado al servidor WebSocket'); };");
      client.println("websocket.onclose = function(event) { console.log('Desconectado del servidor WebSocket'); };");
      client.println("slider1.oninput = function() { angle1.innerHTML = this.value; websocket.send('servo1:' + this.value); };");
      client.println("slider2.oninput = function() { angle2.innerHTML = this.value; websocket.send('servo2:' + this.value); };");
      client.println("bothSlider.oninput = function() { bothAngle.innerHTML = this.value; websocket.send('both:' + this.value); };");
      client.println("</script>");
      client.println("</body></html>");
    }
    client.stop();
    Serial.println("Cliente desconectado.");
  }

  webSocket.loop();
}

// Script WebSocket
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    String message = String((char*)payload);
    if (message.startsWith("servo1:")) {
      int angle1 = message.substring(7).toInt();
      angle1 = constrain(angle1, 0, 180);
      servo1.write(angle1);
    } else if (message.startsWith("servo2:")) {
      int angle2 = message.substring(7).toInt();
      angle2 = constrain(angle2, 0, 180);
      servo2.write(angle2);
    } else if (message.startsWith("both:")) {
      int bothAngle = message.substring(5).toInt();
      bothAngle = constrain(bothAngle, 0, 180);
      servo1.write(bothAngle);
      servo2.write(bothAngle);
    }
  }
}
