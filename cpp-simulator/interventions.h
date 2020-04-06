#ifndef INTERVENTIONS_H_
#define INTERVENTIONS_H_
#include "models.h"
#include <vector>
using std::vector;
double kappa_T(const agent&node, double cur_time);


void get_kappa_no_intervention(vector<agent>& nodes, const vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, int cur_time);

void get_kappa_case_isolation(vector<agent>& nodes, const vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, int cur_time);

void get_kappa_home_quarantine(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, int cur_time);

void get_kappa_lockdown(vector<agent>& nodes, const  vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, int cur_time);

void get_kappa_CI_HQ(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, int cur_time);

void get_kappa_CI_HQ_70P(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, int cur_time);

void get_kappa_LOCKDOWN_21_CI_HQ_SD_70_PLUS_21_CI(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, int cur_time);

void get_kappa_LOCKDOWN_21(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, int cur_time);


#endif

