/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   table_1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 15:57:09 by ebarbash          #+#    #+#             */
/*   Updated: 2026/02/21 15:58:42 by ebarbash         ###   ########.fr       */
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
