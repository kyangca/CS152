#include "synthesis.hpp"

void write_to_file(School *schools, Student *students, string outfile);

void gen_random(int nschools, int nstudents, string outfile)
{
    School *schools = (School *)calloc(nschools, sizeof(School));
    // Part 1: Randomly generate nschools number of schools
    for(int i = 0; i < nschools; i++)
    {
        School current;
        current.school_id = i;
        // Generate a random capacity for between 1 and total number of students.
        current.capacity = (rand() % nstudents) + 1;
        int maxscore = -1;
        for(int j = 0; j < nstudents; j++)
        {
            // Assign each student a score from 0 to RAND_MAX
            int s = rand();
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
    Student *students = (Student *)calloc(nstudents, sizeof(Student));
    // Part 2: Randomly generate nstudents number of students
    for(int i = 0; i < nstudents; i++)
    {
        Student current;
        current.student_id = i;
        // Create a sorted list (1, 2, ..., nschools)
        int *pref = (int *)calloc(sizeof(int), nschools);
        for(int j = 0; j < nschools; j++)
        {
            pref[j] = j + 1;
        }
        // Permute it to get this student's rankings of the available schools.
        next_permutation(pref, pref + nschools);
        current.preferences = pref;
    }
    // Part 3: Write everything to file
    write_to_file(schools, students, outfile);
}

void write_to_file(School *schools, Student *students, string outfile)
{
    //TODO
}
