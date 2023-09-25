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

class Server {
	private:

		pVec		_pfds;
		Config		_config;
		sCMap		_locationMap;
		std::string	_buffer;

		Server();

		void	newConnection(void);
		int		handleClient(const int fd);
		void	handleRequest(const int fd, const Config &location);

	public:
		Server(const Config &config, const sCMap &locationMap);
		~Server();

		void	makePoll(void);
		void	displayServerConfig(void) const;

		pVec	getPfds();
	
};

#endif