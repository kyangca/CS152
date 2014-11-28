class Student;
class Score;
class School;

extern int num_students;
extern int num_schools;

extern Student *students;
extern School *schools;

class Student {
    int student_id;
    int current_school;
    int *preferences;
};

// We could represent student scores as an array where the ith index is the
// score for student i. However it should be faster (at the cost of some space)
// to have an array sorted by score with the student_id attached to each entry.
class Score {
    int student_id;
    int score;
};

class School {
    int school_id;
    int capacity;
    int enrollment_count;
    int threshold;
    // The index of the next student that might surpass the threshold
    int next_admit;
    Score *scores;
};