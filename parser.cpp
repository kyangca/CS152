#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include "classes.hpp"
#include "parser.hpp"

using namespace std;

int num_students;
int num_schools;

Student *students;
School *schools;

void gen_schools(int (*cdistr)(void), int (*sdistr)(void))
{
    schools = new School[num_schools];
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
    students = new Student[num_students];
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

void parse_data(char const *filename) {
    num_students = 4;
    num_schools = 2;
    students = new Student[num_students];
    schools = new School[num_schools];
    
    Student *stu;
    School *sch;
    
    stu = &students[0];
    stu->student_id = 0;
    stu->preferences[0] = 1;
    stu->preferences[1] = 2;
    
    stu = &students[1];
    stu->student_id = 1;
    stu->preferences[0] = 2;
    stu->preferences[1] = 1;
    
    stu = &students[2];
    stu->student_id = 2;
    stu->preferences[0] = 1;
    stu->preferences[1] = 2;
    
    stu = &students[3];
    stu->student_id = 2;
    stu->preferences[0] = 2;
    stu->preferences[1] = 1;
    
    sch = &schools[0];
    sch->school_id = 0;
    sch->capacity = 2;
    sch->threshold = 60;
    sch->scores[0].student_id = 1;
    sch->scores[0].score = 60;
    sch->scores[1].student_id = 3;
    sch->scores[1].score = 50;
    sch->scores[2].student_id = 2;
    sch->scores[2].score = 40;
    sch->scores[3].student_id = 0;
    sch->scores[3].score = 30;

    sch = &schools[1];
    sch->school_id = 1;
    sch->capacity = 2;
    sch->threshold = 45;
    sch->scores[0].student_id = 0;
    sch->scores[0].score = 45;
    sch->scores[1].student_id = 2;
    sch->scores[1].score = 35;
    sch->scores[2].student_id = 1;
    sch->scores[2].score = 25;
    sch->scores[3].student_id = 3;
    sch->scores[3].score = 15;
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
        outfile << i << " (" << schools[i].threshold << "): " << student_ids[0];
        for (k = 1; k < schools[i].enrollment_count; k++) {
            outfile << ", " << student_ids[k];
        }
        outfile << endl;
        delete[] student_ids;
    }
    outfile.close();
}
