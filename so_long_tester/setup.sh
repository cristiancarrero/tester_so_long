#!/bin/bash

# Dar permisos a los directorios de origen primero
chmod -R 755 ../textures

# Crear directorios necesarios
mkdir -p textures/mandatory textures/bonus

# Copiar las texturas en lugar de crear enlaces simbólicos
cp -r ../textures/* textures/

# Dar permisos recursivamente al directorio textures
chmod -R 755 textures
find textures -type f -exec chmod 644 {} \; 