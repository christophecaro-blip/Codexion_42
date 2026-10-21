/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccaro <ccaro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 09:55:11 by ccaro             #+#    #+#             */
/*   Updated: 2026/10/07 10:00:53 by ccaro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	lock_dongles(t_coder *coder)
{
	if (coder->left_dongle < coder->right_dongle)
	{
		pthread_mutex_lock(&coder->left_dongle->mutex);
		pthread_mutex_lock(&coder->right_dongle->mutex);
	}
	else
	{
		pthread_mutex_lock(&coder->right_dongle->mutex);
		pthread_mutex_lock(&coder->left_dongle->mutex);
	}
}

void	unlock_dongles(t_coder *coder)
{
	pthread_mutex_unlock(&coder->left_dongle->mutex);
	pthread_mutex_unlock(&coder->right_dongle->mutex);
}

static void	wait_for_dongles(t_coder *coder)
{
	long			now;
	long			wait_until;
	struct timespec	deadline;

	while (!is_ready(coder) && !coder->sim->stop)
	{
		now = get_timestamp(coder->sim->config->start_time);
		wait_until = coder->left_dongle->available_at;
		if (coder->right_dongle->available_at > wait_until)
			wait_until = coder->right_dongle->available_at;
		if (wait_until > now)
		{
			deadline = ms_to_deadline(coder->sim
					->config->start_time, wait_until);
			pthread_cond_timedwait(&coder->sim->sched_cond,
				&coder->sim->sched_mutex, &deadline);
		}
		else
			pthread_cond_wait(&coder->sim
				->sched_cond, &coder->sim->sched_mutex);
	}
}

int	take_dongle(t_coder *coder)
{
	t_heap_entry	trash;
	long			now;

	register_wait(coder);
	pthread_mutex_lock(&coder->sim->sched_mutex);
	wait_for_dongles(coder);
	now = get_timestamp(coder->sim->config->start_time);
	if (coder->sim->stop)
	{
		pthread_mutex_unlock(&coder->sim->sched_mutex);
		return (0);
	}
	lock_dongles(coder);
	extract_min(coder->left_dongle->wait_queue, &trash);
	extract_min(coder->right_dongle->wait_queue, &trash);
	coder->left_dongle->is_taken = true;
	coder->right_dongle->is_taken = true;
	coder->left_dongle->favor_even = !coder->left_dongle->favor_even;
	coder->right_dongle->favor_even = !coder->right_dongle->favor_even;
	unlock_dongles(coder);
	coder->burnout_at = now + coder->sim->config->time_to_burnout;
	pthread_mutex_unlock(&coder->sim->sched_mutex);
	return (1);
}

void	release_dongle(t_coder *coder)
{
	long	now;
	long	cooldown;

	pthread_mutex_lock(&coder->sim->sched_mutex);
	lock_dongles(coder);
	now = get_timestamp(coder->sim->config->start_time);
	cooldown = coder->sim->config->dongle_cooldown;
	coder->left_dongle->is_taken = false;
	coder->right_dongle->is_taken = false;
	coder->left_dongle->available_at = now + cooldown;
	coder->right_dongle->available_at = now + cooldown;
	unlock_dongles(coder);
	pthread_cond_broadcast(&coder->sim->sched_cond);
	pthread_mutex_unlock(&coder->sim->sched_mutex);
}
