//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0
#ifndef INTERVENTIONS_H_
#define INTERVENTIONS_H_
#include "models.h"
#include <vector>

void set_kappa_base_node(agent& node, double community_factor, int cur_time);

void set_kappa_lockdown_node(agent& node, int cur_time);

void modify_kappa_SDE_node(agent& node);

void modify_kappa_SC_node(agent& node);

void modify_kappa_OE_node(agent& node);

void reset_home_quarantines(std::vector<house>& homes);

void modify_kappa_case_isolate_node(agent& node);

bool should_be_isolated_node(const agent& node, const int cur_time, const int quarantine_days);

void mark_and_isolate_quarantined_homes(std::vector<agent>& nodes, std::vector<house>& homes, int cur_time);

void get_kappa_no_intervention(std::vector<agent>& nodes, const std::vector<house>& homes, const std::vector<workplace>& workplaces, std::vector<community>& communities, int cur_time);

void get_kappa_case_isolation(std::vector<agent>& nodes, const std::vector<house>& homes, const std::vector<workplace>& workplaces, std::vector<community>& communities, int cur_time);

void get_kappa_SC(std::vector<agent>& nodes, const std::vector<house>& homes, const std::vector<workplace>& workplaces, std::vector<community>& communities, int cur_time);

void get_kappa_home_quarantine(std::vector<agent>& nodes, std::vector<house>& homes, const std::vector<workplace>& workplaces, std::vector<community>& communities, int cur_time);

void get_kappa_lockdown(std::vector<agent>& nodes, const  std::vector<house>& homes, const std::vector<workplace>& workplaces, std::vector<community>& communities, int cur_time);

void get_kappa_CI_HQ(std::vector<agent>& nodes, std::vector<house>& homes, const std::vector<workplace>& workplaces, std::vector<community>& communities, int cur_time);

void get_kappa_CI_HQ_65P(std::vector<agent>& nodes, std::vector<house>& homes, const std::vector<workplace>& workplaces, std::vector<community>& communities, int cur_time);

void get_kappa_Mumbai(std::vector<agent>& nodes, std::vector<house>& homes, const std::vector<workplace>& workplaces, const std::vector<community>& communities, int cur_time, double FIRST_PERIOD, double SECOND_PERIOD);

void get_kappa_LOCKDOWN_fper_CI_HQ_SD_65_PLUS_sper_CI(std::vector<agent>& nodes, std::vector<house>& homes, const std::vector<workplace>& workplaces, std::vector<community>& communities, int cur_time, double FIRST_PERIOD, double SECOND_PERIOD);

void get_kappa_LOCKDOWN_fper(std::vector<agent>& nodes, std::vector<house>& homes, const std::vector<workplace>& workplaces, std::vector<community>& communities, int cur_time, double FIRST_PERIOD);

void get_kappa_CI_HQ_65P_SC(std::vector<agent>& nodes, std::vector<house>& homes, const std::vector<workplace>& workplaces, std::vector<community>& communities, int cur_time);

void get_kappa_CI_HQ_65P_SC_OE(std::vector<agent>& nodes, std::vector<house>& homes, const std::vector<workplace>& workplaces, std::vector<community>& communities, int cur_time);

void get_kappa_LD_fper_CI_HQ_SD65_SC_sper_SC_tper(std::vector<agent>& nodes, std::vector<house>& homes, const std::vector<workplace>& workplaces, std::vector<community>& communities, int cur_time, double FIRST_PERIOD, double SECOND_PERIOD, double THIRD_PERIOD);

void get_kappa_LD_fper_CI_HQ_SD65_SC_sper(std::vector<agent>& nodes, std::vector<house>& homes, const std::vector<workplace>& workplaces, std::vector<community>& communities, int cur_time, double FIRST_PERIOD, double SECOND_PERIOD);

void get_kappa_LD_fper_CI_HQ_SD65_SC_OE_sper(std::vector<agent>& nodes, std::vector<house>& homes, const std::vector<workplace>& workplaces, std::vector<community>& communities, int cur_time, double FIRST_PERIOD, double OE_SECOND_PERIOD);

void get_kappa_intv_fper_intv_sper_intv_tper(std::vector<agent>& nodes, std::vector<house>& homes, const std::vector<workplace>& workplaces, std::vector<community>& communities, int cur_time, double FIRST_PERIOD, double SECOND_PERIOD, double THIRD_PERIOD);

void get_kappa_NYC(std::vector<agent>& nodes, std::vector<house>& homes, const std::vector<workplace>& workplaces, std::vector<community>& communities, int cur_time);

void get_kappa_containment(std::vector<agent>& nodes, std::vector<house>& homes, const std::vector<workplace>& workplaces, std::vector<community>& communities, const std::vector<std::vector<nbr_cell>>& nbr_cells, int cur_time, double FIRST_PERIOD, Intervention intv);

void get_kappa_file_read(std::vector<agent>& nodes, std::vector<house>& homes, const std::vector<workplace>& workplaces, std::vector<community>& communities, const std::vector<std::vector<nbr_cell>>& nbr_cells, const std::vector<intervention_params>& intv_params_vector, int cur_time);

void get_kappa_custom_modular(std::vector<agent>& nodes, std::vector<house>& homes, const std::vector<workplace>& workplaces, std::vector<community>& communities, const std::vector<std::vector<nbr_cell>>& nbr_cells, const int cur_time, const intervention_params intv_params);

void get_kappa_Mumbai_alternative_version(std::vector<agent>& nodes, std::vector<house>& homes, const std::vector<workplace>& workplaces, std::vector<community>& communities, const std::vector<std::vector<nbr_cell>>& nbr_cells, int cur_time, double FIRST_PERIOD, double SECOND_PERIOD);

void get_kappa_Mumbai_cyclic(std::vector<agent>& nodes, std::vector<house>& homes, const std::vector<workplace>& workplaces, std::vector<community>& communities, const std::vector<std::vector<nbr_cell>>& nbr_cells, int cur_time, double FIRST_PERIOD, double SECOND_PERIOD);

#endif
