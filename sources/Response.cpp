#include "../includes/Response.hpp"

Response::Response( const Request &request, const int fd )
	: _socket(fd), _request (&request) {

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
	if (_request->getLocation().getAllowedMethods().at(_request->getMethod()) == false)
		throw Error(this, NOT_ALLOWED);
	switch (i) {
		case HEAD:
		case GET: this->handleGet(); break ;
		case POST: this->handlePOST(); break ;
		// case DELETE: this->handleDelete(); break ;
		case PUT: this->handlePut(); break ;
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

	// struct stat s;
	// if( stat(path,&s) == 0 )
	// {
	// 	if( s.st_mode == S_IFDIR )
	// 	{
	// 		sVec index = _location.getIndex();
	// 		for (sVec::const_iterator it = index.begin(); it != index.end(); it++)
	// 		{
	// 			in.open((_request->getTransUri() + "/" + *it).c_str());
	// 			if (in.is_open())
	// 				break ;
	// 		}
	// 	}
	// 	else if ()
	// }
	// DIR *dir = opendir(_request->getTransUri().c_str());
	// std::ifstream in;

	// _status = "200 OK";
	// if (dir != NULL)
	// {
	// 	closedir(dir);
	// 	sVec index = _location.getIndex();
	// 	for (sVec::const_iterator it = index.begin(); it != index.end(); it++)
	// 	{
	// 		in.open((_request->getTransUri() + "/" + *it).c_str());
	// 		if (in.is_open())
	// 			break ;
	// 	}
	// }
	// else
	// 	in.open(_request->getTransUri().c_str());
	// if (!in.is_open())
	// 	throw Error(this, NOT_FOUND);
	// std::stringstream ss;
	// ss << in.rdbuf();
	// _body = ss.str();
	// in.close();
	// this->setTypeHeader();
	// this->setLenghtHeader();
}

void	Response::handlePut( void ) {

	// if (_request->getHeaders().at("Transfer-Encoding") != "chunked")
	// 	throw Error(this, NOT_IMPLEMENTED);

	// std::ofstream file(_request->getTransUri().c_str(), std::ios::out | std::ios::trunc);
	// std::string	body;
	// sVec tmp = _request->getChunks();
	// for (sVec::const_iterator it = tmp.begin(); it != tmp.end(); it++)
	// 	body += *it;

	// if (file.is_open()) {
	// 	file << body;
	// 	file.close();
	// 	_status = "201 Created";
	// 	_headers["Content-Location"] = _request->getTransUri();
	// 	_body = body;
	// 	this->setLenghtHeader();
	// }
}

void	 Response::handlePOST( void ) {

		// std::string response_body;
		// std::string body;
		// sVec tmp = _request->getChunks();
		// for (sVec::const_iterator it = tmp.begin(); it != tmp.end(); it++)
		// 	body += *it;
		// std::ofstream file(_request->getTransUri().c_str(), std::ios::out | std::ios::trunc);
		// if (file.is_open()) {
		// 	std::cout<<"sono dentro"<<std::endl;
		// 	response_body = executeCGI(body);
		// 	if (response_body.find("Status") != std::string::npos)
		// 		response_body.erase(0, response_body.find("\n"));
		// 	if (response_body.find("Content-Type") != std::string::npos)
		// 		response_body.erase(0, response_body.find("\r\n\r\n") + 4);
		// 	file << response_body;
		// 	file.close();
		// 	_status = "201 Created";
		// 	_headers["Content-Type"] = "application/json";
		// 	_headers["Transfer-Encoding"] = "chunked";
		// 	_headers["Accept-Encoding"] = "gzip";
		// 	std::stringstream ss;
		// 	ss << response_body.size();
		// 	_headers["Content-Length"] = ss.str();
		// 	//std::cout<<"BODY:"<<_body<<std::endl;
		// 	return ;
		// }
		// file.close();
		//if (checkTryFiles("$uri/", location)){
}

//std::string	Response::executeCGI(std::string &content){
// 	pid_t	pid;
// 	int		std_cpy[2] = { dup(0), dup(1) };
// 	char	**env = getEnvCgi();
// 	std::string _retBody = "";

// 	FILE*	in = tmpfile();
// 	FILE*	out = tmpfile();
// 	int		fd_in = fileno(in);
// 	int		fd_out = fileno(out);

// 	// UNCOMMENT TO PRINT ENV
// 	 int i = 0;
// 	 while (env[i])
// 			printf("%s\n", env[i++]);

// 	// use tmpFile() instead of pipe() to handle big amount of data

// 	write(fd_in, content.c_str(), content.size());
// 	lseek(fd_in, 0, SEEK_SET);

// 	pid = fork();

// 	if (pid == -1) {
// 		std::cout << "Fork failed" << std::endl;
// 		return ("Status: 500\r\n\r\n");
// 	}
// 	else if (!pid) {
// 		std::string uri = _request->getTransUri();
// 		size_t pos = uri.find("//");
// 		if (pos != std::string::npos)
// 			uri.erase(pos, 1);
// 		const char *filepath = uri.c_str();
// 		std::string script_path = _location.getCgiPass();
// 		if (script_path.at(0) == '/')
// 			script_path.erase(0, 1);
// 		char *const args[3] = {strdup(script_path.c_str()), strdup(filepath), NULL};
// 		// std::cout<<"args0 "<<args[0]<<std::endl;
// 		// std::cout<<"args1 "<<args[1]<<std::endl;
// 		// std::cout<<"args2 "<<args[2]<<std::endl;
// 		dup2(fd_in, 0);
// 		dup2(fd_out, 1);
// 		execve(args[0], args, env);
// 		std::cout << "exxxecve failed" << std::endl;
// 		write(1, "Status: 500\r\n\r\n", 15);
// 		exit (0);
// 	}
// 	else {
// 		waitpid(-1, NULL, 0);

// 		lseek(fd_out, 0, SEEK_SET);

// 		char	buffer[1024];
// 		size_t  dataRead = 0;
// 		while ((dataRead = read(fd_out, buffer, sizeof buffer)) > 0) {
// 			for (size_t i = 0; i < dataRead; i++)
// 				_retBody.push_back(buffer[i]);
// 			memset(buffer, 0, sizeof buffer);
// 		}
// 	}
// 	fclose(in);
// 	fclose(out);
// 	close(fd_in);
// 	close(fd_out);

// 	dup2(std_cpy[0], 0);
// 	dup2(std_cpy[1], 1);

// 	for (short i = 0; env[i] != NULL; i++)
// 		delete[] env[i];
// 	delete[] env;
// 	return _retBody;
// }

// char	**Response::getEnvCgi() {
// 	sSMap envMap;

// 	envMap.insert(std::make_pair("AUTH_TYPE", ""));
// 	envMap.insert(std::make_pair("CONTENT_LENGTH", ""));
// 	envMap.insert(std::make_pair("CONTENT_TYPE", "application/x-www-form-urlencoded"));
// 	envMap.insert(std::make_pair("GATEWAY_INTERFACE", "CGI/1.1"));
// 	envMap.insert(std::make_pair("PATH_INFO", _request->getUri()));
// 	envMap.insert(std::make_pair("PATH_TRANSLATED", _request->getTransUri()));
// 	envMap.insert(std::make_pair("QUERY_STRING", ""));
// 	envMap.insert(std::make_pair("REMOTE_ADDR", _request->getHeaders().at("Host")));
// 	envMap.insert(std::make_pair("REMOTE_HOST", ""));
// 	envMap.insert(std::make_pair("REMOTE_IDENT", ""));
// 	envMap.insert(std::make_pair("REMOTE_USER", ""));
// 	envMap.insert(std::make_pair("REQUEST_METHOD",  _request->getMethod()));
// 	envMap.insert(std::make_pair("REQUEST_URI", _request->getUri()));
// 	envMap.insert(std::make_pair("SCRIPT_NAME", "ubuntu_cgi_tester"));
// 	envMap.insert(std::make_pair("SERVER_NAME", "http://" + _request->getHeaders().at("Host")));
// 	envMap.insert(std::make_pair("SERVER_PORT", "8080" /*locatio.serverport()*/));
// 	envMap.insert(std::make_pair("SERVER_PROTOCOL", "HTTP/1.1"));
// 	envMap.insert(std::make_pair("SERVER_SOFTWARE", "Webserv/1.0"));
// 	envMap.insert(std::make_pair("REDIRECT_STATUS", "200"));
// 	// if (_request.getHeader().find("X-Secret-Header-For-Test") != )
// 	// 	envMap.insert(std::make_pair("HTTP_X_SECRET_HEADER_FOR_TEST", _requestMap["X-Secret-Header-For-Test"]));}ert(std::make_pair("HTTP_X_SECRET_HEADER_FOR_TEST", conn.headers.substr(conn.headers.find("X-Secret") + 26, conn.headers.find_first_of("\r\n"))));
// 	char	**env = new char*[envMap.size() + 1];
// 	int	j = 0;
// 	for (sSMap::const_iterator i = envMap.begin(); i != envMap.end(); i++) {
// 		std::string	element = i->first + "=" + i->second;
// 		env[j] = new char[element.size() + 1];
// 		env[j] = strcpy(env[j], (const char*)element.c_str());
// 		//std::cout<<"riga env:"<<env[j]<<std::endl;
// 		j++;
// 	}
// 	env[j] = NULL;
// 	return env;
//}

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
	_headers.clear();
}
