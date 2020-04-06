#include <vector>
#include <algorithm>
using namespace std;
#include "models.h"
#include "initializers.h"
#include "interventions.h"
#include "updates.h"


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

#pragma omp parallel for
	for(int j = 0; j < GLOBAL.num_people; ++j){
	  update_infection(nodes[j], time_step);
	  nodes[j].psi_T = psi_T(nodes[j], time_step);
	}

	update_all_kappa(nodes, homes, workplaces, communities, time_step);

	for (int h = 0; h < GLOBAL.num_homes; ++h){
	  homes[h].age_independent_mixing = updated_lambda_h_age_independent(nodes, homes[h]);
	  //FEATURE_PROPOSAL: make the mixing dependent on node.age_group;
	}

	for (int w = 0; w < GLOBAL.num_schools + GLOBAL.num_workplaces; ++w){
	  workplaces[w].age_independent_mixing = updated_lambda_w_age_independent(nodes, workplaces[w]);
	  //FEATURE_PROPOSAL: make the mixing dependent on node.age_group;
	}

	for (int c = 0; c < GLOBAL.num_communities; ++c){
	  communities[c].lambda_community = updated_lambda_c_local(nodes, communities[c]);
	  //TODO: output the CSV
	  //auto temp_stats = get_infected_community(nodes, communities[c]);
	  //let row = [time_step/SIM_STEPS_PER_DAY,c,temp_stats[0],temp_stats[1],temp_stats[2],temp_stats[3],temp_stats[4]].join(",");
	  //csvContent += row + "\r\n";
	}

	update_lambda_c_global(communities, community_dist_matrix);

#pragma omp parallel for
	for (int j = 0; j < GLOBAL.num_people; ++j){
	  update_lambdas(nodes[j], homes, workplaces, communities, time_step);
	}
  }
}

int main(){
  run_simulation();
  return 0;
}
