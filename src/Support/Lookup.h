#ifndef LOOKUP_H
#define LOOKUP_H

#include "Framework/SASTProblem.h"

#include <vector>

class Lookup
{
    const SASTProblem& problem;
public:
    Lookup(const SASTProblem& problem);
    
    void initialize(unsigned maxk);
    
    
};

#endif // LOOKUP_H
