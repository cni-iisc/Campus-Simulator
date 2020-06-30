//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0
#include "models.h"
#include "testing.h"
#include "intervention_primitives.h"
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
		if(bernoulli(probabilities.prob_contact_trace_household_symptomatic)){
			for(count_type j=0; j<homes[nodes[i].home].individuals.size(); j++){
				nodes[homes[nodes[i].home].individuals[j]].test_status.contact_traced_epoch = current_time; // Do we need to put it insdie the if condition. If not, some people can potentially be in quarantine for a very long time!
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
							nodes[homes[nodes[i].home].individuals[j]].entered_hospitalised_state == false)){
						nodes[homes[nodes[i].home].individuals[j]].test_status.test_requested = nodes[homes[nodes[i].home].individuals[j]].test_status.test_requested || bernoulli(probabilities.prob_test_household_symptomatic_asymptomatic);
					}
				}
			}
		}
		nodes[i].test_status.test_requested = nodes[i].test_status.test_requested || bernoulli(probabilities.prob_test_index_symptomatic);
	}

	if(nodes[i].infection_status == Progression::hospitalised && time_since_hospitalised >0 && time_since_hospitalised<=1){
		if(bernoulli(probabilities.prob_contact_trace_household_hospitalised)){
			for(count_type j=0; j<homes[nodes[i].home].individuals.size(); j++){
				nodes[homes[nodes[i].home].individuals[j]].test_status.contact_traced_epoch = current_time; // Do we need to put it insdie the if condition. If not, some people can potentially be in quarantine for a very long time!
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
							nodes[homes[nodes[i].home].individuals[j]].entered_hospitalised_state == false)){
						nodes[homes[nodes[i].home].individuals[j]].test_status.test_requested = nodes[homes[nodes[i].home].individuals[j]].test_status.test_requested || bernoulli(probabilities.prob_test_household_hospitalised_asymptomatic);
					}
				}
			}
		}
		nodes[i].test_status.test_requested = nodes[i].test_status.test_requested ||bernoulli(probabilities.prob_test_index_hospitalised);
	}

	if(nodes[i].test_status.state == test_result::positive && time_since_tested>0 && time_since_tested<=1){
		if(bernoulli(probabilities.prob_contact_trace_household_positive)){
			for(count_type j=0; j<homes[nodes[i].home].individuals.size(); j++){	
				nodes[homes[nodes[i].home].individuals[j]].test_status.contact_traced_epoch = current_time; // Do we need to put it insdie the if condition. If not, some people can potentially be in quarantine for a very long time!
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
							nodes[homes[nodes[i].home].individuals[j]].entered_hospitalised_state == false)){
						nodes[homes[nodes[i].home].individuals[j]].test_status.test_requested = nodes[homes[nodes[i].home].individuals[j]].test_status.test_requested || bernoulli(probabilities.prob_test_household_positive_asymptomatic);
					}
				}
			}
		}
		nodes[i].test_status.test_requested = false;
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

			prob_contact_trace_symptomatic = probabilities.prob_contact_trace_class_symptomatic;
		    prob_contact_trace_hospitalised = probabilities.prob_contact_trace_class_hospitalised;
		    prob_contact_trace_positive = probabilities.prob_contact_trace_class_positive;
		}


		if(nodes[i].infection_status == Progression::symptomatic && time_since_symptomatic >0 && time_since_symptomatic<=1){
			if(bernoulli(prob_contact_trace_symptomatic)){
				for(count_type j=0; j<workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals.size(); j++){
					nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].test_status.contact_traced_epoch = current_time; // Do we need to put it insdie the if condition. If not, some people can potentially be in quarantine for a very long time!
					if(nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].disease_label == DiseaseLabel::asymptomatic){
						nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].disease_label = DiseaseLabel::primary_contact;
					}
					if(current_time - nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].test_status.tested_epoch > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
						if(nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].infection_status == Progression::symptomatic){
							nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].test_status.test_requested = nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].test_status.test_requested || bernoulli(prob_symptomatic_symptomatic);
						}
						else if(nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].infection_status == Progression::susceptible || 
								nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].infection_status == Progression::exposed ||
								nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].infection_status == Progression::infective ||
								(nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].infection_status == Progression::recovered && 
								nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].entered_hospitalised_state == false)){
							nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].test_status.test_requested = nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].test_status.test_requested || bernoulli(prob_symptomatic_asymptomatic);
						}
					}
				}
			}
			nodes[i].test_status.test_requested = nodes[i].test_status.test_requested || bernoulli(probabilities.prob_test_index_symptomatic);
		}

		if(nodes[i].infection_status == Progression::hospitalised && time_since_hospitalised >0 && time_since_hospitalised<=1){
			if(bernoulli(prob_contact_trace_hospitalised)){
				for(count_type j=0; j<workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals.size(); j++){
					nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].test_status.contact_traced_epoch = current_time; // Do we need to put it insdie the if condition. If not, some people can potentially be in quarantine for a very long time!
					if(nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].disease_label == DiseaseLabel::asymptomatic){
						nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].disease_label = DiseaseLabel::primary_contact;
					}
					if(current_time - nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].test_status.tested_epoch > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
						if(nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].infection_status == Progression::symptomatic){
							nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].test_status.test_requested = nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].test_status.test_requested || bernoulli(prob_hospitalised_symptomatic);
						}
						else if(nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].infection_status == Progression::susceptible || 
								nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].infection_status == Progression::exposed ||
								nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].infection_status == Progression::infective ||
								(nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].infection_status == Progression::recovered && 
								nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].entered_hospitalised_state == false)){
							nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].test_status.test_requested = nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].test_status.test_requested || bernoulli(prob_hospitalised_asymptomatic);
						}				
					}
				}
			}
			nodes[i].test_status.test_requested = nodes[i].test_status.test_requested || bernoulli(probabilities.prob_test_index_hospitalised);
		}

		if(nodes[i].test_status.state == test_result::positive && time_since_tested>0 && time_since_tested<=1){
			if(bernoulli(prob_contact_trace_positive)){
				for(count_type j=0; j<workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals.size(); j++){	
					nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].test_status.contact_traced_epoch = current_time; // Do we need to put it insdie the if condition. If not, some people can potentially be in quarantine for a very long time!
					if(nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].disease_label == DiseaseLabel::asymptomatic){
						nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].disease_label = DiseaseLabel::primary_contact;
					}
					if(current_time - nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].test_status.tested_epoch > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
						if(nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].infection_status == Progression::symptomatic){
							nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].test_status.test_requested = nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].test_status.test_requested || bernoulli(prob_positive_symptomatic);
						}
						else if(nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].infection_status == Progression::susceptible || 
								nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].infection_status == Progression::exposed ||
								nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].infection_status == Progression::infective ||
								(nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].infection_status == Progression::recovered && 
								nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].entered_hospitalised_state == false)){
							nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].test_status.test_requested = nodes[workplaces[nodes[i].workplace].projects[nodes[i].workplace_subnetwork].individuals[j]].test_status.test_requested || bernoulli(prob_positive_asymptomatic);
						}
					}
				}
			}
			nodes[i].test_status.test_requested = false;
		}
	}

