 #!/bin/bash

# Benchmarks all available algorithms for a given number of test pairs. 
# A series of benchmarks is performed for each algorithm.Each benchmark has a higher percentage of intersecting 
# to non-intersecting pairs. This percentage starts at 0% and increases in every benchmark by a percentage equal to
# GRANULARITY.For example, a granularity of 20 would result in 6 tests with percentages :0% 20% 40% 60% 80% 100%.
# Each benchmark is repeated as specified by the user.Output is given in CSV format in output.csv
# CSV Columns are comma separated

# Usage: ./fullbench.sh TOTAL_PAIRS REPETITIONS GRANULARITY

if (($# < 3 ||$# > 3))
then
    echo "Usage: ./fullbench.sh TOTAL_PAIRS REPETITIONS GRANULARITY"
    exit 1
fi

INTERSECTING=0
NONINTERSECTING=$1
GRANULARITY=$3
CURRENT_PERCENTAGE=0

let  "STEP = $NONINTERSECTING/(100/$GRANULARITY)"
let  "LIMIT = 100/$GRANULARITY"

rm -f output.csv

echo Haines,Moller1,Moller2,Moller3,Segura0,Segura1,Segura2,STP0,STP1,STP2,GPU_Segura0,GPU_STP0,GPU_STP1,GPU_STP2>>output.csv

for ((a=0; a <= LIMIT ; a++)) 
do
  echo -n $a, >>output.csv
  
  echo  
  echo "Benchmarking for "$CURRENT_PERCENTAGE"% intersection rate."

  ./RandomRayTetra input$a -i $INTERSECTING -n $NONINTERSECTING
  
  ./Bench input$a output.csv $2 
  let "INTERSECTING+=$STEP"
  let "NONINTERSECTING+=-$STEP"
  let  "CURRENT_PERCENTAGE += $GRANULARITY"  
done

echo; 
exit 0


