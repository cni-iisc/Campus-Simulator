//#include <cassert>


#ifndef CAMPUS_INTERVENTIONS_H_
#define CAMPUS_INTERVENTIONS_H


#include "models.h"
#include "intervention_primitives.h"
#include "interventions.h"

void modify_kappa_case_isolate_node(agent& node, const std::vector<Interaction_Space>& i_spaces);

void modify_kappa_class_isolate_node(agent& node, const std::vector<Interaction_Space>& i_spaces, std::vector<agent>& nodes);

void set_kappa_base_value(agent& node, const std::vector<Interaction_Space>& i_spaces);

void set_kappa_lockdown_node(agent& node, const int cur_time, const intervention_params intv_params, const std::vector<Interaction_Space>& i_spaces);

bool should_be_isolated_node(const agent& node, const int cur_time, const int quarantine_days);

void get_kappa_custom_modular(std::vector<agent>& nodes, const std::vector<Interaction_Space>& i_spaces, const int cur_time, const intervention_params intv_params);

void get_kappa_file_read(std::vector<agent>& nodes, const std::vector<Interaction_Space>& i_spaces, const std::vector<intervention_params>& intv_params_vector, int cur_time);

void update_all_kappa(std::vector<agent>& nodes, const std::vector<Interaction_Space>& i_spaces, std::vector<intervention_params>& intv_params, int cur_time);

#endif


