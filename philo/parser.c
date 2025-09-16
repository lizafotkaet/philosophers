/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergei_pilman <sergei_pilman@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:48:51 by sergei_pilm       #+#    #+#             */
/*   Updated: 2025/09/10 19:07:08 by sergei_pilm      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	mndtr_args(char **argv, t_data *data)
{
	int	error;
	
	error = 0;
	data->num_philos = ft_atoi(argv[1], &error);
	if (error == 1)
		error_exit();
	data->time_to_die = ft_atoi(argv[2], &error);
	if (error == 1)
		error_exit();
	data->time_to_eat = ft_atoi(argv[3], &error);
	if (error == 1)
		error_exit();
	data->time_to_sleep = ft_atoi(argv[4], &error);
	if (error == 1)
		error_exit();
}

void	check_args(int argc, char **argv, t_data *data)
{
	int	error;

	error = 0;
	if (argc == 5)
	{
		mndtr_args(argv, data);
	}
	else if(argc == 6)
	{
		mndtr_args(argv, data);
		data->num_to_eat = ft_atoi(argv[5], &error);
		if (error == 1)
			error_exit();
	}
	else
	{
		error_exit();
	}
}

void	error_exit(void)
{
	printf("Invalid arguments input, try again.\n");
	exit(EXIT_FAILURE);
}
