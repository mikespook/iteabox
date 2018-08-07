#!/usr/bin/env bash
F=`mktemp`
tr --delete "\n\t" < index.html  > $F
ROOT=`sed 's/"/\\\\"/g' $F`
rm $F

F=`mktemp`
tr --delete "\n\t" < redirect.html  > $F
REDIRECT=`sed 's/"/\\\\"/g' $F`
rm $F

T=`cat template.h`
printf "$T" "$ROOT" "$REDIRECT"> ../iTeaHTML.h
