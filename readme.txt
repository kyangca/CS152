make produces 4 executables which can be run in this way:

./datagen num_schools num_students output_file
./non_private input_file [output_file]
./private
./analysis input_file

datagen generates data, although currently the distributions need to be changed
in the code.

non_private runs the non-private algorithm. If an output file is not specified
it writes the resulting matching and thresholds into "output.txt".

private runs the private algorithm.

analysis runs various tests on the preferences specified by the input file. It
makes little sense without altering the code.

Data format for input files:
num_students
num_schools
students:
0:1,2,3,... (comma separated list of school ids in order of preference)
1:3,2,1,...
...
schools:
0:threshold:58,32,69,... (comma separated list of scores. The ith score is the score for student i)
1:threshold:53,73,10,...
...

Data format of matching output:
0(threshold):1,3,7,... (list of students matched to school)
1(threshold):2,4,5,...