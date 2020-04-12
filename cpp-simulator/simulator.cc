#include <vector>
#include <algorithm>
#include <map>
#include <string>
using namespace std;
#include "models.h"
#include "initializers.h"
#include "interventions.h"
#include "updates.h"
#include "simulator.h"

#if defined DEBUG || defined TIMING
#include <iostream>
#include <cstdlib>
using std::cerr;
#endif

#ifdef TIMING
#include <chrono>

template <class T>
auto duration(const std::chrono::time_point<T>& start, const std::chrono::time_point<T>& end){
  return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}
#endif

plot_data_struct run_simulation(){
#ifdef TIMING
  cerr << "simulator: starting JSON read\n";
  auto start_time = std::chrono::high_resolution_clock::now();
#endif
  
  auto homes = init_homes();
  auto workplaces = init_workplaces();
  auto communities = init_community();
  auto nodes = init_nodes();

  auto community_dist_matrix = compute_community_distances(communities);

#ifdef TIMING
    auto end_time = std::chrono::high_resolution_clock::now();
	cerr << "simulator: time for JSON reads (ms): " << duration(start_time, end_time) << "\n";
	start_time = std::chrono::high_resolution_clock::now();
#endif

  assign_individual_home_community(nodes, homes, workplaces, communities);

  compute_scale_homes(homes);
  compute_scale_workplaces(workplaces);
  compute_scale_communities(nodes, communities);

  double travel_fraction = 0;
  
  //This needs to be done after the initilization.
  plot_data_struct plot_data;
  plot_data.nums =
	{
	 {"num_infected", {}},
	 {"num_exposed", {}},
	 {"num_hospitalised", {}},
	 {"num_critical", {}},
	 {"num_fatalities", {}},
	 {"num_recovered", {}},
	 {"num_affected", {}}
	};
  for(auto& elem: plot_data.nums){
	elem.second.reserve(GLOBAL.NUM_TIMESTEPS);
  }
  plot_data.nums["csvContent"] = {};
  plot_data.nums["csvContent"].reserve(GLOBAL.NUM_TIMESTEPS * GLOBAL.num_communities);

  plot_data.susceptible_lambdas =
	{
	 {"susceptible_lambda", {}},
	 {"susceptible_lambda_H", {}},
	 {"susceptible_lambda_W", {}},
	 {"susceptible_lambda_C", {}},
	 {"susceptible_lambda_T", {}}
	};
  for(auto& elem: plot_data.susceptible_lambdas){
	elem.second.reserve(GLOBAL.NUM_TIMESTEPS);
  }

#ifdef TIMING
  end_time = std::chrono::high_resolution_clock::now();
  cerr << "simulator: time for setup after JSON reads (ms): " << duration(start_time, end_time) << "\n";

  cerr << "simulator: starting simulation\n";
  start_time = std::chrono::high_resolution_clock::now();
#endif

  for(count_type time_step = 0; time_step < GLOBAL.NUM_TIMESTEPS; ++time_step){

	//#pragma omp parallel for
	//
	// Since update_infection uses a random number generator with
	// global state, parallelizing this loop is not straightforward.
	// Puttting the generator in a critical section can keep it
	// correct, but slows down the code too much.
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
	  plot_data.nums["csvContent"].push_back({time_step, {
		  c,
		  temp_stats.infected,
		  temp_stats.affected,
		  temp_stats.hospitalised,
		  temp_stats.critical,
		  temp_stats.dead,
		  temp_stats.hd_area_affected
		  }});
	}

	update_lambda_c_global(communities, community_dist_matrix);


	travel_fraction = updated_travel_fraction(nodes);

	double susceptible_lambda = 0,
	  susceptible_lambda_H = 0,
	  susceptible_lambda_W = 0,
	  susceptible_lambda_C = 0,
	  susceptible_lambda_T = 0;
	
#pragma omp parallel for reduction (+:susceptible_lambda, susceptible_lambda_H, susceptible_lambda_W, susceptible_lambda_C, susceptible_lambda_T)
	for (count_type j = 0; j < GLOBAL.num_people; ++j){
	  update_lambdas(nodes[j], homes, workplaces, communities, travel_fraction, time_step);
	  if(nodes[j].infection_status == Progression::susceptible){
		susceptible_lambda += nodes[j].lambda;
		susceptible_lambda_H += nodes[j].lambda_incoming[0];
		susceptible_lambda_W += nodes[j].lambda_incoming[1];
		susceptible_lambda_C += nodes[j].lambda_incoming[2];
		susceptible_lambda_T += nodes[j].lambda_incoming[3];
	  }
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
	plot_data.nums["num_infected"].push_back({time_step, {n_infected}});
	plot_data.nums["num_exposed"].push_back({time_step, {n_exposed}});
	plot_data.nums["num_hospitalised"].push_back({time_step, {n_hospitalised}});
	plot_data.nums["num_critical"].push_back({time_step, {n_critical}});
	plot_data.nums["num_fatalities"].push_back({time_step, {n_fatalities}});
	plot_data.nums["num_recovered"].push_back({time_step, {n_recovered}});
	plot_data.nums["num_affected"].push_back({time_step, {n_affected}});

	plot_data.susceptible_lambdas["susceptible_lambda"].push_back({time_step, {susceptible_lambda}});
	plot_data.susceptible_lambdas["susceptible_lambda_H"].push_back({time_step, {susceptible_lambda_H}});
	plot_data.susceptible_lambdas["susceptible_lambda_W"].push_back({time_step, {susceptible_lambda_W}});
	plot_data.susceptible_lambdas["susceptible_lambda_C"].push_back({time_step, {susceptible_lambda_C}});
	plot_data.susceptible_lambdas["susceptible_lambda_T"].push_back({time_step, {susceptible_lambda_T}});

  }

#ifdef TIMING
  end_time = std::chrono::high_resolution_clock::now();
  cerr << "simulator: simulation time (ms): " << duration(start_time, end_time) << "\n";
#endif
  

  return plot_data;
}

