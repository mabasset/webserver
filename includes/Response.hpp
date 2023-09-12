#ifndef	RESPONSE_HPP
# define RESPONSE_HPP

# include "Server.hpp"

enum Status {

	OK,
	BAD_REQUEST,
	FORBIDDEN,
	NOT_FOUND,
	METHOD_NOT_ALLOW,
};

class Request;

class Response {

	private:
		enum Status	_status;
		sSMap		_headers;

		Response( void );

	public:

		Response(const Request &request, const sCMap &locationMap);
		~Response( void );

		void		send( const int fd ) const;
		enum Status detectStatus( const Request &request, const sCMap &locationMap );

		enum Status getStatus( void ) const;
		sSMap		getHaders( void) const;

		void	setStatus( const enum Status &status );
		void	setHeaders( const sSMap &headers );
};

#endif
