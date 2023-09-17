#include "../includes/Response.hpp"

Response::Response( const Request &request, const int fd )
	: _request (&request), _socket(fd), _location(request.getLocation()) {

}

void	Response::compile( ) {

	std::string	methods[] = { "GET", "POST", "DELETE", "HEAD", "PUT" };
	int i;

	_status = 
	i = 0;
	while (i < 5)
	{
		if (_request->getMethod() == methods[i])
			break ;
		i++;
	}
	if (i == 5)
		throw Error(this, NOT_IMPLEMENTED);
	switch (i) {
		case HEAD:
		case GET: this->handleGet(); break ;
		// case POST: this->handlePOST(); break ;
		// case DELETE: this->handleDelete(); break ;
		// case PUT: this->handlePUT(); break ;
		default : throw Error(this, SERVER_ERROR);
	}
}

void	Response::commit( void ) const {

	std::string	buffer("HTTP/1.1 ");

	buffer += _status + "\r\n";
	for(sSMap::const_iterator it = _headers.begin(); it != _headers.end(); it++)
		buffer += it->first + ": " + it->second + "\r\n";
	buffer += "\r\n" + _body;
	std::cout << buffer << std::endl;
	send(_socket, buffer.c_str(), buffer.size(), 0);
}

void	Response::handleGet( void ) {

	DIR *dir = opendir(_request->getUri().c_str());
	std::ifstream in;

	_status = "200 OK";
	if (dir != NULL)
	{
		closedir(dir);
		std::cout << "ciao" << std::endl;
		sVec index = _location.getIndex();
		for (sVec::const_iterator it = index.begin(); it != index.end(); it++)
		{
			in.open(_request->getUri() + "/" + *it);
			if (in.is_open())
				break ;
		}
	}
	if (!in.is_open())
	{
		in.open(_location.getRoot() + _location.getErrorPage().at(404));
		_status = "404 Not Found";
	}
	std::stringstream ss;
	ss << in.rdbuf();
	_body = ss.str();
	in.close();
}

Request Response::getRequest( void ) const {

	return *this->_request;
}

sSMap Response::getHaders( void ) const {

	return (_headers);
}

void Response::setStatus( const std::string &status ) {
	
	this->_status = status;
}

void Response::setHeaders( const sSMap &headers ) {
	
	this->_headers = headers;
}

void Response::setBody( const std::string &body ) {
	
	this->_body = body;
}

void	Response::setRequest( Request request ) {

	this->_request = &request;
}
 
Response::~Response( void ) {

}
