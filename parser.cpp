#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <math.h>
#include "classes.hpp"
#include "parser.hpp"

using namespace std;

float alpha = 0.25;
float eta = 0.2;
int capacity_bound = -1;

int num_students = -1;
int num_schools = -1;
int max_score = 0.0;

Student *students = NULL;
School *schools = NULL;

// Computes the bound as stated in the informal theorem. This is apparently
// incorrect as the private algorithm has the log to the 5th power and a huge
// constant factor.
void compute_capacity_bound() {
    if (capacity_bound == -1) {
        capacity_bound = ceil(sqrt(num_schools) * log(num_students) / (eta * alpha));
        if (capacity_bound > num_students) {
            cout << "Warning: capacity bound too high: " << capacity_bound << endl;
            capacity_bound = num_students;
        }
    }
}

// CAPACITY DISTRIBUTIONS
 
int capacity_uniform(void) {
    return (rand() % num_students) + 1;
}

int capacity_uniform_limited(void) {
    int lower_bound = floor(num_students/(num_schools*2));
    return (rand() % (num_students/2-lower_bound)) + lower_bound;
}


int capacity_uniform_above_bound(void) {
    return (rand() % (num_students-capacity_bound+1)) + capacity_bound;
}


// SCORE DISTRIBUTIONS

int score_uniform(void) {
    return (rand() % 100) + 1;
}

void allocate_students() {
    if (num_students == -1) {
        cout << "Need to set the number of students before allocation." << endl;
        exit(1);
    }
    if (students == NULL) {
        students = new Student[num_students];
    }
}

void allocate_schools() {
    if (num_students == -1) {
        cout << "Need to set the number of schools before allocation." << endl;
        exit(1);
    }
    if (schools == NULL) {
        schools = new School[num_schools];
    }
}

void free_memory() {
    delete[] students;
    delete[] schools;
}

void gen_schools(int (*cdistr)(void), int (*sdistr)(void))
{
    allocate_schools();
    compute_capacity_bound();
    // Randomly generate num_schools number of schools
    for(int i = 0; i < num_schools; i++)
    {
        //School current;
        schools[i].school_id = i;
        // Generate a random capacity for between 1 and total number of students, drawn from
        // cdistr.
        schools[i].capacity = cdistr();
        int maxscore = -1;
        //cout << "Generating scores for students..." << endl;
        for(int j = 0; j < num_students; j++)
        {
            // Assign each student a score drawn from sdistr.
            int s = sdistr();
            // Tentatively put this score at the end of the list
            schools[i].scores[j].student_id = j;
            schools[i].scores[j].score = s;
            //cout << "For school " << schools[i].school_id << " and student " << j << ", generated " << s <<".\n";
            sort(schools[i].scores, schools[i].scores + num_students);
            maxscore = max(s, maxscore);
            max_score = max(s, max_score);
        }
        // Set initial threshold to the max of all the scores this school has
        // given to students.
        schools[i].threshold = maxscore;
        //schools[i] = current;
        //cout << "Capacity is: " << schools[i].capacity << endl;
    }
}

void gen_students(void)
{
    allocate_students();
    // Randomly generate num_students number of students
    for(int i = 0; i < num_students; i++)
    {
        //Student current;
        students[i].student_id = i;
        // Create a sorted list (1, 2, ..., num_schools)
        for(int j = 0; j < num_schools; j++)
        {
            students[i].preferences[j] = j + 1;
        }
        // Permute it to get this student's rankings of the available schools.
        random_shuffle(students[i].preferences, students[i].preferences + num_schools);
        //cout << "School preferences for: " << students[i].student_id << endl;
        for(int k = 0; k < num_schools; k++)
        {
            //cout << students[i].preferences[k] << ", ";
        }
        //cout << endl;
    }
}

// Between running matching algorithms, this should be called to reset the
// class fields to their initial states.
void reset_state() {
    int i;
    for (i = 0; i < num_students; i++) {
        students[i].current_school = -1;
    }
    for (i = 0; i < num_schools; i++) {
        schools[i].threshold = max_score;
        schools[i].next_admit = 0;
        schools[i].enrollment_count = 0;
//        schools[i].private_counter.reset_count();
    }
}
    

