
#include "Framework/SASTProblem.h"
#include "Framework/SASTPSolution.h"
#include "Framework/SASTPDotPrinter.h"

#include <assert.h>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {

    if (argc != 4) {
	cout << "Usage: " << argv[0] << " <input.prob> <input.sol> <output.dot>" << endl;
	exit(1);
    }
    
    string inputproblem(argv[1]);
    string inputsolution(argv[2]);
    string outputfile(argv[3]);
    std::cout << argv[1]<<" "<<argv[2]<<argv[3]<<" ";
    SASTProblem problem(inputproblem);
    SASTPSolution solution(problem);
    solution.load(inputsolution);
    
    SASTPDotPrinter printer(problem, solution);
    
    //printer.writeTikzFile(outputfile);
    printer.writeDotFile(outputfile, problem.getSpots().size() > 20);
    
    cout << "Done printing" << endl;
    
    return 0;
}
