/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_output_1.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 15:28:38 by ebarbash          #+#    #+#             */
/*   Updated: 2026/02/21 15:29:20 by ebarbash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	m_philo_print_dead(t_philo *philo)
{
	m_philo_print(philo, "died", false);
}

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
