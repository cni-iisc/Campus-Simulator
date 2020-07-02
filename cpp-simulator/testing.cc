//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0
#include "models.h"
#include "testing.h"
#include "intervention_primitives.h"
#include <cassert>
using std::vector;


void set_test_request(vector<agent>& nodes, const vector<house>& homes, const vector<workplace>& workplaces, const vector<vector<nbr_cell>>& nbr_cells, const vector<community>& communities, const testing_probability probabilities, const count_type current_time){
  for(count_type i=0; i<nodes.size(); ++i){
	double time_since_hospitalised = current_time
                - (nodes[i].time_of_infection
                        + nodes[i].incubation_period
                        + nodes[i].asymptomatic_period
                        + nodes[i].symptomatic_period);
	double time_since_symptomatic = current_time - (nodes[i].time_of_infection
                        + nodes[i].incubation_period
                        + nodes[i].asymptomatic_period);

  	double time_since_tested = current_time - nodes[i].test_status.tested_epoch;



	// Test people in the same household

	if(nodes[i].infection_status == Progression::symptomatic && time_since_symptomatic >0 && time_since_symptomatic<=1){
	  test_contact_trace_household(i,nodes,homes,probabilities.prob_contact_trace_household_symptomatic,probabilities.prob_test_household_symptomatic_symptomatic,probabilities.prob_test_household_symptomatic_asymptomatic, current_time);
	}

	if(nodes[i].infection_status == Progression::hospitalised && time_since_hospitalised >0 && time_since_hospitalised<=1){
	  test_contact_trace_household(i,nodes,homes,probabilities.prob_contact_trace_household_hospitalised,probabilities.prob_test_household_hospitalised_symptomatic,probabilities.prob_test_household_hospitalised_asymptomatic, current_time);
	}

	if(nodes[i].test_status.state == test_result::positive && time_since_tested>0 && time_since_tested<=1){
		test_contact_trace_household(i,nodes,homes,probabilities.prob_contact_trace_household_positive,probabilities.prob_test_household_positive_symptomatic,probabilities.prob_test_household_positive_asymptomatic, current_time);
	}

// Test people in smaller workplace network

	if(nodes[i].workplace_type==WorkplaceType::school || nodes[i].workplace_type==WorkplaceType::office){
		if(nodes[i].workplace_type==WorkplaceType::school){
			if(nodes[i].infection_status == Progression::symptomatic && time_since_symptomatic >0 && time_since_symptomatic<=1){
				test_contact_trace_project(i,nodes,workplaces,probabilities.prob_contact_trace_class_symptomatic,probabilities.prob_test_school_symptomatic_symptomatic,probabilities.prob_test_school_symptomatic_asymptomatic, current_time);
			}

			if(nodes[i].infection_status == Progression::hospitalised && time_since_hospitalised >0 && time_since_hospitalised<=1){
				test_contact_trace_project(i,nodes,workplaces,probabilities.prob_contact_trace_class_hospitalised,probabilities.prob_test_school_hospitalised_symptomatic,probabilities.prob_test_school_hospitalised_asymptomatic, current_time);
			}

			if(nodes[i].test_status.state == test_result::positive && time_since_tested>0 && time_since_tested<=1){
				test_contact_trace_project(i,nodes,workplaces,probabilities.prob_contact_trace_class_positive,probabilities.prob_test_school_positive_symptomatic,probabilities.prob_test_school_positive_asymptomatic, current_time);
			}

		}
		else if(nodes[i].workplace_type==WorkplaceType::office){
			if(nodes[i].infection_status == Progression::symptomatic && time_since_symptomatic >0 && time_since_symptomatic<=1){
				test_contact_trace_project(i,nodes,workplaces,probabilities.prob_contact_trace_project_symptomatic,probabilities.prob_test_workplace_symptomatic_symptomatic,probabilities.prob_test_workplace_symptomatic_asymptomatic, current_time);
			}

			if(nodes[i].infection_status == Progression::hospitalised && time_since_hospitalised >0 && time_since_hospitalised<=1){
				test_contact_trace_project(i,nodes,workplaces,probabilities.prob_contact_trace_project_hospitalised,probabilities.prob_test_workplace_hospitalised_symptomatic,probabilities.prob_test_workplace_hospitalised_asymptomatic, current_time);
			}

			if(nodes[i].test_status.state == test_result::positive && time_since_tested>0 && time_since_tested<=1){
				test_contact_trace_project(i,nodes,workplaces,probabilities.prob_contact_trace_project_positive,probabilities.prob_test_workplace_positive_symptomatic,probabilities.prob_test_workplace_positive_asymptomatic, current_time);
			}
		}		
	}

// Test people in random community network

	if(nodes[i].infection_status == Progression::symptomatic && time_since_symptomatic >0 && time_since_symptomatic<=1){
		test_contact_trace_random_community(i,nodes,homes,probabilities.prob_contact_trace_random_community_symptomatic,probabilities.prob_test_random_community_symptomatic_symptomatic,probabilities.prob_test_random_community_symptomatic_asymptomatic, current_time);
	}

	if(nodes[i].infection_status == Progression::hospitalised && time_since_hospitalised >0 && time_since_hospitalised<=1){
		test_contact_trace_random_community(i,nodes,homes,probabilities.prob_contact_trace_random_community_hospitalised,probabilities.prob_test_random_community_hospitalised_symptomatic,probabilities.prob_test_random_community_hospitalised_asymptomatic, current_time);
	}

	if(nodes[i].test_status.state == test_result::positive && time_since_tested>0 && time_since_tested<=1){
		test_contact_trace_random_community(i,nodes,homes,probabilities.prob_contact_trace_random_community_positive,probabilities.prob_test_random_community_positive_symptomatic,probabilities.prob_test_random_community_positive_asymptomatic, current_time);
	}

	// Test people in neighbourhood cell


	if(nodes[i].infection_status == Progression::symptomatic && time_since_symptomatic >0 && time_since_symptomatic<=1){
		test_contact_trace_neighbourhood_cell(i,nodes,homes,nbr_cells,probabilities.prob_contact_trace_neighbourhood_symptomatic,probabilities.prob_test_neighbourhood_symptomatic_symptomatic,probabilities.prob_test_neighbourhood_symptomatic_asymptomatic, current_time);
	}

	if(nodes[i].infection_status == Progression::hospitalised && time_since_hospitalised >0 && time_since_hospitalised<=1){
		test_contact_trace_neighbourhood_cell(i,nodes,homes,nbr_cells,probabilities.prob_contact_trace_neighbourhood_hospitalised,probabilities.prob_test_neighbourhood_hospitalised_symptomatic,probabilities.prob_test_neighbourhood_hospitalised_asymptomatic, current_time);
	}

	if(nodes[i].test_status.state == test_result::positive && time_since_tested>0 && time_since_tested<=1){
		test_contact_trace_neighbourhood_cell(i,nodes,homes,nbr_cells,probabilities.prob_contact_trace_neighbourhood_positive,probabilities.prob_test_neighbourhood_positive_symptomatic,probabilities.prob_test_neighbourhood_positive_asymptomatic, current_time);
	}

	//Finally, set test_requested for current agent
	if(nodes[i].infection_status == Progression::symptomatic && time_since_symptomatic > 0 && time_since_symptomatic <= 1){
	  nodes[i].test_status.test_requested = nodes[i].test_status.test_requested || bernoulli(probabilities.prob_test_index_symptomatic);
	}
	if(nodes[i].infection_status == Progression::hospitalised && time_since_hospitalised > 0 && time_since_hospitalised <= 1){
	  nodes[i].test_status.test_requested = nodes[i].test_status.test_requested || bernoulli(probabilities.prob_test_index_hospitalised);
	}
	if(nodes[i].test_status.state == test_result::positive && time_since_tested > 0 && time_since_tested <= 1){
	  nodes[i].test_status.test_requested = false;
	}
	// Re-test if somebody is recovered
	if(nodes[i].test_status.state==test_result::positive && nodes[i].infection_status==Progression::recovered){
		nodes[i].test_status.test_requested = bernoulli(probabilities.prob_retest_recovered);
	}
  }
}

