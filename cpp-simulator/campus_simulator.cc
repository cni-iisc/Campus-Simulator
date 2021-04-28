#include <vector>
#include <algorithm>
#include <map>
#include <string>
//#include "models.h"
//#include "initializers.h"
//#include "updates.h"
//#include "simulator.h"
//#include "testing.h"
#include "models.h"
#include "campus_updates.h"
#include "campus_simulator.h"
#include "campus_interventions.h"
#include "campus_testing.h"
//#include "outputs.h"

plot_data_struct run_campus_simulator()
{
#ifdef TIMING
	std::cerr << "simulator: starting JSON read\n";
	//auto start_time = std::chrono::high_resolution_clock::now();
#endif
	init_config_params();
	auto interaction_spaces = init_interaction_spaces();
	init_transmission_coefficients(interaction_spaces);
	auto nodes = init_nodes_campus();
	int day = 0;
	auto intv_params = init_intervention_params();
	auto testing_protocol_file_read = init_testing_protocol();
	init_contact_tracing_hierarchy();
	assign_individual_campus(nodes, interaction_spaces);
	sample_groups(nodes, interaction_spaces);

	//This needs to be done after the initilization.
	plot_data_struct plot_data;
	plot_data.nums =
		{
			{"num_infected", {}},
			{"num_exposed", {}},
			{"num_hospitalised", {}},
			{"num_symptomatic", {}},
			{"num_critical", {}},
			{"num_fatalities", {}},
			{"num_recovered", {}},
			{"num_affected", {}},
			{"num_cases", {}},
			{"num_cumulative_hospitalizations", {}},
			{"num_cumulative_infective", {}}};
	for (auto &elem : plot_data.nums)
	{
		elem.second.reserve(GLOBAL.NUM_TIMESTEPS);
	}

	plot_data.disease_label_stats =
		{
			{"disease_label_stats", {}},
		};

	count_type num_cases = 0; // Total number of agents who have progessed to symptomatic so far
	count_type quarantined_num_cases = 0;
	count_type num_cumulative_hospitalizations = 0; //Total number of agents who have had to go to the hospital so far
	count_type num_cumulative_infective = 0;		//Total number of people who have progressed to the infective state so far

	count_type num_total_infections = 0;
	//Total number of individuals who have become infected via transmission so far
	//This does not included the initially seeded infections

	//   std::vector<long double> infections_by_new_infectives(GLOBAL.NUM_TIMESTEPS, 0);
	//For keeping track of infections ascribed to agents that became infective at
	//each time

	// const auto NUM_PEOPLE = GLOBAL.num_people;

	int time_step = 0;

	for (time_step = 0; time_step < GLOBAL.NUM_TIMESTEPS; ++time_step)
	{
		//TODO: Call update_interaction_spaces_lambda
		//update_interaction_space_lambda();
		//TODO: Call update_individuals_lambda
		//update_individual_lambda();
#ifdef DEBUG
		auto start_time_timestep = std::chrono::high_resolution_clock::now();
#endif
		day = (time_step / 4) % GLOBAL.PERIODICITY;
		count_type num_new_infections = 0;
		for (count_type j = 0; j < nodes.size(); ++j)
		{	
			auto node_update_status = update_infection(nodes[j], time_step, day);
			nodes[j].psi_T = psi_T(nodes[j], time_step);
			if (node_update_status.new_infection)
			{
				++num_new_infections;
				++num_total_infections;
			}
			if (node_update_status.new_symptomatic)
			{
				++num_cases;
			}
			if (node_update_status.new_symptomatic && nodes[j].quarantined)
			{
				++quarantined_num_cases;
			}
			if (node_update_status.new_hospitalization)
			{
				++num_cumulative_hospitalizations;
			}
			if (node_update_status.new_infective)
			{
				++num_cumulative_infective;
			}
		}
		cafeteria_active_duration(nodes, interaction_spaces, day);
		library_active_duration(nodes, interaction_spaces, day);
		recreational_facility_active_duration(nodes, interaction_spaces, day);
		sports_facility_active_duration(nodes, interaction_spaces, day);
		update_interaction_space_lambda(nodes, interaction_spaces, day);
		update_individual_lambda(nodes, interaction_spaces, day);
		update_all_kappa(nodes, interaction_spaces, intv_params, time_step, day);
		if (GLOBAL.ENABLE_TESTING)
		{
			update_test_status(nodes, time_step);
			update_infection_testing(nodes, interaction_spaces, time_step, day);
			update_test_request(nodes, interaction_spaces, time_step, testing_protocol_file_read, day);
		}
		cafeteria_reset(nodes, interaction_spaces, day);
		library_reset(nodes, interaction_spaces, day);
		recreational_facility_reset(nodes, interaction_spaces, day);
		sports_facility_reset(nodes, interaction_spaces, day);
		count_type n_infected = 0,
				   n_exposed = 0,
				   n_hospitalised = 0,
				   n_symptomatic = 0,
				   n_critical = 0,
				   n_fatalities = 0,
				   n_recovered = 0,
				   n_affected = 0,
				   n_infective = 0,
				   quarantined_individuals = 0,
				   quarantined_infectious = 0,
				   n_test_done = 0;

		count_type n_primary_contact = 0,
				   n_mild_symptomatic_tested = 0, //CCC2
				   n_moderate_symptomatic_tested = 0,	  //DCHC
				   n_severe_symptomatic_tested = 0,	  //DCH
				   n_icu = 0,
				   n_requested_tests = 0,
				   n_tested_positive = 0;
		for (count_type j = 0; j < nodes.size(); j++)
		{
			auto infection_status = nodes[j].infection_status;
			if (infection_status == Progression::infective || infection_status == Progression::symptomatic || infection_status == Progression::hospitalised || infection_status == Progression::critical)
			{
				n_infected += 1;
			}
			if (infection_status == Progression::exposed)
			{
				n_exposed += 1;
			}
			if (infection_status == Progression::hospitalised)
			{
				n_hospitalised += 1;
			}
			if (infection_status == Progression::symptomatic)
			{
				n_symptomatic += 1;
			}
			if (infection_status == Progression::critical)
			{
				n_critical += 1;
			}
			if (infection_status == Progression::dead)
			{
				n_fatalities += 1;
			}
			if (nodes[j].test_status.test_requested)
			{
				n_requested_tests ++;
			}
			if (infection_status == Progression::recovered)
			{
				n_recovered += 1;
			}
			if (infection_status != Progression::susceptible)
			{
				n_affected += 1;
			}
			if (nodes[j].infective)
			{
				n_infective += 1;
			}
			if (nodes[j].quarantined)
			{
				quarantined_individuals += 1;
			}
			if (nodes[j].quarantined && (infection_status == Progression::infective || infection_status == Progression::symptomatic || infection_status == Progression::hospitalised || infection_status == Progression::critical))
			{
				quarantined_infectious += 1;
			}
			if (nodes[j].disease_label == DiseaseLabel::primary_contact)
			{
				n_primary_contact += 1;
			}
			if (nodes[j].disease_label == DiseaseLabel::mild_symptomatic_tested)
			{
				n_mild_symptomatic_tested += 1;
			}
			if (nodes[j].disease_label == DiseaseLabel::moderate_symptomatic_tested)
			{
				n_moderate_symptomatic_tested += 1;
			}
			if (nodes[j].disease_label == DiseaseLabel::severe_symptomatic_tested)
			{
				n_severe_symptomatic_tested += 1;
			}
			if (nodes[j].disease_label == DiseaseLabel::icu)
			{
				n_icu += 1;
			}
			if (nodes[j].test_status.tested_positive)
			{
				n_tested_positive += 1;
			}
			if (nodes[j].test_status.test_done)
			{
				n_test_done += 1;
			}
		}
		plot_data.nums["num_infected"].push_back({time_step, {n_infected}});
		plot_data.nums["num_exposed"].push_back({time_step, {n_exposed}});
		plot_data.nums["num_hospitalised"].push_back({time_step, {n_hospitalised}});
		plot_data.nums["num_symptomatic"].push_back({time_step, {n_symptomatic}});
		plot_data.nums["num_critical"].push_back({time_step, {n_critical}});
		plot_data.nums["num_fatalities"].push_back({time_step, {n_fatalities}});
		plot_data.nums["num_recovered"].push_back({time_step, {n_recovered}});
		plot_data.nums["num_affected"].push_back({time_step, {n_affected}});
		plot_data.nums["num_cases"].push_back({time_step, {num_cases}});
		plot_data.nums["num_cumulative_hospitalizations"].push_back({time_step, {num_cumulative_hospitalizations}});
		plot_data.nums["num_cumulative_infective"].push_back({time_step, {num_cumulative_infective}});
		plot_data.disease_label_stats["disease_label_stats"].push_back({time_step, {n_primary_contact, n_mild_symptomatic_tested, n_moderate_symptomatic_tested, n_severe_symptomatic_tested, n_icu, n_requested_tests, n_tested_positive, n_test_done}});
	}
	return plot_data;
}
