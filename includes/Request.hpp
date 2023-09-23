#ifndef	REQUEST_HPP
# define REQUEST_HPP

# include "Server.hpp"
# include <sys/stat.h>

typedef std::pair<std::string, Config>	sCPair;

class Request {

	private:
		std::string	_method;
		std::string _uri;
		sSMap		_headers;
		Config		_location;
		sVec		_chunks;

		Request(void);

		const Config	&detectLocation( const sCMap &locationMap );
		void			detectChuncks( const int fd );

	public:
		Request(std::string &all, const sCMap &locationMap, const int fd );
		Request( const Request &src );
		Request	&operator=( const Request &rhs );
		~Request(void);

		static void	fixUri( std::string &uri );
		void		display( void ) const;

		const std::string	&getMethod( void ) const;
		const std::string	&getUri( void ) const;
		const sSMap			&getHeaders( void ) const;
		const Config		&getLocation( void ) const;
		const sVec			&getChunks( void ) const;

		void	setMethod( const std::string &method );
		void	setUri( const std::string &uri );
		void	setHeaders( const sSMap &headers );
		void	setLocation( const Config &location );
		void	setChunks( const sVec &chunks );
};

#endif
