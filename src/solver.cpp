
#include "Framework/SASTProblem.h"
#include "Framework/SASTPSolution.h"
#include "Support/Environment.h"


int main(int argc, char* argv[]) {
    
    Environment env;
    
    if (int ret = env.parseArguments(argc, argv)) {
	return ret;
    }
    
    
    

    return 0;
}
