# Medidor de Nivel de Tanque de Agua

Sistema IoT para monitorear el nivel de agua en un tanque de 1000 litros usando **NodeMCU ESP8266**, sensor ultrasónico **AJ-SR04M** y caudalímetro **YF-S201**.

## 📦 Componentes

| Componente | Descripción |
|------------|-------------|
| NodeMCU ESP8266 (LoLin) | Microcontrolador con WiFi |
| AJ-SR04M | Sensor ultrasónico impermeable (20-450cm) |
| YF-S201 | Sensor de caudal (1-30 L/min) |
| Panel Solar 6V 5W | Alimentación solar |
| TP4056 + Boost | Cargador de batería + elevador 5V |
| Batería 18650 | Almacenamiento de energía |

## 🔌 Conexiones

Ver esquema completo en [docs/wiring_diagram.md](docs/wiring_diagram.md)

```
NodeMCU LoLin              Sensores
─────────────              ────────
D2 (GPIO4)  ──────────────  TRIG (AJ-SR04M)
D1 (GPIO5)  ──────────────  ECHO (AJ-SR04M)
D5 (GPIO14) ──────────────  Signal (YF-S201)
3.3V        ──────────────  VCC sensores
GND         ──────────────  GND sensores
VIN         ◄─────────────  5V del Boost
```

## 🚀 Instalación

### 1. Configurar

Edita `include/config.h` con tus datos:

```cpp
#define WIFI_SSID     "Tu_WiFi"
#define WIFI_PASSWORD "Tu_Password"
#define MQTT_SERVER   "192.168.1.X"  // IP de tu Raspberry Pi
```

### 2. Compilar y subir

```bash
# Compilar
pio run

# Subir al NodeMCU
pio run --target upload

# Ver salida serial
pio device monitor
```

## 📊 Datos MQTT

Topic: `water-tank/level`

```json
{
  "distance_cm": 65.0,
  "volume_liters": 650,
  "percentage": 65.0,
  "flow_L_per_min": 0,
  "rssi": -67
}
```

## 🔧 Configuración del Tanque

```
     ┌───────────────┐ ← Sensor (en la tapa)
     │    30 cm      │ ← SENSOR_OFFSET_CM
     ├───────────────┤ ← Nivel máximo (100% = 1000L)
     │               │
     │   100 cm      │ ← TANK_HEIGHT_CM
     │               │
     └───────────────┘ ← Fondo (0% = 0L)
```

Edita en `config.h`:
- `TANK_HEIGHT_CM`: Altura del agua (fondo → nivel máximo)
- `SENSOR_OFFSET_CM`: Distancia del sensor al nivel máximo
- `TANK_CAPACITY_L`: Capacidad total en litros

## 📁 Estructura

```
water-tank-monitor/
├── platformio.ini      # Configuración PlatformIO
├── include/
│   └── config.h        # WiFi, MQTT, tanque
├── src/
│   └── main.cpp        # Código principal
├── docs/
│   └── wiring_diagram.md  # Esquema de conexiones
└── README.md
```

## 🐛 Troubleshooting

- **No conecta WiFi**: El ESP8266 solo soporta 2.4GHz
- **Lectura mínima ~20cm**: Es la distancia mínima del sensor AJ-SR04M
- **Error fuera de rango**: Objeto a menos de 28cm del sensor
