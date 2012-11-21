#ifndef DOTPRINTER_H
#define DOTPRINTER_H

#include "Framework/SASTProblem.h"
#include "Framework/SASTPSolution.h"

#include <string>

class DotPrinter {

public:
    DotPrinter(const SASTProblem& problem, const SASTPSolution& solution) : problem(problem), solution(solution) {}
    
    void writeDotFile(const std::string& filename);
    
private:
    const SASTProblem& problem;
    const SASTPSolution& solution;
    
};

#endif // DOTPRINTER_H