// Test people in random community network

	if(nodes[i].infection_status == Progression::symptomatic && time_since_symptomatic >0 && time_since_symptomatic<=1){
		if(bernoulli(probabilities.prob_contact_trace_random_community_symptomatic)){
			for(count_type k=0; k<homes[nodes[i].home].random_households.households.size(); k++){
				for(count_type j=0; j<homes[homes[nodes[i].home].random_households.households[k]].individuals.size(); j++){
					nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].test_status.contact_traced_epoch = current_time; // Do we need to put it insdie the if condition. If not, some people can potentially be in quarantine for a very long time!
					if(nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].disease_label == DiseaseLabel::asymptomatic){
						nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].disease_label = DiseaseLabel::primary_contact;
					}
					if(current_time - nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].test_status.tested_epoch > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
						if(nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].infection_status == Progression::symptomatic){
							nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].test_status.test_requested = nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].test_status.test_requested || bernoulli(probabilities.prob_test_random_community_symptomatic_symptomatic);
						}
						else if(nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].infection_status == Progression::susceptible || 
								nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].infection_status == Progression::exposed ||
								nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].infection_status == Progression::infective ||
								(nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].infection_status == Progression::recovered && 
								nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].entered_hospitalised_state == false)){
							nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].test_status.test_requested = nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].test_status.test_requested || bernoulli(probabilities.prob_test_random_community_symptomatic_asymptomatic);
						}
					}
				}
			}
		}
		nodes[i].test_status.test_requested = nodes[i].test_status.test_requested || bernoulli(probabilities.prob_test_index_symptomatic);
	}

	if(nodes[i].infection_status == Progression::hospitalised && time_since_hospitalised >0 && time_since_hospitalised<=1){
		if(bernoulli(probabilities.prob_contact_trace_random_community_hospitalised)){
			for(count_type k=0; k<homes[nodes[i].home].random_households.households.size(); k++){
				for(count_type j=0; j<homes[homes[nodes[i].home].random_households.households[k]].individuals.size(); j++){
					nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].test_status.contact_traced_epoch = current_time; // Do we need to put it insdie the if condition. If not, some people can potentially be in quarantine for a very long time!
					if(nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].disease_label == DiseaseLabel::asymptomatic){
						nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].disease_label = DiseaseLabel::primary_contact;
					}
					if(current_time - nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].test_status.tested_epoch > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
						if(nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].infection_status == Progression::symptomatic){
							nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].test_status.test_requested = nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].test_status.test_requested || bernoulli(probabilities.prob_test_random_community_hospitalised_symptomatic);
						}
						else if(nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].infection_status == Progression::susceptible || 
								nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].infection_status == Progression::exposed ||
								nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].infection_status == Progression::infective ||
								(nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].infection_status == Progression::recovered && 
								nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].entered_hospitalised_state == false)){
							nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].test_status.test_requested = nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].test_status.test_requested || bernoulli(probabilities.prob_test_random_community_hospitalised_asymptomatic);
						}

					}
				}
			}
		}
		nodes[i].test_status.test_requested = nodes[i].test_status.test_requested || bernoulli(probabilities.prob_test_index_hospitalised);
	}

	if(nodes[i].test_status.state == test_result::positive && time_since_tested>0 && time_since_tested<=1){
		if(bernoulli(probabilities.prob_contact_trace_random_community_positive)){
			for(count_type k=0; k<homes[nodes[i].home].random_households.households.size(); k++){	
				for(count_type j=0; j<homes[homes[nodes[i].home].random_households.households[k]].individuals.size(); j++){
					nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].test_status.contact_traced_epoch = current_time; // Do we need to put it insdie the if condition. If not, some people can potentially be in quarantine for a very long time!
					if(nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].disease_label == DiseaseLabel::asymptomatic){
						nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].disease_label = DiseaseLabel::primary_contact;
					}
					if(current_time - nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].test_status.tested_epoch > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
						if(nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].infection_status == Progression::symptomatic){
							nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].test_status.test_requested = nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].test_status.test_requested || bernoulli(probabilities.prob_test_random_community_positive_symptomatic);
						}
						else if(nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].infection_status == Progression::susceptible || 
								nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].infection_status == Progression::exposed ||
								nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].infection_status == Progression::infective ||
								(nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].infection_status == Progression::recovered && 
								nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].entered_hospitalised_state == false)){
							nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].test_status.test_requested = nodes[homes[homes[nodes[i].home].random_households.households[k]].individuals[j]].test_status.test_requested || bernoulli(probabilities.prob_test_random_community_positive_asymptomatic);
						}
					}
				}
			}
		}
		nodes[i].test_status.test_requested = false;
	}



