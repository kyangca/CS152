#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <cassert>
#include "private.hpp"
#include "classes.hpp"
#include "parser.hpp"


using namespace std;

float lap2(float b) {
    cout << "running lap" << endl;
    float x = (float)rand() * b / RAND_MAX;
    float y = (float)rand() * b / RAND_MAX;
    cout << x << "," << y << endl;
    return log(x / y);
}

void memory_test() {
    srand(time(NULL));
    cout << lap2(0.000001) << endl;
    Counter c(100, 5);
    for (int i = 0; i < 5; i++) {
        cout << c.get_count() << endl;
        c.send(i);
    }
    cout << c.get_count() << endl;
}

int main(int argc, char *argv[]) {
    memory_test();
    
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
    float epsilon = .01;
    float delta = .01;
    private_da_school(epsilon, delta);
    write_matching_output(output_file);
    cout << "Done!" << endl;
}

void private_da_school(float epsilon, float delta) {
    cout << "running private algorithm: epsilon = "
         << epsilon << ", delta = " << delta << endl;
    // test values
    float beta = .01;
    
    
    int T = num_schools * num_students * max_score;
    float epsilon_prime = epsilon / (16 * sqrt(2 * num_schools * log(1 / delta)));
    float E = 128 * sqrt(num_schools * log(1 / delta)) / epsilon *
	log(2 * num_schools / beta) * pow(sqrt(log(num_students * T)), 5);
    E = 0;    // TODO: this shouldn't be here
    // initialize counters and capacities
    for (int j = 0; j < num_schools; j++) {
        // sanity checks
        assert(schools[j].threshold == max_score);
	assert(schools[j].capacity > 0);
//        cout << "before assignment: "
//             << schools[j].private_count.partial_sums[0] << endl;
//        cout << "before: " << c.partial_sums[0] << endl;
//	schools[j].private_count = c; //Counter(epsilon_prime, num_students * T);
        schools[j].private_count = Counter(epsilon_prime, num_students * T);
//        schools[j].private_count = c;
        schools[j].private_count.print();
//        cout << "after assignment: "
//             << schools[j].private_count.partial_sums[0] << endl;
//        cout << "school " << j << " private count "
//             << schools[j].private_count.get_count() << endl;
	schools[j].private_capacity = schools[j].capacity - E;

	// TODO: get rid of this check once we're sure it's consistent
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
    
    bool some_under_enrolled = true;
    while (some_under_enrolled) {
	some_under_enrolled = false;
	for (int j = 0; j < num_schools; j++) { // for each school
            cout << "processing school " << j << endl;
	    School &s = schools[j];
            cout << "  threshold: " << s.threshold << endl;
            cout << "  private count: " << s.private_count.get_count() << endl;
//            cout << "  partial sum: " << s.private_count.partial_sums[0] << endl;
            cout << "  private capacity: " << s.private_capacity << endl;
            cout << "  capacity: " << s.capacity << endl;
	    if ((s.private_count.get_count()
		 < s.private_capacity)
		&& s.threshold > 0) { // if under-enrolled
		some_under_enrolled = true;
		s.threshold--;
                cout << "  under-enrolled! new threshold: " << s.threshold << endl;
		int students_processed = 0;
		while (s.next_admit < num_students &&
		       s.threshold <=
		       s.scores[s.next_admit].score) {
                    cout << "    proposing to student "
                         << s.scores[s.next_admit].student_id
                         << ". score: " << s.scores[s.next_admit].score << endl;
		    Student &student =
			students[s.scores[s.next_admit].student_id];
		    if (student.current_school == -1) {
                        cout << "      accept: was not enrolled" << endl;
                        cout << "      sending 1 to school " << s.school_id << endl;
			s.private_count.send(1);
			// send 0 to all other counters
                        cout << "      sending 0 to other schools" << endl;
			for (int j1 = 0; j1 < num_schools; j1++) {
			    if (schools[j1].school_id != s.school_id) {
				schools[j1].private_count.send(0);
			    }
			}
                        student.current_school = s.school_id;
		    } else if (student.preferences[s.school_id]
			       < student.preferences[student.current_school]) {
                        cout << "      accept: switching schools" << endl;
			s.private_count.send(1);
			schools[student.current_school].private_count.send(-1);
			// send 0 to all other counters
			for (int j1 = 0; j1 < num_schools; j1++) {
			    if (schools[j1].school_id != s.school_id
				&& schools[j1].school_id != student.current_school) {
				schools[j1].private_count.send(0);
			    }
			}
			student.current_school = s.school_id;
		    } else {
                        cout << "      reject" << endl;
			// send 0 to all counters
                        for (int j1 = 0; j1 < num_schools; j1++) {
			    schools[j1].private_count.send(0);
			}
		    }
		    s.next_admit++;
		    students_processed++;
		}
                cout << "      sending 0 to each school for remaining students" << endl;
		// TODO: send 0 to all counters for all students not just considered
                for (int i = students_processed; i < num_students; i++) {
                    for (int j1 = 0; j1 < num_schools; j1++) {
                        schools[j1].private_count.send(0);
                    }
		}
	    }
	}
    }
}
