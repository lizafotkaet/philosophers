/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   table.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 15:18:25 by ebarbash          #+#    #+#             */
/*   Updated: 2026/02/21 16:01:26 by ebarbash         ###   ########.fr       */
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

void	m_table_main(t_table *table)
{
	pthread_t	*threads;
	pthread_t	monitor_thread;
	int			i;
	
	threads = malloc(sizeof(pthread_t) * table->args.num_philos);
	if (!threads)
	{
		m_table_free(table);
		error_exit("Memory allocation failed for threads");
	}
	i = 0;
	table->start_time_ms = time_miliseconds();
	while (i < table->args.num_philos)
	{
		if (pthread_create(&threads[i], NULL, m_philo_run, &table->philos[i]))
		{
			int j;
			j = 0;
			while (j < i)
			{
				pthread_join(threads[j], NULL);
				j++;
			}
			m_table_free(table);
			free(threads);
			error_exit("Thread creation failed");
		}
		i++;
	}
	if (pthread_create(&monitor_thread, NULL, m_table_check_dead_philos, table))
	{
		i = 0;
		while (i < table->args.num_philos)
		{
			pthread_join(threads[i], NULL);
			i++;
		}
		m_table_free(table);
		free(threads);
		error_exit("Monitor thread creation failed");
	}
	i = 0;
	while (i < table->args.num_philos)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
	pthread_join(monitor_thread, NULL);
	free(threads);
	m_table_free(table);
}
////////////////////////////////////////////////////////////////////////////////

void *m_table_check_dead_philos(void *data)
{
	int		i;
	long	now;
	int		state;
	int		finished_philos;
	t_table * table;

	table = (t_table *)data;
	while (true)
	{
		i = 0;
		finished_philos = 0;
		while (i < table->args.num_philos)
		{
			state = m_philo_get_state(&table->philos[i]);
			if (state == E_STATE_DEAD)
			{
				finished_philos++;
				i++;
				continue ;
			}
			now = m_table_time_miliseconds(table);
			if (now - m_philo_get_last_meal(&table->philos[i]) > table->args.time_to_die)
			{
				m_philo_set_state(&table->philos[i], E_STATE_DEAD);
				m_table_set_someone_died(table);
				m_philo_print_dead(&table->philos[i]);
				return (NULL);
			}
			i++;
		}
		if (finished_philos == table->args.num_philos)
		{
			return (NULL);
		}
		usleep(1000); // Check every 1 ms
	}
	return (NULL);
}

// some table fts that didn't fit into the big table file:

bool	m_table_someone_died(t_table *table)
{
	bool	died;

	m_mutex_lock(&table->death_lock);
	died = table->someone_died;
	m_mutex_unlock(&table->death_lock);
	return (died);
}

void	m_table_set_someone_died(t_table *table)
{
	m_mutex_lock(&table->death_lock);
	table->someone_died = true;
	m_mutex_unlock(&table->death_lock);
}

long	m_table_time_miliseconds(t_table *table)
{
	return (time_miliseconds() - table->start_time_ms);
}