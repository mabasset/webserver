#include "../includes/Error.hpp"

Error::Error( Response *response, const int &code )
	: _response(response), _code(code) {

}

void	Error::editResponse( std::string const &root, iSMap const &errorPageMap ) {

	switch (_code) {
		case 204: _response->setStatus("204 No Content"); break ;
		case 400: _response->setStatus("400 Bad Request"); break ;
		case 401: _response->setStatus("401 Unauthorized"); break ;
		case 402: _response->setStatus("402 Payment Required"); break ;
		case 403: _response->setStatus("403 Forbidden"); break ;
		case 404: _response->setStatus("404 Not Found"); break ;
		case 405: _response->setStatus("405 Method Not Allowed"); _response->setAllowHeader(); break ;
		case 406: _response->setStatus("406 Not Acceptable"); break ;
		case 411: _response->setStatus("411 Length Required"); break ;
		case 413: _response->setStatus("413 Request Entity Too Large"); break ;
		case 501: _response->setStatus("501 Not Implemented"); break ;
		case 510: _response->setStatus("510 Not Extended"); break ;
		default:
			_response->setStatus("500 Internal Server Error");
	}
	if (_response->getRequest().getMethod() == "HEAD" || _code == 204)
		return ;

	if (errorPageMap.find(_code) != errorPageMap.end())
	{
		std::ifstream		in(Request::fixUri(root + errorPageMap.at(_code)).c_str());
		if (in.is_open())
		{
			std::stringstream	ss;
			ss << in.rdbuf();
			_response->setBody(ss.str());
		}
		else
			defaultErrorBody();
	}
	else
		defaultErrorBody();
	_response->setTypeHeader();
	_response->setLenghtHeader();
}

void	Error::defaultErrorBody( void ) {

	std::string	body("<html>\n");

	body += "<head><title>" + _response->getStatus() + "</title></head>\n";
	body += "<body>\n";
	body += "<center><h1>" + _response->getStatus() + "</h1></center>\n";
	body += "<hr><center>webserv</center>\n";
	body += "</body>\n";
	body += "</html>\n";
	_response->setBody(body);
}

Response    Error::getResponse( void ) const {
	
	return *_response;
}

void	Error::setResponse( Response response ) {

	this->_response = &response;
}

Error::~Error( void ) {

}
