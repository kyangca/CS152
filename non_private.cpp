#include <iostream>
#include <string>
#include "classes.hpp"
#include "parser.hpp"

float alpha = 0.0;
 
using namespace std;


void School::propose(Student &student) {
    if (student.current_school == -1) {
        enrollment_count++;
        next_admit++;
        student.current_school = school_id;
    }
    else if (student.preferences[school_id] > student.preferences[student.current_school]) {
        enrollment_count++;
        next_admit++;
        schools[student.current_school].enrollment_count--;
        student.current_school = school_id;
    }
}

void School::send_proposals() {
    while (enrollment_count < capacity &&
           next_admit < num_students &&
           threshold <= scores[next_admit].score) {
        
           propose(students[scores[next_admit].student_id]);
    }
}


bool do_iter() {
    bool done = true;
    for (int i = 0; i < num_schools; i++) {
        if (schools[i].threshold > 0 &&
            schools[i].enrollment_count <= (1 - alpha) * schools[i].capacity) {
            done = false;
            schools[i].threshold--;
            schools[i].send_proposals();
        }
    }
    return done;
}


void non_private_da_school() {
    for (int i = 0; i < num_schools; i++) {
        schools[i].send_proposals();
    }
    
    bool done = false;
    while (not done) {
        done = do_iter();
    }
}


int main() {
    string filename = "test.txt";
    parse_data(filename);
    cout << "Hello World!" << endl;
}