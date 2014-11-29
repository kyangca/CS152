#ifndef __CLASSES_INCLUDED__   // if x.h hasn't been included yet...
#define __CLASSES_INCLUDED__ 

class Student;
class Score;
class School;

extern int num_students;
extern int num_schools;

extern Student *students;
extern School *schools;

class Student { 
public:
    // Integer identifier with domain 0 through num_students-1;
    int student_id;
    // school_id of school student is currently going to attend.
    // Defaults to -1.
    int current_school;
    // An array with num_students entries. Entry i is the ranking that the
    // student gives to school i
    int *preferences;
    
    // Constructor/destructor
    Student();
    ~Student();
};

// We could represent student scores as an array where the ith index is the
// score for student i. However it should be faster (at the cost of some space)
// to have an array sorted by score with the student_id attached to each entry.
class Score {
public:
    int student_id;
    int score;
    // Constructors
    Score();
    Score(int student_id, int score);
};

class School {
public:
    // Integer identifier with domain 0 through num_schools-1.
    int school_id;
    // Maximum enrollment count
    int capacity;
    // Current enrollment count
    int enrollment_count;
    int threshold;
    // The index of the next student that might surpass the threshold.
    // Defaults to 0.
    int next_admit;
    Score *scores;
    
    // Constructor/destructor
    School();
    ~School();
    
    // When the threshold lowers below a student's score, the school proposes.
    // This may differ between the private and non-private algorithms.
    void propose(Student &student);
    void send_proposals();
};

#endif