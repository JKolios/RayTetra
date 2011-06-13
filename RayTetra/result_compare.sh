 #!/bin/bash

# Runs intersection tests for the provided number of intersecting and non-intersecting Ray - Tetrahedron pairs.
# Uses the same input data for all included algorithms and checks for possible differences in output.
# Uses diff for output comparison.
# Usage: ./result_compare (Number of intersecting test pairs) (Number of non-intersecting test pairs)

if (($# < 2 ||$# > 2))
then
    echo "Usage: ./result_compare (Number of intersecting test pairs) (Number of non-intersecting test pairs)"
    exit 1
fi

INTERSECTING=$1
NONINTERSECTING=$2

rm -f differences.txt

echo "Generating " $1 "intersecting " $2 "non-intersecting test pairs"
./RandomRayTetra test_input.txt -i $INTERSECTING -n $NONINTERSECTING

echo "Testing CPU"
echo "Haines"
./RayTetra -a 0 -p r test_input.txt outputcpuhaines.txt 1
echo "Moller1"
./RayTetra -m 1 -p r test_input.txt outputcpumoller1.txt 1
echo "Moller2"
./RayTetra -m 2 -p r test_input.txt outputcpumoller2.txt 1
echo "Moller3"
./RayTetra -m 3 -p r test_input.txt outputcpumoller3.txt 1
echo "Segura0"
./RayTetra -s 0 -p r test_input.txt outputcpusegura0.txt 1
echo "Segura1"
./RayTetra -s 1 -p r test_input.txt outputcpusegura1.txt 1
echo "Segura2"
./RayTetra -s 2 -p r test_input.txt outputcpusegura2.txt 1
echo "STP0"
./RayTetra -t 0 -p r test_input.txt outputcpustp0.txt 1
echo "STP1"
./RayTetra -t 1 -p r test_input.txt outputcpustp1.txt 1
echo "STP2"
./RayTetra -t 2 -p r test_input.txt outputcpustp2.txt 1

echo "Testing GPU"
echo "Segura0"
./RayTetra -g 0 -p r test_input.txt outputgpusegura0.txt 1
echo "STP0"
./RayTetra -g 1 -p r test_input.txt outputgpustp0.txt 1
echo "STP1"
./RayTetra -g 2 -p r test_input.txt outputgpustp1.txt 1
echo "STP2"
./RayTetra -g 3 -p r test_input.txt outputgpustp2.txt 1

echo "Printing comparison to differences.txt"
echo -e "CPU Haines - CPU Moller1\n">> differences.txt
diff -syw -W 300 --suppress-common-lines  outputcpuhaines.txt outputcpumoller1.txt >> differences.txt
echo -e "CPU Moller1 - CPU Moller2\n">> differences.txt
diff -syw -W 300 --suppress-common-lines  outputcpumoller1.txt outputcpumoller2.txt >> differences.txt
echo -e "CPU Moller2 - CPU Moller3\n">> differences.txt
diff -syw -W 300 --suppress-common-lines  outputcpumoller2.txt outputcpumoller3.txt >> differences.txt
echo -e "CPU Moller1 - CPU Segura0\n">> differences.txt
diff -syw -W 300 --suppress-common-lines  outputcpumoller1.txt outputcpusegura0.txt >> differences.txt
echo -e "CPU Segura0 - CPU Segura1\n">> differences.txt
diff -syw -W 300 --suppress-common-lines  outputcpusegura0.txt outputcpusegura1.txt >> differences.txt
echo -e "CPU Segura1 - CPU Segura2\n">> differences.txt
diff -syw -W 300 --suppress-common-lines  outputcpusegura1.txt outputcpusegura2.txt >> differences.txt
echo -e "CPU Segura0 - CPU STP0\n">> differences.txt
diff -syw -W 300 --suppress-common-lines  outputcpusegura0.txt outputcpustp0.txt >> differences.txt
echo -e "CPU STP0 - CPU STP1\n">> differences.txt
diff -syw -W 300 --suppress-common-lines  outputcpustp0.txt outputcpustp1.txt >> differences.txt
echo -e "CPU STP1 - CPU STP2\n">> differences.txt
diff -syw -W 300 --suppress-common-lines  outputcpustp1.txt outputcpustp2.txt >> differences.txt

echo -e "CPU Segura0 - GPU Segura0\n">> differences.txt
diff -syw -W 300 --suppress-common-lines  outputcpusegura0.txt outputgpusegura0.txt >> differences.txt
echo -e "CPU STP0 - GPU STP0\n">> differences.txt
diff -syw -W 300 --suppress-common-lines  outputcpustp0.txt outputgpustp0.txt >> differences.txt
echo -e "CPU STP1 - GPU STP1\n">> differences.txt
diff -syw -W 300 --suppress-common-lines  outputcpustp1.txt outputgpustp1.txt >> differences.txt
echo -e "CPU STP2 - GPU STP2\n">> differences.txt
diff -syw -W 300 --suppress-common-lines  outputcpustp2.txt outputgpustp2.txt >> differences.txt




