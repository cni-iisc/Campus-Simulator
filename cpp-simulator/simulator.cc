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

double updated_lambda_w_age_independent(const vector<agent>& nodes, const workplace& workplace){
  double sum_value = 0;

  for (int i=0; i < workplace.individuals.size(); ++i){
	sum_value += nodes[workplace.individuals[i]].lambda_w;
  }
  return workplace.scale*sum_value;
  // Populate it afterwards...
}

double updates_lambda_w_age_independent(const vector<agent>& nodes, const house& home){
  double sum_value = 0;
  for (int i=0; i<home.individuals.size(); ++i){
	sum_value += nodes[home.individuals[i]].lambda_h;
  }
  return home.scale*sum_value;
  // Populate it afterwards...
}

void update_lambdas(agent&node, const vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, int cur_time){
  node.lambda_incoming={0,0,0};

  //No null check for home as every agent has a home
  node.lambda_incoming[0] = node.kappa_H_incoming*homes[node.home].age_independent_mixing;
  //FEATURE_PROPOSAL: make the mixing dependent on node.age_group;
  if(node.workplace != WORKPLACE_HOME) {
	node.lambda_incoming[1] = node.kappa_W_incoming* workplaces[node.workplace].age_independent_mixing;
	//FEATURE_PROPOSAL: make the mixing dependent on node.age_group;
  }
  // No null check for community as every node has a community.
  //
  // For all communities add the community lambda with a distance
  // related scaling factor
  node.lambda_incoming[2] = node.kappa_C_incoming*node.zeta_a*node.funct_d_ck*communities[node.community].lambda_community_global;

  node.lambda = node.lambda_incoming[0] + node.lambda_incoming[1] + node.lambda_incoming[2];

}


double updated_lambda_c_local(const vector<agent>& nodes, community& community){
  double sum_value = 0;
  for(int i = 0; i < community.individuals.size(); ++i){
	sum_value += nodes[community.individuals[i]].lambda_c;
  }
  return community.scale*sum_value;
}

void update_lambda_c_global(vector<community>& communities, const matrix<double>& community_distance_matrix){
  for (int c1=0; c1 < communities.size(); ++c1){
	double num = 0;
	double denom = 0;
	for (int c2 = 0;c2<communities.size(); ++c2){
	  num += f_kernel(community_distance_matrix[c1][c2])*communities[c2].lambda_community;
	  denom += f_kernel(community_distance_matrix[c1][c2]);
	}
	communities[c1].lambda_community_global = num/denom;
  }
}

struct casualty_stats{
  double infected = 0;
  double affected = 0;
  double hospitalised = 0;
  double critical = 0;
  double dead = 0;
  double exposed = 0;
  double recovered = 0;
};

casualty_stats get_infected_community(const vector<agent>& nodes, const community& community){
  casualty_stats stat;
  
  for (int i=0; i<community.individuals.size(); i++){
	if (nodes[community.individuals[i]].infection_status==Progression::exposed) {stat.exposed +=1; }
	if (nodes[community.individuals[i]].infection_status==Progression::recovered) {stat.recovered += 1;}
	if (nodes[community.individuals[i]].infection_status==Progression::hospitalised) {stat.hospitalised += 1;}
	if (nodes[community.individuals[i]].infection_status==Progression::critical) {stat.critical += 1;}
	if (nodes[community.individuals[i]].infection_status==Progression::dead) {stat.dead += 1;}
	

	if (nodes[community.individuals[i]].infection_status==Progression::infective ||
		nodes[community.individuals[i]].infection_status==Progression::symptomatic ||
		nodes[community.individuals[i]].infection_status==Progression::hospitalised ||
		nodes[community.individuals[i]].infection_status==Progression::critical) {stat.infected += 1;}
  }
  stat.affected = stat.exposed+stat.infected+stat.recovered+stat.dead;
  
  //return [infected_stat,affected_stat,hospitalised_stat,critical_stat,dead_stat];
  return stat;
  // Populate it afterwards...
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
