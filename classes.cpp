#include <math.h>
#include <iostream>
#include <cassert>
#include <stdlib.h>
#include "classes.hpp"

using namespace std;

float alpha = 0.0;

// Constructor
Student::Student() {
    current_school = -1;
    preferences = new int[num_schools];
}

// Copy Constructor
Student::Student(const Student& other) {
    current_school = other.current_school;
    student_id = other.student_id;
    preferences = new int[num_schools];
    for(int i = 0; i < num_schools; i++)
    {
        preferences[i] = other.preferences[i];
    }
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
    return b * log((float)rand() / (float)rand());
}

Counter::Counter(float epsilon, int length) {
    this->epsilon = epsilon;
    this->length = length;
    time = 0;
    int T = (int)ceil(log2(length));
    partial_sums = new float[T];
    noisy_partial_sums = new float[T];
    for (int j = 0; j < T; j++) {
        partial_sums[j] = 0;
	noisy_partial_sums[j] = 0;
    }
}

Counter::Counter() {
    epsilon = 0;
    length = 0;
    time = 0;
    partial_sums = NULL;
    noisy_partial_sums = NULL;
}

Counter::~Counter() {
    delete[] partial_sums;
    delete[] noisy_partial_sums;
}

Counter & Counter::operator=(const Counter &c) {
    if (this != &c) {
        delete[] partial_sums;
        delete[] noisy_partial_sums;
        epsilon = c.epsilon;
        length = c.length;
        time = c.time;
        int T = (int)ceil(log2(length));
        partial_sums = new float[T];
        noisy_partial_sums = new float[T];
        for (int j = 0; j < T; j++) {
            partial_sums[j] = c.partial_sums[j];
            noisy_partial_sums[j] = c.noisy_partial_sums[j];
        }
    }
    return *this;
}

void Counter::send(float val) {
    assert(time < length);
    assert(partial_sums != NULL);
    assert(noisy_partial_sums != NULL);
    time++;
//    cout << "sending " << val << " at time " << time << ": ";
//    cout << "before: " << get_count() << ", ";
//    cout << "time " << time << endl;
//    cout << "partial_sum " << partial_sums[0] << endl;
    int i = get_least_bit(time);
//    cout << "least bit: " << i << endl;
    for (int j = 0; j < i; j++) {
//        cout << "setting bit j: " << j << endl;
	partial_sums[i] += partial_sums[j];
	partial_sums[j] = 0;
	noisy_partial_sums[j] = 0;
    }
//    cout << "partial sum i: " << partial_sums[i] << endl;
//    cout << "adding val: " << val << endl;
    partial_sums[i] += val;
    noisy_partial_sums[i] = partial_sums[i] + lap(log2(length)/epsilon);
//    cout << "noisy sum " << noisy_partial_sums[i] << endl;
//    cout << "after: " << get_count() << endl;
}

float Counter::get_count() {
//    cout << "getting count" << endl;
//    cout << "  partial sum: " << partial_sums[0] << endl;
    float sum = 0;
    int num = time;
    int j = 0;
    while (num) {
        assert(j < ceil(log2(length)));
	if (num & 1) {
	    sum += noisy_partial_sums[j];
	}
	num >>= 1;
	j++;
    }
    return sum;
}

void Counter::reset_count() {
}


