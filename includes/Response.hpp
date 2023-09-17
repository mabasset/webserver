#ifndef	RESPONSE_HPP
# define RESPONSE_HPP

# include "Server.hpp"

class Request;
class Config;

class Response {

	private:
		std::string		_status;
		sSMap			_headers;
		std::string		_body;
		int				_fd;
		Request			*_request;
		Config			*_location;

		Response( void );

	public:

		Response(Config &location, Request &request, const sCMap &locationMap, const int fd);
		~Response( void );

		void		sendResponse() const;
		std::string	detectStatus( const Request &request, const sCMap &locationMap );
		std::string	readFile( const Request &request, const Config &location);
		std::string getChunks();
		void		handlePUT( const Request &request);
		void 		handlePUTChunked();
		void		handlePOST( void );
		std::string	executeCGI(std::string &content);


		std::string	getStatus( void ) const;
		sSMap		getHeaders( void ) const;
		int			getFd( void ) const;
		char		**getEnvCgi();

		void	setFd( int fd );
		void	setStatus( const std::string &status );
		void	setHeaders( const sSMap &headers );

};

#endif
