/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergei_pilman <sergei_pilman@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/18 21:46:18 by sergei_pilm       #+#    #+#             */
/*   Updated: 2025/09/15 23:20:00 by sergei_pilm      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <limits.h>

typedef struct s_philo t_philo;
typedef struct s_data t_data;

struct s_philo
{
	pthread_t		philo;
	int				index;
};

struct s_data
{
	pthread_mutex_t	fork;
	int	num_philos;
	int	time_to_die;
	int	time_to_eat;
	int	time_to_sleep;
	int	num_to_eat; // optional argument	
};


// parser/utils
void	check_args(int argc, char **argv, t_data *data);
void	mndtr_args(char **argv, t_data *data);
int		ft_atoi(const char *nptr, int *error);

void	error_exit(void);

// to change later
void	*test_ft(void *);

// threads
void	make_philos(t_data data, t_philo *philo);