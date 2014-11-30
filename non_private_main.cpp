#include <iostream>
#include <string>
#include <stdlib.h>
#include "classes.hpp"
#include "parser.hpp"
#include "non_private.hpp"

using namespace std;

int main(int argc, char *argv[]) {
    char const *input_file, *output_file;
    if (argc < 2 || argc > 3) {
        cout << "Usage: non_private input_file [output_file]" << endl;
        exit(1);
    }
    input_file = argv[1];
    if (argc == 3) {
        output_file = argv[2];
    }
    else {
        output_file = "output.txt";
    }
    parse_data(input_file);
    non_private_da_school();
    write_matching_output(output_file);
    cout << "Done!" << endl;
}