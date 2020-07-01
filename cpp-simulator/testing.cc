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
	  if(bernoulli(probabilities.prob_contact_trace_household_symptomatic)){
		for(count_type j=0; j<homes[nodes[i].home].individuals.size(); ++j){
		  nodes[homes[nodes[i].home].individuals[j]].test_status.contact_traced_epoch = current_time;
		  // Do we need to put it insdie the if condition. If not, some people can potentially be in quarantine for a very long time!
		  if(nodes[homes[nodes[i].home].individuals[j]].disease_label == DiseaseLabel::asymptomatic){
			nodes[homes[nodes[i].home].individuals[j]].disease_label = DiseaseLabel::primary_contact;
		  }
		  if(current_time - nodes[homes[nodes[i].home].individuals[j]].test_status.tested_epoch > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
			if(nodes[homes[nodes[i].home].individuals[j]].infection_status == Progression::symptomatic){
			  nodes[homes[nodes[i].home].individuals[j]].test_status.test_requested = nodes[homes[nodes[i].home].individuals[j]].test_status.test_requested || bernoulli(probabilities.prob_test_household_symptomatic_symptomatic);
			}
			else if(nodes[homes[nodes[i].home].individuals[j]].infection_status == Progression::susceptible ||
					nodes[homes[nodes[i].home].individuals[j]].infection_status == Progression::exposed ||
					nodes[homes[nodes[i].home].individuals[j]].infection_status == Progression::infective ||
					(nodes[homes[nodes[i].home].individuals[j]].infection_status == Progression::recovered &&
					 !nodes[homes[nodes[i].home].individuals[j]].entered_hospitalised_state)){
			  nodes[homes[nodes[i].home].individuals[j]].test_status.test_requested = nodes[homes[nodes[i].home].individuals[j]].test_status.test_requested || bernoulli(probabilities.prob_test_household_symptomatic_asymptomatic);
			}
		  }
		}
	  }
	}

	if(nodes[i].infection_status == Progression::hospitalised && time_since_hospitalised >0 && time_since_hospitalised<=1){
	  if(bernoulli(probabilities.prob_contact_trace_household_hospitalised)){
		for(count_type j=0; j<homes[nodes[i].home].individuals.size(); ++j){
		  nodes[homes[nodes[i].home].individuals[j]].test_status.contact_traced_epoch = current_time;
		  // Do we need to put it insdie the if condition. If not, some people can potentially be in quarantine for a very long time!
		  if(nodes[homes[nodes[i].home].individuals[j]].disease_label == DiseaseLabel::asymptomatic){
			nodes[homes[nodes[i].home].individuals[j]].disease_label = DiseaseLabel::primary_contact;
		  }
		  if(current_time - nodes[homes[nodes[i].home].individuals[j]].test_status.tested_epoch > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
			if(nodes[homes[nodes[i].home].individuals[j]].infection_status == Progression::symptomatic){
			  nodes[homes[nodes[i].home].individuals[j]].test_status.test_requested = nodes[homes[nodes[i].home].individuals[j]].test_status.test_requested || bernoulli(probabilities.prob_test_household_hospitalised_symptomatic);
			}
			else if(nodes[homes[nodes[i].home].individuals[j]].infection_status == Progression::susceptible ||
					nodes[homes[nodes[i].home].individuals[j]].infection_status == Progression::exposed ||
					nodes[homes[nodes[i].home].individuals[j]].infection_status == Progression::infective ||
					(nodes[homes[nodes[i].home].individuals[j]].infection_status == Progression::recovered &&
					 !nodes[homes[nodes[i].home].individuals[j]].entered_hospitalised_state)){
			  nodes[homes[nodes[i].home].individuals[j]].test_status.test_requested = nodes[homes[nodes[i].home].individuals[j]].test_status.test_requested || bernoulli(probabilities.prob_test_household_hospitalised_asymptomatic);
			}
		  }
		}
	  }
	}

	if(nodes[i].test_status.state == test_result::positive && time_since_tested>0 && time_since_tested<=1){
		if(bernoulli(probabilities.prob_contact_trace_household_positive)){
		  for(count_type j=0; j<homes[nodes[i].home].individuals.size(); ++j){
			nodes[homes[nodes[i].home].individuals[j]].test_status.contact_traced_epoch = current_time;
			// Do we need to put it insdie the if condition. If not, some people can potentially be in quarantine for a very long time!
			if(nodes[homes[nodes[i].home].individuals[j]].disease_label == DiseaseLabel::asymptomatic){
			  nodes[homes[nodes[i].home].individuals[j]].disease_label = DiseaseLabel::primary_contact;
			}
			if(current_time - nodes[homes[nodes[i].home].individuals[j]].test_status.tested_epoch > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
			  if(nodes[homes[nodes[i].home].individuals[j]].infection_status == Progression::symptomatic){
				nodes[homes[nodes[i].home].individuals[j]].test_status.test_requested = nodes[homes[nodes[i].home].individuals[j]].test_status.test_requested || bernoulli(probabilities.prob_test_household_positive_symptomatic);
			  }
			  else if(nodes[homes[nodes[i].home].individuals[j]].infection_status == Progression::susceptible ||
					  nodes[homes[nodes[i].home].individuals[j]].infection_status == Progression::exposed ||
					  nodes[homes[nodes[i].home].individuals[j]].infection_status == Progression::infective ||
					  (nodes[homes[nodes[i].home].individuals[j]].infection_status == Progression::recovered &&
					   !nodes[homes[nodes[i].home].individuals[j]].entered_hospitalised_state)){
				nodes[homes[nodes[i].home].individuals[j]].test_status.test_requested = nodes[homes[nodes[i].home].individuals[j]].test_status.test_requested || bernoulli(probabilities.prob_test_household_positive_asymptomatic);
			  }
			}
		  }
		}
	}

