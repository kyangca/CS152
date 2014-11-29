#include <iostream>
#include <string>
#include "classes.hpp"
#include "parser.hpp"

int num_students;
int num_schools;

Student *students;
School *schools;

/* A file containing parsing functions for school matching data.
 * Note that this should not have a main function so that it can be used
 * by other files.
 */

using namespace std;

void parse_data(string filename) {
    num_students = 420;
    num_schools = 42;
    students = new Student[num_students];
    schools = new School[num_schools];
}

/*
int main() {
  cout << "Hello World!" << endl;
}
*/