#ifndef	CHUNK_HPP
# define CHUNK_HPP

# include <iostream>
# include <sstream>
# include <sys/socket.h>

class Chunk {
	private:
		size_t		_size;
		std::string	_body;
		
		Chunk(void);

	public:
		Chunk( const int fd );
		~Chunk(void);

		size_t		getSize( void ) const;
		std::string getBody( void ) const;

		void		setSize( const size_t size );
		void		setBody( const std::string body );
};

#endif
