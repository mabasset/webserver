#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <netinet/in.h>
# include "unistd.h"
# include <poll.h>
# include <fcntl.h>
# include <vector>
# include <fstream>
# include <string>
# include <sstream>
# include <sys/wait.h>
# include <string.h>
# include "Config.hpp"

typedef std::map<std::string, std::string>	sSMap;
typedef std::vector<struct pollfd>			pVec;
typedef	std::map<std::string, Config>		sCMap;

# include "Request.hpp"
# include "Response.hpp"
# include "Error.hpp"
# include <ctime>

class Server {
	private:

		struct pollfd	_socket;
		pVec			_pfds;
		Config			_config;
		sCMap			_locationMap;

		Server();


	public:
		Server(const Config &config, const sCMap &locationMap);
		~Server();

		void	makePoll( void );
		void	newConnection( void );
		int		handleClient( const int fd );
		void	handleRequest( const int fd, const Config &location );
		
		void	displayServerConfig(void) const;

		struct pollfd		&getSocket( void );
		const pVec			&getPfds( void ) const;
		const Config		&getConfig( void ) const;
		const sCMap			&getLocationMap( void ) const;

		void			setSocket( const struct pollfd &socket );
		void			setPfds( const pVec &pfds );
		void			setConfig( const Config &config );
		void			setLocationMap( const sCMap &locationMap );
};

#endif