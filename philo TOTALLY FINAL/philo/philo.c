/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 15:03:10 by ebarbash          #+#    #+#             */
/*   Updated: 2026/02/21 15:17:28 by ebarbash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <sys/time.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>

////////////////////////////////////////////////////////////////////////////////

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

bool	m_philo_take_forks(t_philo *philo)
{
	m_philo_set_state(philo, E_STATE_THINKING);
	if (philo->table->args.num_philos == 1)
	{
		m_mutex_lock(philo->left_fork);
		m_philo_print_taken_fork(philo);
		while (!m_philo_get_dead(philo))
		{
			usleep(100);
		}
		m_mutex_unlock(philo->left_fork);
		m_philo_print_put_fork(philo);
		return (true);
	}
	if (is_even(philo))
	{
		m_mutex_lock(philo->right_fork);
		if (m_philo_get_dead(philo))
		{
			m_mutex_unlock(philo->right_fork);
			return (true);
		}
		m_philo_print_taken_fork(philo);
		m_mutex_lock(philo->left_fork);
		m_philo_print_taken_fork(philo);
	}
	else
	{
		m_mutex_lock(philo->left_fork);
		if (m_philo_get_dead(philo))
		{
		    m_mutex_unlock(philo->left_fork);
		    return (true);
		}
		m_philo_print_taken_fork(philo);
		m_mutex_lock(philo->right_fork);
		m_philo_print_taken_fork(philo);
	}
	if (m_philo_get_dead(philo))
	{
		m_mutex_unlock(philo->left_fork);
		m_mutex_unlock(philo->right_fork);
		return (true);
	}
	return (false);
}
// Мы выводим перед разблокировкой чтобы избежать ситуации когда
// другой философ сразу же захватывает вилку и пишет в консоль
// до того как текущий философ успеет написать что он положил вилку
// Чтобы избежать некорректного порядка сообщений в консоли
// (на самом деле мьютексы захватываются в корректном порядке, но при этом
// запись в консоль будет выглядеть как будто первый философ взял обе вилки и начал есть до того как сосед отпустил )

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
	long started_sleeping;
	long now;

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
	t_philo	*p = (t_philo *)data;

	bool eat_indefinitely;
	eat_indefinitely = p->table->args.num_to_eat == 0;
	m_philo_update_last_meal(p);
	m_philo_delay_before_start(p);
	m_philo_set_state(p, E_STATE_THINKING);
	while (true)
	{
		if (p->meals_eaten >= p->table->args.num_to_eat)
		{
			if (!eat_indefinitely)
			{
				break ;
			}
		}
		m_philo_print_thinking(p);
		if (m_philo_take_forks(p))
		{
			break ;
		}
		m_philo_eat(p);
		m_philo_put_forks(p);
		if(m_philo_sleep(p))
		{
			break ;
		}
	}
	m_philo_set_state(p, E_STATE_DEAD);
	return (NULL);
}
