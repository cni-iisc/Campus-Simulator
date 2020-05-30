//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0
#include "models.h"
#include "testing.h"
#include "interventions.h"
using std::vector;


void set_test_request(vector<agent>& nodes, vector<house>& homes, vector<workplace>& workplaces, vector<vector<nbr_cell>>& nbr_cells, vector<community>& communities, testing_probability probabilities, count_type current_time){
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
		for(count_type j=0; j<homes[nodes[i].home].individuals.size(); j++){
			if(current_time - nodes[homes[nodes[i].home].individuals[j]].test_status.tested_epoch > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
				nodes[homes[nodes[i].home].individuals[j]].test_status.test_requested = bernoulli(probabilities.prob_test_household_symptomatic);
			}
		}
	}

	if(nodes[i].infection_status == Progression::hospitalised && time_since_hospitalised >0 && time_since_hospitalised<=1){
		for(count_type j=0; j<homes[nodes[i].home].individuals.size(); j++){
			if(current_time - nodes[homes[nodes[i].home].individuals[j]].test_status.tested_epoch > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
				nodes[homes[nodes[i].home].individuals[j]].test_status.test_requested = bernoulli(probabilities.prob_test_household_hospitalised);
			}
		}
	}

	if(nodes[i].test_status.state == test_result::positive && time_since_tested>0 && time_since_tested<=1){
		for(count_type j=0; j<homes[nodes[i].home].individuals.size(); j++){	
			if(current_time - nodes[homes[nodes[i].home].individuals[j]].test_status.tested_epoch > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
				nodes[homes[nodes[i].home].individuals[j]].test_status.test_requested = bernoulli(probabilities.prob_test_household_positive);
			}
		}
	}

// Test people in smaller workplace network

	if(nodes[i].workplace_type==WorkplaceType::school || nodes[i].workplace_type==WorkplaceType::office){
		double prob_symptomatic = 0;
		double prob_hospitalised = 0;
		double prob_positive = 0;
		if(nodes[i].workplace_type==WorkplaceType::school){
			prob_symptomatic = probabilities.prob_test_school_symptomatic;  
			prob_hospitalised = probabilities.prob_test_school_hospitalised; 
			prob_positive = probabilities.prob_test_school_positive; 
		}
		else if(nodes[i].workplace_type==WorkplaceType::office){
			prob_symptomatic = probabilities.prob_test_workplace_symptomatic;  
			prob_hospitalised = probabilities.prob_test_workplace_hospitalised; 
			prob_positive = probabilities.prob_test_workplace_positive; 
		}


		if(nodes[i].infection_status == Progression::symptomatic && time_since_symptomatic >0 && time_since_symptomatic<=1){
			for(count_type j=0; j<workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals.size(); j++){
				if(current_time - nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].test_status.tested_epoch > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
					nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].test_status.test_requested = bernoulli(prob_symptomatic);
				}
			}
		}

		if(nodes[i].infection_status == Progression::hospitalised && time_since_hospitalised >0 && time_since_hospitalised<=1){
			for(count_type j=0; j<workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals.size(); j++){
				if(current_time - nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].test_status.tested_epoch > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
					nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].test_status.test_requested = bernoulli(prob_hospitalised);
				}
			}
		}

		if(nodes[i].test_status.state == test_result::positive && time_since_tested>0 && time_since_tested<=1){
			for(count_type j=0; j<workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals.size(); j++){	
				if(current_time - nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].test_status.tested_epoch > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
					nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].test_status.test_requested = bernoulli(prob_positive);
				}
			}
		}
	}

// Test people in random community network

	if(nodes[i].infection_status == Progression::symptomatic && time_since_symptomatic >0 && time_since_symptomatic<=1){
		for(count_type k=0; k<homes[nodes[i].home].random_households.households.size(); k++){
			for(count_type j=0; j<homes[homes[nodes[i].home].random_households.households[k]].individuals.size(); j++){
				if(current_time - nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].test_status.tested_epoch > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
					nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].test_status.test_requested = bernoulli(probabilities.prob_test_random_community_symptomatic);
				}
			}
		}
	}

	if(nodes[i].infection_status == Progression::hospitalised && time_since_hospitalised >0 && time_since_hospitalised<=1){
		for(count_type k=0; k<homes[nodes[i].home].random_households.households.size(); k++){
			for(count_type j=0; j<homes[homes[nodes[i].home].random_households.households[k]].individuals.size(); j++){
				if(current_time - nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].test_status.tested_epoch > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
					nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].test_status.test_requested = bernoulli(probabilities.prob_test_random_community_hospitalised);

				}
			}
		}
	}

	if(nodes[i].test_status.state == test_result::positive && time_since_tested>0 && time_since_tested<=1){
		for(count_type k=0; k<homes[nodes[i].home].random_households.households.size(); k++){	
			for(count_type j=0; j<homes[homes[nodes[i].home].random_households.households[k]].individuals.size(); j++){
				if(current_time - nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].test_status.tested_epoch > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
					nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].test_status.test_requested = bernoulli(probabilities.prob_test_random_community_positive);
				}
			}
		}
	}

  }
}

bool should_be_isolated_node_testing(const agent& node, const int current_time, const int quarantine_days){
  double time_since_tested = current_time - node.test_status.tested_epoch;
 return (node.test_status.state==test_result::positive && (node.infection_status==Progression::exposed || node.infection_status==Progression::infective) && 
   (time_since_tested > 0) &&
   (time_since_tested <= quarantine_days*GLOBAL.SIM_STEPS_PER_DAY));
}


node_update_status_testing update_infection_testing(agent& node, vector<agent>& nodes, vector<house>& houses, count_type current_time){
  node_update_status_testing temp;
  if(node.test_status.state==test_result::positive){
	  if(node.infection_status==Progression::symptomatic){
		  node.infection_status=Progression::hospitalised;
		  node.infective = false;
		  node.entered_hospitalised_state = true;
		  temp.new_hospitalization = true;
	  }
	  if(should_be_isolated_node_testing(node, current_time, HOME_QUARANTINE_DAYS)){
		  for(count_type i=0; i<houses[node.home].individuals.size(); ++i){
			  modify_kappa_case_isolate_node(nodes[houses[node.home].individuals[i]]);	  
		  }
	  }
  }
  return temp;
}


