#include "Nibbler.hpp"
#include "NibblerException.hpp"
#include "ParsingException.hpp"

#include <iostream>
#include <exception>

#define PORT_MIN	49152
#define PORT_MAX	65535

typedef struct		s_options
{
	int				numPlayers;
	int				boardWidth;
	int				boardHeight;

	bool			isOnline;
	bool			isServer;
	std::string		ipAddress;
	unsigned short	port;

}					t_options;

int					parseNumber(std::string expr)
{
	try
	{
		if (expr.find_first_not_of("0123456789") != std::string::npos)
			throw std::exception();
		return (std::stoi(expr));						// can get std::invalid argument, std::out_of_range exceptions
	}
	catch (std::exception & exception)
	{
		throw ParsingException("Invalid number: " + expr);
	}
}

unsigned short		parsePort(std::string expr)
{
	int				port;

	try
	{
		if (expr.find_first_not_of("0123456789") != std::string::npos)
			throw std::exception();
		port = std::stoi(expr);							// can get std::invalid argument, std::out_of_range exceptions
		if (!(PORT_MIN <= port && port <= PORT_MAX))
			throw std::exception();
		return (static_cast<unsigned short>(port));
	}
	catch (std::exception & exception)
	{
		throw ParsingException("Invalid port: " + expr + "\nPlease use an acceptable port in the range [ " +
			std::to_string(PORT_MIN) + ", " + std::to_string(PORT_MAX) + " ]");
	}	
}

void				startGame(t_options & options)
{
	if (options.isOnline)
	{
		if (options.isServer)
			Nibbler::createOnlineGameAsServer(options.boardWidth, options.boardHeight, options.port);
		else
			Nibbler::createOnlineGameAsClient(options.ipAddress, options.port);
	}
	else
		Nibbler::createLocalGame(options.boardWidth, options.boardHeight, options.numPlayers);
}

void				terminateWithUsageError(void)
{
	std::cout << "usage:\n" \
		"Local Game\t\t: Nibbler -l numPlayers boardWidth boardHeight\n" \
		"Online 2P as Server\t: Nibbler -s port boardWidth boardHeight\n" \
		"Online 2P as Client\t: Nibbler -c ipAddress port" << std::endl;
	exit(EXIT_FAILURE);
}

/*
	0			1			2			3			4

	Nibbler		-l			numPlayers	boardWidth	boardHeight		// local game
	Nibbler		-s			port		boardWidth	boardHeight		// online game as server
	Nibbler		-c			ipAddress	port						// online game as client
*/

t_options			parseOptions(int argc, char * argv[])
{
	t_options		options;

	if (argc < 2)
		terminateWithUsageError();

	std::string		flag = argv[1];

	if (flag == "-l")
	{
		if (argc != 5)
			terminateWithUsageError();
		options.numPlayers = parseNumber(argv[2]);
		options.isOnline = false;
		options.isServer = false;
		options.boardWidth = parseNumber(argv[3]);
		options.boardHeight = parseNumber(argv[4]);
	}
	else if (flag == "-s")
	{
		if (argc != 5)
			terminateWithUsageError();
		options.numPlayers = 2;
		options.isOnline = true;
		options.isServer = true;
		options.port = parsePort(argv[2]);
		options.boardWidth = parseNumber(argv[3]);
		options.boardHeight = parseNumber(argv[4]);
	}
	else if (flag == "-c")
	{
		if (argc != 4)
			terminateWithUsageError();
		options.numPlayers = 2;
		options.isOnline = true;
		options.isServer = false;
		options.ipAddress = argv[2];
		options.port = parsePort(argv[3]);		
	}
	else
		terminateWithUsageError();
	return (options);
}

int					main(int argc, char * argv[])
{
	try
	{
		t_options options = parseOptions(argc, argv);
		startGame(options);
	}
	catch (ParsingException & exception)
	{
		std::cout << "ParsingException: " << exception.what() << std::endl;

		terminateWithUsageError();
	}
	catch (NibblerException & exception)
	{
		std::cout << "NibblerException: " << exception.what() << std::endl;
	}
	catch (std::bad_alloc & exception)
	{
		std::cout << "bad_alloc exception: " << exception.what() << std::endl;
	}
	catch (std::exception & exception)
	{
		std::cout << "Exception: " << exception.what() << std::endl;	
	}

	return (EXIT_SUCCESS);
}
