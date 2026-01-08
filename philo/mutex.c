#include "philo.h"
#include <pthread.h>


t_mutex	m_mutex_new()
{
	t_mutex	mutex;

	mutex.initialized = false;
	return mutex;
}

void	m_mutex_lock(t_mutex *mutex)
{
	pthread_mutex_lock(&mutex->mutex);
}

void	m_mutex_unlock(t_mutex *mutex)
{
	pthread_mutex_unlock(&mutex->mutex);
}

bool	m_mutex_init(t_mutex *mutex)
{
	if (pthread_mutex_init(&mutex->mutex, NULL))
		return (true);
	mutex->initialized = true;
	return (false);
}

void	m_mutex_destroy(t_mutex *mutex)
{
	if (!mutex->initialized)
		return ;
	pthread_mutex_destroy(&mutex->mutex);
}
