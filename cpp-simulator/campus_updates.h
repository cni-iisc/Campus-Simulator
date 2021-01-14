//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0
#ifndef UPDATES_H_
#define UPDATES_H_

#include "models.h"
#include <vector>

std::vector<agent> init_nodes_campus();

std::vector<Interaction_Space> init_interaction_spaces();

std::vector<intervention_params> init_intervention_params();

void init_transmission_coefficients(std::vector<Interaction_Space> &interaction_spaces);

//void update_all_kappa(std::vector<agent>& nodes, const std::vector<Interaction_Space>& i_spaces, std::vector<intervention_params>& intv_params, int cur_time);

double update_interaction_spaces(agent& node, int cur_day);

double update_n_k(agent& node, int cur_day, Interaction_Space& i_space);

void update_interaction_space_lambda(std::vector<agent> &nodes, std::vector<Interaction_Space> &i_spaces, int day);

void update_individual_lambda(std::vector<agent>& nodes, std::vector<Interaction_Space>& i_spaces, int cur_day);

void assign_individual_campus(std::vector<agent>& nodes, std::vector<Interaction_Space>& interaction_space, int cur_day);

//void print_interaction_strength(agent& node, std::vector<Interaction_Space>& ispace, int day);

node_update_status update_infection(agent& node, int cur_time, int day);

//update_all_kappa(nodes, homes, workplaces, communities, nbr_cells, intv_params, time_step);

#endif
