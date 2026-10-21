/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fifo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccaro <ccaro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 09:54:24 by ccaro             #+#    #+#             */
/*   Updated: 2026/10/07 09:54:25 by ccaro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	fifo_key(t_coder *coder, t_dongle *dongle)
{
	long	now;
	int		matches_favor;

	now = get_timestamp(coder->sim->config->start_time);
	matches_favor = ((coder->id % 2 == 0) == dongle->favor_even);
	return (now * 2 + !matches_favor);
}
