/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: liza <liza@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:48:51 by sergei_pilm       #+#    #+#             */
/*   Updated: 2026/02/22 14:34:18 by liza             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <string.h>

static t_args	parse_optional_args(t_args args, int argc, char **argv)
{
	int	error;

	error = 0;
	args.num_to_eat = 0;
	if (argc == 5)
		return (args);
	if (!ft_valid_arg(argv[5]))
		exit_on_args_error();
	args.num_to_eat = ft_atoi(argv[5], &error);
	if (error == 1 || args.num_to_eat <= 0)
		exit_on_args_error();
	return (args);
}

t_args	parse_args(int argc, char **argv)
{
	t_args	args;
	int		error;

	memset((void *)&args, 0, sizeof(t_args));
	error = 0;
	if (argc < 5 || argc > 6)
		exit_on_args_error();
	if (!ft_valid_arg(argv[1]) || !ft_valid_arg(argv[2])
		|| !ft_valid_arg(argv[3]) || !ft_valid_arg(argv[4]))
		exit_on_args_error();
	args.num_philos = ft_atoi(argv[1], &error);
	if (error == 1 || args.num_philos <= 0)
		exit_on_args_error();
	args.time_to_die = ft_atoi(argv[2], &error);
	if (error == 1 || args.time_to_die <= 0)
		exit_on_args_error();
	args.time_to_eat = ft_atoi(argv[3], &error);
	if (error == 1 || args.time_to_eat <= 0)
		exit_on_args_error();
	args.time_to_sleep = ft_atoi(argv[4], &error);
	if (error == 1 || args.time_to_sleep <= 0)
		exit_on_args_error();
	return (parse_optional_args(args, argc, argv));
}

void	error_exit(char *msg)
{
	printf("%s\n", msg);
	exit(EXIT_FAILURE);
}

void	exit_on_args_error(void)
{
	error_exit("Error: Invalid arguments");
}
