#include "../includes/Cluster.hpp"

int main(int argc, char *argv[]){
	(void)argc;
	try {
		Cluster	cluster(argv[1]);
		cluster.startListening();
	}
	catch(std::exception &e) {
		std::cout << e.what() << std::endl;
		return 1;
	}
	return 0;
}