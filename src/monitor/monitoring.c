/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitoring.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccaro <ccaro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 09:54:49 by ccaro             #+#    #+#             */
/*   Updated: 2026/10/07 09:54:50 by ccaro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	is_burnout(t_coder *coder, long now)
{
	return (now >= coder->burnout_at);
}

static void	stop_simulation(t_simulation *sim)
{
	sim->stop = 1;
	pthread_cond_broadcast(&sim->sched_cond);
}

static int	scan_coders(t_simulation *sim, long now, long *earliest)
{
	int	finished;
	int	i;

	finished = 1;
	*earliest = -1;
	i = 0;
	while (i < sim->config->number_of_coders)
	{
		if (sim->coders[i].curr_nb_compil != 0)
		{
			finished = 0;
			if (is_burnout(&sim->coders[i], now))
			{
				log_event(&sim->coders[i], "\033[31mburned out\033[0m");
				return (-1);
			}
			if (*earliest == -1 || sim->coders[i].burnout_at < *earliest)
				*earliest = sim->coders[i].burnout_at;
		}
		i++;
	}
	return (finished);
}

void	*monitor_routine(void *arg)
{
	t_simulation	*sim;
	long			now;
	long			earliest;
	int				status;
	struct timespec	deadline;

	sim = (t_simulation *)arg;
	pthread_mutex_lock(&sim->sched_mutex);
	while (1)
	{
		now = get_timestamp(sim->config->start_time);
		status = scan_coders(sim, now, &earliest);
		if (status != 0)
		{
			stop_simulation(sim);
			pthread_mutex_unlock(&sim->sched_mutex);
			return (NULL);
		}
		deadline = ms_to_deadline(sim->config->start_time, earliest);
		pthread_cond_timedwait(&sim->sched_cond, &sim->sched_mutex, &deadline);
	}
}
