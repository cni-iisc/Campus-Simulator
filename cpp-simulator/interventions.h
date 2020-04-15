#ifndef INTERVENTIONS_H_
#define INTERVENTIONS_H_
#include "models.h"
#include <vector>

double kappa_T(const agent&node, double cur_time);

void get_kappa_no_intervention(std::vector<agent>& nodes, const std::vector<house>& homes, const std::vector<workplace>& workplaces, const std::vector<community>& communities, int cur_time);

void get_kappa_case_isolation(std::vector<agent>& nodes, const std::vector<house>& homes, const std::vector<workplace>& workplaces, const std::vector<community>& communities, int cur_time);

void get_kappa_SC(std::vector<agent>& nodes, const std::vector<house>& homes, const std::vector<workplace>& workplaces, const std::vector<community>& communities, int cur_time);

void get_kappa_home_quarantine(std::vector<agent>& nodes, std::vector<house>& homes, const std::vector<workplace>& workplaces, const std::vector<community>& communities, int cur_time);

void get_kappa_lockdown(std::vector<agent>& nodes, const  std::vector<house>& homes, const std::vector<workplace>& workplaces, const std::vector<community>& communities, int cur_time);

void get_kappa_CI_HQ(std::vector<agent>& nodes, std::vector<house>& homes, const std::vector<workplace>& workplaces, const std::vector<community>& communities, int cur_time);

void get_kappa_CI_HQ_65P(std::vector<agent>& nodes, std::vector<house>& homes, const std::vector<workplace>& workplaces, const std::vector<community>& communities, int cur_time);

void get_kappa_LOCKDOWN_21_CI_HQ_SD_65_PLUS_21_CI(std::vector<agent>& nodes, std::vector<house>& homes, const std::vector<workplace>& workplaces, const std::vector<community>& communities, int cur_time);

void get_kappa_LOCKDOWN_21(std::vector<agent>& nodes, std::vector<house>& homes, const std::vector<workplace>& workplaces, const std::vector<community>& communities, int cur_time);

void get_kappa_CI_HQ_65P_SC(std::vector<agent>& nodes, std::vector<house>& homes, const std::vector<workplace>& workplaces, const std::vector<community>& communities, int cur_time);

void get_kappa_CI_HQ_65P_SC_OE(std::vector<agent>& nodes, std::vector<house>& homes, const std::vector<workplace>& workplaces, const std::vector<community>& communities, int cur_time);

void get_kappa_LD_21_CI_HQ_SD65_SC_21_SC_42(std::vector<agent>& nodes, std::vector<house>& homes, const std::vector<workplace>& workplaces, const std::vector<community>& communities, int cur_time);

void get_kappa_LD_21_CI_HQ_SD65_SC_21(std::vector<agent>& nodes, std::vector<house>& homes, const std::vector<workplace>& workplaces, const std::vector<community>& communities, int cur_time);

void get_kappa_LD_21_CI_HQ_SD65_SC_OE_30(std::vector<agent>& nodes, std::vector<house>& homes, const std::vector<workplace>& workplaces, const std::vector<community>& communities, int cur_time);


#endif

