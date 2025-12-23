/**
 * Medidor de Nivel de Tanque de Agua
 * ===================================
 * ESP32-C3 Super Mini + Sensor AJ-SR04M
 *
 * Mide el nivel de agua en un tanque de 1000L y envía
 * los datos vía MQTT a una Raspberry Pi.
 *
 * Fecha: 2024-12-23
 */

#include "config.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>

// ============================================
// Variables Globales
// ============================================
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

unsigned long lastReadTime = 0;
bool ledState = false;

// ============================================
// Funciones del Sensor Ultrasónico
// ============================================

/**
 * Mide la distancia usando el sensor ultrasónico
 * @return Distancia en centímetros, -1 si hay error
 */
float measureDistance() {
  // Asegurar que trigger está en bajo
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);

  // Enviar pulso de 10us
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  // Medir duración del pulso de eco (timeout 30ms ≈ 500cm)
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0) {
    return -1;
  }

  // Calcular distancia: velocidad del sonido = 343 m/s = 0.0343 cm/us
  // Distancia = (tiempo * velocidad) / 2 (ida y vuelta)
  float distance = (duration * 0.0343) / 2.0;

  return distance;
}

/**
 * Obtiene un promedio de varias lecturas para mayor precisión
 * @return Distancia promedio en cm, -1 si hay error
 */
float getAverageDistance() {
  float readings[NUM_SAMPLES];
  int validReadings = 0;

  for (int i = 0; i < NUM_SAMPLES; i++) {
    float dist = measureDistance();
    if (dist > 0) {
      readings[validReadings++] = dist;
    }
    delay(SAMPLE_DELAY_MS);
  }

  if (validReadings == 0) {
    return -1;
  }

  // Ordenar para eliminar outliers
  for (int i = 0; i < validReadings - 1; i++) {
    for (int j = i + 1; j < validReadings; j++) {
      if (readings[i] > readings[j]) {
        float temp = readings[i];
        readings[i] = readings[j];
        readings[j] = temp;
      }
    }
  }

  // Calcular promedio excluyendo min y max si hay suficientes lecturas
  float sum = 0;
  int start = (validReadings >= 3) ? 1 : 0;
  int end = (validReadings >= 3) ? validReadings - 1 : validReadings;

  for (int i = start; i < end; i++) {
    sum += readings[i];
  }

  return sum / (end - start);
}

// ============================================
// Funciones WiFi
// ============================================

/**
 * Conecta a la red WiFi
 * @return true si la conexión fue exitosa
 */
bool connectWiFi() {
  Serial.printf("Conectando a WiFi: %s", WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long startTime = millis();

  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - startTime > WIFI_TIMEOUT_MS) {
      Serial.println("\n✗ Error: Timeout de conexión WiFi");
      return false;
    }
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.printf("✓ Conectado! IP: %s\n", WiFi.localIP().toString().c_str());
  return true;
}

// ============================================
// Funciones MQTT
// ============================================

/**
 * Callback para mensajes MQTT recibidos (no usado actualmente)
 */
void mqttCallback(char *topic, byte *payload, unsigned int length) {
  // Reservado para comandos futuros
}

/**
 * Conecta al broker MQTT
 * @return true si la conexión fue exitosa
 */
bool connectMQTT() {
  Serial.printf("Conectando a MQTT: %s:%d\n", MQTT_SERVER, MQTT_PORT);

  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);

  // Intentar conectar con Last Will
  String willTopic = String(MQTT_TOPIC) + "/status";

  if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD,
                         willTopic.c_str(), 0, true,
                         "{\"status\":\"offline\"}")) {
    Serial.println("✓ Conectado a MQTT!");

    // Publicar estado online
    mqttClient.publish(willTopic.c_str(), "{\"status\":\"online\"}", true);
    return true;
  } else {
    Serial.printf("✗ Error MQTT, código: %d\n", mqttClient.state());
    return false;
  }
}

/**
 * Asegura que la conexión MQTT esté activa
 */
void ensureMQTTConnection() {
  if (!mqttClient.connected()) {
    Serial.println("Reconectando MQTT...");
    connectMQTT();
  }
}

// ============================================
// Funciones de Cálculo
// ============================================

/**
 * Calcula el nivel de agua y genera el JSON para publicar
 */
void calculateAndPublish(float distance) {
  // Calcular distancia al agua (restando offset del sensor)
  float distanceToWater = distance - SENSOR_OFFSET_CM;

  // Calcular nivel de agua
  float waterLevelCm = TANK_HEIGHT_CM - distanceToWater;

  // Limitar valores
  if (waterLevelCm < 0)
    waterLevelCm = 0;
  if (waterLevelCm > TANK_HEIGHT_CM)
    waterLevelCm = TANK_HEIGHT_CM;

  // Calcular porcentaje y volumen
  float percentage = (waterLevelCm / TANK_HEIGHT_CM) * 100.0;
  float volumeLiters = (percentage / 100.0) * TANK_CAPACITY_L;

  // Crear JSON
  JsonDocument doc;
  doc["distance_cm"] = round(distance * 10) / 10.0;
  doc["water_level_cm"] = round(waterLevelCm * 10) / 10.0;
  doc["volume_liters"] = round(volumeLiters);
  doc["percentage"] = round(percentage * 10) / 10.0;
  doc["sensor_offset_cm"] = SENSOR_OFFSET_CM;
  doc["rssi"] = WiFi.RSSI();

  // Serializar y publicar
  char buffer[256];
  serializeJson(doc, buffer);

  if (mqttClient.publish(MQTT_TOPIC, buffer)) {
    Serial.printf("✓ Publicado: %s\n", buffer);
  } else {
    Serial.println("✗ Error al publicar");
  }
}

// ============================================
// Setup y Loop Principal
// ============================================

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("==================================================");
  Serial.println("  MEDIDOR DE NIVEL DE TANQUE DE AGUA");
  Serial.println("  ESP32-C3 Super Mini + AJ-SR04M");
  Serial.println("==================================================");
  Serial.println();

  // Configurar pines del sensor
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIGGER_PIN, LOW);

  // LED integrado (GPIO8 en ESP32-C3 Super Mini)
  pinMode(8, OUTPUT);

  // Conectar WiFi
  if (!connectWiFi()) {
    Serial.println("Reiniciando en 10 segundos...");
    delay(10000);
    ESP.restart();
  }

  // Conectar MQTT
  if (!connectMQTT()) {
    Serial.println("Reiniciando en 10 segundos...");
    delay(10000);
    ESP.restart();
  }

  Serial.printf("\nIniciando lecturas cada %d segundos...\n\n",
                READ_INTERVAL_MS / 1000);
}

void loop() {
  // Mantener conexión MQTT
  mqttClient.loop();

  // Verificar si es tiempo de leer
  if (millis() - lastReadTime >= READ_INTERVAL_MS) {
    lastReadTime = millis();

    // Toggle LED para indicar actividad
    ledState = !ledState;
    digitalWrite(8, ledState);

    // Verificar conexiones
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi desconectado. Reconectando...");
      connectWiFi();
    }

    ensureMQTTConnection();

    // Leer sensor
    float distance = getAverageDistance();

    if (distance > 0) {
      calculateAndPublish(distance);
    } else {
      Serial.println("⚠ Lectura de sensor inválida");
    }
  }
}