void parse_data(char const *filename) {
    int i, j, student_id, school_id, score;
    ifstream infile;
    string line, cell;
    stringstream line_stream;
    
    infile.open(filename, ios::in);
    // First two lines are the number of students and schools
    getline(infile, line);
    num_students = atoi(line.c_str());
    getline(infile, line);
    num_schools = atoi(line.c_str());
    
    getline(infile, line);
    if (line != "students:") {
        cout << "Error: expected but did not find line \"students\"." << endl;
        exit(1);
    }
    
    allocate_students();
    allocate_schools();
    
    for (i = 0; i < num_students; i++) {
        getline(infile, line);
        line_stream.str(line);
        line_stream.clear();
        getline(line_stream, cell, ':');
        student_id = atoi(cell.c_str());
        if (student_id != i) {
            cout << "Error: expected student id " << i << " but found " << student_id << endl;
            exit(1);
        }
        students[i].student_id = i;
        students[i].preferences = new int[num_schools]();
        
        for (j = 0; j < num_schools; j++) {
            if (not getline(line_stream, cell, ',')) {
                cout << "Error: student " << i << " has too few rankings." << endl;
                exit(1);
            }
            
            school_id = atoi(cell.c_str());
            
            if (students[i].preferences[school_id] != 0) {
                cout << "Error: student " << i << " has invalid school_id " << school_id << endl;
                exit(1);
            }
            
            students[i].preferences[school_id] = j + 1;
        }
        if (getline(line_stream, cell, '.')) {
            cout << "Error: student " << i << " has too many preferences." << endl;
            exit(1);
        }
    }
        
    getline(infile, line);
    if (line != "schools:") {
        cout << "Error: expected but did not find line \"schools\"." << endl;
    }
    
    max_score = 0;
    for (i = 0; i < num_schools; i++) {
        getline(infile, line);
        line_stream.str(line);
        line_stream.clear();
        getline(line_stream, cell, ':');
        school_id = atoi(cell.c_str());
        if (school_id != i) {
            cout << "Error: expected school id " << i << " but found "
                 << school_id << endl;
            exit(1);
        }
        schools[i].school_id = i;
        getline(line_stream, cell, ':');
        schools[i].capacity = atoi(cell.c_str());
        schools[i].private_capacity = schools[i].capacity;
        schools[i].scores = new Score[num_students];
        for (j = 0; j < num_students; j++) {
            if (not getline(line_stream, cell, ',')) {
                cout << "Error: school " << i << " has too few scores (" 
                     << j << ")." << endl;
                exit(1);
            }
            score = atoi(cell.c_str());
            max_score = max(score, max_score);
            schools[i].scores[j] = Score(j, score);
        }
        if (getline(line_stream, cell, '.')) {
            cout << "Error: school " << i << " has too many scores." << endl;
            exit(1);
        }
        sort(schools[i].scores, schools[i].scores+num_students);
    }
    
    for (i = 0; i < num_schools; i++) {
        schools[i].threshold = max_score;
    }
    
    infile.close();
}

// In this situation we are going to create a large dataset in which all but
// 2 students immediately get into their most preferred school. After the
// first iteration all schools will immediately be at capacity except for
// one school which will be undernerolled that everyone prefers least.
// Student 0 will be in the only
// school they do not prefer less than the underenrolled school. Student 1 will
// prefer the school student 0 is at. Student 0 can then lie, move into the
// vacant school, and both student 0 and 1 get moved to their most
// preferred.

