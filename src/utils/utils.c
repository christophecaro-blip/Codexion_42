/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccaro <ccaro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 09:53:50 by ccaro             #+#    #+#             */
/*   Updated: 2026/10/07 10:03:12 by ccaro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

struct timespec	ms_to_deadline(struct timespec start, long ms)
{
	struct timespec	deadline;

	deadline.tv_sec = start.tv_sec + ms / 1000;
	deadline.tv_nsec = start.tv_nsec + (ms % 1000) * 1000000;
	if (deadline.tv_nsec >= 1000000000)
	{
		deadline.tv_sec += 1;
		deadline.tv_nsec -= 1000000000;
	}
	return (deadline);
}

long	get_timestamp(struct timespec start)
{
	struct timespec	now;

	clock_gettime(CLOCK_MONOTONIC, &now);
	return ((now.tv_sec - start.tv_sec) * 1000
		+ (now.tv_nsec - start.tv_nsec) / 1000000);
}
