#include <iostream>
#include <string>
#include <stdlib.h>
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include "classes.hpp"
#include "parser.hpp"
#include "non_private.hpp"

using namespace std;

// The uniform utility function, with utility proportional to rank
float utility_uniform(int rank) {
    return float(num_schools - rank)/(num_schools-1);
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

// Computes the maximum utility that can be acquired by the student while
// fixing all other preferences, given that the input algorithm is being
// used for matching
float Student::max_utility(void (*matching_algorithm)(), float (*utility)(int)) {
    float max_utility = 0.0;
    int *true_preferences = new int[num_schools];
    memcpy(true_preferences, preferences, sizeof(int)*num_schools);
    sort(preferences, preferences+num_schools);
    do {
        reset_state();
        matching_algorithm();
        max_utility = max(utility(true_preferences[current_school]), max_utility);
    } while (next_permutation(preferences, preferences+num_schools));
    memcpy(preferences, true_preferences, sizeof(int)*num_schools);
    delete[] true_preferences;
    return max_utility;
}

// Given the current set of preferences, finds the largest advantage that
// any student could get by lieing.
float max_utility_advantage(void (*matching_algorithm)(), float (*utility)(int)) {
    int i;
    float max_advantage = 0.0;
    float *true_utilities = new float[num_students];
    for (i = 0; i < num_students; i++) {
        true_utilities[i] = utility(students[i].preferences[students[i].current_school]);
    }
    for (i = 0; i < num_students; i++) {
        max_advantage = max(max_advantage,
                            students[i].max_utility(matching_algorithm, utility) -
                            true_utilities[i]);
    }
    delete[] true_utilities;
    return max_advantage;
}


int main(int argc, char *argv[], char *envp[]) {
    char const *input_file;
    if (argc != 2 ) {
        cout << "Usage: analyze input_file" << endl;
        exit(1);
    }
    input_file = argv[1];
    parse_data(input_file);
    non_private_da_school();
    write_matching_output("output.txt");
    cout << "Max: " << max_utility_advantage(&non_private_da_school, &utility_uniform) << endl;
}