#ifndef __CLASSES_INCLUDED__   // if x.h hasn't been included yet...
#define __CLASSES_INCLUDED__ 


class Student;
class Score;
class School;
class Counter;

extern float alpha;

extern int num_students;
extern int num_schools;
extern int max_score;

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
    
    // Returns the maximum utility that a student can get while fixing all
    // other preferences. The first argument is the matching algorithm and
    // the second argument is a utility function. The utility function should
    // have range [0, 1] and be strictly decreasing with rank.
    float max_utility(void (*)(), float(*)(int));
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
    
    bool operator<(const Score &other) const;
};


class Counter {

    float epsilon;
    int length;
    int time;
    float *partial_sums;
    float *noisy_partial_sums;
public:
    Counter();
    Counter(float epsilon, int length);
    ~Counter();
    Counter & operator=(const Counter &c);
    void send(float val);
    float get_count();
    void reset_count();
    void print();
};

class School {
public:
    // Integer identifier with domain 0 through num_schools-1.
    int school_id;
    // Maximum enrollment count
    int capacity;
    float private_capacity;
    // Current enrollment count
    int enrollment_count;
    // Private enrollment count
    Counter private_count;
    int threshold;
    // The index of the next student that might surpass the threshold.
    // Defaults to 0.
    int next_admit;
    Score *scores;
    
    // Constructor/destructor
    School();
    ~School();
};

#endif