// Not used now, to be removed
bool should_be_isolated_node_testing(const agent& node, const int current_time, const int quarantine_days){
  double time_since_tested = current_time - node.test_status.tested_epoch;
 return (node.test_status.state==test_result::positive && (node.infection_status==Progression::exposed || node.infection_status==Progression::infective) &&
   (time_since_tested > 0) &&
   (time_since_tested <= quarantine_days*GLOBAL.SIM_STEPS_PER_DAY));
}


void update_infection_testing(vector<agent>& nodes, vector<house>& houses, count_type current_time){
  for(auto& node: nodes){
	if(node.test_status.state==test_result::positive){
		if(node.infection_status==Progression::symptomatic){
			if(node.severity==1){
				node.disease_label = DiseaseLabel::moderate_symptomatic_tested;
			}
			else{
				node.disease_label = DiseaseLabel::mild_symptomatic_tested;
			}
		}
		else if(node.infection_status==Progression::exposed || node.infection_status==Progression::infective){
			node.disease_label = DiseaseLabel::mild_symptomatic_tested;
		}
		else if(node.infection_status==Progression::hospitalised){
			node.disease_label=DiseaseLabel::severe_symptomatic_tested;
		}
		else if(node.infection_status==Progression::critical){
			node.disease_label=DiseaseLabel::icu;
		}
		else if(node.infection_status==Progression::recovered){
			node.disease_label=DiseaseLabel::recovered;
		}
		else if(node.infection_status==Progression::dead){
			node.disease_label=DiseaseLabel::dead;
		}

	}
	if(node.disease_label==DiseaseLabel::primary_contact || node.disease_label==DiseaseLabel::mild_symptomatic_tested || node.disease_label==DiseaseLabel::moderate_symptomatic_tested){
		if(current_time - node.test_status.contact_traced_epoch <= HOME_QUARANTINE_DAYS*GLOBAL.SIM_STEPS_PER_DAY){
			modify_kappa_case_isolate_node(node);
		}
		else{
				node.disease_label=DiseaseLabel::asymptomatic;
		}
	}
}

}

