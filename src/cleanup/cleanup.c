/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccaro <ccaro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 15:33:08 by ccaro             #+#    #+#             */
/*   Updated: 2026/10/15 15:33:09 by ccaro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	destroy_heap(t_heap *heap)
{
	free(heap->array);
	free(heap);
}

void	destroy_dongle_table(t_dongle *table, int n)
{
	int	i;

	i = 0;
	while (i < n)
	{
		pthread_mutex_destroy(&table[i].mutex);
		destroy_heap(table[i].wait_queue);
		i++;
	}
}

void	cleanup_simulation(t_simulation *sim, t_dongle *dongles,
		t_coder *coders, pthread_t *threads)
{
	destroy_dongle_table(dongles, sim->config->number_of_coders);
	free(dongles);
	free(coders);
	free(threads);
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->sched_mutex);
	pthread_cond_destroy(&sim->sched_cond);
}
