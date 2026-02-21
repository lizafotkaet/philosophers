/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 15:30:53 by ebarbash          #+#    #+#             */
/*   Updated: 2026/02/21 15:35:01 by ebarbash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

t_philo	m_philo_new(t_table *table, int id)
{
	t_philo	philo;

	philo.id = id;
	philo.meals_eaten = 0;
	philo.state = E_STATE_CREATED;
	philo.last_meal_time = 0;
	philo.table = table;
	philo.state_check_lock = m_mutex_new();
	return (philo);
}

bool	m_philo_get_dead(t_philo *philo)
{
	return (m_table_someone_died(philo->table));
}

int	m_philo_get_state(t_philo *philo)
{
	int	state;

	m_mutex_lock(&philo->state_check_lock);
	state = philo->state;
	m_mutex_unlock(&philo->state_check_lock);
	return (state);
}

void	m_philo_set_state(t_philo *philo, int state)
{
	m_mutex_lock(&philo->state_check_lock);
	if (philo->state != E_STATE_DEAD)
	{
		philo->state = state;
	}
	m_mutex_unlock(&philo->state_check_lock);
}

bool	is_even(t_philo *philo)
{
	return (philo->id % 2 == 0);
}
