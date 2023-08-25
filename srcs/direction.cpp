#include "direction.hpp"

#include <cstdlib>

e_direction		rand_direction(void)
{
	return (static_cast<e_direction>(std::rand() % 4));
}
