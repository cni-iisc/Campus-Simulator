//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0
#ifndef TESTING_H_
#define TESTING_H_
#include "models.h"
#include <vector>


struct node_update_status_testing{
  bool new_hospitalization = false;
};

void set_test_request(std::vector<agent>& nodes, std::vector<house>& homes, std::vector<workplace>& workplaces, std::vector<std::vector<nbr_cell>>& nbr_cells, std::vector<community>& communities, testing_probability probabilities, count_type current_time);
void update_infection_testing(std::vector<agent>& nodes, std::vector<house>& homes, count_type current_time);

#endif
