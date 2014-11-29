#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "classes.hpp"
#include "parser.hpp"

using namespace std;

float alpha = 0.51;

string outfile = "output.txt";

// Given a school and student, switches that student to that school if it
// is more preferable than their current school.
void School::propose(Student &student) {
    if (student.current_school == -1) {
        enrollment_count++;
        student.current_school = school_id;
    }
    // Smaller value means higher ranking
    else if (student.preferences[school_id] < student.preferences[student.current_school]) {
        enrollment_count++;
        schools[student.current_school].enrollment_count--;
        student.current_school = school_id;
    }
    next_admit++;
}

// Sends proposals to all students currently above the threshold
void School::send_proposals() {
    while (enrollment_count < capacity &&
           next_admit < num_students &&
           threshold <= scores[next_admit].score) {
        
           propose(students[scores[next_admit].student_id]);
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
            schools[i].send_proposals();
        }
    }
    return done;
}

// Sends the initial proposals, then carries out the algorithm
void non_private_da_school() {
    for (int i = 0; i < num_schools; i++) {
        schools[i].send_proposals();
    }
    
    bool done = false;
    while (not done) {
        done = do_iter();
    }
}

void verify_results() {
    int i, j, k;
    bool leftover_students = true;
    // First make sure that each school has either reached its enrollment
    // goal, has threshold 0, or there are no more students left.
    for (i = 0; i < num_students; i++) {
        if (students[i].current_school == -1) {
            leftover_students = false;
            break;
        }
    }
    
    for (i = 0; i < num_schools; i++) {
        if (schools[i].enrollment_count > schools[i].capacity) {
            cout << "Error: school " << i << " above capacity" << endl;
            exit(1);
        }
        if (schools[i].enrollment_count < (1 - alpha) * schools[i].capacity)
            if (schools[i].threshold > 0) {
                cout << "Error: school " << i << " underenrolled but stopped "
                     << "before threshold 0." << endl;
                exit(1);
            }
            if (leftover_students) {
                cout << "Error: students still remain despite a threshold of 0"
                     << endl;
                exit(1);
            }
    }
    
    // Verify that everybody is in the school they most preferred.
    for (i = 0; i < num_students; i++) {
        // Student was not assigned to a school
        if (students[i].current_school == -1) {
            continue;
        }
        // Look over every school
        for (j = 0; j < num_schools; j++) {
            // If the student preferred this school more...
            if (students[i].preferences[j] < students[i].preferences[students[i].current_school]) {
                // Verify that its threshold is above the score
                for (k = 0; k < num_students; k++) {
                    if (schools[j].scores[k].student_id == i) {
                        // There is technically an edge case where these are
                        // equal and there happened to be too many students
                        // with that exact score, which we ignore
                        if (schools[j].threshold < schools[j].scores[k].score) {
                            cout << "Error: student " << i << "preferred shool "
                                 << j << "but did not accept..." << endl;
                            exit(1);
                        }
                        break;
                    }
                }
            }
        }
    }
}

// We need to group up all the students for each school, We could do this
// most quickly with a hash table or a series of linked lists, but since
// it should not take too long we will take the easy way out and iterate over
// all students for each school.
void write_output() {
    int i, j, k, school_id, *student_ids;
    ofstream output;
    output.open(outfile.c_str(), ios::out);
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
        output << i << " (" << schools[i].threshold << "): " << student_ids[0];
        for (k = 1; k < schools[i].enrollment_count; k++) {
            output << ", " << student_ids[k];
        }
        output << endl;
        delete[] student_ids;
    }
    output.close();
}

int main() {
    string filename = "test.txt";
    parse_data(filename);
    non_private_da_school();
    write_output();
    verify_results();
    cout << "Done!" << endl;
}