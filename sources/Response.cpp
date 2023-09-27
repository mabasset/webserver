#include "../includes/Response.hpp"

Response::Response( const Request &request, const int fd )
	: _socket(fd), _request (&request), _location(request.getLocation()), _uri(request.getUri()) {

}

void	Response::compile( ) {

	std::string	methods[] = { "GET", "POST", "DELETE", "HEAD", "PUT" };
	int i = 0;
	while (i < 5)
	{
		if (_request->getMethod() == methods[i])
			break ;
		i++;
	}
	if (i == 5)
		throw Error(this, NOT_IMPLEMENTED);
	if (_location.getAllowedMethods().at(_request->getMethod()) == false)
		throw Error(this, NOT_ALLOWED);

	_uri = _request->getUri();
	if (_location.getLocationName() == "/")
		_uri = _location.getRoot() + "/" + _request->getUri();
	else
		_uri.replace(0, _location.getLocationName().size(), _location.getRoot());
	_uri = Request::fixUri(_uri);
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
	if (_status == "NOT")
		return ;
	buffer += _status + "\r\n";
	for(sSMap::const_iterator it = _headers.begin(); it != _headers.end(); it++)
		buffer += (*it).first + ": " + (*it).second + "\r\n";
	if (!_headers.empty())
		buffer += "\r\n";
	buffer += _body;
	//if (buffer.size() < 1000)
	//	std::cout<< buffer << std::endl;
	send(_socket, buffer.c_str(), buffer.size(), 0);
}

void	Response::handleGet( void ) {

	sVec try_files = _location.getTryFiles();
	std::ifstream in;
	struct stat fileStat;
	stat(_uri.c_str(), &fileStat);
	if (std::find(try_files.begin(), try_files.end(), "$uri") != try_files.end() && !S_ISDIR(fileStat.st_mode))
		in.open(_uri.c_str());
	else if (std::find(try_files.begin(), try_files.end(), "$uri/") != try_files.end())
	{
		for (sVec::const_iterator it = _location.getIndex().begin(); it != _location.getIndex().end(); it++)
		{
			in.open(Request::fixUri(_uri + "/" + *it).c_str());
			if (in.is_open())
				break ;
		}
	}
	if (!in.is_open())
		throw Error(this, NOT_FOUND);
	std::cout << _uri << std::endl;
	std::stringstream ss;
	ss << in.rdbuf();
	_body = ss.str();
	this->setTypeHeader();
	this->setLenghtHeader();
	_headers["Connection"] = "close";
	if (_request->getMethod() == "HEAD")
		_body.clear();
	_status = "200 OK";
}

void	Response::handlePut( void ) {

	if (_request->getHeaders().at("Transfer-Encoding") != "chunked")
		throw Error(this, NOT_IMPLEMENTED);
	sVec try_files = _location.getTryFiles();
	std::ofstream out;
	struct stat fileStat;
	stat(_uri.c_str(), &fileStat);
	if (std::find(try_files.begin(), try_files.end(), "$uri") != try_files.end() && !S_ISDIR(fileStat.st_mode))
		out.open(_uri.c_str(), std::ios::out | std::ios::trunc);
	else if (std::find(try_files.begin(), try_files.end(), "$uri/") != try_files.end())
	{
		for (sVec::const_iterator it = _location.getIndex().begin(); it != _location.getIndex().end(); it++)
		{
			out.open(Request::fixUri(_uri + "/" + *it).c_str(), std::ios::out | std::ios::trunc);
			if (out.is_open())
				break ;
		}
	}
	if (!out.is_open())
		throw Error(this, FORBIDDEN);
	for (sVec::const_iterator it = _request->getChunks().begin(); it != _request->getChunks().end(); it++)
		_body += *it;
	std::cout<<"body_size:"<< _body.size() << " uri:"<< _request->getUri() <<std::endl;
	if (_body.size() == 0){
		std::string res = "HTTP/1.1 204 No Content\r\nConnection: close\r\n\r\n";
		send(_socket, res.c_str(), res.size(), 0);
		_status = "NOT";
		return ;
		//throw Error(this, NO_CONTENT);
	}
	if (_location.getClientMaxBodySize() != 0 && _body.size() > _location.getClientMaxBodySize()) {
		std::cout<<"max body:"<<_location.getClientMaxBodySize()<<"body size:"<<_body.size()<<std::endl;
		std::string res = "HTTP/1.1 413 Request Entity Too Large\r\nConnection: close\r\nContent-Length: " + to_String(_body.size()) + "\r\n\r\n" + _body;
		send(_socket, res.c_str(), res.size(), 0);
		_status = "NOT";
		return ;
		//throw Error(this, TOO_LARGE);
	}
	if (_request->getMethod() == "POST")
	{
		std::cout<<"entro POST"<<_request->getUri();
		_body = executeCGI(_body);
		if (_body.find("Status") != std::string::npos)
			_body.erase(0, _body.find("\n"));
		if (_body.find("Content-Type") != std::string::npos)
			_body.erase(0, _body.find("\r\n\r\n") + 4);
		std::cout<<"body size:"<<_body.size() << std::endl;

		std::string res = "HTTP/1.1 201 Created\r\nContent-Type: text/html; charset=utf-8\r\nConnection: close\r\nContent-Length: " + to_String(_body.size()) + "\r\n\r\n" + _body;
		send(_socket, res.c_str(), res.size(), 0);
		_status = "NOT";
		return ;
		//_headers["Connection"] = "close";
	}
	out << _body;
	std::string res = "HTTP/1.1 201 Created\r\nContent-Type: text/html; charset=utf-8\r\nConnection: close\r\nContent-Length: " + to_String(_body.size()) + "\r\n\r\n" + _body;
	send(_socket, res.c_str(), res.size(), 0);
	std::cout<<res<<std::endl;
	_status = "NOT";
	return ;
	// _headers["Content-Location"] = _uri;
	// this->setLenghtHeader();
	// _status = "201 Created";
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
	//  int i = 0;
	//  while (env[i])
	// 		printf("%s\n", env[i++]);

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
		std::string script_path = _location.getCgiPass();
		
		if (script_path != "" && script_path.at(0) == '/')
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
	sSMap prova(_request->getHeaders());
	if (prova["X-Secret-Header-For-Test"] != "")
		envMap.insert(std::make_pair("HTTP_X_SECRET_HEADER_FOR_TEST", prova["X-Secret-Header-For-Test"]));
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

void Response::setAllowHeader( void ) {

	std::string	methods;
	sBMap		tmp = _location.getAllowedMethods();

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

std::string to_String(int num) {
	std::stringstream ss;
	ss << num;
	return ss.str();
}