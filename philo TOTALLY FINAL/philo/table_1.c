/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   table_1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: liza <liza@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 15:57:09 by ebarbash          #+#    #+#             */
/*   Updated: 2026/02/22 13:45:42 by liza             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

////////////////////////////////////////////////////////////////////////////////

// Тут выделяем память потому что мы просовываем указатель на table
// в каждого философа при создании ("каждый филосов знает за каким столом сидит")
// и поэтому нужно чтобы память была валидна после выхода из функции.
// Поэтому мы не можем вернуть локальную переменную.

static void	m_table_init_philos(t_table *table, t_args *args)
{
	int	i;

	i = 0;
	while (i < args->num_philos)
	{
		table->philos[i] = m_philo_new(table, i + 1);
		i++;
	}
	i = 0;
	while (i < args->num_philos)
	{
		if (m_mutex_init(&table->philos[i].state_check_lock))
		{
			m_table_free(table);
			error_exit("Mutex initialization failed for state_check_lock");
		}
		i++;
	}
	if (m_mutex_init(&table->death_lock))
	{
		m_table_free(table);
		error_exit("Mutex initialization failed for death_lock");
	}
}

t_table	*m_table_new(t_args *args)
{
	t_table	*table;

	table = malloc(sizeof(t_table));
	if (!table)
		error_exit("Memory allocation failed for table");
	table->someone_died = false;
	table->death_lock = m_mutex_new();
	table->args = *args;
	table->philos = malloc(sizeof(t_philo) * args->num_philos);
	if (!table->philos)
	{
		free(table);
		error_exit("Memory allocation failed for philosophers");
	}
	table->forks = malloc(sizeof(t_mutex) * args->num_philos);
	if (!table->forks)
	{
		free(table->philos);
		free(table);
		error_exit("Memory allocation failed for forks");
	}
	m_table_init_philos(table, args);
	return (table);
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
