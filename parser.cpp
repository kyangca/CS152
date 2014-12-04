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
int max_score;

Student *students = NULL;
School *schools = NULL;

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
            schools[i].scores[j] = Score(j, s);
            //cout << "For school " << schools[i].school_id << " and student " << j << ", generated " << s <<".\n";
            sort(schools[i].scores, schools[i].scores + num_students);
            maxscore = max(s, maxscore);
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
        schools[i].scores = new Score[num_students];
        for (j = 0; j < num_students; j++) {
            if (not getline(line_stream, cell, ',')) {
                cout << "Error: school " << i << " has too few scores." << endl;
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
