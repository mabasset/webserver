#include "../includes/Response.hpp"

Response::Response( const Request &request, const sCMap &locationMap ) {

	_status = detectStatus(request, locationMap);
	if (_status != OK)
		return ;
	_body = readFile(request, locationMap.at(request.getUri()));
	std::stringstream size;
	size << _body.size();
	_headers["Content-Length"] = size.str();
}

std::string Response::readFile( const Request &request, const Config &location ) {

	std::ifstream 		t;
	std::stringstream	buffer;

	sVec tmp = location.getIndex();
	for (sVec::iterator it = tmp.begin(); it != tmp.end(); it++)
	{
		t.open(location.getRoot() + "/" + *it);
		if (t.is_open())
			break ;
	}
	buffer << t.rdbuf();
	_headers["Content-Type"] = "text/html";
	return buffer.str();
}

enum Status Response::detectStatus( const Request &request, const sCMap &locationMap ) {

	std::string	uri;
	sBMap		allowedMethods;

	uri = request.getUri();
	if (locationMap.find(uri) == locationMap.end())
		return NOT_FOUND;
	allowedMethods = locationMap.at(uri).getAllowedMethods();
	if (allowedMethods.at(request.getMethod()) == false)
		return METHOD_NOT_ALLOW;
	return OK;
}

void	Response::sendResponse( const int fd ) const {

	std::string	buffer;
	sSMap tmp = _headers;

	switch(_status) {
		case OK:
			buffer += "HTTP/1.1 200 OK\r\n";
			break ;
		case NOT_FOUND:
			buffer += "HTTP/1.1 200 Not Found\r\n";
			break ;
		case METHOD_NOT_ALLOW:
			buffer += "HTTP/1.1 405 Method Not Allowed\r\n";
			break ;
		default :
			buffer += "HTTP/1.1 200 OK\r\n";
	}
	for (sSMap::iterator it = tmp.begin(); it != tmp.end(); it++)
		buffer += it->first + ": " + it->second + "\r\n";
	buffer += _body + "\r\n\r\n";
	std::cout << buffer << std::endl;
	send(fd, buffer.c_str(), buffer.size(), 0);
}

enum Status Response::getStatus( void ) const {

	return (_status);
}

sSMap Response::getHaders( void ) const {

	return (_headers);
}

void Response::setStatus( const enum Status &status ) {

	this->_status = status;
}

void Response::setHeaders( const sSMap &headers ) {
	
	this->_headers = headers;
}
 
Response::~Response( void ) {

}
