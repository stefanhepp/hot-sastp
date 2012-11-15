
#include "Framework/SASTProblem.h"
#include "Framework/SASTPSolution.h"

#include <iostream>

using namespace std;

void usage(const char *filename) {
    cout << "Usage: " << filename << " <filename.prob>\n";

}

int main(int argc, char* argv[]) {
    
    if (argc != 2) {
	usage(argv[0]);
	return 1;
    }

    // --- Handle arguments  ---

    string fin(argv[1]);

    

    return 0;
}
