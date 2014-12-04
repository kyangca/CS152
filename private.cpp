#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <cassert>
#include "private.hpp"
#include "classes.hpp"
#include "parser.hpp"


using namespace std;


int main(int argc, char *argv[]) {
//    test(); exit(1);
    char const *input_file, *output_file;
    if (argc < 2 || argc > 3) {
	cout << "Usage: private input_file [output_file]" << endl;
	exit(1);
    }
    input_file = argv[1];
    if (argc == 3) {
	output_file = argv[2];
    } else {
	output_file = "output.txt";
    }
    parse_data(input_file);

    srand(time(NULL));
    
    // TODO: take these as input?
    float epsilon = .0001;
    float delta = .5;
    float beta = 0.01;

    private_da_school(epsilon, delta, beta);
    write_private_matching_output(output_file);
    cout << "Done!" << endl;
}


void propose(School &school, Student &student) {
    
    // not currently matched: accept proposal
    if (student.current_school == -1) {
        
        cout << "      accept: was not enrolled" << endl;
        cout << "      sending 1 to school " << school.school_id << endl;
        school.private_count.send(1);
        // send 0 to all other counters
        cout << "      sending 0 to other schools" << endl;
        for (int j1 = 0; j1 < num_schools; j1++) {
            if (schools[j1].school_id != school.school_id) {
                schools[j1].private_count.send(0);
            }
        }
        student.current_school = school.school_id;
    }
    
    // already matched but prefer this school: accept proposal
    else if (student.preferences[school.school_id]
               < student.preferences[student.current_school]) {
        cout << "      accept: switching schools" << endl;
        school.private_count.send(1);
        schools[student.current_school].private_count.send(-1);
        // send 0 to all other counters
        for (int j1 = 0; j1 < num_schools; j1++) {
            if (schools[j1].school_id != school.school_id
                && schools[j1].school_id != student.current_school) {
                schools[j1].private_count.send(0);
            }
        }
        student.current_school = school.school_id;
    }
    
    // prefer current match: reject proposal
    else {
        cout << "      reject" << endl;
        // send 0 to all counters
        for (int j1 = 0; j1 < num_schools; j1++) {
            schools[j1].private_count.send(0);
        }
    }
}


void send_proposals(School &school) {
    school.threshold--;
    cout << "  under-enrolled! new threshold: " << school.threshold << endl;
    int students_processed = 0;

    // loop through newly eligible students
    while (school.next_admit < num_students &&
           school.threshold <= school.scores[school.next_admit].score) {
        
        cout << "    proposing to student "
             << school.scores[school.next_admit].student_id
             << ". score: " << school.scores[school.next_admit].score << endl;
                    
        propose(school, students[school.scores[school.next_admit].student_id]);
                    
        school.next_admit++;
        students_processed++;
    }
    
    cout << "      sending 0 to each school for remaining students" << endl;
    // send 0 to all counters for all students not just considered
    for (int i = students_processed; i < num_students; i++) {
        for (int j1 = 0; j1 < num_schools; j1++) {
            schools[j1].private_count.send(0);
        }
    }

}


void init(float epsilon, float delta, float beta) {
    cout << "initializing schools and students" << endl;
    // delta < 1, otherwise epsilon_prime becomes imaginary
    assert(delta <= 1);
    
    // set up constants
    const int T = num_schools * num_students * max_score;
    const float epsilon_prime = epsilon / (16 * sqrt(2 * num_schools * log(1 / delta)));
    float E = 128 * sqrt(num_schools * log(1 / delta)) / epsilon *
	log(2 * num_schools / beta) * pow(sqrt(log2(num_students * T)), 5);
    cout << "T = " << T << ", e' = " << epsilon_prime << ", E = " << E << endl;

    // initialize counters and capacities
    for (int j = 0; j < num_schools; j++) {
        // sanity checks
        assert(schools[j].threshold == max_score);
	assert(schools[j].capacity > 0);

        schools[j].private_count = Counter(epsilon_prime, num_students * T);
	schools[j].private_capacity = schools[j].capacity - E;

	// could get rid of this check once we're sure it's consistent
	// in the meantime, check scores less than max, and sorted
        int prev = schools[j].scores[0].score;
	for (int i = 0; i < num_students; i++) {
	    int score = schools[j].scores[i].score;
	    assert(score >= 0);
	    assert(score <= prev);
	    assert(score <= max_score);
	    prev = score;
	}
    }
    // assign no schools to begin with
    for (int i = 0; i < num_students; i++) {
	assert(students[i].current_school == -1);
    }
}


void private_da_school(float epsilon, float delta, float beta) {
    cout << "running private algorithm: epsilon = " << epsilon
         << ", delta = " << delta
         << ", beta = " << beta << endl;

    init(epsilon, delta, beta);

    bool some_under_enrolled = true;
    while (some_under_enrolled) {
        some_under_enrolled = false;
        
        for (int j = 0; j < num_schools; j++) { // for each school
            School &s = schools[j];
            
            cout << "processing school " << j << endl;
            cout << "  threshold: " << s.threshold << endl;
            cout << "  private count: " << s.private_count.get_count() << endl;
            cout << "  private capacity: " << s.private_capacity << endl;
            cout << "  capacity: " << s.capacity << endl;

            // if under-enrolled
            if ((s.private_count.get_count()
                 < s.private_capacity)
                && s.threshold > 0) {
                some_under_enrolled = true;
                send_proposals(s);
            }
        }
    }
}
