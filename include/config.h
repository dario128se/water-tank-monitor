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
#define WIFI_SSID "Darío WiFi_IoT"
#define WIFI_PASSWORD "P!nch0C@st3ll4n0"

// ============================================
// Configuración MQTT
// ============================================
#define MQTT_SERVER "192.168.1.39"
#define MQTT_PORT 1883
#define MQTT_USER "nodered"
#define MQTT_PASSWORD "nodered040873"
#define MQTT_TOPIC "water-tank/level"
#define MQTT_CLIENT_ID "nodemcu-water-tank"

// ============================================
// Configuración del Sensor AJ-SR04M
// ============================================
#define TRIGGER_PIN 4 // NodeMCU Pin D2
#define ECHO_PIN 5    // NodeMCU Pin D1

// ============================================
// Configuración del Sensor de Caudal (YF-S201)
// ============================================
#define FLOW_SENSOR_PIN 14 // NodeMCU Pin D5
// Este factor de calibración convierte la frecuencia (Hz) a Litros/Minuto.
// Deberás ajustarlo para tu sensor específico. 7.5 es un valor común.
#define FLOW_CALIBRATION_FACTOR 7.5

// ============================================
// Configuración del Tanque
// ============================================
// Altura del tanque desde el fondo hasta el nivel máximo (en cm)
#define TANK_HEIGHT_CM 100.0

// Capacidad total del tanque en litros
#define TANK_CAPACITY_L 1000.0

// Distancia del sensor al nivel máximo de agua (en cm)
// Sensor en la tapa, 30cm sobre el nivel de llenado
#define SENSOR_OFFSET_CM 30.0

// Tolerancia para mediciones fuera de rango (en cm)
#define MEASUREMENT_TOLERANCE_CM 2.0

// ============================================
// Configuración de Batería (Divisor de voltaje)
// ============================================
// Resistencias: R1=100K, R2=33K
// Ratio = R2 / (R1 + R2) = 33 / 133 = 0.248
#define BATTERY_R1 100000.0 // 100K ohms
#define BATTERY_R2 33000.0  // 33K ohms
#define BATTERY_MAX_V 4.2   // Voltaje máximo batería 18650
#define BATTERY_MIN_V 3.0   // Voltaje mínimo batería 18650
#define ADC_MAX_VALUE 1024  // ESP8266 ADC es de 10 bits

// ============================================
// Configuración de Intervalos
// ============================================
// Intervalo entre lecturas (en milisegundos)
#define READ_INTERVAL_MS 60000 // 1 minuto

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