void set_test_request_fileread(vector<agent>& nodes, const vector<house>& homes,
						 const vector<workplace>& workplaces, const matrix<nbr_cell>& nbr_cells,
						 const vector<community>& communities,						 
						 const vector<testing_probability>& testing_probability_vector, const int cur_time){
  count_type time_threshold = GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS;
  count_type cur_day = cur_time/GLOBAL.SIM_STEPS_PER_DAY; //get current day. Division to avoid multiplication inside for loop.
  const auto SIZE = testing_probability_vector.size();

  assert(SIZE > 0);
  assert(cur_day >= time_threshold);
  count_type intv_index = 0;

  for (count_type count = 0; count < SIZE - 1; ++count){
	time_threshold += testing_probability_vector[count].num_days;
	if(cur_day >= time_threshold){
	  ++intv_index;
	} else {
	  break;
	}
  }
  set_test_request(nodes, homes, workplaces, nbr_cells, communities, testing_probability_vector[intv_index], cur_time);
}


void test_contact_trace_household(count_type node_index, vector<agent>& nodes, const vector<house>& homes, double probability_contact_trace, double probability_test_symptomatic, double probability_test_asymptomatic, const count_type current_time ){
	if(bernoulli(probability_contact_trace)){
		for(auto household_member: homes[nodes[node_index].home].individuals){
		  nodes[household_member].test_status.contact_traced_epoch = current_time;
		  // Do we need to put it insdie the if condition. If not, some people can potentially be in quarantine for a very long time!
		  if(nodes[household_member].disease_label == DiseaseLabel::asymptomatic){
			nodes[household_member].disease_label = DiseaseLabel::primary_contact;
		  }
		  if(current_time - nodes[household_member].test_status.tested_epoch > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
			if(nodes[household_member].infection_status == Progression::symptomatic){
			  nodes[household_member].test_status.test_requested = nodes[household_member].test_status.test_requested || bernoulli(probability_test_symptomatic);
			}
			else if(nodes[household_member].infection_status == Progression::susceptible ||
					nodes[household_member].infection_status == Progression::exposed ||
					nodes[household_member].infection_status == Progression::infective ||
					(nodes[household_member].infection_status == Progression::recovered &&
					 !nodes[household_member].entered_hospitalised_state)){
			  nodes[household_member].test_status.test_requested = nodes[household_member].test_status.test_requested || bernoulli(probability_test_asymptomatic);
			}
		  }
		}
	}
}

void test_contact_trace_project(count_type node_index, vector<agent>& nodes, const vector<workplace>& workplaces, double probability_contact_trace, double probability_test_symptomatic, double probability_test_asymptomatic, const count_type current_time ){
	if(bernoulli(probability_contact_trace)){
		for(const auto colleague_index: workplaces[nodes[node_index].workplace].projects[nodes[node_index].workplace_subnetwork].individuals){
			nodes[colleague_index].test_status.contact_traced_epoch = current_time;
			// Do we need to put it insdie the if condition. If not, some people can potentially be in quarantine for a very long time!
			if(nodes[colleague_index].disease_label == DiseaseLabel::asymptomatic){
				nodes[colleague_index].disease_label = DiseaseLabel::primary_contact;
			}
			if(current_time - nodes[colleague_index].test_status.tested_epoch
				> GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
				if(nodes[colleague_index].infection_status == Progression::symptomatic){
					nodes[colleague_index].test_status.test_requested = nodes[colleague_index].test_status.test_requested || bernoulli(probability_test_symptomatic);
				}
				else if(nodes[colleague_index].infection_status == Progression::susceptible ||
						nodes[colleague_index].infection_status == Progression::exposed ||
						nodes[colleague_index].infection_status == Progression::infective ||
						(nodes[colleague_index].infection_status == Progression::recovered &&
						!nodes[colleague_index].entered_hospitalised_state)){
					nodes[colleague_index].test_status.test_requested = nodes[colleague_index].test_status.test_requested || bernoulli(probability_test_asymptomatic);
				}
			}
		}
	}
}

