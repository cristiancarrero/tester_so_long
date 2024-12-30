#!/bin/bash

# Colores para la salida
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Función de limpieza
cleanup() {
    echo -e "\n${YELLOW}Limpiando procesos...${NC}"
    pkill -f so_long
    pkill -f so_long_bonus
    pkill -f mlx
    rm -f ./tester
    echo -e "${GREEN}Limpieza completada${NC}"
}

# Asegurarse de que no hay procesos previos
cleanup

# Registrar la función de limpieza para ser ejecutada al salir
trap cleanup EXIT

# Función para manejar Ctrl+C
handle_interrupt() {
    echo -e "\n${YELLOW}Test interrumpido por el usuario${NC}"
    cleanup
    exit 1
}

# Registrar el manejador de interrupciones
trap handle_interrupt INT

# Obtener el directorio del script
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

# Cambiar al directorio del script
cd "$SCRIPT_DIR"

# Dar permisos a los scripts
chmod +x setup.sh create_maps.sh run_game.sh

# Crear directorios y enlaces simbólicos
./setup.sh

# Crear mapas de prueba
./create_maps.sh

# Verificar y compilar el proyecto principal si es necesario
if [ ! -f "../so_long" ]; then
    echo -e "${YELLOW}Compilando el proyecto mandatory...${NC}"
    (cd .. && make) || {
        echo -e "${RED}Error: No se pudo compilar so_long${NC}"
        exit 1
    }
fi

# Dar permisos de ejecución a so_long
chmod +x ../so_long

# Compilar el bonus si se solicita
if [ "$1" = "bonus" ] || [ "$1" = "all" ]; then
    echo -e "${YELLOW}Compilando el proyecto bonus...${NC}"
    (cd .. && make bonus) || {
        echo -e "${RED}Error: No se pudo compilar so_long_bonus${NC}"
        exit 1
    }
    # Dar permisos de ejecución a so_long_bonus
    chmod +x ../so_long_bonus
fi

# Compilar el tester
gcc -o tester tester.c || {
    echo -e "${RED}Error: No se pudo compilar el tester${NC}"
    exit 1
}

echo -e "\n${GREEN}=== SO_LONG TESTER ===${NC}\n"

# Ejecutar los tests según los argumentos
if [ "$1" = "bonus" ] && [ -f "../so_long_bonus" ]; then
    ./tester bonus
elif [ "$1" = "all" ] && [ -f "../so_long_bonus" ]; then
    ./tester all
else
    ./tester
fi

echo -e "\n${GREEN}=== TESTS COMPLETADOS ===${NC}\n" 