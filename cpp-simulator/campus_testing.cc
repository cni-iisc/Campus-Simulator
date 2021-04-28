//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0
#include "models.h"
#include "campus_testing.h"
#include "campus_interventions.h"
#include "campus_updates.h"
#include <cassert>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using std::vector;


void test_contact_trace(count_type node_index, count_type interaction_index, std::vector<agent>& nodes, std::vector<Interaction_Space>& ispaces, double probability_contact_trace, double probability_test_symptomatic, double probability_test_asymptomatic, count_type current_time, count_type day ){
	for(auto& space : nodes[node_index].spaces){
		if(ispaces[space].interaction_type == static_cast<InteractionType>(interaction_index)){
			for(auto &individual: ispaces[space].individuals[day]){
				if(bernoulli(probability_contact_trace)){//contact trace a household individual with this probability.
				  nodes[individual].test_status.contact_traced_epoch = current_time;
				  if(nodes[individual].disease_label == DiseaseLabel::asymptomatic){
					nodes[individual].disease_label = DiseaseLabel::primary_contact;
				  }
				  if((current_time - nodes[individual].test_status.tested_epoch
						> GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL) &&
					!nodes[individual].test_status.tested_positive){//If the individual was not tested yet.
					if(nodes[individual].infection_status == Progression::symptomatic &&
					 bernoulli(probability_test_symptomatic)){
					  nodes[individual].test_status.test_requested = true;
					  nodes[individual].test_status.node_test_trigger = test_trigger::contact_traced;
					}
					else if((nodes[individual].infection_status == Progression::susceptible ||
							nodes[individual].infection_status == Progression::exposed ||
							nodes[individual].infection_status == Progression::infective ||
							(nodes[individual].infection_status == Progression::recovered &&
							 !nodes[individual].entered_hospitalised_state)) && //could remove this check of entered hospital as we are already checking if the node ever tested positive.
							bernoulli(probability_test_asymptomatic)){
					  nodes[individual].test_status.test_requested = true;
					  nodes[individual].test_status.node_test_trigger = test_trigger::contact_traced;
					}
				  }
				}
			}
		}
	}
}

