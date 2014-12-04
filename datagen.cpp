#include <iostream>
#include "parser.hpp"
#include "classes.hpp"

using namespace std;

int capacity_rand(void)
{
    //srand(time(0));
    return (rand() % num_students) + 1;
}

int score_rand(void)
{
    //srand(time(0));
    return (rand() % 100) + 1;
}

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
    gen_schools(&capacity_rand, &score_rand);
    write_schools_and_students(outname);
    return 0;
}
