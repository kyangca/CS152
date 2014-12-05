#include <iostream>
#include <string>
#include <stdlib.h>
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include "classes.hpp"
#include "parser.hpp"
#include "non_private.hpp"
#include "private.hpp"

using namespace std;

int **true_preferences = NULL;

/*
 * Calculates the school utility for a given school s.
 * School utility is a measure we define as the sum of the scores
 * school s gives to all of the, say, n students that are matched to 
 * it in the final matching, divided by the sum of the scores school
 * s assigns to its top n students, which would be the max possible
 * sum of scores.
 *
 * NOTE: You should only call this method AFTER a matching has been done.
 * NOTE: priv denotes if you are calculating school utility in private
 * or non-private case.  priv = false means non-private.
 */
double school_utility(int s, bool priv)
{
    int num = 0, denom = 0;
    // I am assuming that the Score *array is sorted in order
    // of highest scores to lowest.  If that is not the case, then
    // TODO: Sort scores
    int j = ((!priv) ? schools[s].enrollment_count : schools[s].private_count.get_count());
    for(int i = 0; i < j; i++)
    {
        denom += schools[s].scores[i].score;
    }
    if(denom <= 0) return 0;
    int *temp = (int *)calloc(num_students, sizeof(int));
    for(int i = 0; i < num_students; i++)
    {
        temp[schools[s].scores[i].student_id] = schools[s].scores[i].score;
    }
    for(int i = 0; i < num_students; i++)
    {
        if(students[i].current_school == s)
        {
            num += temp[i];
        }
    }
    free(temp);
    return (num * 1.0 / denom);
}

// The uniform utility function, with utility proportional to rank
float utility_uniform(Student &student) {
    if (student.current_school == -1) {
        return 0;
    }
    return float(num_schools - true_preferences[student.student_id][student.current_school])/(num_schools-1);
}

// Saves all student preferences into a backup array so we can easily access
// them while altering the preferences for the sake of the algorithm.
void copy_preferences() {
    if (true_preferences == NULL) {
        true_preferences = new int*[num_students];
        for (int i = 0; i < num_students; i++) {
            true_preferences[i] = new int[num_schools];
        }
    }
    for (int i = 0; i < num_students; i++) {
        memcpy(true_preferences[i], students[i].preferences, sizeof(int)*num_schools);
    }
}

void deallocate_true_preferences() {
    for (int i = 0; i < num_students; i++) {
        delete[] true_preferences[i];
    }
    delete[] true_preferences;
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
float Student::max_utility(void (*matching_algorithm)(), float (*utility)(Student&)) {
    float max_utility = 0.0;
    int *original_preferences = new int[num_schools];
    memcpy(original_preferences, preferences, sizeof(int)*num_schools);
    sort(preferences, preferences+num_schools);
    do {
        reset_state();
        matching_algorithm();
        max_utility = max(utility(*this), max_utility);
        
    } while (next_permutation(preferences, preferences+num_schools));
    memcpy(preferences, original_preferences, sizeof(int)*num_schools);
    delete[] original_preferences;
    return max_utility;
}

// Given the current set of preferences, finds the largest advantage that
// any student could get by lieing.
float max_utility_advantage(void (*matching_algorithm)(), float (*utility)(Student&)) {
    float max_advantage = 0.0;
    float *true_utilities = new float[num_students];
    for (int i = 0; i < num_students; i++) {
        true_utilities[i] = utility(students[i]);
    }
    
    for (int i = 0; i < num_students; i++) {
        max_advantage = max(max_advantage,
                            students[i].max_utility(matching_algorithm, utility) -
                            true_utilities[i]);
    }
    return max_advantage;
}

// This function takes a number of students and number of iterations. It
// randomly selects num_students students. Then it repeatedly generates
// new student preferences and determines the maximum advantage that any
// one can gain by lieing in any of the sampled preferences.
float max_random_sampling(void (*matching_algorithm)(), float (*utility)(Student&),
                          int samp_size, int num_iter) {
    samp_size = min(samp_size, num_students);
    // Get a random sample of students
    int *sample = new int[samp_size];
    int *permute = new int[num_students];
    for (int i = 0; i < num_students; i++) {
        permute[i] = i;
    }
    random_shuffle(permute, permute + num_students);
    memcpy(sample, permute, sizeof(int)*samp_size);
    delete[] permute;
    
    float max_advantage = 0.0;
    int *original_preferences = new int[num_schools];
    // For each iteration...
    for (int i = 0; i < num_iter; i++) {
        // Generate a new sample of student preferences
        gen_students();
        
        // For each student in the sample
        for (int j = 0; j < samp_size; j++) {
            // Save their preferences from this set and load their true ones
            memcpy(original_preferences, students[sample[j]].preferences, sizeof(int)*num_schools);
            memcpy(students[sample[j]].preferences, true_preferences[sample[j]], sizeof(int)*num_schools);
            // Get their truthful utility
            reset_state();
            matching_algorithm();
            float true_utility = utility(students[sample[j]]);
            // Now find the largest advantage for this student
            max_advantage = max(max_advantage, students[sample[j]].max_utility(matching_algorithm, utility) - true_utility);
            // Return the student's preferences back to normal
            memcpy(students[sample[j]].preferences, original_preferences, sizeof(int)*num_schools);
        }
    }
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
    for(int i = 0; i < num_schools; i++)
    {
        cout << "Non-private school utility for school " << i << " is: " << school_utility(i, false) << endl;
    }
    copy_preferences();
    //cout << "Max: " << max_utility_advantage(&non_private_da_school, &utility_uniform) << endl;
    cout << "Max: " << max_random_sampling(&non_private_da_school, &utility_uniform, 1000, 100) << endl;

    /* Test */
    // School next_admit enrollment_count private_count
    // Student 
    cout << "Test A: " << schools[1].private_capacity << endl;
    parse_data(input_file);
    cout << "Test B: " << schools[1].private_capacity << endl;
    for(int i = 0; i < num_schools; i++)
    {
        schools[i].next_admit = 0;
        schools[i].enrollment_count = 0;
        schools[i].private_count.reset_count();
    }
    for(int i = 0; i < num_students; i++)
    {
        students[i].current_school = -1;
    }
    float epsilon = 1;
    float delta = .5;
    float beta = 0.01;

    private_da_school(epsilon, delta, beta);
    for(int i = 0; i < num_schools; i++)
    {
        cout << "Private school utility for school " << i << " is: " << school_utility(i, true) << endl;
    }
    write_private_matching_output("testprivate.txt");
    /* End test */

    deallocate_true_preferences();
    free_memory();
}
