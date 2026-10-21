/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccaro <ccaro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 09:54:53 by ccaro             #+#    #+#             */
/*   Updated: 2026/10/07 09:54:54 by ccaro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	log_event(t_coder	*coder, const char	*message)
{
	long	now;

	now = get_timestamp(coder->sim->config->start_time);
	pthread_mutex_lock(&coder->sim->log_mutex);
	printf("%ld %ld %s\n", now, coder->id + 1, message);
	pthread_mutex_unlock(&coder->sim->log_mutex);
}
