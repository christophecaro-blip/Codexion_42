/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_init.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccaro <ccaro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 09:55:08 by ccaro             #+#    #+#             */
/*   Updated: 2026/10/07 10:02:06 by ccaro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_coder	initialize_coder(t_simulation *sim, t_dongle *dongle, int id)
{
	t_coder	coder;

	coder.id = id;
	coder.sim = sim;
	coder.burnout_at = sim->config->time_to_burnout;
	coder.right_dongle = &dongle[id];
	if (id == 0)
		coder.left_dongle = &dongle[sim->config->number_of_coders - 1];
	else
		coder.left_dongle = &dongle[id - 1];
	coder.state = CODER_WAITING;
	coder.curr_nb_compil = sim->config->number_of_compiles_required;
	return (coder);
}

t_coder	*initialize_table(t_simulation	*sim, t_dongle	*dongle)
{
	t_coder		*table;
	int			i;

	table = malloc(sim->config->number_of_coders * (sizeof(t_coder)));
	if (!table)
		return (NULL);
	i = 0;
	while (i < sim->config->number_of_coders)
	{
		table[i] = initialize_coder(sim, dongle, i);
		i++;
	}
	return (table);
}

pthread_t	*create_threads(const t_config	*config, t_coder	*coder_tab)
{
	pthread_t	*thread_tab;
	int			i;

	thread_tab = malloc(config->number_of_coders * (sizeof(pthread_t)));
	if (!thread_tab)
		return (NULL);
	i = 0;
	while (i < config->number_of_coders)
	{
		if (pthread_create(&thread_tab[i], NULL, coder_routine, &coder_tab[i]))
		{
			while (--i >= 0)
				pthread_join(thread_tab[i], NULL);
			free(thread_tab);
			return (NULL);
		}
		i++;
	}
	return (thread_tab);
}
