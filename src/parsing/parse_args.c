/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_args.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccaro <ccaro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 09:54:43 by ccaro             #+#    #+#             */
/*   Updated: 2026/10/07 09:58:35 by ccaro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	parse_numeric_args(char **argv, t_config *config)
{
	long			*fields[7];
	int				i;

	fields[0] = &config->number_of_coders;
	fields[1] = &config->time_to_burnout;
	fields[2] = &config->time_to_compile;
	fields[3] = &config->time_to_debug;
	fields[4] = &config->time_to_refactor;
	fields[5] = &config->number_of_compiles_required;
	fields[6] = &config->dongle_cooldown;
	i = 0;
	while (i < 7)
	{
		if (!parse_positive_long(argv[i + 1], fields[i]))
		{
			fprintf(stderr, "codexion: invalid argument '%s'\n", argv[i + 1]);
			return (0);
		}
		i++;
	}
	return (1);
}

static int	parse_scheduler(const char *str, t_scheduler *scheduler)
{
	if (strcmp(str, "fifo") == 0)
	{
		*scheduler = SCHEDULER_FIFO;
		return (1);
	}
	if (strcmp(str, "edf") == 0)
	{
		*scheduler = SCHEDULER_EDF;
		return (1);
	}
	fprintf(stderr, "codexion: scheduler must be 'fifo' or 'edf'\n");
	return (0);
}

static void	print_usage(const char *prog_name)
{
	fprintf(stderr, "Usage: %s number_of_coders time_to_burnout "
		"time_to_compile time_to_debug time_to_refactor "
		"number_of_compiles_required dongle_cooldown scheduler\n",
		prog_name);
}

int	parse_args(int argc, char **argv, t_config *config)
{
	if (argc != 9)
	{
		print_usage(argv[0]);
		return (0);
	}
	if (!parse_numeric_args(argv, config))
		return (0);
	if (config->number_of_coders < 1)
	{
		fprintf(stderr, "codexion: number_of_coders must be at least 1\n");
		return (0);
	}
	if (!parse_scheduler(argv[8], &config->scheduler))
		return (0);
	return (1);
}
