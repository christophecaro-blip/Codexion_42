/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccaro <ccaro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 09:53:41 by ccaro             #+#    #+#             */
/*   Updated: 2026/10/07 09:53:42 by ccaro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	setup_simulation(t_simulation *sim, t_dongle **dongles,
		t_coder **coders, pthread_t **threads)
{
	*dongles = initialize_dongle_table(sim->config);
	if (!*dongles)
		return (0);
	*coders = initialize_table(sim, *dongles);
	if (!*coders)
	{
		free(*dongles);
		return (0);
	}
	sim->coders = *coders;
	*threads = create_threads(sim->config, *coders);
	if (!*threads)
	{
		free(*coders);
		free(*dongles);
		return (0);
	}
	return (1);
}

static void	join_all(t_simulation *sim, pthread_t *threads)
{
	int	i;

	i = 0;
	while (i < sim->config->number_of_coders)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
}

void	run_simulation(t_simulation *sim)
{
	t_dongle	*dongle_table;
	t_coder		*coder_table;
	pthread_t	*thread_tab;
	pthread_t	monitor;

	if (!setup_simulation(sim, &dongle_table, &coder_table, &thread_tab))
		return ;
	pthread_create(&monitor, NULL, monitor_routine, sim);
	join_all(sim, thread_tab);
	pthread_join(monitor, NULL);
	cleanup_simulation(sim, dongle_table, coder_table, thread_tab);
}

int	main(int argc, char **argv)
{
	t_config			config;
	t_simulation		sim;
	pthread_condattr_t	attr;

	if (!parse_args(argc, argv, &config))
		return (1);
	clock_gettime(CLOCK_MONOTONIC, &config.start_time);
	sim.config = &config;
	sim.stop = 0;
	sim.started_count = 0;
	pthread_condattr_init(&attr);
	pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
	pthread_cond_init(&sim.sched_cond, &attr);
	pthread_condattr_destroy(&attr);
	pthread_mutex_init(&sim.log_mutex, NULL);
	pthread_mutex_init(&sim.sched_mutex, NULL);
	run_simulation(&sim);
	return (0);
}
