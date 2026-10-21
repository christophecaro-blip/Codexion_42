/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccaro <ccaro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 09:54:38 by ccaro             #+#    #+#             */
/*   Updated: 2026/10/07 09:54:39 by ccaro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <limits.h>

static int	is_valid_uint(const char *str)
{
	int	i;

	if (str == NULL || str[0] == '\0')
		return (0);
	i = 0;
	while (str[i] != '\0')
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

static int	str_to_long(const char *str, long *out)
{
	long	result;
	int		digit;
	int		i;

	result = 0;
	i = 0;
	while (str[i] != '\0')
	{
		digit = str[i] - '0';
		if (result > (LONG_MAX - digit) / 10)
			return (0);
		result = result * 10 + digit;
		i++;
	}
	*out = result;
	return (1);
}

int	parse_positive_long(const char *str, long *out)
{
	if (!is_valid_uint(str))
		return (0);
	if (!str_to_long(str, out))
		return (0);
	return (1);
}
