#include "../includes/Request.hpp"

Request::Request( std::string &all, const sCMap &locationMap, const int fd ) {

	size_t		pos;
	std::string	key, val;

	pos = all.find_first_of(" ");
	_method = all.substr(0, pos);
	pos = all.find_first_not_of(" ", pos);
	_uri = all.substr(pos, all.find_first_of(" ", pos) - pos);
	all.erase(0, all.find_first_of("\n") + 1);
	std::stringstream ss(all);
	while (std::getline(std::getline(ss, key, ':') >> std::ws, val))
		_headers.insert(std::make_pair(key, val.substr(0, val.size() - 1)));
	_location = this->detectLocation(locationMap);
	if ((_method != "PUT" && _method != "POST") || _headers.at("Transfer-Encoding") != "chunked")
		return ;
	this->detectChuncks(fd);
}

void	Request::check( void ) const {

}

const Config	&Request::detectLocation( const sCMap &locationMap ) {

	for (sCMap::const_iterator it = locationMap.begin(); it != locationMap.end(); it++)
		if (it->first != "/" && std::equal(it->first.begin(), it->first.end(), _uri.begin()))
			return it->second;
	for (sCMap::const_iterator it = locationMap.begin(); it != locationMap.end(); it++)
	{
		if (it->first != "/" && it->first.at(0) == '~')
		{
			size_t		pos = it->first.find_last_of(".") + 1;
			std::string extension = it->first.substr(pos, it->first.find_last_of("$") - pos);
			if (std::equal(extension.rbegin(), extension.rend(), _uri.rbegin()))
				return it->second;
		}
	}
	return locationMap.at("/");
}

void	Request::detectChuncks( const int fd ) {

	char		c;
	int		size;

	while(true)
	{
		std::string	buf;
		while (buf.find("\r\n") == std::string::npos)
		{
			if (recv(fd, &c, 1, 0) < 1)
				throw std::runtime_error("recv error");
			buf += c;
		}
		std::stringstream	ss(buf.substr(0, buf.find("\r\n")));
		ss >> std::hex >> size;
		buf.clear();
		size += 2;
		char	*buffer;
		int		n_bytes;
		while (size)
		{
			buffer = (char *) malloc (sizeof(char) * size);
			if ((n_bytes = recv(fd, buffer, size, 0)) < 1)
			{
				free(buffer);
				throw std::runtime_error("recv error");
			}
			for (int i = 0; i < n_bytes; i++)
				buf.push_back(buffer[i]);
			size -= n_bytes;
			free(buffer);
		}
		if (buf.find("\r\n") == 0)
			break ;
		_chunks.push_back(buf.substr(0, buf.find("\r\n")));
	}
}

std::string	Request::fixUri( std::string uri ) {

	char prev;
	for (std::string::iterator it = uri.begin(); it != uri.end(); it++)
	{
		if (prev == *it && *it == '/')
		{
			uri.erase(it);
			it--;
		}
		else
			prev = *it;
	}
	return uri;
}

void	Request::display( void ) const {

	std::cout << "method: " << _method << std::endl;
	std::cout << "uri: " << _uri << std::endl;
	for (sSMap::const_iterator it = _headers.begin(); it != _headers.end(); it++){
		std::cout<<it->first<<": "<<it->second<<std::endl;
	}
	std::cout << "location: " << _location.getLocationName() << std::endl;
}

Request::Request( const Request &src ) {
	*this = src;
}

Request	&Request::operator=( const Request &rhs ) {
	if (this == &rhs)
		return *this;
	_method = rhs.getMethod();
	_uri = rhs.getUri();
	_headers = rhs.getHeaders();
	_location = rhs.getLocation();
	return *this;
}

const std::string	&Request::getMethod( void ) const {

	return _method;
}

const std::string	&Request::getUri( void ) const {

	return _uri;
}

const sSMap	&Request::getHeaders( void ) const {

	return _headers;
}

const Config	&Request::getLocation( void ) const {

	return _location;
}

const sVec	&Request::getChunks( void ) const {

	return _chunks;
}

void	Request::setMethod( const std::string &method ) {

	this->_method = method;
}

void	Request::setUri( const std::string &uri ) {

	this->_uri = uri;
}

void	Request::setHeaders( const sSMap &headers ) {

	this->_headers = headers;
}

void	Request::setLocation( const Config &location ) {

	this->_location = location;
}

void	Request::setChunks( const sVec &chunks ) {

	this->_chunks = chunks;
}

Request::~Request(void) {

}
