#ifndef IMODULE_HPP
# define IMODULE_HPP

# include "cell_data.hpp"
# include "event.hpp"

# include <vector>

class IModule
{
public:

	virtual							~IModule(void) { }

	virtual void					disable(void) = 0;
	virtual void					enable(void) = 0;

	virtual std::vector<e_event>	getEvents(void) = 0;
	virtual void					render(std::vector<t_cell_data> cellData) = 0;

};

#endif
