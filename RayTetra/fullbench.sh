#!/bin/bash
INTERSECTING=0
NONINTERSECTING=$1
GRANULARITY=$3

let  "STEP = $NONINTERSECTING/(100/$GRANULARITY)"
let  "LIMIT = 100/$GRANULARITY"
echo Haines,Moller1,Moller2,Moller3,Segura0,Segura1,Segura2,STP0,STP1,STP2 >>output.csv

for ((a=0; a <= LIMIT ; a++))  # Double parentheses, and "LIMIT" with no "$".
do
  echo -n $a, >>output.csv
  ./RandomRayTetra input$a -i $INTERSECTING -n $NONINTERSECTING
  
  ./Bench input$a output.csv $2 
  let "INTERSECTING+=$STEP"
  let "NONINTERSECTING+=-$STEP"
done                           # A construct borrowed from 'ksh93'.

echo; echo
exit 0


