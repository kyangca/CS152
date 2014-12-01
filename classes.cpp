#include "classes.hpp"

using namespace std;

float alpha = 0.0;

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

// We define this in reverse so that the sort function will sort it in
// descending order.
bool Score::operator<(const Score &other) const {
    return score > other.score;
}


School::School() {
    next_admit = 0;
    enrollment_count = 0;
    scores = new Score[num_students];
}

School::~School() {
    delete[] scores;
}