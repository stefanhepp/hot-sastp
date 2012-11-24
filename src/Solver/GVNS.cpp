#include "GVNS.h"

GVNS::GVNS(Environment& env, const Instance& init)
: AbstractSearch(env, env.getConfig().getMaxStepsNoChange()), instance(init)
{
}

void GVNS::run()
{

}

