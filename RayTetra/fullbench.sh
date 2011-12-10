 #!/bin/bash

# Benchmarks all available algorithms for a given number of test pairs. 
# A series of benchmarks is performed for each algorithm.Each benchmark has a higher percentage of intersecting 
# to non-intersecting pairs. This percentage starts at 0% and increases in every benchmark by a percentage equal to
# GRANULARITY.For example, a granularity of 20 would result in 6 tests with percentages :0% 20% 40% 60% 80% 100%.
# The optional argument [OpenCL Device Number] selects the OpenCL compatible device to use for GPU algorithms.
# Device numbers are positive integers >=0.
# Default value is 0 which signifies the first GPU identified by the OpenCl platform.
# Each benchmark is repeated as specified by the user.Output is given in CSV format in output.csv
# CSV Columns are comma separated

# Usage: ./fullbench.sh TOTAL_PAIRS REPETITIONS GRANULARITY [OpenCL Device Number]

if (($# < 3 ||$# > 4))
then
echo "Usage: ./fullbench.sh TOTAL_PAIRS REPETITIONS GRANULARITY [OpenCL Device Number]"
echo
echo "	Benchmarks all available algorithms for a given number of test pairs." 
echo "	A series of benchmarks is performed for each algorithm.Each benchmark has a higher percentage of intersecting" 
echo "	to non-intersecting pairs. This percentage starts at 0% and increases in every benchmark by a percentage equal "
echo "	to GRANULARITY.For example, a granularity of 20 would result in 6 tests with percentages :0% 20% 40% 60% 80% 100%."
echo "	The optional argument [OpenCL Device Number] selects the OpenCL compatible device to use for GPU algorithms."
echo "	Device numbers are positive integers >=0."
echo "	Default value is 0 which signifies the first GPU identified by the OpenCl platform."
echo "	Each benchmark is repeated as specified by the user.Output is given in CSV format in output.csv"
echo "	CSV Columns are comma separated"    
exit 1
fi

if (($# == 4))
then
DEVICE_NUMBER=$4
else
DEVICE_NUMBER=0
fi

INTERSECTING=0
NONINTERSECTING=$1
REPETITIONS=$2
GRANULARITY=$3
CURRENT_PERCENTAGE=0

let  "STEP = $NONINTERSECTING/(100/$GRANULARITY)"
let  "LIMIT = 100/$GRANULARITY"

rm -f  bench_output.csv
echo $NONINTERSECTING,$REPETITIONS,$GRANULARITY >> bench_output_$1_$2_$3_$HOSTNAME.csv
echo  \#Intersecting_Pairs,Haines,Moller1,Moller2,Moller3,Segura0,Segura1,Segura2,STP0,STP1,STP2,GPU_Segura0_Write,GPU_Segura0,GPU_Segura0_Read,GPU_STP0_Write,GPU_STP0,GPU_STP0_Read,GPU_STP1_Write,GPU_STP1,GPU_STP1_Read,GPU_STP2_Write,GPU_STP2,GPU_STP2_Read>> bench_output.csv

for ((a=0; a <= LIMIT ; a++)) 
do
  echo -n $CURRENT_PERCENTAGE, >> bench_output_$1_$2_$3_$HOSTNAME.csv 
  
  echo  
  echo "Benchmarking for "$CURRENT_PERCENTAGE"% intersection rate."

  ./RandomRayTetra input$CURRENT_PERCENTAGE -i $INTERSECTING -n $NONINTERSECTING
  
  ./Bench input$CURRENT_PERCENTAGE bench_output_$1_$2_$3_$HOSTNAME.csv $REPETITIONS $DEVICE_NUMBER
  let "INTERSECTING+=$STEP"
  let "NONINTERSECTING+=-$STEP"
  let  "CURRENT_PERCENTAGE += $GRANULARITY"  
done

echo "Creating graph."
echo;

echo "bench_output_$1_$2_$3_$HOSTNAME" |asy draw_graphs.asy
echo; 

exit 0