// Test people in smaller workplace network

	if(nodes[i].workplace_type==WorkplaceType::school || nodes[i].workplace_type==WorkplaceType::office){
		double prob_symptomatic_symptomatic = 0;
		double prob_hospitalised_symptomatic = 0;
		double prob_positive_symptomatic = 0;
		double prob_symptomatic_asymptomatic = 0;
		double prob_hospitalised_asymptomatic = 0;
		double prob_positive_asymptomatic = 0;

		double prob_contact_trace_symptomatic = 0;
		double prob_contact_trace_hospitalised = 0;
		double prob_contact_trace_positive = 0;

		if(nodes[i].workplace_type==WorkplaceType::school){
		  prob_symptomatic_symptomatic = probabilities.prob_test_school_symptomatic_symptomatic;
		  prob_hospitalised_symptomatic = probabilities.prob_test_school_hospitalised_symptomatic;
		  prob_positive_symptomatic = probabilities.prob_test_school_positive_symptomatic;

		  prob_symptomatic_asymptomatic = probabilities.prob_test_school_symptomatic_asymptomatic;
		  prob_hospitalised_asymptomatic = probabilities.prob_test_school_hospitalised_asymptomatic;
		  prob_positive_asymptomatic = probabilities.prob_test_school_positive_asymptomatic;

		  prob_contact_trace_symptomatic = probabilities.prob_contact_trace_class_symptomatic;
		  prob_contact_trace_hospitalised = probabilities.prob_contact_trace_class_hospitalised;
		  prob_contact_trace_positive = probabilities.prob_contact_trace_class_positive;

		}
		else if(nodes[i].workplace_type==WorkplaceType::office){
		  prob_symptomatic_symptomatic = probabilities.prob_test_workplace_symptomatic_symptomatic;
		  prob_hospitalised_symptomatic = probabilities.prob_test_workplace_hospitalised_symptomatic;
		  prob_positive_symptomatic = probabilities.prob_test_workplace_positive_symptomatic;

		  prob_symptomatic_asymptomatic = probabilities.prob_test_workplace_symptomatic_asymptomatic;
		  prob_hospitalised_asymptomatic = probabilities.prob_test_workplace_hospitalised_asymptomatic;
		  prob_positive_asymptomatic = probabilities.prob_test_workplace_positive_asymptomatic;

		  prob_contact_trace_symptomatic = probabilities.prob_contact_trace_project_symptomatic;
		  prob_contact_trace_hospitalised = probabilities.prob_contact_trace_project_hospitalised;
		  prob_contact_trace_positive = probabilities.prob_contact_trace_project_positive;
		}


		if(nodes[i].infection_status == Progression::symptomatic && time_since_symptomatic >0 && time_since_symptomatic<=1){
			if(bernoulli(prob_contact_trace_symptomatic)){
			  for(const auto colleague_index: workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals){
				nodes[colleague_index].test_status.contact_traced_epoch = current_time;
				// Do we need to put it insdie the if condition. If not, some people can potentially be in quarantine for a very long time!
				if(nodes[colleague_index].disease_label == DiseaseLabel::asymptomatic){
				  nodes[colleague_index].disease_label = DiseaseLabel::primary_contact;
				}
				if(current_time - nodes[colleague_index].test_status.tested_epoch
				   > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
				  if(nodes[colleague_index].infection_status == Progression::symptomatic){
					nodes[colleague_index].test_status.test_requested = nodes[colleague_index].test_status.test_requested || bernoulli(prob_symptomatic_symptomatic);
				  }
				  else if(nodes[colleague_index].infection_status == Progression::susceptible ||
						  nodes[colleague_index].infection_status == Progression::exposed ||
						  nodes[colleague_index].infection_status == Progression::infective ||
						  (nodes[colleague_index].infection_status == Progression::recovered &&
						   !nodes[colleague_index].entered_hospitalised_state)){
					nodes[colleague_index].test_status.test_requested = nodes[colleague_index].test_status.test_requested || bernoulli(prob_symptomatic_asymptomatic);
				  }
				}
			  }
			}
		}

		if(nodes[i].infection_status == Progression::hospitalised && time_since_hospitalised >0 && time_since_hospitalised<=1){
		  if(bernoulli(prob_contact_trace_hospitalised)){
			for(const auto colleague_index: workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals){
			  nodes[colleague_index].test_status.contact_traced_epoch = current_time;
			  // Do we need to put it insdie the if condition. If not, some people can potentially be in quarantine for a very long time!
			  if(nodes[colleague_index].disease_label == DiseaseLabel::asymptomatic){
				nodes[colleague_index].disease_label = DiseaseLabel::primary_contact;
			  }
			  if(current_time - nodes[colleague_index].test_status.tested_epoch
				 > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
				if(nodes[colleague_index].infection_status == Progression::symptomatic){
				  nodes[colleague_index].test_status.test_requested = nodes[colleague_index].test_status.test_requested || bernoulli(prob_hospitalised_symptomatic);
				}
				else if(nodes[colleague_index].infection_status == Progression::susceptible ||
						nodes[colleague_index].infection_status == Progression::exposed ||
						nodes[colleague_index].infection_status == Progression::infective ||
						(nodes[colleague_index].infection_status == Progression::recovered &&
						 !nodes[colleague_index].entered_hospitalised_state)){
				  nodes[colleague_index].test_status.test_requested = nodes[colleague_index].test_status.test_requested || bernoulli(prob_hospitalised_asymptomatic);
				}
			  }
			}
		  }
		}

		if(nodes[i].test_status.state == test_result::positive && time_since_tested>0 && time_since_tested<=1){
		  if(bernoulli(prob_contact_trace_positive)){
			for(const auto colleague_index: workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals){
			  nodes[colleague_index].test_status.contact_traced_epoch = current_time;
			  // Do we need to put it insdie the if condition. If not, some people can potentially be in quarantine for a very long time!
			  if(nodes[colleague_index].disease_label == DiseaseLabel::asymptomatic){
				nodes[colleague_index].disease_label = DiseaseLabel::primary_contact;
			  }
			  if(current_time - nodes[colleague_index].test_status.tested_epoch > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
				if(nodes[colleague_index].infection_status == Progression::symptomatic){
				  nodes[colleague_index].test_status.test_requested = nodes[colleague_index].test_status.test_requested || bernoulli(prob_positive_symptomatic);
				}
				else if(nodes[colleague_index].infection_status == Progression::susceptible ||
						nodes[colleague_index].infection_status == Progression::exposed ||
						nodes[colleague_index].infection_status == Progression::infective ||
						(nodes[colleague_index].infection_status == Progression::recovered &&
						 !nodes[colleague_index].entered_hospitalised_state)){
				  nodes[colleague_index].test_status.test_requested = nodes[colleague_index].test_status.test_requested || bernoulli(prob_positive_asymptomatic);
				}
			  }
			}
		  }
		}
	}

