/**
 * Configuración del Medidor de Nivel de Tanque
 * =============================================
 * Edita estos valores según tu instalación
 */

#ifndef CONFIG_H
#define CONFIG_H

// ============================================
// Configuración WiFi
// ============================================
#define WIFI_SSID "Dario WiFi_IoT"
#define WIFI_PASSWORD "P!nch0C@st3ll4n0"

// ============================================
// Configuración MQTT
// ============================================
#define MQTT_SERVER "192.168.1.39"
#define MQTT_PORT 1883
#define MQTT_USER "nodered"
#define MQTT_PASSWORD "nodered040873"
#define MQTT_TOPIC "water-tank/level"
#define MQTT_CLIENT_ID "esp32c3-water-tank"

// ============================================
// Configuración del Sensor AJ-SR04M
// ============================================
#define TRIGGER_PIN 4 // GPIO4
#define ECHO_PIN 5    // GPIO5

// ============================================
// Configuración del Tanque
// ============================================
// IMPORTANTE: Mide estos valores en tu tanque real

// Altura interior útil del tanque (en centímetros)
// Desde el fondo hasta el nivel máximo de agua
#define TANK_HEIGHT_CM 100.0

// Capacidad total del tanque en litros
#define TANK_CAPACITY_L 1000.0

// Distancia del sensor al nivel máximo de agua (en centímetros)
// Cuando el tanque está completamente lleno
#define SENSOR_OFFSET_CM 5.0

// ============================================
// Configuración de Intervalos
// ============================================
// Intervalo entre lecturas (en milisegundos)
#define READ_INTERVAL_MS 30000 // 30 segundos

// Número de lecturas para promediar
#define NUM_SAMPLES 5

// Delay entre cada muestra (en milisegundos)
#define SAMPLE_DELAY_MS 50

// ============================================
// Configuración de Reconexión
// ============================================
#define WIFI_TIMEOUT_MS 20000 // 20 segundos
#define MQTT_RECONNECT_DELAY_MS 5000

#endif // CONFIG_H
