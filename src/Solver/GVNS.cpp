#include "GVNS.h"

GVNS::GVNS(Environment& env, const Instance& init)
  // TODO get number of steps without improvement from config
: AbstractSearch(env, 10), instance(init)
{
}

void GVNS::run()
{

}

