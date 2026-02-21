/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_output.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 14:59:47 by ebarbash          #+#    #+#             */
/*   Updated: 2026/02/21 15:02:56 by ebarbash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <sys/time.h>
////////////////////////////////////////////////////////////////////////////////

/*
Если философ умер симуляция должна остановиться.
Поэтому остальные философы ничего не будут писать и должны завершиться.
*/
void	m_philo_print(t_philo *philo, const char *msg, bool check_dead)
{
	m_mutex_lock(&philo->table->print_lock);
	if (check_dead && m_philo_get_dead(philo))
	{
		m_mutex_unlock(&philo->table->print_lock);
		return ;
	}
	printf("%ld %d %s\n",
		m_table_time_miliseconds(philo->table), philo->id, msg);
	m_mutex_unlock(&philo->table->print_lock);
}

void	m_philo_print_taken_fork(t_philo *philo)
{
	m_philo_print(philo, "has taken a fork", true);
}

void	m_philo_print_put_fork(t_philo *philo)
{
	m_philo_print(philo, "has put down a fork", true);
}

void	m_philo_print_eating(t_philo *philo)
{
	m_philo_print(philo, "is eating", true);
}

void	m_philo_print_sleeping(t_philo *philo)
{
	m_philo_print(philo, "is sleeping", true);
}

void	m_philo_print_thinking(t_philo *philo)
{
	m_philo_print(philo, "is thinking", true);
}

void	m_philo_print_dead(t_philo *philo)
{
	m_philo_print(philo, "died", false);
}
