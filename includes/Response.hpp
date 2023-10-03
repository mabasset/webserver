#ifndef	RESPONSE_HPP
# define RESPONSE_HPP

# include "Server.hpp"
# include <sys/types.h>
# include <sys/stat.h>
# include <climits>
# include <stdio.h>
# include <dirent.h>
# include <ctime>

class Request;

enum Status {
	CONTINUE = 100,
	OK = 200,
	CREATED = 201,
	ACCEPTED = 202,
	NON_AUTHORITATIVE = 203,
	NO_CONTENT = 204,
	RESET_CONTENT = 205,
	PARTIAL_CONTENT = 206,
	BAD_REQUEST = 400,
	UNAUTHORIZED = 401,
	PAYMENT_REQUIRED = 402,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	NOT_ALLOWED = 405,
	NOT_ACCEPTABLE = 406,
	LENGHT_REQUIRED = 411,
	TOO_LARGE = 413,
	SERVER_ERROR = 500,
	NOT_IMPLEMENTED = 501,
	NOT_EXTENDED = 510
};

enum Method {
	GET,
	POST,
	DELETE,
	HEAD,
	PUT,
};

class Response {

	private:
		int				_socket;
		Request const	*_request;
		Config			_location;
		std::string		_uri;
		std::string		_status;
		sSMap			_headers;
		std::string		_body;

		Response( void );

		void	handleGet( void );
		void	handlePut( void );
		//void	handlePOST( void );
		void	handleDelete( void );
		void	autoindexPage( void );
		void	redirectPage( void );
		void	executeCGI( void );
		char	**getEnvCgi( void );
		std::string gen_random( const int len);

	public:
		Response( const Request &request, const int fd);
		~Response( void );

		void	compile( void );
		void	commit( void ) const;

		void	setAllowHeader( void );
		void	setTypeHeader( void );
		void	setLenghtHeader( void );

		const int			&getSocket( void ) const;
		const Request		&getRequest( void ) const;
		const std::string	&getUri( void ) const;
		const std::string	&getStatus( void ) const;
		const sSMap			&getHaders( void) const;
		const std::string	&getBody( void ) const;

		void	setSocket( const int &socket );
		void	setRequest( const Request &request );
		void 	setUri( const std::string &uri );
		void	setStatus( const std::string &status );
		void	setHeaders( const sSMap &headers );
		void	setBody( const std::string &body );

};

#endif
