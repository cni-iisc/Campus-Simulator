#include <vector>
#include <algorithm>

#include "models.h"
#include "initializers.h"


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

}

int main(){
  run_simulation();
  return 0;
}
