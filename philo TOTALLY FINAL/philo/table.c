/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   table.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: liza <liza@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 15:18:25 by ebarbash          #+#    #+#             */
/*   Updated: 2026/02/22 13:49:14 by liza             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <iso646.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>

long	time_miliseconds(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000L) + tv.tv_usec / 1000L);
}

static void	m_table_assign_forks(t_table *table, t_args *data)
{
	int	i;

	i = 0;
	while (i < data->num_philos - 1)
	{
		table->philos[i].left_fork = &table->forks[i];
		table->philos[i].right_fork = &table->forks[i + 1];
		i++;
	}
	table->philos[i].left_fork = &table->forks[i];
	table->philos[i].right_fork = &table->forks[0];
}

void	m_table_init(t_table *table, t_args *data)
{
	int	i;

	i = 0;
	while (i < data->num_philos)
	{
		table->forks[i] = m_mutex_new();
		if (m_mutex_init(&table->forks[i]))
		{
			m_table_free(table);
			error_exit("Mutex initialization failed");
		}
		i++;
	}
	if (m_mutex_init(&table->print_lock))
	{
		m_table_free(table);
		error_exit("Print mutex initialization failed");
	}
	m_table_assign_forks(table, data);
}

void	m_table_free(t_table *table)
{
	int	i;

	i = 0;
	t_philo *philo;
	while(i < table->args.num_philos)
	{
		philo = &table->philos[i];
		m_mutex_destroy(&philo->state_check_lock);
		m_mutex_destroy(&table->forks[i]);
		i++;
	}
	m_mutex_destroy(&table->print_lock);
	m_mutex_destroy(&table->death_lock);
	free(table->philos);
	free(table->forks);
	free(table);
}
