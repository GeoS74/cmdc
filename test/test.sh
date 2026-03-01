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

TOTAL=0
PASSED=0

while read -r INPUT_FILE; do
    TOTAL=$((TOTAL + 1))
    
    # Получаем базовое имя файла без пути и расширения
    BASENAME=$(basename "$INPUT_FILE" .txt)
    DIRNAME=$(dirname "$INPUT_FILE")
    
    # Ожидаемый файл лежит рядом с входным
    EXPECT_FILE="${DIRNAME}/${BASENAME}_expect.txt"
    
    if [[ ! -f "$EXPECT_FILE" ]]; then
        echo -e "${RED}${BASENAME} in ${DIRNAME} - ожидаемый файл не найден${NC}"
        continue
    fi
    
    $APP < "$INPUT_FILE" > output.txt
    if diff -u "$EXPECT_FILE" output.txt; then
        PASSED=$((PASSED + 1))
        echo -e "${GREEN}${BASENAME} in ${DIRNAME} - ok${NC}"
        rm output.txt
    else
        echo -e "${RED}${BASENAME} in ${DIRNAME} - fail${NC}"
        # exit 1
    fi
done < <(find . -type f -name "ex[0-9]*.txt" ! -name "*_expect.txt" | 
    while read -r f; do
        num=$(basename "$f" | sed -n 's/ex\([0-9]*\)\.txt/\1/p')
        echo "$num:$f"
    done | sort -n | cut -d: -f2-)

# Вывод статистики
echo -e "\n${GREEN}Passed ${PASSED}/${TOTAL} tests${NC}"