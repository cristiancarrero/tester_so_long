#!/bin/bash

# Matar cualquier instancia previa
pkill -9 so_long 2>/dev/null
pkill -9 so_long_bonus 2>/dev/null

# Ejecutar el juego
cd .. && ./$1 so_long_tester/$2 >/dev/null 2>&1 &
game_pid=$!

# Esperar un momento y matar el proceso
sleep 0.5
kill -9 $game_pid 2>/dev/null

# Asegurarse de que no queden procesos
pkill -9 so_long 2>/dev/null
pkill -9 so_long_bonus 2>/dev/null

exit 0 