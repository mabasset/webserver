#include "../includes/Server.hpp"

Server::Server(const Config &config, const sCMap &locationMap)
	: _config(config), _locationMap(locationMap) {
	struct sockaddr_in	addr;
	int					listener;

	listener = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
	if (fcntl(listener, F_SETFL, fcntl(listener, F_GETFL, 0) | O_NONBLOCK) == -1)
		throw std::runtime_error("Unable to make socket non-blocking");
	if (listener < 0)
		throw std::runtime_error("Unable to create socket");
	addr.sin_family = AF_INET;
	addr.sin_port = htons(config.getListen());
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	int yes = 1;
	if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1)
		throw std::runtime_error("Unable to reuse port");
	if (bind(listener,(struct sockaddr *)&addr, sizeof(addr)) != 0)
		throw std::runtime_error("Unable to bind socket");
	if (listen(listener, 1000) == -1)
		throw std::runtime_error("Listen error");
	struct pollfd socket;
	socket.fd = listener;
	socket.events = POLLIN;
	_pfds.push_back(socket);
}

void	Server::makePoll( void ) {
	int pollCount;

	pollCount = poll(&_pfds[0], _pfds.size(), 0);
	if (pollCount == -1)
		throw std::runtime_error("Poll error");
	if (pollCount == 0)
		return ;
	for(pVec::iterator it = _pfds.begin(); it != _pfds.end(); it++)
	{
		if (it->revents == POLLIN)
		{
			if (it->fd == _pfds.at(0).fd)
				newConnection();
			else if (handleClient(it->fd))
				_pfds.erase(it);
			break ;
		}
	}
}

void	Server::newConnection(void) {
	int 					new_fd;
	struct sockaddr_storage client_addr;
	socklen_t				sin_size;

	sin_size = sizeof(client_addr);
	new_fd = accept(_pfds.at(0).fd, (struct sockaddr *)&client_addr, &sin_size);
	if (new_fd == -1)
		throw std::runtime_error("Accept error");
	struct pollfd socket;
	socket.fd = new_fd;
	socket.events = POLLIN;
	_pfds.push_back(socket);
}

int	Server::handleClient(const int fd) {
	std::string	buffer;
	char		c;

	std::cout << fd << std::endl;
	while (buffer.find("\r\n\r\n") == std::string::npos)
	{
		if (recv(fd, &c, 1, 0) < 1)
			throw std::runtime_error("recv error");
		buffer.push_back(c);
	}
	Request request(buffer, _locationMap, fd);
	//std::cout << "request: " << clock() - start << std::endl;
	Response response(request, fd);
	try {
		//clock_t	start = clock();
		response.compile();
		//std::cout << "response: " << clock() - start << std::endl;
	}
	catch (Error &e) {
		e.editResponse(_config.getRoot(), _config.getErrorPage());
	}
	response.commit();
	close(fd);
	return 1;
}

void	Server::displayServerConfig(void) const {
	std::cout << "Server" << std::endl;
	_config.displayConfig();
	for (sCMap::const_iterator it = _locationMap.begin(); it != _locationMap.end(); it++)
	{
		std::cout << "Name: " << it->first << std::endl;
		it->second.displayConfig();
	}
}

struct pollfd	&Server::getSocket( void ) {

	return _socket;
}

const pVec			&Server::Server::getPfds() const {

	return _pfds;
}

const Config			&Server::getConfig( void ) const {

	return _config;
}

const sCMap			&Server::getLocationMap( void ) const {

	return _locationMap;
}

void			Server::setSocket( const struct pollfd &socket ) {

	_socket = socket;
}

void			Server::setPfds( const pVec &pfds ) {

	_pfds = pfds;
}

void			Server::setConfig( const Config &config ) {

	_config = config;
}

void			Server::setLocationMap( const sCMap &locationMap ) {

	_locationMap = locationMap;
}

Server::~Server(void) {

}