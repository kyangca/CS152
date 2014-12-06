#include <iostream>
#include "parser.hpp"
#include "classes.hpp"

using namespace std;

int main(int argc, char **argv)
{
    if(argc != 4)
    {
        cout << "usage: datagen numschools numstudents outfile" << endl;
        return 1;
    }
    srand(time(0));
    num_schools = atoi(argv[1]);
    num_students = atoi(argv[2]);
    compute_capacity_bound();
    char *outname = argv[3];
    gen_students();
    gen_schools(&capacity_uniform_limited, &score_uniform);
    //contrived_example();
    write_schools_and_students(outname);
    free_memory();
    return 0;
}
