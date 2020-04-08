#include <vector>
#include <algorithm>
#include <map>
#include <string>
using namespace std;
#include "models.h"
#include "initializers.h"
#include "interventions.h"
#include "updates.h"

#ifdef DEBUG
#include <iostream>
#include <cstdlib>
using std::cerr;
#endif


map<string, matrix<count_type>> run_simulation(){
  auto homes = init_homes();
  auto workplaces = init_workplaces();
  auto communities = init_community();
  auto nodes = init_nodes();

  auto community_dist_matrix = compute_community_distances(communities);

  assign_individual_home_community(nodes, homes, workplaces, communities);

  compute_scale_homes(homes);
  compute_scale_workplaces(workplaces);
  compute_scale_communities(nodes, communities);

  //This needs to be done after the initilization.
  map<string, matrix<count_type>> plot_data =
	{
	 {"num_infected", {}},
	 {"num_exposed", {}},
	 {"num_hospitalised", {}},
	 {"num_critical", {}},
	 {"num_fatalities", {}},
	 {"num_recovered", {}},
	 {"num_affected", {}}
	};
  for(auto& elem: plot_data){
	elem.second.reserve(GLOBAL.NUM_TIMESTEPS);
  }
  plot_data["csvContent"] = {};
  plot_data["csvContent"].reserve(GLOBAL.NUM_TIMESTEPS * GLOBAL.num_communities);

  for(count_type time_step = 0; time_step < GLOBAL.NUM_TIMESTEPS; ++time_step){

#pragma omp parallel for
	for(count_type j = 0; j < GLOBAL.num_people; ++j){
	  update_infection(nodes[j], time_step);
	  nodes[j].psi_T = psi_T(nodes[j], time_step);
	}

	update_all_kappa(nodes, homes, workplaces, communities, time_step);

	for (count_type h = 0; h < GLOBAL.num_homes; ++h){
	  homes[h].age_independent_mixing = updated_lambda_h_age_independent(nodes, homes[h]);
	  //FEATURE_PROPOSAL: make the mixing dependent on node.age_group;
	}

	for (count_type w = 0; w < GLOBAL.num_schools + GLOBAL.num_workplaces; ++w){
	  workplaces[w].age_independent_mixing = updated_lambda_w_age_independent(nodes, workplaces[w]);
	  //FEATURE_PROPOSAL: make the mixing dependent on node.age_group;
	}

	for (count_type c = 0; c < GLOBAL.num_communities; ++c){
	  communities[c].lambda_community = updated_lambda_c_local(nodes, communities[c]);

	  auto temp_stats = get_infected_community(nodes, communities[c]);
	  //let row = [time_step/SIM_STEPS_PER_DAY,c,temp_stats[0],temp_stats[1],temp_stats[2],temp_stats[3],temp_stats[4]].join(",");
	  plot_data["csvContent"].push_back({
		  time_step,
		  c,
		  temp_stats.infected,
		  temp_stats.affected,
		  temp_stats.hospitalised,
		  temp_stats.critical,
		  temp_stats.dead
		});
	}

	update_lambda_c_global(communities, community_dist_matrix);

#pragma omp parallel for
	for (count_type j = 0; j < GLOBAL.num_people; ++j){
	  update_lambdas(nodes[j], homes, workplaces, communities, time_step);
	}
	
	//Get data for this simulation step
	count_type n_infected = 0,
	  n_exposed = 0,
	  n_hospitalised = 0,
	  n_critical = 0,
	  n_fatalities = 0,
	  n_recovered = 0,
	  n_affected = 0;
	
#pragma omp parallel for reduction (+:n_infected,n_exposed,n_hospitalised,n_critical,n_fatalities,n_recovered,n_affected)
	for(count_type j = 0; j < GLOBAL.num_people; ++j){
	  auto infection_status = nodes[j].infection_status;
	  if(infection_status == Progression::infective
		 || infection_status == Progression::symptomatic
		 || infection_status == Progression::hospitalised
		 || infection_status == Progression::critical){
		n_infected += 1;
	  }
	  if(infection_status == Progression::exposed){
		n_exposed += 1;
	  }
	  if(infection_status == Progression::hospitalised){
		n_hospitalised += 1;
	  }
	  if(infection_status == Progression::critical){
		n_critical += 1;
	  }
	  if(infection_status == Progression::dead){
		n_fatalities += 1;
	  }
	  if(infection_status == Progression::recovered){
		n_recovered += 1;
	  }
	  if(infection_status != Progression::susceptible){
		n_affected += 1;
	  }
	}
	plot_data["num_infected"].push_back({time_step, n_infected});
	plot_data["num_exposed"].push_back({time_step, n_exposed});
	plot_data["num_hospitalised"].push_back({time_step, n_hospitalised});
	plot_data["num_critical"].push_back({time_step, n_critical});
	plot_data["num_fatalities"].push_back({time_step, n_fatalities});
	plot_data["num_recovered"].push_back({time_step, n_recovered});
	plot_data["num_affected"].push_back({time_step, n_affected});
  }
  return plot_data;
}

