#!/bin/bash

# Controllo argomenti
if [ $# -lt 2 ]; then
    echo "Uso corretto: $(basename "$0") <visualizzatori> <N>"
    exit 1
fi

# Estrae valori passati da linea di comando
VISUALIZZATORI=$1
N=$2

# Percorso assoluto dello script
SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
# Definizione variabili per i file 
EXECUTABLE="$SCRIPTPATH/coordinatore"
SOURCE1="$SCRIPTPATH/coordinatore.c"
SOURCE2="$SCRIPTPATH/utility.c"
FLAGS="-lrt -pthread"

# Compilazione
echo "Compilazione in corso..."
gcc -o "$EXECUTABLE" "$SOURCE1" "$SOURCE2" $FLAGS
RET=$?

if [ $RET -ne 0 ]; then
    echo "Compilazione fallita con codice $RET"
    exit $RET
fi

echo "Compilazione completata con successo."

# Avvio del programma
echo "▶ Avvio del coordinatore con $VISUALIZZATORI visualizzatori, fino a $N"
"$EXECUTABLE" "$VISUALIZZATORI" "$N"

exit 0