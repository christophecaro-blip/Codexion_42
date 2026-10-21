/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ready.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccaro <ccaro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 09:54:09 by ccaro             #+#    #+#             */
/*   Updated: 2026/10/07 10:02:58 by ccaro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	dongle_available(t_dongle *dongle, long now)
{
	return (!dongle->is_taken && now >= dongle->available_at);
}

static int	is_front(t_heap *heap, t_coder *coder)
{
	return (heap->size > 0 && heap->array[0].coder == coder);
}

int	is_ready(t_coder *coder)
{
	long	now;
	int		ready;

	if (coder->left_dongle == coder->right_dongle)
		return (0);
	now = get_timestamp(coder->sim->config->start_time);
	lock_dongles(coder);
	ready = dongle_available(coder->left_dongle, now)
		&& dongle_available(coder->right_dongle, now)
		&& is_front(coder->left_dongle->wait_queue, coder)
		&& is_front(coder->right_dongle->wait_queue, coder);
	unlock_dongles(coder);
	return (ready);
}

static long	scheduling_key(t_coder *coder, t_dongle *dongle)
{
	if (coder->sim->config->scheduler == SCHEDULER_FIFO)
		return (fifo_key(coder, dongle));
	return (edf_key(coder, dongle));
}

void	register_wait(t_coder *coder)
{
	if (coder->left_dongle == coder->right_dongle)
		return ;
	pthread_mutex_lock(&coder->left_dongle->mutex);
	insert(coder->left_dongle->wait_queue,
		scheduling_key(coder, coder->left_dongle), coder);
	pthread_mutex_unlock(&coder->left_dongle->mutex);
	pthread_mutex_lock(&coder->right_dongle->mutex);
	insert(coder->right_dongle->wait_queue,
		scheduling_key(coder, coder->right_dongle), coder);
	pthread_mutex_unlock(&coder->right_dongle->mutex);
}
