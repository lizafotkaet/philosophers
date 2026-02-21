/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/18 21:46:18 by sergei_pilm       #+#    #+#             */
/*   Updated: 2026/02/21 15:58:04 by ebarbash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <limits.h>

typedef struct s_mutex	t_mutex;
typedef struct s_philo	t_philo;
typedef struct s_args	t_args;
typedef struct s_table	t_table;

#define E_STATE_CREATED 0
#define E_STATE_THINKING 1
#define E_STATE_EATING 2
#define E_STATE_SLEEPING 3
#define E_STATE_DEAD 4

struct s_mutex
{
	pthread_mutex_t	mutex;
	bool			initialized;
};

struct s_philo
{
	int		id;
	t_mutex	state_check_lock;
	int		state;
	long	last_meal_time;
	int		meals_eaten;
	t_mutex	*left_fork;
	t_mutex	*right_fork;
	t_table	*table;
};

struct s_args
{
	int		num_philos;
	int		time_to_die;
	int		time_to_eat;
	int		time_to_sleep;
	int		num_to_eat; // optional argument
};

// Лок чтобы писать в консоль без пересечений (принт лок)

struct s_table
{
	t_philo	*philos;
	t_mutex	*forks;
	t_mutex	print_lock;
	t_mutex	death_lock;
	bool	someone_died;
	long	start_time_ms;
	t_args	args;
};

// parser/utils
t_args	parse_args(int argc, char **argv);
int		ft_atoi(const char *nptr, int *error);
void	error_exit(char *msg);
void	exit_on_args_error(void);
long	m_table_time_miliseconds(t_table *table);

////////////////////////////////////////////////////////////////////////////////

t_mutex	m_mutex_new(void);
void	m_mutex_lock(t_mutex *mutex);
void	m_mutex_unlock(t_mutex *mutex);
bool	m_mutex_init(t_mutex *mutex);
void	m_mutex_destroy(t_mutex *mutex);

////////////////////////////////////////////////////////////////////////////////

t_table	*m_table_new(t_args *data);
void	m_table_init(t_table *table, t_args *data);
void	m_table_free(t_table *table);
bool	m_table_someone_died(t_table *table);
void	m_table_set_someone_died(t_table *table);

////////////////////////////////////////////////////////////////////////////////

void	m_table_main(t_table *table);
void	*m_table_check_dead_philos(void *data);

////////////////////////////////////////////////////////////////////////////////

t_philo	m_philo_new(t_table *table, int id);

////////////////////////////////////////////////////////////////////////////////

void	m_philo_print(t_philo *philo, const char *msg, bool check_dead);
void	m_philo_print_taken_fork(t_philo *philo);
void	m_philo_print_put_fork(t_philo *philo);
void	m_philo_print_eating(t_philo *philo);
void	m_philo_print_sleeping(t_philo *philo);
void	m_philo_print_thinking(t_philo *philo);
void	m_philo_print_dead(t_philo *philo);

////////////////////////////////////////////////////////////////////////////////

void	m_philo_delay_before_start(t_philo *philo);
bool	m_philo_take_forks(t_philo *philo);
void	m_philo_put_forks(t_philo *philo);
void	m_philo_eat(t_philo *philo);
bool	m_philo_sleep(t_philo *philo);

bool	m_philo_get_dead(t_philo *philo);
int		m_philo_get_state(t_philo *philo);
void	m_philo_set_state(t_philo *philo, int state);

void	m_philo_update_last_meal(t_philo *philo);
long	m_philo_get_last_meal(t_philo *philo);
void	*m_philo_run(void *data);
bool	is_even(t_philo *philo);
bool	m_philo_take_ordered(t_philo *philo, t_mutex *first, t_mutex *second);
long	time_miliseconds(void);