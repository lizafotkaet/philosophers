/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   table_2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: liza <liza@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 15:18:25 by ebarbash          #+#    #+#             */
/*   Updated: 2026/02/22 14:16:23 by liza             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	m_table_create_threads(t_table *table, pthread_t *threads,
		pthread_t *monitor)
{
	int	i;
	int	j;

	i = -1;
	table->start_time_ms = time_miliseconds();
	while (++i < table->args.num_philos)
	{
		if (!pthread_create(&threads[i], NULL, m_philo_run, &table->philos[i]))
			continue ;
		j = -1;
		while (++j < i)
			pthread_join(threads[j], NULL);
		m_table_free(table);
		free(threads);
		error_exit("Thread creation failed");
	}
	if (!pthread_create(monitor, NULL, m_table_check_dead_philos, table))
		return ;
	i = -1;
	while (++i < table->args.num_philos)
		pthread_join(threads[i], NULL);
	m_table_free(table);
	free(threads);
	error_exit("Monitor thread creation failed");
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
	m_table_create_threads(table, threads, &monitor_thread);
	i = -1;
	while (++i < table->args.num_philos)
		pthread_join(threads[i], NULL);
	pthread_join(monitor_thread, NULL);
	free(threads);
	m_table_free(table);
}
////////////////////////////////////////////////////////////////////////////////

static int	check_one_philo(t_table *table, int i, int *finished)
{
	int		state;
	long	now;

	state = m_philo_get_state(&table->philos[i]);
	if (state == E_STATE_DEAD)
	{
		(*finished)++;
		return (0);
	}
	now = m_table_time_miliseconds(table);
	if (now - m_philo_get_last_meal(&table->philos[i])
		> table->args.time_to_die)
	{
		m_philo_set_state(&table->philos[i], E_STATE_DEAD);
		m_table_set_someone_died(table);
		m_philo_print_dead(&table->philos[i]);
		return (1);
	}
	return (0);
}

void	*m_table_check_dead_philos(void *data)
{
	int		i;
	int		finished_philos;
	t_table	*table;

	table = (t_table *)data;
	while (true)
	{
		i = 0;
		finished_philos = 0;
		while (i < table->args.num_philos)
		{
			if (check_one_philo(table, i, &finished_philos))
				return (NULL);
			i++;
		}
		if (finished_philos == table->args.num_philos)
			return (NULL);
		usleep(1000);
	}
	return (NULL);
}
