/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 15:03:10 by ebarbash          #+#    #+#             */
/*   Updated: 2026/02/21 15:53:05 by ebarbash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <sys/time.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>

////////////////////////////////////////////////////////////////////////////////

long	m_philo_get_last_meal(t_philo *philo)
{
	long	last_meal;

	m_mutex_lock(&philo->state_check_lock);
	last_meal = philo->last_meal_time;
	m_mutex_unlock(&philo->state_check_lock);
	return (last_meal);
}

void	m_philo_update_last_meal(t_philo *philo)
{
	m_mutex_lock(&philo->state_check_lock);
	philo->last_meal_time = m_table_time_miliseconds(philo->table);
	m_mutex_unlock(&philo->state_check_lock);
}

////////////////////////////////////////////////////////////////////////////////
// Специальный случай для одного философа (1 условие)
//
// Ждем пока философ не умрет
// Один философ не может ни есть ни пить и умирает

// Когда философ обнаруживает что кто-то умер, он должен отпустить вилку
// чтобы другие философы не застряли в ожидании

bool	m_philo_take_ordered(t_philo *philo, t_mutex *first, t_mutex *second)
{
	m_mutex_lock(first);
	if (m_philo_get_dead(philo))
	{
		m_mutex_unlock(first);
		return (true);
	}
	m_philo_print_taken_fork(philo);
	m_mutex_lock(second);
	m_philo_print_taken_fork(philo);
	if (m_philo_get_dead(philo))
	{
		m_mutex_unlock(philo->left_fork);
		m_mutex_unlock(philo->right_fork);
		return (true);
	}
	return (false);
}

bool	m_philo_take_forks(t_philo *philo)
{
	m_philo_set_state(philo, E_STATE_THINKING);
	if (philo->table->args.num_philos == 1)
	{
		m_mutex_lock(philo->left_fork);
		m_philo_print_taken_fork(philo);
		while (!m_philo_get_dead(philo))
			usleep(100);
		m_mutex_unlock(philo->left_fork);
		m_philo_print_put_fork(philo);
		return (true);
	}
	if (is_even(philo))
		return (m_philo_take_ordered(philo,
				philo->right_fork, philo->left_fork));
	return (m_philo_take_ordered(philo, philo->left_fork, philo->right_fork));
}
