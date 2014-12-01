#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <assert.h>
#include "private.hpp"
#include "classes.hpp"
#include "parser.hpp"


using namespace std;


int main(int argc, char *argv[]) {
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

void private_da_school(float e, float d) {
    // test values
    int J = 100; // scores from 0 to 100
    int m = num_schools; // 4 colleges
    int n = num_students; // 50 students
    float b = .01;

    
    int T = m * n * J;
    float eprime = e / (16 * sqrt(2 * m * log(1 / d)));
    float E = 128 * sqrt(m * log(1 / d)) / e *
	log(2 * m / b) * pow(sqrt(log(n * T)), 5);


    for (int j = 0; j < m; j++) {
	cout << "initializing school " << j << endl;
	schools[j].private_counter.init(eprime, n * T);
	// TODO: why is this threshold already set?
	schools[j].threshold = J;

	assert(schools[j].capacity > 0);
	schools[j].effective_capacity = schools[j].capacity - E;
	// assert(schools[j].effective_capacity > 0);

	// TODO: get rid of this check once we're sure it's consistent
	// in the meantime, check scores less than max, and sorted
        int prev = schools[j].scores[0].score;
	for (int i = 0; i < n; i++) {
	    int score = schools[j].scores[i].score;
	    assert(score >= 0);
	    assert(score <= prev);
	    assert(score <= J);
	    prev = score;
	}
    }
    // assign no schools to begin with
    for (int i = 0; i < n; i++) {
	cout << "initializing student " << i << endl;
	assert(students[i].current_school == -1);
    }

    bool some_under_enrolled = true;
    while (some_under_enrolled) {
	some_under_enrolled = false;
	for (int j = 0; j < m; j++) { // for each school
	    cout << "processing school " << j << endl;
	    School school = schools[j];
	    if ((school.private_counter.get_count()
		 < school.effective_capacity)
		&& school.threshold > 0) { // if under_enrolled
		cout << school.school_id << " under-enrolled" << endl;
		some_under_enrolled = true;
		school.threshold--;
		// TODO: this assertion should be fine if assignment
		// works as I expect it to
		assert(school.threshold == schools[j].threshold);
		int students_processed = 0;
		while (school.next_admit < num_students &&
		       school.threshold <=
		       school.scores[school.next_admit].score) {
		    Student student =
			students[school.scores[school.next_admit].student_id];
		    if (student.current_school == -1) {
			school.private_counter.send(1);
			// send 0 to all other counters
			for (int j1 = 0; j1 < m; j1++) {
			    if (schools[j1].school_id != school.school_id) {
				schools[j1].private_counter.send(0);
			    }
			}
		    } else if (student.preferences[school.school_id]
			       < student.preferences[student.current_school]) {
			school.private_counter.send(1);
			schools[student.current_school].private_counter.send(-1);
			// send 0 to all other counters
			for (int j1 = 0; j1 < m; j1++) {
			    if (schools[j1].school_id != school.school_id
				&& schools[j1].school_id != student.current_school) {
				schools[j1].private_counter.send(0);
			    }
			}
			student.current_school = school.school_id;
		    } else {
			// send 0 to all counters
			for (int j1 = 0; j1 < m; j1++) {
			    schools[j1].private_counter.send(0);
			}
		    }
		    school.next_admit++;
		    students_processed++;
		}
		// TODO: send 0 to all counters for all students not just considered
		for (int j1 = 0; j1 < m; j1++) {
		    schools[j1].private_counter.send(0);
		}
	    }
	}
    }
}

// TODO
float counter(float e, int T) {
    return 1;
}
