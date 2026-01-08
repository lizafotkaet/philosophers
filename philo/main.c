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