// Tese people in neighbourhood cell

	grid_cell my_grid_cell = homes[nodes[i].home].neighbourhood;
	nbr_cell my_nbr_cell = nbr_cells[my_grid_cell.cell_x][my_grid_cell.cell_y];
	count_type my_nbr_size = my_nbr_cell.houses_list.size();

	if(nodes[i].infection_status == Progression::symptomatic && time_since_symptomatic >0 && time_since_symptomatic<=1){
		if(bernoulli(probabilities.prob_contact_trace_neighbourhood_symptomatic)){
			for(count_type k=0; k<my_nbr_size; k++){
				for(count_type j=0; j<homes[my_nbr_cell.houses_list[k]].individuals.size(); j++){
					nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].test_status.contact_traced_epoch = current_time; // Do we need to put it insdie the if condition. If not, some people can potentially be in quarantine for a very long time!
					if(nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].disease_label == DiseaseLabel::asymptomatic){
						nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].disease_label = DiseaseLabel::primary_contact;
					}
					if(current_time - nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].test_status.tested_epoch > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
						if(nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].infection_status == Progression::symptomatic){
							nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].test_status.test_requested = nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].test_status.test_requested || bernoulli(probabilities.prob_test_neighbourhood_symptomatic_symptomatic);
						}
						else if(nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].infection_status == Progression::susceptible || 
								nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].infection_status == Progression::exposed ||
								nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].infection_status == Progression::infective ||
								(nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].infection_status == Progression::recovered && 
								nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].entered_hospitalised_state == false)){
							nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].test_status.test_requested = nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].test_status.test_requested || bernoulli(probabilities.prob_test_neighbourhood_symptomatic_asymptomatic);
						}
					}
				}
			}
		}
		nodes[i].test_status.test_requested = nodes[i].test_status.test_requested || bernoulli(probabilities.prob_test_index_symptomatic);
	}

	if(nodes[i].infection_status == Progression::hospitalised && time_since_hospitalised >0 && time_since_hospitalised<=1){
		if(bernoulli(probabilities.prob_contact_trace_neighbourhood_hospitalised)){
			for(count_type k=0; k<my_nbr_size; k++){
				for(count_type j=0; j<homes[my_nbr_cell.houses_list[k]].individuals.size(); j++){
					nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].test_status.contact_traced_epoch = current_time; // Do we need to put it insdie the if condition. If not, some people can potentially be in quarantine for a very long time!
					if(nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].disease_label == DiseaseLabel::asymptomatic){
						nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].disease_label = DiseaseLabel::primary_contact;
					}
					if(current_time - nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].test_status.tested_epoch > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
						if(nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].infection_status == Progression::symptomatic){
							nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].test_status.test_requested = nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].test_status.test_requested || bernoulli(probabilities.prob_test_neighbourhood_hospitalised_symptomatic);
						}
						else if(nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].infection_status == Progression::susceptible || 
								nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].infection_status == Progression::exposed ||
								nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].infection_status == Progression::infective ||
								(nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].infection_status == Progression::recovered && 
								nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].entered_hospitalised_state == false)){
							nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].test_status.test_requested = nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].test_status.test_requested || bernoulli(probabilities.prob_test_neighbourhood_hospitalised_asymptomatic);
						}
					}
				}
			}
		}
		nodes[i].test_status.test_requested = nodes[i].test_status.test_requested || bernoulli(probabilities.prob_test_index_hospitalised);
	}

	if(nodes[i].test_status.state == test_result::positive && time_since_tested>0 && time_since_tested<=1){
		if(bernoulli(probabilities.prob_contact_trace_neighbourhood_positive)){
			for(count_type k=0; k<my_nbr_size; k++){
				for(count_type j=0; j<homes[my_nbr_cell.houses_list[k]].individuals.size(); j++){
					nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].test_status.contact_traced_epoch = current_time; // Do we need to put it insdie the if condition. If not, some people can potentially be in quarantine for a very long time!
					if(nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].disease_label == DiseaseLabel::asymptomatic){
						nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].disease_label = DiseaseLabel::primary_contact;
					}
					if(current_time - nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].test_status.tested_epoch > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
						if(nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].infection_status == Progression::symptomatic){
							nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].test_status.test_requested = nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].test_status.test_requested || bernoulli(probabilities.prob_test_neighbourhood_positive_symptomatic);
						}
						else if(nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].infection_status == Progression::susceptible || 
								nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].infection_status == Progression::exposed ||
								nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].infection_status == Progression::infective ||
								(nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].infection_status == Progression::recovered && 
								nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].entered_hospitalised_state == false)){
							nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].test_status.test_requested = nodes[homes[my_nbr_cell.houses_list[k]].individuals[j]].test_status.test_requested || bernoulli(probabilities.prob_test_neighbourhood_positive_asymptomatic);
						}
					}
				}
			}
		}
		nodes[i].test_status.test_requested = false;
	}
					



// Re-test if somebody is recovered
	if(nodes[i].test_status.state==test_result::positive && nodes[i].infection_status==Progression::recovered){
		nodes[i].test_status.test_requested=bernoulli(probabilities.prob_retest_recovered);
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


