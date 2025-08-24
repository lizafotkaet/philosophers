/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergei_pilman <sergei_pilman@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/18 21:46:08 by sergei_pilm       #+#    #+#             */
/*   Updated: 2025/08/21 11:35:19 by sergei_pilm      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*test_ft(void *)
{
	sleep(3);
	printf("Thread test\n");
	return (NULL);
}
void	*test_ft1(void *)
{
	sleep(3);
	printf("another thread test\n");
	return (NULL);
}

int	main(void)
{
	pthread_t	t1;
	pthread_t	t2;
	pthread_t	t3;
	
	pthread_create(&t1, NULL, &test_ft, NULL);
	pthread_create(&t2, NULL, &test_ft, NULL);
	pthread_create(&t3, NULL, &test_ft1, NULL);
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	pthread_join(t3, NULL);
	sleep(3);
	return (0);
}
