/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:48:51 by sergei_pilm       #+#    #+#             */
/*   Updated: 2026/02/21 14:58:57 by ebarbash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <string.h>

static t_args	parse_optional_args(t_args args, int argc, char **argv)
{
	int	error;

	error = 0;
	if (argc == 5)
		return (args);
	args.num_to_eat = ft_atoi(argv[5], &error);
	if (error == 1)
		exit_on_args_error();
	if (args.num_to_eat < 0)
		exit_on_args_error();
	if (args.num_philos <= 0 || args.time_to_die < 0
		|| args.time_to_eat < 0 || args.time_to_sleep < 0)
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
	args.num_philos = ft_atoi(argv[1], &error);
	if (error == 1)
		exit_on_args_error();
	if (args.num_philos <= 0)
		exit_on_args_error();
	args.time_to_die = ft_atoi(argv[2], &error);
	if (error == 1)
		exit_on_args_error();
	args.time_to_eat = ft_atoi(argv[3], &error);
	if (error == 1)
		exit_on_args_error();
	args.time_to_sleep = ft_atoi(argv[4], &error);
	if (error == 1)
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
