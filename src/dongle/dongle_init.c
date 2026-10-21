/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_init.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccaro <ccaro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 09:55:01 by ccaro             #+#    #+#             */
/*   Updated: 2026/10/07 10:02:29 by ccaro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_dongle	initialize_dongle(void)
{
	t_dongle		dongle;

	pthread_mutex_init(&dongle.mutex, NULL);
	dongle.is_taken = false;
	dongle.available_at = 0;
	dongle.wait_queue = initialize_heap(2);
	dongle.favor_even = true;
	return (dongle);
}

t_dongle	*initialize_dongle_table(const t_config	*config)
{
	t_dongle	*table;
	int			i;

	table = malloc(config->number_of_coders * (sizeof(t_dongle)));
	if (!table)
		return (NULL);
	i = 0;
	while (i < config->number_of_coders)
	{
		table[i] = initialize_dongle();
		i++;
	}
	return (table);
}
