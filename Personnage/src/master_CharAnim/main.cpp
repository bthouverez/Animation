#include "CharAnimViewer.h"

using namespace std;

int main( int argc, char **argv )
{
	if(argc != 3 && argc != 2) {
		std::cout << "Usage : ./bin/master_CharAnim <bvh_start_file> [<bvh_directory>]" << std::endl << std::endl;
		std::cout << "\t<bvh_start_file> : fichier BVH  pour l'animation de départ du personnage" << std::endl;
		std::cout << "\t<bvh_directory> : chemin vers répertoire contenant des fichiers BVH pour Motion graph" << std::endl;
		exit(2);
	}

	char * arg2 = (char*) malloc( 100 ); // reserve 100 bytes in memory
	strcpy(arg2, "null"); 

	if(argc == 3) strcpy(arg2, argv[2]);

    CharAnimViewer v(argv[1], arg2);
    v.run();
    return 0;
}