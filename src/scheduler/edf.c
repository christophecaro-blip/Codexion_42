/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   edf.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccaro <ccaro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 09:54:30 by ccaro             #+#    #+#             */
/*   Updated: 2026/10/07 09:54:31 by ccaro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	edf_key(t_coder *coder, t_dongle *dongle)
{
	int	matches_favor;

	matches_favor = ((coder->id % 2 == 0) == dongle->favor_even);
	return (coder->burnout_at * 2 + !matches_favor);
}
