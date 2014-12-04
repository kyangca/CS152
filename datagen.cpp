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
    char *outname = argv[3];
    gen_students();
    gen_schools(&capacity_uniform, &score_uniform);
    write_schools_and_students(outname);
    free_memory();
    return 0;
}
