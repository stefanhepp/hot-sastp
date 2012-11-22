#include "DotPrinter.h"

#include <iostream>
#include <fstream>
#include <stdexcept>

/**
 * This is still just some monkey work. It just prints out a .dot file with a directed graph, 
 * in which all spots are printed, and then the connections between nodes which are included 
 * in the graph are done. 
 * 
 * There is a nice way around it, by using boost::graphviz library. I think I will have a look 
 * at that library and try it out so that we do not have to print out the .dot file. 
 * 
 * One change that I will have to do is "layering" the nodes. At the moment the tour looks ugly
 * a very deep graph and that's it! 
 * 
 * One ideea would be to have on each layer a fixed number of nodes (let's say 5) and arrange them
 * so that we get a nicer graph. 
 * 
 */
void DotPrinter::writeDotFile ( const std::string& filename ) {
    
    // initialize output file and write dot header
    std::ofstream output(filename);
    if(!output)throw std::logic_error("no output stream to "+filename);
    // write out origin spot
    output << "digraph Tour { \n";
    output << " compound = true; \n " << "rankstep = 1.25; \n";
    output << " node[shape = box, fontsize =\"8\"];\n";
    output << problem.getStartAsSpot().getName() << " [style = filled , color =\" .7 .3 1.0 \"];\n";
    output << " edge[arrowsize = 1, color = black];"; 
    
    // write out all spots
   
    for (const Spot* spot : problem.getSpots()) {
	output<<spot->getName() <<";\n";
    }
    unsigned i=2;
    output << " { rank = same ; "; 
    for (const Spot* spot : problem.getSpots()){
      if (i == 1 )
      {
	  output<< " { rank = same; ";
      } 
      if ( i == 5)
      {
	output << " };\n";
	i=0;
      }
      
      output << "\"" <<spot->getName()<<"\" ;";
      i++;
            
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
