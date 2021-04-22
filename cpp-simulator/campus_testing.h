//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0
#ifndef TESTING_H_
#define TESTING_H_
#include "models.h"
#include <vector>


struct node_update_status_testing{
  bool new_hospitalization = false;
};

void set_test_request(std::vector<agent>& nodes, std::vector<Interaction_Space>& ispaces, testing_probability probabilities, count_type current_time, count_type day);
void update_infection_testing(std::vector<agent>& nodes, std::vector<Interaction_Space>& ispaces, count_type current_time, count_type day);
void set_test_request_fileread(std::vector<agent>& nodes, std::vector<Interaction_Space>& ispaces,						 
						 std::vector<testing_probability>& testing_probability_vector, int cur_time, count_type day);
void test_contact_trace(count_type node_index, std::vector<agent>& nodes, std::vector<Interaction_Space>& ispaces, double probability_contact_trace, double probability_test_symptomatic, double probability_test_asymptomatic, const count_type current_time, count_type day );
#endif
