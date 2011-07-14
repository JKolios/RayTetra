 
 #!/bin/bash

# Runs intersection tests for the provided number of intersecting and non-intersecting Ray - Tetrahedron pairs
# Uses the same input data for all included algorithms and checks for possible differences in results
# Uses diff for filename comparison
# Repeats all tests  a number of times equal to (Iterations).
# Usage:"
# Method 1:Generate a new input file using RandomRayTetra
# /result_compare_iterated (Number of intersecting test pairs) (Number of non-intersecting test pairs) (Iterations)
# Method 2:Use an existing input file
# /result_compare_iterated (Input Filename) (Iterations)

if ([ $# != 2 ] &&[ $# != 4 ]) 
then
    echo "Runs intersection tests for the provided number of intersecting and non-intersecting Ray - Tetrahedron pairs"
    echo "Uses the same input data for all included algorithms and checks for possible differences in output"
    echo "Uses diff for output comparison"
    echo "Repeats all tests  a number of times equal to (Iterations)."
    echo "Usage:"
    echo "Method 1:Generate a new input file using RandomRayTetra"
    echo "/result_compare_iterated (Number of intersecting test pairs) (Number of non-intersecting test pairs) (Iterations)"
    echo "Method 2:Use an existing input file"
    echo "/result_compare_iterated (Input Filename) (Iterations)"
    exit 1
fi

rm -f $FILENAME.differences

if (($# == 2)) 
then
    FILENAME="$1"
    ITERATIONS=$2
    if [ ! -e $FILENAME ]
    then
	echo "File "$FILENAME" does not exist."
	exit 1
    fi
    echo "Using file "$FILENAME
else
  INTERSECTING=$1
  NONINTERSECTING=$2
  ITERATIONS=$3
  FILENAME=$4

  echo "Generating" $1"intersecting" $2"non-intersecting test pairs.Filename:"$FILENAME
  ./RandomRayTetra $FILENAME -i $INTERSECTING -n $NONINTERSECTING
fi

echo "Running tests" $ITERATIONS "times."

for ((a=1; a <= ITERATIONS ; a++)) 
do
  echo "Iteration" $a
  mkdir $a
  
  echo "Testing CPU"
  echo "Haines"
  ./RayTetra -a 0 -p r $FILENAME $a/$FILENAME.cpuhaines 1
  echo "Moller1"
  ./RayTetra -m 1 -p r $FILENAME $a/$FILENAME.cpumoller1 1
  echo "Moller2"
  ./RayTetra -m 2 -p r $FILENAME $a/$FILENAME.cpumoller2 1
  echo "Moller3"
  ./RayTetra -m 3 -p r $FILENAME $a/$FILENAME.cpumoller3 1
  echo "Segura0"
  ./RayTetra -s 0 -p r $FILENAME $a/$FILENAME.cpusegura0 1
  echo "Segura1"
  ./RayTetra -s 1 -p r $FILENAME $a/$FILENAME.cpusegura1 1
  echo "Segura2"
  ./RayTetra -s 2 -p r $FILENAME $a/$FILENAME.cpusegura2 1
  echo "STP0"
  ./RayTetra -t 0 -p r $FILENAME $a/$FILENAME.cpustp0 1
  echo "STP1"
  ./RayTetra -t 1 -p r $FILENAME $a/$FILENAME.cpustp1 1
  echo "STP2"
  ./RayTetra -t 2 -p r $FILENAME $a/$FILENAME.cpustp2 1

  echo "Testing GPU"
  echo "Segura0"
  ./RayTetra -g 0 -p r $FILENAME $a/$FILENAME.gpusegura0 1
  echo "STP0"
  ./RayTetra -g 1 -p r $FILENAME $a/$FILENAME.gpustp0 1
  echo "STP1"
  ./RayTetra -g 2 -p r $FILENAME $a/$FILENAME.gpustp1 1
  echo "STP2"
  ./RayTetra -g 3 -p r $FILENAME $a/$FILENAME.gpustp2 1

  echo "Printing comparison to "$a/$FILENAME.differences
  echo -e "CPU Haines - CPU Moller1">> $a/$FILENAME.differences
  diff -syw -W 300 --suppress-common-lines  $a/$FILENAME.cpuhaines $a/$FILENAME.cpumoller1 >> $a/$FILENAME.differences
  echo -e "CPU Moller1 - CPU Moller2">> $a/$FILENAME.differences
  diff -syw -W 300 --suppress-common-lines  $a/$FILENAME.cpumoller1 $a/$FILENAME.cpumoller2 >> $a/$FILENAME.differences
  echo -e "CPU Moller2 - CPU Moller3">> $a/$FILENAME.differences
  diff -syw -W 300 --suppress-common-lines  $a/$FILENAME.cpumoller2 $a/$FILENAME.cpumoller3 >> $a/$FILENAME.differences
  echo -e "CPU Moller1 - CPU Segura0">> $a/$FILENAME.differences
  diff -syw -W 300 --suppress-common-lines  $a/$FILENAME.cpumoller1 $a/$FILENAME.cpusegura0 >> $a/$FILENAME.differences
  echo -e "CPU Segura0 - CPU Segura1">> $a/$FILENAME.differences
  diff -syw -W 300 --suppress-common-lines  $a/$FILENAME.cpusegura0 $a/$FILENAME.cpusegura1 >> $a/$FILENAME.differences
  echo -e "CPU Segura1 - CPU Segura2">> $a/$FILENAME.differences
  diff -syw -W 300 --suppress-common-lines  $a/$FILENAME.cpusegura1 $a/$FILENAME.cpusegura2 >> $a/$FILENAME.differences
  echo -e "CPU Segura0 - CPU STP0">> $a/$FILENAME.differences
  diff -syw -W 300 --suppress-common-lines  $a/$FILENAME.cpusegura0 $a/$FILENAME.cpustp0 >> $a/$FILENAME.differences
  echo -e "CPU STP0 - CPU STP1">> $a/$FILENAME.differences
  diff -syw -W 300 --suppress-common-lines  $a/$FILENAME.cpustp0 $a/$FILENAME.cpustp1 >> $a/$FILENAME.differences
  echo -e "CPU STP1 - CPU STP2">> $a/$FILENAME.differences
  diff -syw -W 300 --suppress-common-lines  $a/$FILENAME.cpustp1 $a/$FILENAME.cpustp2 >> $a/$FILENAME.differences

  echo -e "CPU Segura0 - GPU Segura0">> $a/$FILENAME.differences
  diff -syw -W 300 --suppress-common-lines  $a/$FILENAME.cpusegura0 $a/$FILENAME.gpusegura0 >> $a/$FILENAME.differences
  echo -e "CPU STP0 - GPU STP0">> $a/$FILENAME.differences
  diff -syw -W 300 --suppress-common-lines  $a/$FILENAME.cpustp0 $a/$FILENAME.gpustp0 >> $a/$FILENAME.differences
  echo -e "CPU STP1 - GPU STP1">> $a/$FILENAME.differences
  diff -syw -W 300 --suppress-common-lines  $a/$FILENAME.cpustp1 $a/$FILENAME.gpustp1 >> $a/$FILENAME.differences
  echo -e "CPU STP2 - GPU STP2">> $a/$FILENAME.differences
  diff -syw -W 300 --suppress-common-lines  $a/$FILENAME.cpustp2 $a/$FILENAME.gpustp2 >> $a/$FILENAME.differences

done







