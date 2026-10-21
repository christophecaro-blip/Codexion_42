/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccaro <ccaro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 09:55:06 by ccaro             #+#    #+#             */
/*   Updated: 2026/10/07 09:55:07 by ccaro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	try_compile(t_coder *coder)
{
	if (!take_dongle(coder))
		return (0);
	log_event(coder, "has taken a dongle");
	log_event(coder, "has taken a dongle");
	log_event(coder, "is compiling");
	usleep(coder->sim->config->time_to_compile * 1000);
	release_dongle(coder);
	coder->state = CODER_DEBUGING;
	return (1);
}

static void	do_debugging(t_coder *coder)
{
	log_event(coder, "is debugging");
	usleep(coder->sim->config->time_to_debug * 1000);
	coder->state = CODER_REFACTORING;
}

static void	do_refactoring(t_coder *coder)
{
	log_event(coder, "is refactoring");
	usleep(coder->sim->config->time_to_refactor * 1000);
	coder->state = CODER_WAITING;
}

static void	wait_for_start(t_simulation *sim)
{
	pthread_mutex_lock(&sim->sched_mutex);
	sim->started_count++;
	if (sim->started_count >= sim->config->number_of_coders)
		pthread_cond_broadcast(&sim->sched_cond);
	else
		while (sim->started_count < sim->config->number_of_coders)
			pthread_cond_wait(&sim->sched_cond, &sim->sched_mutex);
	pthread_mutex_unlock(&sim->sched_mutex);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	wait_for_start(coder->sim);
	while (coder->curr_nb_compil != 0 && !coder->sim->stop)
	{
		if (!try_compile(coder))
			continue ;
		if (coder->sim->stop)
			break ;
		do_debugging(coder);
		if (coder->sim->stop)
			break ;
		do_refactoring(coder);
		if (coder->sim->stop)
			break ;
		pthread_mutex_lock(&coder->sim->sched_mutex);
		coder->curr_nb_compil--;
		pthread_mutex_unlock(&coder->sim->sched_mutex);
	}
	return (NULL);
}
