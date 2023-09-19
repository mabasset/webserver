#ifndef	RESPONSE_HPP
# define RESPONSE_HPP

# include "Server.hpp"

class Request;
class Config;

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
<<<<<<< HEAD
		int			_socket;
		Request const *_request;
		Config		_location;
		std::string	_status;
		sSMap		_headers;
		std::string	_body;
=======
		std::string		_status;
		sSMap			_headers;
		std::string		_body;
		int				_fd;
		Request			*_request;
		Config			*_location;
>>>>>>> origin/frudello

		Response( void );

	public:

<<<<<<< HEAD
		Response( const Request &request, const int fd);
		~Response( void );

		std::string	translateUri( const Request &request );
		void		compile( void );
		void		commit( void ) const;

		void		handleGet( void );

		Request		getRequest( void ) const;
		int			getSocket( void ) const;
		sSMap		getHaders( void) const;

		void	setRequest( Request request );
		void	setStatus( const std::string &status );
		void	setHeaders( const sSMap &headers );
		void	setBody( const std::string &body );

		class Error {
			private:
				Response	*_response;
				int			_code;
				Error( void );
			public:
				Error( Response *response, const int &code );
				~Error( void );
				void		editResponse( std::string const &root, iSMap const &errorPageMap );
				Response	getResponse( void ) const;
				int			getCode( void ) const;
				void		setResponse( Response response );
				void		setCode( const int code );
		};
=======
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

>>>>>>> origin/frudello
};

#endif
