#include "classes.hpp"

using namespace std;

// Constructor
Student::Student() {
    current_school = -1;
    preferences = new int[num_schools];
}

// Desturctor
Student::~Student() {
    delete[] preferences;
}

Score::Score() {}

Score::Score(int student_id, int score) {
    this->student_id = student_id;
    this->score = score;
}


School::School() {
    next_admit = 0;
    enrollment_count = 0;
    scores = new Score[num_students];
}

School::~School() {
    delete[] scores;
}