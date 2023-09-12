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
# include <sys/stat.h>
# include <sys/wait.h>
# include <string.h>
# include "Config.hpp"

typedef std::map<std::string, Config>		sCMap;
typedef std::map<std::string, std::string>	sSMap;
typedef std::vector<struct pollfd>			pVec;

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
		int		handleClient(int fd);
		bool	checkRequest(int fd, Config &location);
		void	parseRequest(std::string request);
		void	handleRequest(int fd, Config &location);
		void	handleGET(int fd, Config &location);
		void	handleDELETE(int fd);
		void	handlePUT(int fd, Config &location);
		void	handlePUTChunked(int fd,Config &location);
		bool	getChunk(std::string &line, int fd);
		//short	getRequestContent(int fd, std::string line, std::stringstream &content, int maxBodySize);
		void	handlePOST(int fd, Config &location);
		std::string	executeCGI(Config &location, std::stringstream &content);
		char	**getEnvCgi(Config &location);
		int		getBody(std::ifstream &body, Config &location);
		bool	getIcon(std::ifstream &body);
		bool	checkTryFiles(std::string check, Config &location);
		bool	checkExtensionCgi(std::string end, Config &location);
		void	default_error_answer(int err, int fd, Config &location);

	public:
		Server(Config &config, sCMap &locationMap);
		~Server();

		void	makePoll(void);
		void	displayServerConfig(void);

		pVec	getPfds();
	
};

bool	isDirectory(const std::string& path);

#endif