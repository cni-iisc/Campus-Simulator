#include <vector>
#include <algorithm>
using namespace std;
#include "models.h"
#include "initializers.h"
#include "interventions.h"


double update_individual_lambda_h(const agent& node){
  return node.infective * node.kappa_T * node.infectiousness * (1 + node.severity)*node.kappa_H;
}

double update_individual_lambda_w(const agent& node){
  return node.infective * node.kappa_T * node.infectiousness * (1 + node.severity*(2*node.psi_T-1))*node.kappa_W;
}

double update_individual_lambda_c(const agent& node){
  return node.infective * node.kappa_T * node.infectiousness * node.funct_d_ck * (1 + node.severity)*node.kappa_C;
	// optimised version: return node.lambda_h * node.funct_d_ck;
}


void update_lambda_stats(const agent& node){}

void update_infection(agent& node, int cur_time){
  int age_index = node.age_index;

  //console.log(1-Math.exp(-node['lambda']/SIM_STEPS_PER_DAY))
  ///TODO: Parametrise transition times
  if (node.infection_status==Progression::susceptible
	  && bernoulli(1-exp(-node.lambda/GLOBAL.SIM_STEPS_PER_DAY))){
	node.infection_status = Progression::exposed; //move to exposed state
	node.time_of_infection = cur_time;
	node.infective = false;
	update_lambda_stats(node);
  }
  else if(node.infection_status==Progression::exposed
		  && (cur_time - node.time_of_infection
			  > node.incubation_period)){
	node.infection_status = Progression::infective; //move to infective state
	node.infective = true;
  }
  else if(node.infection_status==Progression::infective
		  && (cur_time - node.time_of_infection
			  > (node.incubation_period
				 + node.asymptomatic_period))){
	if(bernoulli(GLOBAL.SYMPTOMATIC_FRACTION)){
	  node.infection_status = Progression::symptomatic; //move to symptomatic
	  node.infective = true;
	}
	else {
	  node.infection_status = Progression::recovered; //move to recovered
	  node.infective = false;
	}
  }
  else if(node.infection_status==Progression::symptomatic
		  && (cur_time - node.time_of_infection
			  > (node.incubation_period
				 + node.asymptomatic_period
				 + node.symptomatic_period))){
	if(bernoulli(STATE_TRAN[age_index][0])){
	  node.infection_status = Progression::hospitalised; //move to hospitalisation
	  node.infective = 0;
	}
	else {
	  node.infection_status = Progression::recovered; //move to recovered
	  node.infective = false;
	}
  }
  else if(node.infection_status==Progression::hospitalised
		  && (cur_time - node.time_of_infection
			  > (node.incubation_period+node.asymptomatic_period
				 + node.symptomatic_period
				 + node.hospital_regular_period))){
	if(bernoulli(STATE_TRAN[age_index][1])){
	  node.infection_status = Progression::critical; //move to critical care
	  node.infective = false;
	}
	else {
	  node.infection_status = Progression::recovered; //move to recovered
	  node.infective = false;
	}
  }
  else if(node.infection_status==Progression::critical
		  && (cur_time - node.time_of_infection
			  > (node.incubation_period
				 + node.asymptomatic_period
				 + node.symptomatic_period
				 + node.hospital_regular_period
				 + node.hospital_critical_period))){
	if(bernoulli(STATE_TRAN[age_index][2])){
	  node.infection_status = Progression::dead;//move to dead
	  node.infective = false;
	}
	else {
	  node.infection_status = Progression::recovered;//move to recovered
	  node.infective = false;
	}
	node.lambda_h = update_individual_lambda_h(node);
	node.lambda_w = update_individual_lambda_w(node);
	node.lambda_c = update_individual_lambda_c(node);
  }
}

void update_all_kappa(vector<agent>& nodes, vector<house>& homes, vector<workplace>& workplaces, vector<community>& communities, int cur_time){
  switch(GLOBAL.INTERVENTION){
  case Intervention::no_intervention:
	get_kappa_no_intervention(nodes, homes, workplaces, communities,cur_time);
	break;
  case Intervention::case_isolation:
	get_kappa_case_isolation(nodes, homes, workplaces, communities,cur_time);
	break;
  case Intervention::home_quarantine:
	get_kappa_home_quarantine(nodes, homes, workplaces, communities, cur_time);
	break;
  case Intervention::lockdown:
	get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
	break;
  case Intervention::case_isolation_and_home_quarantine:
	get_kappa_CI_HQ(nodes, homes, workplaces, communities, cur_time);
	break;
  case Intervention::case_isolation_and_home_quarantine_sd_70_plus:
	get_kappa_CI_HQ_70P(nodes, homes, workplaces, communities, cur_time);
	break;
  case Intervention::lockdown_21_ci_hq_sd_70_plus_21_ci:
	get_kappa_LOCKDOWN_21_CI_HQ_SD_70_PLUS_21_CI(nodes, homes, workplaces, communities,cur_time);
	break;
  case Intervention::lockdown_21:
	get_kappa_LOCKDOWN_21(nodes, homes, workplaces, communities,cur_time);
	break;
  default:
	break;
  }
}

void run_simulation(){
  auto homes = init_homes();
  auto workplaces = init_workplaces();
  auto communities = init_community();
  auto nodes = init_nodes();

  auto community_dist_matrix = compute_community_distances(communities);

  assign_individual_home_community(nodes, homes, workplaces, communities);

  compute_scale_homes(homes);
  compute_scale_workplaces(workplaces);
  compute_scale_communities(nodes, communities);

  for(int time_step = 0; time_step < GLOBAL.NUM_TIMESTEPS; ++time_step){
	for(int j = 0; j < GLOBAL.num_people; ++j){
	  update_infection(nodes[j], time_step);
	  nodes[j].psi_T = psi_T(nodes[j], time_step);
	}
	update_all_kappa(nodes, homes, workplaces, communities, time_step);
  }
}

int main(){
  run_simulation();
  return 0;
}
