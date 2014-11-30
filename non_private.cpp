#include <iostream>
#include <string>
#include <fstream>
#include "classes.hpp"

using namespace std;

extern float alpha;

// Given a school and student, switches that student to that school if it
// is more preferable than their current school.
void propose(School &school, Student &student) {
    if (student.current_school == -1) {
        school.enrollment_count++;
        student.current_school = school.school_id;
    }
    // Smaller value means higher ranking
    else if (student.preferences[school.school_id] < student.preferences[student.current_school]) {
        school.enrollment_count++;
        schools[student.current_school].enrollment_count--;
        student.current_school = school.school_id;
    }
    school.next_admit++;
}

// Sends proposals to all students currently above the threshold
void send_proposals(School &school) {
    while (school.enrollment_count < school.capacity &&
           school.next_admit < num_students &&
           school.threshold <= school.scores[school.next_admit].score) {
        propose(school, students[school.scores[school.next_admit].student_id]);
    }
}

// Does an iteration of the algorithm by reducing the threshhold of each
// school by 1, then sending proposals.
bool do_iter() {
    bool done = true;
    for (int i = 0; i < num_schools; i++) {
        if (schools[i].threshold > 0 &&
            schools[i].enrollment_count <= (1 - alpha) * schools[i].capacity) {
            
            done = false;
            schools[i].threshold--;
            send_proposals(schools[i]);
        }
    }
    return done;
}

// Sends the initial proposals, then carries out the algorithm
void non_private_da_school() {
    for (int i = 0; i < num_schools; i++) {
        send_proposals(schools[i]);
    }
    
    bool done = false;
    while (not done) {
        done = do_iter();
    }
}