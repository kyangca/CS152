#include <math.h>
#include <assert.h>
#include <stdlib.h>
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


int get_least_bit(int num) {
    assert(num);
    int i = 0;
    while (num) {
	if (num & 1) {
	    return i;
	}
	num >>= 1;
	i++;
    }
    return -1;
}

float lap(float b) {
    float x = (float)rand() / RAND_MAX * b;
    float y = (float)rand() / RAND_MAX * b;
    return log(x / y);
}

Counter::Counter() {
    count = 0;
    e = 0;
    T = 0;
    t = 0;
}

Counter::~Counter() {
    delete[] this->a;
    delete[] this->ahat;
}

void Counter::init(float epsilon, int Time) {
    this->count = 0;
    this->e = epsilon;
    this->T = Time;
    this->t = 0;
    this->a = new int[(int)ceil(log2(T))];
    this->ahat = new int[(int)ceil(log2(T))];
    for (int j = 0; j < ceil(log2(T)); j++) {
	a[j] = 0;
	assert(a[j] == 0);
	ahat[j] = 0;
	assert(ahat[j] == 0);
    }
}

void Counter::send(float val) {
    t++;
    int i = get_least_bit(t);
    for (int j = 0; j < i; j++) {
	a[i] += a[j];
	a[j] = 0;
	ahat[j] = 0;
    }
    a[i] += val;
    ahat[i] = a[i] + lap(log(T)/e);
    float sum = 0;
    int num = t;
    int j = 0;
    while (num) {
	if (num & 1) {
	    sum += ahat[j];
	}
	num >>= 1;
	i++;
    }
}

float Counter::get_count() {
    return count;
}


