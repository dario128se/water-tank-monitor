# Medidor de Nivel de Tanque de Agua - ESP32-C3 + PlatformIO

Sistema IoT para monitorear el nivel de agua en un tanque de 1000 litros usando un **ESP32-C3 Super Mini** y sensor ultrasónico **AJ-SR04M**.

## 📦 Componentes

| Componente | Descripción |
|------------|-------------|
| ESP32-C3 Super Mini | Microcontrolador con WiFi |
| AJ-SR04M | Sensor ultrasónico impermeable (2-450cm) |
| Tanque | 1000 litros |

## 🔌 Conexiones

```
ESP32-C3 Super Mini          AJ-SR04M
┌─────────────────────┐     ┌──────────────┐
│        3.3V         │─────│     VCC      │
│        GND          │─────│     GND      │
│       GPIO4         │─────│     TRIG     │
│       GPIO5         │─────│     ECHO     │
└─────────────────────┘     └──────────────┘
```

> ⚠️ **Importante**: El sensor debe montarse en la parte superior del tanque, apuntando hacia abajo.

## 🚀 Instalación

### 1. Requisitos

- [PlatformIO](https://platformio.org/install) (CLI o extensión VS Code)
- Cable USB-C para el ESP32-C3

### 2. Configurar el proyecto

Edita `include/config.h` con tus datos (ya están configurados con tus credenciales):

```cpp
// WiFi
#define WIFI_SSID     "#######"
#define WIFI_PASSWORD "##########"

// MQTT
#define MQTT_SERVER   "###########"

// Tanque - AJUSTA ESTOS VALORES
#define TANK_HEIGHT_CM    100.0   // Altura interior del tanque
#define SENSOR_OFFSET_CM  5.0     // Distancia sensor → agua llena
```

### 3. Compilar y subir

```bash
cd water-tank-monitor

# Compilar
pio run

# Subir al ESP32-C3
pio run --target upload

# Ver salida serial
pio device monitor
```

## 📊 Datos MQTT

El dispositivo publica en el topic `water-tank/level`:

```json
{
  "distance_cm": 25.4,
  "water_level_cm": 74.6,
  "volume_liters": 746,
  "percentage": 74.6,
  "sensor_offset_cm": 5.0,
  "rssi": -45
}
```

### Recibir datos en la Raspberry Pi

```bash
mosquitto_sub -h localhost -u nodered -P nodered040873 -t "water-tank/level"
```

## 🔧 Calibración

```
    ┌─────────────────┐ ← Sensor AJ-SR04M
    │ SENSOR_OFFSET   │ ← Distancia sensor → agua llena (ej: 5cm)
    ├─────────────────┤ ← Nivel máximo agua
    │                 │
    │  TANK_HEIGHT    │ ← Altura útil del tanque
    │                 │
    └─────────────────┘ ← Fondo del tanque
```

## 📁 Estructura del Proyecto

```
water-tank-monitor/
├── platformio.ini          # Configuración PlatformIO
├── include/
│   └── config.h            # Configuración (WiFi, MQTT, tanque)
├── src/
│   └── main.cpp            # Código principal
└── README.md
```

## 🐛 Troubleshooting

- **No conecta WiFi**: Verifica SSID/password. El ESP32-C3 solo soporta 2.4GHz.
- **No conecta MQTT**: Verifica que Mosquitto esté corriendo en la Raspberry Pi.
- **Lecturas erráticas**: Asegúrate de que el sensor esté bien fijado y perpendicular.
