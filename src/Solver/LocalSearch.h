#ifndef LOCALSEARCH_H
#define LOCALSEARCH_H

#include "Framework/SASTProblem.h"
#include "Support/Environment.h"
#include "Support/Instance.h"
#include "Support/SpotSearch.h"
#include "Support/ProblemHelper.h"

class LocalSearch
{
    SASTProblem& problem;
    Instance instance;

public:
    LocalSearch(Environment& env, const Instance& init);

    Instance& getInstance() { return instance; };
    
    void run();
    
};

#endif // LOCALSEARCH_H
