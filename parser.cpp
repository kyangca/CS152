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
    /*
    num_students = 420;
    num_schools = 42;
    students = new Student[num_students];
    schools = new School[num_schools];
    */
    num_students = 4;
    num_schools = 2;
    students = new Student[num_students];
    schools = new School[num_schools];
    
    Student *stu;
    School *sch;
    
    stu = &students[0];
    stu->student_id = 0;
    stu->preferences[0] = 0;
    stu->preferences[1] = 1;
    
    stu = &students[1];
    stu->student_id = 1;
    stu->preferences[0] = 1;
    stu->preferences[1] = 0;
    
    stu = &students[2];
    stu->student_id = 2;
    stu->preferences[0] = 0;
    stu->preferences[1] = 1;
    
    stu = &students[3];
    stu->student_id = 2;
    stu->preferences[0] = 1;
    stu->preferences[1] = 0;
    
    sch = &schools[0];
    sch->school_id = 0;
    sch->capacity = 2;
    sch->threshold = 50;
    sch->scores[0].student_id = 0;
    sch->scores[0].score = 70;
    sch->scores[1].student_id = 1;
    sch->scores[1].score = 46;
    sch->scores[2].student_id = 2;
    sch->scores[2].score = 45;
    sch->scores[3].student_id = 3;
    sch->scores[3].score = 30;

    sch = &schools[1];
    sch->school_id = 1;
    sch->capacity = 2;
    sch->threshold = 45;
    sch->scores[0].student_id = 2;
    sch->scores[0].score = 44;
    sch->scores[1].student_id = 3;
    sch->scores[1].score = 44;
    sch->scores[2].student_id = 0;
    sch->scores[2].score = 20;
    sch->scores[3].student_id = 1;
    sch->scores[3].score = 10;
}

/*
int main() {
  cout << "Hello World!" << endl;
}
*/