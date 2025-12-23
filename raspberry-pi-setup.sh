#!/bin/bash
# =============================================
# Script de configuración MQTT para Raspberry Pi
# Ejecutar en la Raspberry Pi 5
# =============================================

echo "=== Configurando Mosquitto MQTT Broker ==="

# Instalar Mosquitto si no está instalado
if ! command -v mosquitto &> /dev/null; then
    echo "Instalando Mosquitto..."
    sudo apt update
    sudo apt install -y mosquitto mosquitto-clients
else
    echo "Mosquitto ya está instalado"
fi

# Habilitar servicio
sudo systemctl enable mosquitto
sudo systemctl start mosquitto

# Crear archivo de configuración
echo "Configurando Mosquitto..."
sudo tee /etc/mosquitto/conf.d/water-tank.conf > /dev/null << 'EOF'
# Configuración para Water Tank Monitor
listener 1883
allow_anonymous false
password_file /etc/mosquitto/passwd
EOF

# Crear usuario MQTT (nodered)
echo "Creando usuario MQTT..."
sudo mosquitto_passwd -c -b /etc/mosquitto/passwd nodered nodered040873

# Reiniciar Mosquitto
sudo systemctl restart mosquitto

echo ""
echo "=== Configuración completada ==="
echo ""
echo "Para probar, ejecuta:"
echo "  mosquitto_sub -h localhost -u nodered -P nodered040873 -t 'water-tank/level'"
echo ""
echo "El ESP32 publicará los datos del nivel del tanque en ese topic."
