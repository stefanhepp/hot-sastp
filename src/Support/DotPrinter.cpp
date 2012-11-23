#include "DotPrinter.h"

#include <iostream>
#include <fstream>
#include <stdexcept>

/*
 * command line to generate the actual graph : neato -Tpng -o outputFile.png input.dot 
 */
void DotPrinter::writeDotFile ( const std::string& filename ) {
    
    // initialize output file and write dot header
    std::ofstream output(filename);
    if(!output)throw std::logic_error("no output stream to "+filename);
    // write out origin spot
    output << "digraph Tour { \n";
    output << " compound = true; \n " << "rankstep = 1.25; \n";
    output << " node[shape = point, fontsize =\"1\"];\n";
    output << problem.getStartAsSpot().getName() << " [ pos= \""<<problem.getStartAsSpot().getX()<<","<<problem.getStartAsSpot().getY()<<"\",style = filled , color =\" .7 .3 1.0 \"];\n";
    output << " edge[arrowsize = 1, color = black];"; 
    
    // write out all spots
   
    for (const Spot* spot : problem.getSpots()) {
	output<<spot->getName() <<"[pos =\""<<spot->getX()<<","<<spot->getY() <<"!\"];\n";
    }

   output << problem.getStartAsSpot().getName()<< " -> ";
    // write out tour as edges connecting spots
    
    int lastSpot = -1;
    for (const Stop node: solution.getTour()) {
	output<<problem.getSpot(node.spot).getName()<<";\n";
	output<<problem.getSpot(node.spot).getName()<<" -> ";
	// edge from lastSpot to node.spot
    }
    output<< problem.getStartAsSpot().getName()<<";\n";
    // write out connection from last spot to origin
    output << "}\n";
    std::cout<<"Done printing"<<std::endl;
}

void DotPrinter::writeTikzFile(const std::string& filename)
{
    std::ofstream out(filename);
    if(!out)throw std::logic_error("no output stream to "+filename);
    
    out << " \\begin{tikzpicture}\n";
    out << "[scale=.5 , auto=left, every node/.style={circle,fill=blue!20}]\n";
    for (Spot* spot:problem.getSpots()){
      out <<"\\node  ("<< spot->getName()<<") at ("<< spot->getX()<<","<<spot->getY()<<") {}; \n";
    }
    
    out << " \\end{tikzpicture}\n";
    
}