void set_test_request(vector<agent>& nodes,
		      std::vector<Interaction_Space>& ispaces,
		      testing_probability probabilities,
		      count_type current_time, count_type day){
  
  for(count_type i=0; i<nodes.size(); ++i){
	double time_since_hospitalised = current_time
                - (nodes[i].time_of_infection
                        + nodes[i].incubation_period
                        + nodes[i].asymptomatic_period
                        + nodes[i].symptomatic_period);
	double time_since_symptomatic = current_time - (nodes[i].time_of_infection
                        + nodes[i].incubation_period
                        + nodes[i].asymptomatic_period);

  	//double time_since_tested = current_time - nodes[i].test_status.tested_epoch;

	//First, decide whether to test the node. Triggers for testing.

	//1) Node just turned symptomatic, 2) node was not tested positive before,  and 3) the coin toss decided to test the node
	if(nodes[i].infection_status == Progression::symptomatic &&
	  time_since_symptomatic > 0 && time_since_symptomatic <= 1 &&
	  bernoulli(probabilities.prob_test_index_symptomatic) &&
	  !nodes[i].test_status.tested_positive){
		nodes[i].test_status.test_requested = true;
		// GLOBAL.debug_count_tests_requested ++;
		nodes[i].test_status.node_test_trigger = test_trigger::symptomatic;
		nodes[i].test_status.contact_traced_epoch = current_time;
		// nodes[i].time_tested.push_back(current_time);
 //This is to ensure that if the node's test turns positive, they are also subjected to restrictions.
	}
	//1) Node just turned symptomatic, 2) node was not tested positive before,  and 3) the coin toss decided to test the node
	else if(nodes[i].infection_status == Progression::hospitalised &&
	  time_since_hospitalised > 0 && time_since_hospitalised <= 1 &&
	  bernoulli(probabilities.prob_test_index_hospitalised) &&
	  !nodes[i].test_status.tested_positive){
		nodes[i].test_status.test_requested = true;
		// GLOBAL.debug_count_tests_requested ++;
		nodes[i].test_status.node_test_trigger = test_trigger::hospitalised;
		nodes[i].test_status.contact_traced_epoch = current_time;
		// nodes[i].time_tested.push_back(current_time);
		 //This is to ensure that if their test turns positive, they are also subjected to contact traced restrictions.
	}
	// Re-test if somebody is recovered
	else if(nodes[i].test_status.state==test_result::positive &&
	  nodes[i].infection_status==Progression::recovered &&
	  bernoulli(probabilities.prob_retest_recovered)){
		nodes[i].test_status.test_requested = true;
		// GLOBAL.debug_count_tests_requested ++;
		nodes[i].test_status.node_test_trigger = test_trigger::re_test;
		// nodes[i].time_tested.push_back(current_time);
	}
	if (nodes[i].test_status.test_requested){
		// GLOBAL.debug_count_tests_requested ++;
	}
	// Trigger contact trace from node. Enter only if the node has not yet triggered a contact trace, and if the node tested postive.
	if(!nodes[i].test_status.triggered_contact_trace && nodes[i].test_status.tested_positive){
		nodes[i].test_status.triggered_contact_trace = true; // record that contact tracing was triggered.
		if(nodes[i].test_status.node_test_trigger == test_trigger::symptomatic){
			for(count_type j = 0; j < static_cast<int>(InteractionType::count); j++){
				test_contact_trace(i,GLOBAL.contact_tracing_hierarchy[j],nodes,ispaces,probabilities.prob_contact_trace_symptomatic[GLOBAL.contact_tracing_hierarchy[j]],probabilities.prob_test_symptomatic_symptomatic[GLOBAL.contact_tracing_hierarchy[j]],probabilities.prob_test_symptomatic_asymptomatic[GLOBAL.contact_tracing_hierarchy[j]], current_time, day);
			}

		}
		else if(nodes[i].test_status.node_test_trigger == test_trigger::hospitalised){
			for(count_type j = 0; j < static_cast<int>(InteractionType::count); j++){
				test_contact_trace(i,GLOBAL.contact_tracing_hierarchy[j],nodes,ispaces,probabilities.prob_contact_trace_hospitalised[GLOBAL.contact_tracing_hierarchy[j]],probabilities.prob_test_hospitalised_symptomatic[GLOBAL.contact_tracing_hierarchy[j]],probabilities.prob_test_hospitalised_asymptomatic[GLOBAL.contact_tracing_hierarchy[j]], current_time, day);
			}
		}
		else if(nodes[i].test_status.node_test_trigger == test_trigger::contact_traced){
			for(count_type j = 0; j < static_cast<int>(InteractionType::count); j++){
				test_contact_trace(i,GLOBAL.contact_tracing_hierarchy[j],nodes,ispaces,probabilities.prob_contact_trace_positive[GLOBAL.contact_tracing_hierarchy[j]],probabilities.prob_test_positive_symptomatic[GLOBAL.contact_tracing_hierarchy[j]],probabilities.prob_test_positive_asymptomatic[GLOBAL.contact_tracing_hierarchy[j]], current_time, day);
			}
		}

	}
  }
}



// Not used now, to be removed
[[deprecated("Auxilliary function not to be used now")]] bool should_be_isolated_node_testing(const agent& node, const int current_time, const int quarantine_days){
  double time_since_tested = current_time - node.test_status.tested_epoch;
 return (node.test_status.state==test_result::positive && (node.infection_status==Progression::exposed || node.infection_status==Progression::infective) &&
   (time_since_tested > 0) &&
   (time_since_tested <= quarantine_days*GLOBAL.SIM_STEPS_PER_DAY));
}


void update_infection_testing(std::vector<agent>& nodes, std::vector<Interaction_Space>& ispaces, count_type current_time, count_type day){
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
				modify_kappa_case_isolate_node(node, ispaces, day);
			}
			else{
					node.disease_label=DiseaseLabel::asymptomatic;
			}
		}
	}
}

void set_test_request_fileread(std::vector<agent>& nodes, std::vector<Interaction_Space>& ispaces,
						 std::vector<testing_probability>& testing_probability_vector, int cur_time, count_type day){
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
  set_test_request(nodes, ispaces, testing_probability_vector[intv_index], cur_time, day);
}