// Test people in random community network

	if(nodes[i].infection_status == Progression::symptomatic && time_since_symptomatic >0 && time_since_symptomatic<=1){
	  if(bernoulli(probabilities.prob_contact_trace_random_community_symptomatic)){
		for(count_type k=0; k<homes[nodes[i].home].random_households.households.size(); k++){
		  for(const auto cohabitant_index: homes[homes[nodes[i].home].random_households.households[k]].individuals){
			nodes[cohabitant_index].test_status.contact_traced_epoch = current_time;
			// Do we need to put it insdie the if condition. If not, some people can potentially be in quarantine for a very long time!
			if(nodes[cohabitant_index].disease_label == DiseaseLabel::asymptomatic){
			  nodes[cohabitant_index].disease_label = DiseaseLabel::primary_contact;
			}
			if(current_time - nodes[cohabitant_index].test_status.tested_epoch > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
			  if(nodes[cohabitant_index].infection_status == Progression::symptomatic){
				nodes[cohabitant_index].test_status.test_requested = nodes[cohabitant_index].test_status.test_requested || bernoulli(probabilities.prob_test_random_community_symptomatic_symptomatic);
			  }
			  else if(nodes[cohabitant_index].infection_status == Progression::susceptible ||
					  nodes[cohabitant_index].infection_status == Progression::exposed ||
					  nodes[cohabitant_index].infection_status == Progression::infective ||
					  (nodes[cohabitant_index].infection_status == Progression::recovered &&
					   !nodes[cohabitant_index].entered_hospitalised_state)){
				nodes[cohabitant_index].test_status.test_requested = nodes[cohabitant_index].test_status.test_requested || bernoulli(probabilities.prob_test_random_community_symptomatic_asymptomatic);
			  }
			}
		  }
		}
	  }
	}

	if(nodes[i].infection_status == Progression::hospitalised && time_since_hospitalised >0 && time_since_hospitalised<=1){
		if(bernoulli(probabilities.prob_contact_trace_random_community_hospitalised)){
			for(count_type k=0; k<homes[nodes[i].home].random_households.households.size(); k++){
			  for(const auto cohabitant_index: homes[homes[nodes[i].home].random_households.households[k]].individuals){
				nodes[cohabitant_index].test_status.contact_traced_epoch = current_time;
				// Do we need to put it insdie the if condition. If not, some people can potentially be in quarantine for a very long time!
				if(nodes[cohabitant_index].disease_label == DiseaseLabel::asymptomatic){
				  nodes[cohabitant_index].disease_label = DiseaseLabel::primary_contact;
				}
				if(current_time - nodes[cohabitant_index].test_status.tested_epoch > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
				  if(nodes[cohabitant_index].infection_status == Progression::symptomatic){
					nodes[cohabitant_index].test_status.test_requested = nodes[cohabitant_index].test_status.test_requested || bernoulli(probabilities.prob_test_random_community_hospitalised_symptomatic);
				  }
				  else if(nodes[cohabitant_index].infection_status == Progression::susceptible ||
						  nodes[cohabitant_index].infection_status == Progression::exposed ||
						  nodes[cohabitant_index].infection_status == Progression::infective ||
						  (nodes[cohabitant_index].infection_status == Progression::recovered &&
						   !nodes[cohabitant_index].entered_hospitalised_state)){
					nodes[cohabitant_index].test_status.test_requested = nodes[cohabitant_index].test_status.test_requested || bernoulli(probabilities.prob_test_random_community_hospitalised_asymptomatic);
				  }
				}
			  }
			}
		}
	}

	if(nodes[i].test_status.state == test_result::positive && time_since_tested>0 && time_since_tested<=1){
		if(bernoulli(probabilities.prob_contact_trace_random_community_positive)){
		  for(count_type k=0; k<homes[nodes[i].home].random_households.households.size(); k++){
			for(const auto cohabitant_index: homes[homes[nodes[i].home].random_households.households[k]].individuals){
			  nodes[cohabitant_index].test_status.contact_traced_epoch = current_time;
			  // Do we need to put it insdie the if condition. If not, some people can potentially be in quarantine for a very long time!
			  if(nodes[cohabitant_index].disease_label == DiseaseLabel::asymptomatic){
				nodes[cohabitant_index].disease_label = DiseaseLabel::primary_contact;
			  }
			  if(current_time - nodes[cohabitant_index].test_status.tested_epoch > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
				if(nodes[cohabitant_index].infection_status == Progression::symptomatic){
				  nodes[cohabitant_index].test_status.test_requested = nodes[cohabitant_index].test_status.test_requested || bernoulli(probabilities.prob_test_random_community_positive_symptomatic);
				}
				else if(nodes[cohabitant_index].infection_status == Progression::susceptible ||
						nodes[cohabitant_index].infection_status == Progression::exposed ||
						nodes[cohabitant_index].infection_status == Progression::infective ||
						(nodes[cohabitant_index].infection_status == Progression::recovered &&
						 !nodes[cohabitant_index].entered_hospitalised_state)){
				  nodes[cohabitant_index].test_status.test_requested = nodes[cohabitant_index].test_status.test_requested || bernoulli(probabilities.prob_test_random_community_positive_asymptomatic);
				}
			  }
			}
		  }
		}
	}

	// Test people in neighbourhood cell

	grid_cell my_grid_cell = homes[nodes[i].home].neighbourhood;
	nbr_cell my_nbr_cell = nbr_cells[my_grid_cell.cell_x][my_grid_cell.cell_y];
	count_type my_nbr_size = my_nbr_cell.houses_list.size();

	if(nodes[i].infection_status == Progression::symptomatic && time_since_symptomatic >0 && time_since_symptomatic<=1){
	  if(bernoulli(probabilities.prob_contact_trace_neighbourhood_symptomatic)){
		  for(count_type k=0; k<my_nbr_size; k++){
			for(const auto neighbor_index: homes[my_nbr_cell.houses_list[k]].individuals){
			  nodes[neighbor_index].test_status.contact_traced_epoch = current_time;
			  // Do we need to put it insdie the if condition. If not, some people can potentially be in quarantine for a very long time!
			  if(nodes[neighbor_index].disease_label == DiseaseLabel::asymptomatic){
				nodes[neighbor_index].disease_label = DiseaseLabel::primary_contact;
			  }
			  if(current_time - nodes[neighbor_index].test_status.tested_epoch > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
				if(nodes[neighbor_index].infection_status == Progression::symptomatic){
				  nodes[neighbor_index].test_status.test_requested = nodes[neighbor_index].test_status.test_requested || bernoulli(probabilities.prob_test_neighbourhood_symptomatic_symptomatic);
				}
				else if(nodes[neighbor_index].infection_status == Progression::susceptible ||
						nodes[neighbor_index].infection_status == Progression::exposed ||
						nodes[neighbor_index].infection_status == Progression::infective ||
						(nodes[neighbor_index].infection_status == Progression::recovered &&
						 !nodes[neighbor_index].entered_hospitalised_state)){
				  nodes[neighbor_index].test_status.test_requested = nodes[neighbor_index].test_status.test_requested || bernoulli(probabilities.prob_test_neighbourhood_symptomatic_asymptomatic);
				}
			  }
			}
		  }
	  }
	}

	if(nodes[i].infection_status == Progression::hospitalised && time_since_hospitalised >0 && time_since_hospitalised<=1){
	  if(bernoulli(probabilities.prob_contact_trace_neighbourhood_hospitalised)){
		for(count_type k=0; k<my_nbr_size; k++){
		  for(const auto neighbor_index: homes[my_nbr_cell.houses_list[k]].individuals){
			nodes[neighbor_index].test_status.contact_traced_epoch = current_time; // Do we need to put it insdie the if condition. If not, some people can potentially be in quarantine for a very long time!
			if(nodes[neighbor_index].disease_label == DiseaseLabel::asymptomatic){
			  nodes[neighbor_index].disease_label = DiseaseLabel::primary_contact;
			}
			if(current_time - nodes[neighbor_index].test_status.tested_epoch > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
			  if(nodes[neighbor_index].infection_status == Progression::symptomatic){
				nodes[neighbor_index].test_status.test_requested = nodes[neighbor_index].test_status.test_requested || bernoulli(probabilities.prob_test_neighbourhood_hospitalised_symptomatic);
			  }
			  else if(nodes[neighbor_index].infection_status == Progression::susceptible ||
					  nodes[neighbor_index].infection_status == Progression::exposed ||
					  nodes[neighbor_index].infection_status == Progression::infective ||
					  (nodes[neighbor_index].infection_status == Progression::recovered &&
					   !nodes[neighbor_index].entered_hospitalised_state)){
				nodes[neighbor_index].test_status.test_requested = nodes[neighbor_index].test_status.test_requested || bernoulli(probabilities.prob_test_neighbourhood_hospitalised_asymptomatic);
			  }
			}
		  }
		}
	  }
	}

	if(nodes[i].test_status.state == test_result::positive && time_since_tested>0 && time_since_tested<=1){
	  if(bernoulli(probabilities.prob_contact_trace_neighbourhood_positive)){
		for(count_type k=0; k<my_nbr_size; k++){
		  for(const auto neighbor_index: homes[my_nbr_cell.houses_list[k]].individuals){
			nodes[neighbor_index].test_status.contact_traced_epoch = current_time; // Do we need to put it insdie the if condition. If not, some people can potentially be in quarantine for a very long time!
			if(nodes[neighbor_index].disease_label == DiseaseLabel::asymptomatic){
			  nodes[neighbor_index].disease_label = DiseaseLabel::primary_contact;
			}
			if(current_time - nodes[neighbor_index].test_status.tested_epoch > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
			  if(nodes[neighbor_index].infection_status == Progression::symptomatic){
				nodes[neighbor_index].test_status.test_requested = nodes[neighbor_index].test_status.test_requested || bernoulli(probabilities.prob_test_neighbourhood_positive_symptomatic);
			  }
			  else if(nodes[neighbor_index].infection_status == Progression::susceptible ||
					  nodes[neighbor_index].infection_status == Progression::exposed ||
					  nodes[neighbor_index].infection_status == Progression::infective ||
					  (nodes[neighbor_index].infection_status == Progression::recovered &&
					   !nodes[neighbor_index].entered_hospitalised_state)){
				nodes[neighbor_index].test_status.test_requested = nodes[neighbor_index].test_status.test_requested || bernoulli(probabilities.prob_test_neighbourhood_positive_asymptomatic);
			  }
			}
		  }
		}
	  }
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