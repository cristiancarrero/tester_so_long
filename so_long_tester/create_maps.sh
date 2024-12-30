#!/bin/bash

# Crear directorios si no existen
mkdir -p maps/mandatory maps/bonus

# Mapas para mandatory (sin enemigos)
cat > maps/mandatory/valid_map.ber << 'EOF'
1111111111
1P00000C01
1000011001
100C000001
1000E00001
1111111111
EOF

cat > maps/mandatory/valid_large.ber << 'EOF'
11111111111111
1P000000C00001
1000011111C001
100C000000E001
1000000C000001
11111111111111
EOF

# Mapas para bonus (con enemigos)
cat > maps/bonus/valid_map.ber << 'EOF'
1111111111
1P00N00C01
1000011001
100C0M0001
1000E00001
1111111111
EOF

cat > maps/bonus/valid_large.ber << 'EOF'
11111111111111
1P000N00C00001
1000011111C001
100C0M0000E001
1000000C0N0001
11111111111111
EOF

cat > maps/bonus/enemy_near_player.ber << 'EOF'
1111111111
1PN0000C01
1000011001
100C000001
1000E00001
1111111111
EOF

cat > maps/bonus/enemy_near_exit.ber << 'EOF'
1111111111
1P00000C01
1000011001
100C000001
1000EN0001
1111111111
EOF

cat > maps/bonus/multiple_enemies.ber << 'EOF'
1111111111
1P00N00C01
10M0011N01
100C0M0001
1N00E00M01
1111111111
EOF

cat > maps/bonus/invalid_enemy_char.ber << 'EOF'
1111111111
1P00X00C01
1000011001
100C000001
1000E00001
1111111111
EOF

# Mapas de error comunes
cat > maps/invalid_chars.ber << 'EOF'
1111111111
1P00000X01
1000011001
100C000001
1000E00001
1111111111
EOF

# Crear mapa sin salida
cat > maps/no_exit.ber << 'EOF'
1111111111
1P00000C01
1000011001
100C000001
1000000001
1111111111
EOF

# Crear mapa sin coleccionables
cat > maps/no_collectibles.ber << 'EOF'
1111111111
1P00000001
1000011001
1000000001
1000E00001
1111111111
EOF

# Crear mapa no rectangular
cat > maps/not_rectangular.ber << 'EOF'
1111111111
1P00000C01
100001
100C000001
1000E00001
1111111111
EOF

# Crear mapa con paredes incompletas (sin EOF extra)
cat > maps/walls_incomplete.ber << 'EOF'
1111111111
1P00000C01
1000011001
100C000001
1000E00000
1111111110
EOF

# Crear mapa 1x1 (demasiado pequeño)
cat > maps/too_small_1x1.ber << 'EOF'
1
EOF

# Crear mapa 2x2 (demasiado pequeño)
cat > maps/too_small_2x2.ber << 'EOF'
11
11
EOF

# Crear mapa 3x3 (demasiado pequeño para tener todos los elementos)
cat > maps/too_small_3x3.ber << 'EOF'
111
1P1
111
EOF

# Crear mapa mínimo válido (el más pequeño posible con todos los elementos)
# Nota: Usar '0' (número cero, ASCII 48) para espacios vacíos
cat > maps/minimal_valid.ber << 'EOF'
1111
1PC1
1E11
1111
EOF

# Mapas adicionales para mandatory
cat > maps/multiple_players.ber << 'EOF'
1111111111
1P00000C01
1000011001
100C0P0001
1000E00001
1111111111
EOF

cat > maps/multiple_exits.ber << 'EOF'
1111111111
1P00000C01
1000011E01
100C000001
1000E00001
1111111111
EOF

cat > maps/no_walls.ber << 'EOF'
0000000000
0P00000C00
0000011000
000C000000
0000E00000
0000000000
EOF

cat > maps/map_with_spaces.ber << 'EOF'
1111111111
1P00000C01
1000 11001
100C000001
1000E00001
1111111111
EOF

cat > maps/empty_with_ber.ber << 'EOF'

EOF

# Mapas adicionales para bonus
cat > maps/bonus/blocked_path.ber << 'EOF'
1111111111
1P00000C01
1NNN011001
100C000001
1000E00001
1111111111
EOF

cat > maps/bonus/surrounded_player.ber << 'EOF'
1111111111
1NPN000C01
1NNN011001
100C000001
1000E00001
1111111111
EOF

cat > maps/bonus/surrounded_exit.ber << 'EOF'
1111111111
1P00000C01
1000011001
100C0NNN01
1N00ENM001
1111111111
EOF

cat > maps/bonus/surrounded_collectibles.ber << 'EOF'
1111111111
1P00N0NC01
1000011001
10NCN00001
1000E00001
1111111111
EOF