// Some eligable (num_school, num_student) pairs are:
// (3, 450), (4, 600), (5, 1000), (10, 3500)
void contrived_example() {
    int offset;
    int bound = ceil((1 - alpha) * capacity_bound);
    // We require 2 schools to be at exactly at this bound, and the rest just
    // need to be above it. 
    if (bound * num_schools <= num_students || bound * (num_schools - 1) > num_students) {
        cout << "Capacity " << capacity_bound << " is not in range for example to work." << endl;
        free_memory();
        exit(1);
    }
    cout << "Running with capacities: " << bound << endl;
    
    // Allocate memory
    allocate_students();
    allocate_schools();
    
    // Set school capacities
    for (int i = 0; i < num_schools; i++) {
        schools[i].capacity = capacity_bound;
        schools[i].school_id = i;
    }
    
    for (int i = 0; i < num_students; i++) {
        students[i].student_id = i;
    }
    
    // There are a lot of preferences we will never use, so lets just fill
    // everything in with dummy values for now.
    for (int i = 0; i < num_students; i++) {
        for (int j = 0; j < num_schools; j++) {
            students[i].preferences[j] = j+1;
        }
    }
   
    // Now for each school we allocate bound number of students to that
    // school, for which the students and the schools both get optimal
    // matchings.
    
    for (int i = 0; i < num_schools - 1; i ++) {
        offset = i * bound;
        for (int j = 0; j < bound; j++) {
            students[offset+j].preferences[i] = 1;
            students[offset+j].preferences[0] = i+1;
            schools[i].scores[j].student_id = offset+j;
            schools[i].scores[j].score = 1;
        }
    }
    
    // Put leftovers in the underenrolled school_id
    offset = bound * (num_schools - 1);
    int students_left = num_students - offset;
    for (int i = 0; i < students_left; i++) {
        students[offset+i].preferences[num_schools-1] = 1;
        students[offset+i].preferences[0] = num_schools;
        schools[num_schools-1].scores[i].student_id = offset+i;
        schools[num_schools-1].scores[i].score = 1;
    }
    for (int i = 0; i < offset; i++) {
        schools[num_schools-1].scores[students_left+i].student_id = i;
        schools[num_schools-1].scores[students_left+i].score = 0;
    }

    // Now generate scores for all of the other students
    for (int i = 0; i < num_schools - 1; i++) {
        int lower_bound = i * bound;
        int upper_bound = (i + 1) * bound;
        // Iterate over all students below the bound
        for (int j = 0; j < lower_bound; j++) {
            schools[i].scores[bound+j].student_id = j;
            schools[i].scores[bound+j].score = 0;
        }
        for (int j = upper_bound; j < num_students; j++) {
            schools[i].scores[j].student_id = j;
            schools[i].scores[j].score = 0;
        }
    }
    
    // Now wreck the first student at two schools
    schools[0].scores[0].student_id = bound;
    schools[0].scores[bound].student_id = 0;
    schools[1].scores[0].student_id = 0;
    schools[1].scores[bound].student_id = bound;
    // Tweak the first student to prefer school 1 the second least
    students[0].preferences[1] = num_schools - 1;
    students[0].preferences[num_schools - 2] = 2;
}   
    
  
void write_schools_and_students(char const *filename)
{
    ofstream outfile;
    outfile.open(filename, ios::out);
    // Start by writing the number of students, then the number of 
    // schools to the start of the file
    outfile << num_students << endl;
    outfile << num_schools << endl;
    outfile << "students:" << endl;
    int *temp = new int[num_schools];
    for(int i = 0; i < num_students; i++)
    {
        Student current = students[i];
        for (int k = 0; k < num_schools; k++) {
            temp[students[i].preferences[k]-1] = k;
        }
        outfile << current.student_id << ":";
        outfile << temp[0];
        for (int k = 1; k < num_schools; k++) {
            outfile << "," << temp[k];
        }
        outfile << endl;
    }
    delete[] temp;
    outfile << "schools:" << endl;
    for(int i = 0; i < num_schools; i++)
    {
        //School current = schools[i];
        /* sort(schools[i].scores, schools[i].scores + num_students);
        cout << "After the sort." << endl;
        outfile << schools[i].school_id << ":" << schools[i].capacity << ":";
        for(int j = 0; j < num_students; j++)
        {
            if(j != (num_students - 1))
            {
                outfile << schools[i].scores[j].score << ",";
            }
            else
            {
                cout << "A\n";
                outfile << schools[i].scores[j].score << endl;
                cout << "B\n";
            }
        } */
        outfile << schools[i].school_id << ":" << schools[i].capacity << ":";
        for(int j = 0; j < num_students; j++)
        {
            for(int k = 0; k < num_students; k++)
            {
                if(schools[i].scores[k].student_id == j)
                {
                    if(j != num_students - 1)
                    {
                        outfile << schools[i].scores[k].score << ","; 
                        break;
                    }
                    else
                    {
                        outfile << schools[i].scores[k].score << endl;
                    }
                }
            }
        }
    }
    outfile.close();
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
        outfile << i << "(" << schools[i].threshold << "):" << student_ids[0];
        for (k = 1; k < schools[i].enrollment_count; k++) {
            outfile << "," << student_ids[k];
        }
        outfile << endl;
        delete[] student_ids;
    }
    outfile.close();
}

void write_private_matching_output(char const *filename) {

    ofstream outfile;
    outfile.open(filename, ios::out);
    for (int j = 0; j < num_schools; j++) {
        int student_count = 0;
        for (int i = 0; i < num_students; i++) {
            if (students[i].current_school == schools[j].school_id) {
                student_count++;
                if (student_count == 1) {
                    outfile << j << "(" << schools[j].threshold << "):";
                }
                if (student_count > 1) {
                    outfile << ",";
                }
                outfile << i;
            }
        }
        if (student_count > 0) {
            outfile << endl;
        }

    }
    outfile.close();
}
