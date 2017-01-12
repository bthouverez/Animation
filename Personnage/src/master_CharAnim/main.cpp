#include "CharAnimViewer.h"

using namespace std;






int main( int argc, char **argv )
{
	if(argc != 2) {
		std::cout << "Usage : ./bin/master_CharAnim <bvh>" << std::endl;
		exit(2);
	}

    CharAnimViewer v(argv[1]);
    v.run();
    return 0;
}
