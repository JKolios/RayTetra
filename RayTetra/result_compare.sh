 #!/bin/bash

# Runs intersection tests for the provided number of intersecting and non-intersecting Ray - Tetrahedron pairs_
# Uses the same input data for all included algorithms and checks for possible differences in $3__
# Uses diff for $3_ comparison_
# Usage:"
# Method 1:Generate a new input file using RandomRayTetra_
# /result_compare (Number of intersecting test pairs) (Number of non-intersecting test pairs) (Input Filename)
# Method 2:Use an existing input file_
# /result_compare (Input Filename)

if ([ $# != 1 ] &&[ $# != 3 ]) 
then
    echo "Runs intersection tests for the provided number of intersecting and non-intersecting Ray - Tetrahedron pairs"
    echo "Uses the same input data for all included algorithms and checks for possible differences in output"
    echo "Uses diff for output comparison"
    echo "Usage:"
    echo "Method 1:Generate a new input file using RandomRayTetra"
    echo "/result_compare (Number of intersecting test pairs) (Number of non-intersecting test pairs) (input filename)"
    echo "Method 2:Use an existing input file"
    echo "/result_compare (Filename)"
    exit 1
fi

rm -f differences

if (($# == 1)) 
then
    FILENAME="$1"
    if [ ! -e $FILENAME ]
    then
	echo "File "$FILENAME" does not exist."
	exit 1
    fi
    echo "Using file "$FILENAME""
else
  INTERSECTING=$1
  NONINTERSECTING=$2
  FILENAME=$3
  echo "Generating " $1 "intersecting " $2 "non-intersecting test pairs.Filename:"$FILENAME
  ./RandomRayTetra $FILENAME -i $INTERSECTING -n $NONINTERSECTING
fi

echo "Testing CPU"
echo "Haines"
./RayTetra -a 0 -p r $FILENAME $FILENAME.cpuhaines.txt 1
echo "Moller1"
./RayTetra -m 1 -p r $FILENAME $FILENAME.cpumoller1.txt 1
echo "Moller2"
./RayTetra -m 2 -p r $FILENAME $FILENAME.cpumoller2.txt 1
echo "Moller3"
./RayTetra -m 3 -p r $FILENAME $FILENAME.cpumoller3.txt 1
echo "Segura0"
./RayTetra -s 0 -p r $FILENAME $FILENAME.cpusegura0.txt 1
echo "Segura1"
./RayTetra -s 1 -p r $FILENAME $FILENAME.cpusegura1.txt 1
echo "Segura2"
./RayTetra -s 2 -p r $FILENAME $FILENAME.cpusegura2.txt 1
echo "STP0"
./RayTetra -t 0 -p r $FILENAME $FILENAME.cpustp0.txt 1
echo "STP1"
./RayTetra -t 1 -p r $FILENAME $FILENAME.cpustp1.txt 1
echo "STP2"
./RayTetra -t 2 -p r $FILENAME $FILENAME.cpustp2.txt 1

echo "Testing GPU"
echo "Segura0"
./RayTetra -g 0 -p r $FILENAME $FILENAME.gpusegura0.txt 1
echo "STP0"
./RayTetra -g 1 -p r $FILENAME $FILENAME.gpustp0.txt 1
echo "STP1"
./RayTetra -g 2 -p r $FILENAME $FILENAME.gpustp1.txt 1
echo "STP2"
./RayTetra -g 3 -p r $FILENAME $FILENAME.gpustp2.txt 1

echo "Printing comparison to "$FILENAME.differences
echo -e "CPU Haines - CPU Moller1">> $FILENAME.differences
diff -syw -W 300 --suppress-common-lines  $FILENAME.cpuhaines.txt $FILENAME.cpumoller1.txt >> $FILENAME.differences
echo -e "CPU Moller1 - CPU Moller2">> $FILENAME.differences
diff -syw -W 300 --suppress-common-lines  $FILENAME.cpumoller1.txt $FILENAME.cpumoller2.txt >> $FILENAME.differences
echo -e "CPU Moller2 - CPU Moller3">> $FILENAME.differences
diff -syw -W 300 --suppress-common-lines  $FILENAME.cpumoller2.txt $FILENAME.cpumoller3.txt >> $FILENAME.differences
echo -e "CPU Moller1 - CPU Segura0">> $FILENAME.differences
diff -syw -W 300 --suppress-common-lines  $FILENAME.cpumoller1.txt $FILENAME.cpusegura0.txt >> $FILENAME.differences
echo -e "CPU Segura0 - CPU Segura1">> $FILENAME.differences
diff -syw -W 300 --suppress-common-lines  $FILENAME.cpusegura0.txt $FILENAME.cpusegura1.txt >> $FILENAME.differences
echo -e "CPU Segura1 - CPU Segura2">> $FILENAME.differences
diff -syw -W 300 --suppress-common-lines  $FILENAME.cpusegura1.txt $FILENAME.cpusegura2.txt >> $FILENAME.differences
echo -e "CPU Segura0 - CPU STP0">> $FILENAME.differences
diff -syw -W 300 --suppress-common-lines  $FILENAME.cpusegura0.txt $FILENAME.cpustp0.txt >> $FILENAME.differences
echo -e "CPU STP0 - CPU STP1">> $FILENAME.differences
diff -syw -W 300 --suppress-common-lines  $FILENAME.cpustp0.txt $FILENAME.cpustp1.txt >> $FILENAME.differences
echo -e "CPU STP1 - CPU STP2">> $FILENAME.differences
diff -syw -W 300 --suppress-common-lines  $FILENAME.cpustp1.txt $FILENAME.cpustp2.txt >> $FILENAME.differences

echo -e "CPU Segura0 - GPU Segura0">> $FILENAME.differences
diff -syw -W 300 --suppress-common-lines  $FILENAME.cpusegura0.txt $FILENAME.gpusegura0.txt >> $FILENAME.differences
echo -e "CPU STP0 - GPU STP0">> $FILENAME.differences
diff -syw -W 300 --suppress-common-lines  $FILENAME.cpustp0.txt $FILENAME.gpustp0.txt >> $FILENAME.differences
echo -e "CPU STP1 - GPU STP1">> $FILENAME.differences
diff -syw -W 300 --suppress-common-lines  $FILENAME.cpustp1.txt $FILENAME.gpustp1.txt >> $FILENAME.differences
echo -e "CPU STP2 - GPU STP2">> $FILENAME.differences
diff -syw -W 300 --suppress-common-lines  $FILENAME.cpustp2.txt $FILENAME.gpustp2.txt >> $FILENAME.differences




