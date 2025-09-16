/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergei_pilman <sergei_pilman@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/18 21:46:08 by sergei_pilm       #+#    #+#             */
/*   Updated: 2025/09/15 23:22:40 by sergei_pilm      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*test_ft(pthread_mutex_t fork)
{
	pthread_mutex_lock(&fork); // playground shitz
	sleep(3);
	printf("Thread test\n");
	pthread_mutex_unlock(&fork); // playground shitz x2
	gettimeofday();
	return (NULL);
}
void	*test_ft1(void *)
{
	sleep(3);
	printf("another thread test\n");
	return (NULL);
}

void	make_philos(t_data data, t_philo *philo)
{
	int	n = data.num_philos;
	int	i = 0;

	n = data.num_philos;
	while (n > 0)
	{
		if (pthread_create(&philo[i].philo, NULL, &test_ft, NULL) != 0)
		{
			exit(EXIT_FAILURE);	
		}
		philo[i].index = i + 1;
		n--;
		i++;
	}
	n = data.num_philos;
	i = 0;
	while (n > 0)
	{
		pthread_join(philo[i].philo, NULL);
		n--;
		i++;
	}
}

int	main(int argc, char **argv)
{
	t_data		data;
	t_philo		philo[5];
	pthread_mutex_t	mutex;

	pthread_mutex_init(&mutex, NULL);

	check_args(argc, argv, &data);
	make_philos(data, philo);
	int	n = data.num_philos;
	int	i = 0;
	while (n > 0)
	{
		printf("%dй философ: %ld\n", philo[i].index, philo[i].philo);
		n--;
		i++;
	}
	pthread_mutex_destroy(&mutex);

	pthread_detach();
	// printf("\n--------------------------\n\n");
	// printf("Number of philosophers: %d\n", data.num_philos);
	// printf("Time to die (in milliseconds): %d\n", data.time_to_die);
	// printf("Time to eat (in milliseconds): %d\n", data.time_to_sleep);
	// printf("Time to sleep (in milliseconds): %d\n", data.time_to_sleep);
	// printf("How many times philosophers must eat (if specified): %d\n", data.num_to_eat);
	// printf("\n--------------------------\n\n");
	return (0);
}
