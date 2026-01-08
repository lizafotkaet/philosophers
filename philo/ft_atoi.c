/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 18:39:10 by ebarbash          #+#    #+#             */
/*   Updated: 2026/01/08 15:46:03 by ebarbash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	ft_atoi(const char *nptr, int *error)
{
	long	num;
	int		sign;

	num = 0;
	sign = 1;
	while (*nptr == ' ' || (*nptr >= '\t' && *nptr <= '\r'))
		nptr++;
	if (*nptr == '+')
		nptr++;
	else if (*nptr == '-')
	{
		sign = -1;
		nptr++;
	}
	while (*nptr >= '0' && *nptr <= '9')
	{
		num = num * 10 + (*nptr - '0');
		if (num * sign > INT_MAX || num * sign < INT_MIN)
		{
			*error = 1;
			return (0);
		}
		nptr++;
	}
	return (num * sign);
}
