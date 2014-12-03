#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include "classes.hpp"
#include "parser.hpp"

using namespace std;

int num_students = -1;
int num_schools = -1;
int max_score;

Student *students = NULL;
School *schools = NULL;

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

void gen_schools(int (*cdistr)(void), int (*sdistr)(void))
{
    allocate_schools();
    // Randomly generate num_schools number of schools
    for(int i = 0; i < num_schools; i++)
    {
        School current;
        current.school_id = i;
        // Generate a random capacity for between 1 and total number of students, drawn from
        // cdistr.
        current.capacity = cdistr();
        int maxscore = -1;
        for(int j = 0; j < num_students; j++)
        {
            // Assign each student a score drawn from sdistr.
            int s = sdistr();
            // Tentatively put this score at the end of the list
            current.scores[j] = Score(j, s);
            int k = j;
            // "Bubble" the score to its proper place in the list
            //  Yeah, yeah, it's inefficient, but it's also simple to write.
            // And right now, I'm looking to get a functional implementation up fast.
            while(k > 0 && current.scores[k].score > current.scores[k - 1].score)
            {
                Score temp = current.scores[k];
                current.scores[k] = current.scores[k - 1];
                current.scores[k - 1] = temp;
                k--;
            }
            if(s >= maxscore)
            {
                maxscore = s;
            }
        }
        // Set initial threshold to the max of all the scores this school has
        // given to students.
        current.threshold = maxscore;
        schools[i] = current;
    }
}

void gen_students(void)
{
    allocate_students();
    // Randomly generate num_students number of students
    for(int i = 0; i < num_students; i++)
    {
        Student current;
        current.student_id = i;
        // Create a sorted list (1, 2, ..., num_schools)
        int *pref = new int[num_schools];
        for(int j = 0; j < num_schools; j++)
        {
            pref[j] = j + 1;
        }
        // Permute it to get this student's rankings of the available schools.
        random_shuffle(pref, pref + num_schools);
        current.preferences = pref;
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
        students[i].preferences = new int[num_schools];
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
    for(int i = 0; i < num_students; i++)
    {
        Student current = students[i];
        outfile << current.student_id << ": ";
        for(int j = 0; j < num_schools - 1; j++)
        {
            cout << "TODO" << endl;
        }
    }
    for(int i = 0; i < num_schools; i++)
    {
        School current = schools[i];
        outfile << current.school_id << ": " << current.capacity << ", " << current.threshold << ", ";
        
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
