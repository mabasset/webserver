#include "../includes/Response.hpp"

Response::Response( const Request &request, const int fd )
	: _socket(fd), _request (&request), _uri(request.getUri()) {

}

void	Response::compile( ) {

	std::string	methods[] = { "GET", "POST", "DELETE", "HEAD", "PUT" };
	int i;

	_uri = this->translateUri();
	i = 0;
	while (i < 5)
	{
		if (_request->getMethod() == methods[i])
			break ;
		i++;
	}
	if (i == 5)
		throw Error(this, NOT_IMPLEMENTED);
	if (_request->getLocation().getAllowedMethods().at(_request->getMethod()) == false)
		throw Error(this, NOT_ALLOWED);
	switch (i) {
		case HEAD:
		case GET: this->handleGet(); break ;
		case POST:
		case PUT: this->handlePut(); break ;
		// case DELETE: this->handleDelete(); break ;
		default : throw Error(this, SERVER_ERROR);
	}
}

void	Response::commit( void ) const {

	std::string	buffer("HTTP/1.1 ");

	buffer += _status + "\r\n";
	for(sSMap::const_iterator it = _headers.begin(); it != _headers.end(); it++)
		buffer += it->first + ": " + it->second + "\r\n";
	if (!_headers.empty())
		buffer += "\r\n";
	buffer += _body;
	std::cout<< buffer << std::endl;
	send(_socket, buffer.c_str(), buffer.size(), 0);
}

void	Response::handleGet( void ) {

	_status = "200 OK";
	std::ifstream in(_uri.c_str());
	if (!in.is_open())
		throw Error(this, SERVER_ERROR);
	std::stringstream ss;
	ss << in.rdbuf();
	_body = ss.str();
	in.close();
	this->setTypeHeader();
	this->setLenghtHeader();
	if (_request->getMethod() == "HEAD")
		_body.clear();
}

void	Response::handlePut( void ) {

	if (_request->getHeaders().at("Transfer-Encoding") != "chunked")
		throw Error(this, NOT_IMPLEMENTED);

	for (sVec::const_iterator it = _request->getChunks().begin(); it != _request->getChunks().end(); it++)
		_body += *it;
	struct stat buffer;
	_status = "201 Created";
  	if (stat (_uri.c_str(), &buffer) == 0)
		_status = "204 No Content";
	std::ofstream out(_uri.c_str(), std::ios::out | std::ios::trunc);
	if (!out.is_open())
		throw Error(this, SERVER_ERROR);
	if (_request->getMethod() == "POST")
	{
		_body = executeCGI(_body);
		if (_body.find("Status") != std::string::npos)
			_body.erase(0, _body.find("\n"));
		if (_body.find("Content-Type") != std::string::npos)
			_body.erase(0, _body.find("\r\n\r\n") + 4);
	}
	out << _body;
	out.close();
	_headers["Content-Location"] = _uri;
	this->setLenghtHeader();
}

std::string	Response::executeCGI(std::string &content){
	pid_t	pid;
	int		std_cpy[2] = { dup(0), dup(1) };
	char	**env = getEnvCgi();
	std::string _retBody = "";

	FILE*	in = tmpfile();
	FILE*	out = tmpfile();
	int		fd_in = fileno(in);
	int		fd_out = fileno(out);

	// UNCOMMENT TO PRINT ENV
	 int i = 0;
	 while (env[i])
			printf("%s\n", env[i++]);

	// use tmpFile() instead of pipe() to handle big amount of data

	write(fd_in, content.c_str(), content.size());
	lseek(fd_in, 0, SEEK_SET);

	pid = fork();

	if (pid == -1) {
		std::cout << "Fork failed" << std::endl;
		return ("Status: 500\r\n\r\n");
	}
	else if (!pid) {
		std::string uri = _uri;
		size_t pos = uri.find("//");
		if (pos != std::string::npos)
			uri.erase(pos, 1);
		const char *filepath = uri.c_str();
		std::string script_path = _request->getLocation().getCgiPass();
		if (script_path.at(0) == '/')
			script_path.erase(0, 1);
		char *const args[3] = {strdup(script_path.c_str()), strdup(filepath), NULL};
		// std::cout<<"args0 "<<args[0]<<std::endl;
		// std::cout<<"args1 "<<args[1]<<std::endl;
		// std::cout<<"args2 "<<args[2]<<std::endl;
		dup2(fd_in, 0);
		dup2(fd_out, 1);
		execve(args[0], args, env);
		std::cout << "exxxecve failed" << std::endl;
		write(1, "Status: 500\r\n\r\n", 15);
		exit (0);
	}
	else {
		waitpid(-1, NULL, 0);

		lseek(fd_out, 0, SEEK_SET);

		char	buffer[1024];
		size_t  dataRead = 0;
		while ((dataRead = read(fd_out, buffer, sizeof buffer)) > 0) {
			for (size_t i = 0; i < dataRead; i++)
				_retBody.push_back(buffer[i]);
			memset(buffer, 0, sizeof buffer);
		}
	}
	fclose(in);
	fclose(out);
	close(fd_in);
	close(fd_out);

	dup2(std_cpy[0], 0);
	dup2(std_cpy[1], 1);

	for (short i = 0; env[i] != NULL; i++)
		delete[] env[i];
	delete[] env;
	return _retBody;
}