void test_contact_trace_random_community(count_type node_index, vector<agent>& nodes, const vector<house>& homes, double probability_contact_trace, double probability_test_symptomatic, double probability_test_asymptomatic, const count_type current_time ){
	if(bernoulli(probability_contact_trace)){
		for(count_type k=0; k<homes[nodes[node_index].home].random_households.households.size(); k++){
		  for(const auto cohabitant_index: homes[homes[nodes[node_index].home].random_households.households[k]].individuals){
			nodes[cohabitant_index].test_status.contact_traced_epoch = current_time;
			// Do we need to put it insdie the if condition. If not, some people can potentially be in quarantine for a very long time!
			if(nodes[cohabitant_index].disease_label == DiseaseLabel::asymptomatic){
			  nodes[cohabitant_index].disease_label = DiseaseLabel::primary_contact;
			}
			if(current_time - nodes[cohabitant_index].test_status.tested_epoch > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
			  if(nodes[cohabitant_index].infection_status == Progression::symptomatic){
				nodes[cohabitant_index].test_status.test_requested = nodes[cohabitant_index].test_status.test_requested || bernoulli(probability_test_symptomatic);
			  }
			  else if(nodes[cohabitant_index].infection_status == Progression::susceptible ||
					  nodes[cohabitant_index].infection_status == Progression::exposed ||
					  nodes[cohabitant_index].infection_status == Progression::infective ||
					  (nodes[cohabitant_index].infection_status == Progression::recovered &&
					   !nodes[cohabitant_index].entered_hospitalised_state)){
				nodes[cohabitant_index].test_status.test_requested = nodes[cohabitant_index].test_status.test_requested || bernoulli(probability_test_asymptomatic);
			  }
			}
		  }
		}
	}
}

void test_contact_trace_neighbourhood_cell(count_type node_index, vector<agent>& nodes, const vector<house>& homes, const matrix<nbr_cell> nbr_cells, double probability_contact_trace, double probability_test_symptomatic, double probability_test_asymptomatic, const count_type current_time ){
	grid_cell my_grid_cell = homes[nodes[node_index].home].neighbourhood;
	nbr_cell my_nbr_cell = nbr_cells[my_grid_cell.cell_x][my_grid_cell.cell_y];
	count_type my_nbr_size = my_nbr_cell.houses_list.size();
	if(bernoulli(probability_contact_trace)){
		for(count_type k=0; k<my_nbr_size; k++){
			for(const auto neighbor_index: homes[my_nbr_cell.houses_list[k]].individuals){
				nodes[neighbor_index].test_status.contact_traced_epoch = current_time;
				// Do we need to put it insdie the if condition. If not, some people can potentially be in quarantine for a very long time!
				if(nodes[neighbor_index].disease_label == DiseaseLabel::asymptomatic){
					nodes[neighbor_index].disease_label = DiseaseLabel::primary_contact;
				}
				if(current_time - nodes[neighbor_index].test_status.tested_epoch > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
					if(nodes[neighbor_index].infection_status == Progression::symptomatic){
						nodes[neighbor_index].test_status.test_requested = nodes[neighbor_index].test_status.test_requested || bernoulli(probability_test_symptomatic);
					}
					else if(nodes[neighbor_index].infection_status == Progression::susceptible ||
							nodes[neighbor_index].infection_status == Progression::exposed ||
							nodes[neighbor_index].infection_status == Progression::infective ||
							(nodes[neighbor_index].infection_status == Progression::recovered &&
								!nodes[neighbor_index].entered_hospitalised_state)){
						nodes[neighbor_index].test_status.test_requested = nodes[neighbor_index].test_status.test_requested || bernoulli(probability_test_asymptomatic);
					}
				}
			}
		}
	}
}