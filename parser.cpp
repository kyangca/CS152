#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include "classes.hpp"
#include "parser.hpp"

using namespace std;

int num_students;
int num_schools;

Student *students;
School *schools;

void parse_data(char const *filename) {
    num_students = 4;
    num_schools = 2;
    students = new Student[num_students];
    schools = new School[num_schools];
    
    Student *stu;
    School *sch;
    
    stu = &students[0];
    stu->student_id = 0;
    stu->preferences[0] = 1;
    stu->preferences[1] = 2;
    
    stu = &students[1];
    stu->student_id = 1;
    stu->preferences[0] = 2;
    stu->preferences[1] = 1;
    
    stu = &students[2];
    stu->student_id = 2;
    stu->preferences[0] = 1;
    stu->preferences[1] = 2;
    
    stu = &students[3];
    stu->student_id = 2;
    stu->preferences[0] = 2;
    stu->preferences[1] = 1;
    
    sch = &schools[0];
    sch->school_id = 0;
    sch->capacity = 2;
    sch->threshold = 60;
    sch->scores[0].student_id = 1;
    sch->scores[0].score = 60;
    sch->scores[1].student_id = 3;
    sch->scores[1].score = 50;
    sch->scores[2].student_id = 2;
    sch->scores[2].score = 40;
    sch->scores[3].student_id = 0;
    sch->scores[3].score = 30;

    sch = &schools[1];
    sch->school_id = 1;
    sch->capacity = 2;
    sch->threshold = 45;
    sch->scores[0].student_id = 0;
    sch->scores[0].score = 45;
    sch->scores[1].student_id = 2;
    sch->scores[1].score = 35;
    sch->scores[2].student_id = 1;
    sch->scores[2].score = 25;
    sch->scores[3].student_id = 3;
    sch->scores[3].score = 15;
}


// We need to group up all the students for each school, We could do this
// most quickly with a hash table or a series of linked lists, but since
// it should not take too long we will take the easy way out and iterate over
// all students for each school.
void write_matching_output(char const *filename) {
    int i, j, k, school_id, *student_ids;
    ofstream outfile;
    outfile.open(filename, ios::out);
    for (i = 0; i < num_schools; i++) {
        if (schools[i].enrollment_count == 0) {
            continue;
        }
        k = 0;
        school_id = schools[i].school_id;
        student_ids = new int[schools[i].enrollment_count];
        for (j = 0; j < num_students; j++) {
            if (students[j].current_school == school_id) {
                student_ids[k] = j;
                k++;
            }
        }
        if (k != schools[i].enrollment_count) {
            cout << "Error: enrollment inconsistency." << endl;
            exit(1);
        }
        outfile << i << " (" << schools[i].threshold << "): " << student_ids[0];
        for (k = 1; k < schools[i].enrollment_count; k++) {
            outfile << ", " << student_ids[k];
        }
        outfile << endl;
        delete[] student_ids;
    }
    outfile.close();
}