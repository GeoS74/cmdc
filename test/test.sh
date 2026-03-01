#!/bin/bash

# Цвета
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

# Путь к папке со скриптом
SCRIPT_DIR=$(dirname "$(realpath "$0")")
APP="$SCRIPT_DIR/../cmdc"

if [ ! -f "$APP" ]; then
    echo -e "${RED}Ошибка: парсер не найден. Сначала выполните make.${NC}"
    exit 1
fi

cd "$SCRIPT_DIR" || exit

# Номера примеров из спецификации
examples=(1 2)

# Цикл по массиву
for NUM in "${examples[@]}"; do
    $APP < "ex${NUM}.txt" > output.txt
    if diff -u "ex${NUM}_expect.txt" output.txt; then
        echo -e "${GREEN}Example ${NUM} - ok${NC}"
        rm output.txt
    else
        echo -e "${RED}Example ${NUM} - fail${NC}"
        # exit 1
    fi
done
