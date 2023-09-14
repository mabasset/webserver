#ifndef	RESPONSE_HPP
# define RESPONSE_HPP

# include "Server.hpp"

class Request;

class Response {

	private:
		std::string		_status;
		sSMap			_headers;
		std::string		_body;

		Response( void );

	public:

		Response( const Request &request, const sCMap &locationMap);
		~Response( void );

		void		sendResponse( const int fd ) const;
		std::string	detectStatus( const Request &request, const sCMap &locationMap );
		std::string	readFile( const Request &request, const Config &location);

		std::string	getStatus( void ) const;
		sSMap		getHaders( void) const;

		void	setStatus( const std::string &status );
		void	setHeaders( const sSMap &headers );
};

#endif
