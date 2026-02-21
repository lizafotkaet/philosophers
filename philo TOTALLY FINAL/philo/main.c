/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 14:53:07 by ebarbash          #+#    #+#             */
/*   Updated: 2026/02/21 14:53:41 by ebarbash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	main(int argc, char **argv)
{
	t_args	args;
	t_table	*table;

	args = parse_args(argc, argv);
	table = m_table_new(&args);
	m_table_init(table, &args);
	m_table_main(table);
	return (0);
}
