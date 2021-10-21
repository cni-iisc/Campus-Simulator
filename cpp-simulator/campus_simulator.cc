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
	if(GLOBAL.restart == 1){
		initial_batch(nodes);
	}
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

	plot_data.susceptible_lambdas =
		{
			{"susceptible_lambda", {}},
			{"susceptible_lambda_hostel", {}},
			{"susceptible_lambda_classroom", {}},
			{"susceptible_lambda_mess", {}},
			{"susceptible_lambda_cafeteria", {}},
			{"susceptible_lambda_library", {}},
			{"susceptible_lambda_sports_facility", {}},
			{"susceptible_lambda_recreational_facility", {}},
			{"susceptible_lambda_residential_block", {}},
			{"susceptible_lambda_house", {}},
			{"susceptible_lambda_smaller_networks", {}}};

	plot_data.total_lambda_fractions =
		{
			{"total_fraction_lambda_hostel", {}},
			{"total_fraction_lambda_classroom", {}},
			{"total_fraction_lambda_mess", {}},
			{"total_fraction_lambda_cafeteria", {}},
			{"total_fraction_lambda_library", {}},
			{"total_fraction_lambda_sports_facility", {}},
			{"total_fraction_lambda_recreational_facility", {}},
			{"total_fraction_lambda_residential_block", {}},
			{"total_fraction_lambda_house", {}},
			{"total_fraction_lambda_smaller_networks", {}}};

	plot_data.mean_lambda_fractions =
		{
			{"mean_fraction_lambda_hostel", {}},
			{"mean_fraction_lambda_classroom", {}},
			{"mean_fraction_lambda_mess", {}},
			{"mean_fraction_lambda_cafeteria", {}},
			{"mean_fraction_lambda_library", {}},
			{"mean_fraction_lambda_sports_facility", {}},
			{"mean_fraction_lambda_recreational_facility", {}},
			{"mean_fraction_lambda_residential_block", {}},
			{"mean_fraction_lambda_house", {}},
			{"mean_fraction_lambda_smaller_networks", {}}};

	plot_data.cumulative_mean_lambda_fractions =
		{
			{"cumulative_mean_fraction_lambda_hostel", {}},
			{"cumulative_mean_fraction_lambda_classroom", {}},
			{"cumulative_mean_fraction_lambda_mess", {}},
			{"cumulative_mean_fraction_lambda_cafeteria", {}},
			{"cumulative_mean_fraction_lambda_library", {}},
			{"cumulative_mean_fraction_lambda_sports_facility", {}},
			{"cumulative_mean_fraction_lambda_recreational_facility", {}},
			{"cumulative_mean_fraction_lambda_residential_block", {}},
			{"cumulative_mean_fraction_lambda_house", {}},
			{"cumulative_mean_fraction_lambda_smaller_networks", {}}};

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

	lambda_incoming_data total_lambda_fraction_data;
	lambda_incoming_data mean_lambda_fraction_data;
	lambda_incoming_data cumulative_mean_lambda_fraction_data;
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

		total_lambda_fraction_data.set_zero();
		mean_lambda_fraction_data.set_zero();

		count_type num_new_infections = 0;
		for (count_type j = 0; j < nodes.size(); ++j)
		{	
			auto node_update_status = update_infection(nodes[j], time_step, day);
			nodes[j].psi_T = psi_T(nodes[j], time_step);
			if(true){
				if (node_update_status.new_infection)
				{
					++num_new_infections;
					++num_total_infections;

					{
						total_lambda_fraction_data += nodes[j].lambda_incoming;
						auto normalized_lambda = (nodes[j].lambda_incoming / nodes[j].lambda);
						mean_lambda_fraction_data.mean_update(normalized_lambda, num_new_infections);
						cumulative_mean_lambda_fraction_data.mean_update(normalized_lambda, num_total_infections);
					}
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
			
		}
		//random_time_allocation(nodes, interaction_spaces, day);
		cafeteria_active_duration(nodes, interaction_spaces, day);
		library_active_duration(nodes, interaction_spaces, day);
		recreational_facility_active_duration(nodes, interaction_spaces, day);
		sports_facility_active_duration(nodes, interaction_spaces, day);
		if(GLOBAL.restart == 1 && time_step > 0 && time_step%(GLOBAL.restart_batch_frequency*4) == 0){
			subsequent_batches(nodes);
		}
		update_interaction_space_lambda(nodes, interaction_spaces, day);
		update_individual_lambda(nodes, interaction_spaces, day);
		update_all_kappa(nodes, interaction_spaces, intv_params, time_step, day);
		if (GLOBAL.ENABLE_TESTING)
		{
			update_test_status(nodes, time_step);
			update_infection_testing(nodes, interaction_spaces, time_step, day);
			update_test_request(nodes, interaction_spaces, time_step, testing_protocol_file_read, day);
		}
		//random_time_reset(nodes, interaction_spaces, day);
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

		double susceptible_lambda = 0,
			   susceptible_lambda_classroom = 0,
			   susceptible_lambda_hostel = 0,
			   susceptible_lambda_mess = 0,
			   susceptible_lambda_cafeteria = 0,
			   susceptible_lambda_library = 0,
			   susceptible_lambda_sports_facility = 0,
			   susceptible_lambda_recreational_facility = 0,
			   susceptible_lambda_residential_block = 0,
			   susceptible_lambda_house = 0,
			   susceptible_lambda_smaller_networks = 0;

		for (count_type j = 0; j < nodes.size(); j++)
		{

			auto infection_status = nodes[j].infection_status;

			if(true){
				if (infection_status == Progression::susceptible)
				{
					susceptible_lambda += nodes[j].lambda;
					susceptible_lambda_classroom += nodes[j].lambda_incoming.classroom;
					susceptible_lambda_hostel += nodes[j].lambda_incoming.hostel;
					susceptible_lambda_mess += nodes[j].lambda_incoming.mess;
					susceptible_lambda_cafeteria += nodes[j].lambda_incoming.cafeteria;
					susceptible_lambda_library += nodes[j].lambda_incoming.library;
					susceptible_lambda_sports_facility += nodes[j].lambda_incoming.sports_facility;
					susceptible_lambda_recreational_facility += nodes[j].lambda_incoming.recreational_facility;
					susceptible_lambda_residential_block += nodes[j].lambda_incoming.residential_block;
					susceptible_lambda_house += nodes[j].lambda_incoming.house;
					susceptible_lambda_smaller_networks += nodes[j].lambda_incoming.smaller_networks;
				}

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

		plot_data.susceptible_lambdas["susceptible_lambda"].push_back({time_step, {susceptible_lambda}});
		plot_data.susceptible_lambdas["susceptible_lambda_classroom"].push_back({time_step, {susceptible_lambda_classroom}});
		plot_data.susceptible_lambdas["susceptible_lambda_hostel"].push_back({time_step, {susceptible_lambda_hostel}});
		plot_data.susceptible_lambdas["susceptible_lambda_mess"].push_back({time_step, {susceptible_lambda_mess}});
		plot_data.susceptible_lambdas["susceptible_lambda_cafeteria"].push_back({time_step, {susceptible_lambda_cafeteria}});
		plot_data.susceptible_lambdas["susceptible_lambda_library"].push_back({time_step, {susceptible_lambda_library}});
		plot_data.susceptible_lambdas["susceptible_lambda_sports_facility"].push_back({time_step, {susceptible_lambda_sports_facility}});
		plot_data.susceptible_lambdas["susceptible_lambda_recreational_facility"].push_back({time_step, {susceptible_lambda_recreational_facility}});
		plot_data.susceptible_lambdas["susceptible_lambda_residential_block"].push_back({time_step, {susceptible_lambda_residential_block}});
		plot_data.susceptible_lambdas["susceptible_lambda_house"].push_back({time_step, {susceptible_lambda_house}});
		plot_data.susceptible_lambdas["susceptible_lambda_smaller_networks"].push_back({time_step, {susceptible_lambda_smaller_networks}});	


		auto total_lambda_fraction_data_sum = total_lambda_fraction_data.sum();
		total_lambda_fraction_data /= total_lambda_fraction_data_sum;

		plot_data.total_lambda_fractions["total_fraction_lambda_classroom"].push_back({time_step, {total_lambda_fraction_data.classroom}});
		plot_data.total_lambda_fractions["total_fraction_lambda_hostel"].push_back({time_step, {total_lambda_fraction_data.hostel}});
		plot_data.total_lambda_fractions["total_fraction_lambda_mess"].push_back({time_step, {total_lambda_fraction_data.mess}});
		plot_data.total_lambda_fractions["total_fraction_lambda_cafeteria"].push_back({time_step, {total_lambda_fraction_data.cafeteria}});
		plot_data.total_lambda_fractions["total_fraction_lambda_library"].push_back({time_step, {total_lambda_fraction_data.library}});
		plot_data.total_lambda_fractions["total_fraction_lambda_sports_facility"].push_back({time_step, {total_lambda_fraction_data.sports_facility}});
		plot_data.total_lambda_fractions["total_fraction_lambda_recreational_facility"].push_back({time_step, {total_lambda_fraction_data.recreational_facility}});
		plot_data.total_lambda_fractions["total_fraction_lambda_residential_block"].push_back({time_step, {total_lambda_fraction_data.residential_block}});
		plot_data.total_lambda_fractions["total_fraction_lambda_house"].push_back({time_step, {total_lambda_fraction_data.house}});
		plot_data.total_lambda_fractions["total_fraction_lambda_smaller_networks"].push_back({time_step, {total_lambda_fraction_data.smaller_networks}});

		plot_data.mean_lambda_fractions["mean_fraction_lambda_classroom"].push_back({time_step, {mean_lambda_fraction_data.classroom}});
		plot_data.mean_lambda_fractions["mean_fraction_lambda_hostel"].push_back({time_step, {mean_lambda_fraction_data.hostel}});
		plot_data.mean_lambda_fractions["mean_fraction_lambda_mess"].push_back({time_step, {mean_lambda_fraction_data.mess}});
		plot_data.mean_lambda_fractions["mean_fraction_lambda_cafeteria"].push_back({time_step, {mean_lambda_fraction_data.cafeteria}});
		plot_data.mean_lambda_fractions["mean_fraction_lambda_library"].push_back({time_step, {mean_lambda_fraction_data.library}});
		plot_data.mean_lambda_fractions["mean_fraction_lambda_sports_facility"].push_back({time_step, {mean_lambda_fraction_data.sports_facility}});
		plot_data.mean_lambda_fractions["mean_fraction_lambda_recreational_facility"].push_back({time_step, {mean_lambda_fraction_data.recreational_facility}});
		plot_data.mean_lambda_fractions["mean_fraction_lambda_residential_block"].push_back({time_step, {mean_lambda_fraction_data.residential_block}});
		plot_data.mean_lambda_fractions["mean_fraction_lambda_house"].push_back({time_step, {mean_lambda_fraction_data.house}});
		plot_data.mean_lambda_fractions["mean_fraction_lambda_smaller_networks"].push_back({time_step, {mean_lambda_fraction_data.smaller_networks}});

		plot_data.cumulative_mean_lambda_fractions["cumulative_mean_fraction_lambda_classroom"].push_back({time_step, {cumulative_mean_lambda_fraction_data.classroom}});
		plot_data.cumulative_mean_lambda_fractions["cumulative_mean_fraction_lambda_hostel"].push_back({time_step, {cumulative_mean_lambda_fraction_data.hostel}});
		plot_data.cumulative_mean_lambda_fractions["cumulative_mean_fraction_lambda_mess"].push_back({time_step, {cumulative_mean_lambda_fraction_data.mess}});
		plot_data.cumulative_mean_lambda_fractions["cumulative_mean_fraction_lambda_cafeteria"].push_back({time_step, {cumulative_mean_lambda_fraction_data.cafeteria}});
		plot_data.cumulative_mean_lambda_fractions["cumulative_mean_fraction_lambda_library"].push_back({time_step, {cumulative_mean_lambda_fraction_data.library}});
		plot_data.cumulative_mean_lambda_fractions["cumulative_mean_fraction_lambda_sports_facility"].push_back({time_step, {cumulative_mean_lambda_fraction_data.sports_facility}});
		plot_data.cumulative_mean_lambda_fractions["cumulative_mean_fraction_lambda_recreational_facility"].push_back({time_step, {cumulative_mean_lambda_fraction_data.recreational_facility}});
		plot_data.cumulative_mean_lambda_fractions["cumulative_mean_fraction_lambda_residential_block"].push_back({time_step, {cumulative_mean_lambda_fraction_data.residential_block}});
		plot_data.cumulative_mean_lambda_fractions["cumulative_mean_fraction_lambda_house"].push_back({time_step, {cumulative_mean_lambda_fraction_data.house}});
		plot_data.cumulative_mean_lambda_fractions["cumulative_mean_fraction_lambda_smaller_networks"].push_back({time_step, {cumulative_mean_lambda_fraction_data.smaller_networks}});
	}
	return plot_data;
}
