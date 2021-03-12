//#include <cassert>


#ifndef CAMPUS_INTERVENTIONS_H_
#define CAMPUS_INTERVENTIONS_H


#include "models.h"
#include "intervention_primitives.h"
#include "interventions.h"

void modify_kappa_case_isolate_node(agent& node, std::vector<Interaction_Space>& i_spaces, int day);

void modify_kappa_class_isolate_node(agent& node, std::vector<Interaction_Space>& i_spaces, std::vector<agent>& nodes, int day);

void set_kappa_base_value(agent& node, std::vector<Interaction_Space>& i_spaces, int day);

void set_kappa_lockdown_node(agent& node, const int cur_time, const intervention_params intv_params, std::vector<Interaction_Space>& i_spaces, int day);

bool should_be_isolated_node(const agent& node, const int cur_time, const int quarantine_days);

void get_kappa_custom_modular(std::vector<agent>& nodes, std::vector<Interaction_Space>& i_spaces, const int cur_time, const intervention_params intv_params, int day);

void get_kappa_file_read(std::vector<agent>& nodes, std::vector<Interaction_Space>& i_spaces, const std::vector<intervention_params>& intv_params_vector, int cur_time, int day);

void update_all_kappa(std::vector<agent>& nodes, std::vector<Interaction_Space>& i_spaces, std::vector<intervention_params>& intv_params, int cur_time, int day);

void reset_class_quarantines(std::vector<Interaction_Space>& i_space);

void mark_class_for_quarantine(const std::vector<agent>& nodes, std::vector<Interaction_Space> &i_space, const int cur_time);

#endif


