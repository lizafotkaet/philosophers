/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_output.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 14:59:47 by ebarbash          #+#    #+#             */
/*   Updated: 2026/02/21 15:29:29 by ebarbash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <sys/time.h>
////////////////////////////////////////////////////////////////////////////////

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
