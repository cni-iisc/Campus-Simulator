#ifndef UPDATES_H_
#define UPDATES_H_

#include "models.h"
#include <vector>

double update_individual_lambda_h(const agent& node);

double update_individual_lambda_w(const agent& node);

double update_individual_lambda_c(const agent& node);

void update_lambda_stats(const agent& node);

void update_infection(agent& node, int cur_time);

void update_all_kappa(std::vector<agent>& nodes, std::vector<house>& homes, std::vector<workplace>& workplaces, std::vector<community>& communities, int cur_time);

double updated_lambda_w_age_independent(const std::vector<agent>& nodes, const workplace& workplace);

double updated_lambda_h_age_independent(const std::vector<agent>& nodes, const house& home);

void update_lambdas(agent&node, const std::vector<house>& homes, const std::vector<workplace>& workplaces, const std::vector<community>& communities, int cur_time);

double updated_lambda_c_local(const std::vector<agent>& nodes, community& community);

void update_lambda_c_global(std::vector<community>& communities, const matrix<double>& community_distance_matrix);

struct casualty_stats{
  double infected = 0;
  double affected = 0;
  double hospitalised = 0;
  double critical = 0;
  double dead = 0;
  double exposed = 0;
  double recovered = 0;
};

casualty_stats get_infected_community(const std::vector<agent>& nodes, const community& community);

#endif
