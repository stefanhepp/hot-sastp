#include "DotPrinter.h"

void DotPrinter::writeDotFile ( const std::string& filename ) {
    
    // initialize output file and write dot header
    
    // write out origin spot
    
    
    
    // write out all spots
    
    for (const Spot* spot : problem.getSpots()) {
	
    }
    
    // write out tour as edges connecting spots
    
    int lastSpot = -1;
    for (Stop node: solution.getTour()) {
	
	// edge from lastSpot to node.spot
	
	
    }
    
    // write out connection from last spot to origin
    
}
