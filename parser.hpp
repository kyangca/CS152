#include <string>
#include <algorithm>

void reset_state(void);
void parse_data(char const *filename);
void write_matching_output(char const *filename);
void write_private_matching_output(char const *filename);

void gen_schools(int (*cdistr)(void), int (*sdistr)(void));
void gen_students(void);
void write_schools_and_students(char const *filename);
void free_memory(void);

int capacity_uniform(void);
int capacity_uniform_above_bound(void);
int score_uniform(void);