char	**Response::getEnvCgi() {
	sSMap envMap;

	envMap.insert(std::make_pair("AUTH_TYPE", ""));
	envMap.insert(std::make_pair("CONTENT_LENGTH", ""));
	envMap.insert(std::make_pair("CONTENT_TYPE", "application/x-www-form-urlencoded"));
	envMap.insert(std::make_pair("GATEWAY_INTERFACE", "CGI/1.1"));
	envMap.insert(std::make_pair("PATH_INFO", _request->getUri()));
	envMap.insert(std::make_pair("PATH_TRANSLATED", _uri));
	envMap.insert(std::make_pair("QUERY_STRING", ""));
	envMap.insert(std::make_pair("REMOTE_ADDR", _request->getHeaders().at("Host")));
	envMap.insert(std::make_pair("REMOTE_HOST", ""));
	envMap.insert(std::make_pair("REMOTE_IDENT", ""));
	envMap.insert(std::make_pair("REMOTE_USER", ""));
	envMap.insert(std::make_pair("REQUEST_METHOD",  _request->getMethod()));
	envMap.insert(std::make_pair("REQUEST_URI", _request->getUri()));
	envMap.insert(std::make_pair("SCRIPT_NAME", "ubuntu_cgi_tester"));
	envMap.insert(std::make_pair("SERVER_NAME", "http://" + _request->getHeaders().at("Host")));
	envMap.insert(std::make_pair("SERVER_PORT", "8080" /*locatio.serverport()*/));
	envMap.insert(std::make_pair("SERVER_PROTOCOL", "HTTP/1.1"));
	envMap.insert(std::make_pair("SERVER_SOFTWARE", "Webserv/1.0"));
	envMap.insert(std::make_pair("REDIRECT_STATUS", "200"));
	// if (_request.getHeader().find("X-Secret-Header-For-Test") != )
	// 	envMap.insert(std::make_pair("HTTP_X_SECRET_HEADER_FOR_TEST", _requestMap["X-Secret-Header-For-Test"]));}ert(std::make_pair("HTTP_X_SECRET_HEADER_FOR_TEST", conn.headers.substr(conn.headers.find("X-Secret") + 26, conn.headers.find_first_of("\r\n"))));
	char	**env = new char*[envMap.size() + 1];
	int	j = 0;
	for (sSMap::const_iterator i = envMap.begin(); i != envMap.end(); i++) {
		std::string	element = i->first + "=" + i->second;
		env[j] = new char[element.size() + 1];
		env[j] = strcpy(env[j], (const char*)element.c_str());
		//std::cout<<"riga env:"<<env[j]<<std::endl;
		j++;
	}
	env[j] = NULL;
	return env;
}

std::string	Response::translateUri( void ) {

	std::string uri;
	Config		location = _request->getLocation();
	struct stat stat_data;

	if (location.getLocationName() == "/")
	{
		uri = location.getRoot() + _request->getUri();
		Request::fixUri(uri);
	}
	else
	{
		uri = _request->getUri();
		uri.replace(0, location.getLocationName().size(), location.getRoot());
		Request::fixUri(uri);
	}
	for (sVec::const_iterator it = location.getTryFiles().begin(); it != location.getTryFiles().end(); it++)
	{
		if (*it == "$uri")
		{
  			if (stat(uri.c_str(), &stat_data) == 0)
  				if (stat_data.st_mode & S_IFREG)
					return uri;
		}
		else if (*it == "$uri/")
		{
			for (sVec::const_iterator ite = location.getIndex().begin(); ite != location.getIndex().end(); ite++)
			{
				std::string tmp(uri + "/" + *ite);
				Request::fixUri(tmp);
				if (stat(tmp.c_str(), &stat_data) == 0)
  					if (stat_data.st_mode & S_IFREG)
						return tmp;
			}
		}
		else if (it->find_first_not_of("01234567890") == std::string::npos)
			throw Error(this, atoi(it->c_str()));
	}
	throw Error(this, BAD_REQUEST);
	return NULL;
}

void Response::setAllowHeader( void ) {

	std::string	methods;
	sBMap		tmp = _request->getLocation().getAllowedMethods();

	for (sBMap::const_iterator it = tmp.begin(); it != tmp.end(); it++)
		if (it->second)
	  		methods += it->first + ", ";
	if (!methods.empty())
		methods.erase(methods.length() - 2);
	_headers["Allow"] = methods;
}

void Response::setTypeHeader( void ) {
	_headers["Content-Type"] = "text/html";
}

void Response::setLenghtHeader( ) {

	std::stringstream ss;
	ss << _body.size();   
	_headers["Content-Length"] = ss.str();
}

const int			&Response::getSocket( void ) const {

	return _socket;
}

const Request		&Response::getRequest( void ) const {

	return *this->_request;
}

const std::string	&Response::getUri( void ) const {

	return _uri;
}

const sSMap			&Response::getHaders( void ) const {

	return _headers;
}

const std::string	&Response::getBody( void ) const {

	return _body;
}

void Response::setSocket( const int &socket ) {
	
	this->_socket = socket;
}

void	Response::setRequest( const Request &request ) {

	this->_request = &request;
}

void Response::setUri( const std::string &uri ) {

	this->_uri = uri;
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
 
Response::~Response( void ) {

	_headers.clear();
}
