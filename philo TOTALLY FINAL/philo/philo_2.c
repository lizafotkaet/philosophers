/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 15:35:36 by ebarbash          #+#    #+#             */
/*   Updated: 2026/02/21 15:52:49 by ebarbash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

// Мы выводим перед разблокировкой чтобы избежать ситуации когда
// другой философ сразу же захватывает вилку и пишет в консоль
// до того как текущий философ успеет написать что он положил вилку
// Чтобы избежать некорректного порядка сообщений в консоли
// (на самом деле мьютексы захватываются в корректном порядке, но при этом
// запись в консоль будет выглядеть как будто первый философ 
// взял обе вилки и начал есть до того как сосед отпустил )

void	m_philo_put_forks(t_philo *philo)
{
	if (philo->table->args.num_philos == 1)
	{
		m_mutex_unlock(philo->left_fork);
		m_philo_print_put_fork(philo);
		return ;
	}
	m_philo_print_put_fork(philo);
	m_mutex_unlock(philo->left_fork);
	m_philo_print_put_fork(philo);
	m_mutex_unlock(philo->right_fork);
}

bool	m_philo_sleep(t_philo *philo)
{
	long	started_sleeping;
	long	now;

	if (m_philo_get_dead(philo))
		return (true);
	m_philo_print_sleeping(philo);
	m_philo_set_state(philo, E_STATE_SLEEPING);
	started_sleeping = m_table_time_miliseconds(philo->table);
	now = m_table_time_miliseconds(philo->table);
	while (now - started_sleeping < philo->table->args.time_to_sleep)
	{
		if (m_philo_get_dead(philo))
			return (true);
		usleep(1000);
		now = m_table_time_miliseconds(philo->table);
	}
	if (m_philo_get_dead(philo))
		return (true);
	return (false);
}

// тут мы проверяем не умер ли кто-то (в ф-ции м-фило-гет-дед)

void	m_philo_eat(t_philo *philo)
{
	long	started_eating;
	long	now;

	if (m_philo_get_dead(philo))
		return ;
	m_philo_print_eating(philo);
	m_philo_set_state(philo, E_STATE_EATING);
	started_eating = m_table_time_miliseconds(philo->table);
	m_philo_update_last_meal(philo);
	now = m_table_time_miliseconds(philo->table);
	while (now - started_eating < philo->table->args.time_to_eat)
	{
		if (m_philo_get_dead(philo))
			return ;
		usleep(100);
		now = m_table_time_miliseconds(philo->table);
	}
	philo->meals_eaten += 1;
}

////////////////////////////////////////////////////////////////////////////////

void	m_philo_delay_before_start(t_philo *philo)
{
	if (philo->table->args.num_philos <= 20)
		usleep(1000 * philo->id);
	else if (philo->table->args.num_philos <= 100)
		usleep(500 * (philo->id % 10));
	else
		usleep(1500 * (philo->id % 2));
}

void	*m_philo_run(void *data)
{
	t_philo	*p;
	bool	eat_indefinitely;

	p = (t_philo *)data;
	eat_indefinitely = p->table->args.num_to_eat == 0;
	m_philo_update_last_meal(p);
	m_philo_delay_before_start(p);
	m_philo_set_state(p, E_STATE_THINKING);
	while (true)
	{
		if (p->meals_eaten >= p->table->args.num_to_eat)
		{
			if (!eat_indefinitely)
				break ;
		}
		m_philo_print_thinking(p);
		if (m_philo_take_forks(p))
			break ;
		m_philo_eat(p);
		m_philo_put_forks(p);
		if (m_philo_sleep(p))
			break ;
	}
	m_philo_set_state(p, E_STATE_DEAD);
	return (NULL);
}
