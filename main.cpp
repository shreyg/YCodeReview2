#include <fstream>
#include <stdlib.h>
#include <queue>
#include <string>
#include <sstream>
#include "fwk/String.h"
#include "Tissue.h"
#include <map>
#include <set>
#include <deque>
using namespace std;


class TissueReactor : public Tissue::Notifiee {
public:
   void onCellNew(Cell::Ptr cell) {
	  cell->membraneNew("MembraneN", CellMembrane::north());
	  cell->membraneNew("MembraneS", CellMembrane::south());
	  cell->membraneNew("MembraneE", CellMembrane::east());
	  cell->membraneNew("MembraneW", CellMembrane::west());
	  cell->membraneNew("MembraneU", CellMembrane::up());
	  cell->membraneNew("MembraneD", CellMembrane::down());
	  int val;
	  // Intialize Cell Membranes strengths based on Cell Type
	  val=(cell->cellType() == Cell::cytotoxicCell())?100:0;
	  for (Cell::MembraneIterator i = cell->membraneIter(); i; ++i) {
		CellMembrane* cm = i.ptr()->ptr(); 
		cm->antibodyStrengthIs(val);
		}
	  if(cell->cellType() == Cell::cytotoxicCell()){
		  c_cells++;
	  }
	  else{
		  h_cells++;
	  }
   }
   // Update cell ccount on Cell Deletion
   void onCellDel(Cell::Ptr cell) {
	   if(cell->cellType() == Cell::cytotoxicCell()){
		  c_cells--;
	  }
	  else{
		  h_cells--;
	  }
	}
   // Helper function for retreiving values
   int num_c_cells(){
	   return c_cells;
   }

   int num_h_cells(){
		return h_cells;
   }

   static TissueReactor * TissueReactorIs(Tissue *t) {
      TissueReactor *m = new TissueReactor(t);
      return m;
   }
protected:
   TissueReactor(Tissue *t) : Tissue::Notifiee() {
      notifierIs(t);
	  c_cells =0;
	  h_cells =0;
   }

   int c_cells;
   int h_cells;;
};

int handle(string);
Cell::Coordinates transform(Cell::Coordinates, string);
CellMembrane::Side mapDirToSide(string dir);
void infectionspread(string& cmd, map<string,Tissue::Ptr>& tissues, map<string,TissueReactor*>& reactors);
void process_infection(queue<Cell::Ptr>& in_queue,int& b,int& c,int& g,int& i_strength, map<string,Tissue::Ptr>& tissues,string& tissue_id, map<string,TissueReactor*>& reactors);
void makeNewCell(string textLine,map<string,Tissue::Ptr>& tissues,int command);
void removeInfecCells(string textLine,map<string,Tissue::Ptr>& tissues);
void cloneSingleCell(string textLine,map<string,Tissue::Ptr>& tissues);
void setSideStrength(string textLine,map<string,Tissue::Ptr>& tissues);
void cloneCells(string textLine,map<string,Tissue::Ptr>& tissues);


/*
  The main takes in one input, the file name with the rules.
  The rules are then executed and the appropriate statistics are printed
  to the console.
*/

int main(int argc, const char* argv[]) {
  ifstream infile(argv[1]);
  if(infile.fail()){
    //File error. Halt program.
    cerr << "error reading file" << endl;
    return 1;
  }

	map<string,Tissue::Ptr> tissues;
	map<string,TissueReactor*> reactors;

  //read data in, parse it, excute commands.
  string textLine;
  string buffer;
  stringstream strm;

  while(!infile.eof()){
    getline(infile, textLine);
	if(textLine[0]=='#' || !textLine.compare("\n")  || !textLine.compare("") || textLine.length()==0){
		continue;
	}
	else{
		int command = handle(textLine);
	
		strm.clear();
		strm.str(textLine);

		if(command==0){
			cerr << "Skipping Malformed Command: " << textLine << endl;
			continue;
		}

		else if(command == 1){
			strm >> buffer >> buffer >> buffer;
			tissues[buffer] = Tissue::TissueNew(buffer);
			reactors[buffer] = TissueReactor::TissueReactorIs((tissues[buffer]).ptr());
		}

		else if(command==2 || command ==3){
			makeNewCell(textLine,tissues,command);
		}

		else if(command == 4){
				infectionspread(textLine,tissues,reactors);
		}

		else if(command==5){
			removeInfecCells(textLine,tissues);
		}

		else if(command==6){
			cloneSingleCell(textLine,tissues);
		}

		else if(command==7){
			cloneCells(textLine,tissues);
		}



		else if(command==8){
			setSideStrength(textLine,tissues);
		}

	}
  }
  return 0;
}





//---------------------- MAIN ENDS

/*
Examines the command read from inut file and returns a numerical code for the same
*/
int handle(string command){
	stringstream strm(command);
	string option;
	strm >> option;
	if(option.compare("Tissue")==0){
		strm >> option;
		if(option.compare("tissueNew")==0){
			return 1;
		}
		strm >> option;
		if(option.compare("cytotoxicCellNew")==0){
			return 2;
		}
		else if(option.compare("helperCellNew")==0){
			return 3;
		}
		else if(option.compare("infectionStartLocationIs")==0){
			return 4;
		}
		else if(option.compare("infectedCellsDel")==0){
			return 5;
		}
		else if(option.compare("cloneCellsNew")==0){
			return 7;
		}
	}
	if(option.compare("Cell")==0){
		int x,y,z;
		strm >> option;
		strm >> x >> y >> z;
		strm >> option;
		if(option.compare("cloneNew")==0)
			return 6;
		else
			return 8;
	}
	
	return 0;
}

Cell::Coordinates transform(Cell::Coordinates coord, string dir){
	if(dir.compare("north")==0){
		coord.y+=1;
	}
	else if(dir.compare("south")==0){
		coord.y-=1;
	}
	if(dir.compare("east")==0){
		coord.x+=1;
	}
	else if(dir.compare("west")==0){
		coord.x-=1;
	}
	if(dir.compare("up")==0){
		coord.z+=1;
	}
	else if(dir.compare("down")==0){
		coord.z-=1;
	}
	return coord;
}

CellMembrane::Side mapDirToSide(string dir){
	if(dir.compare("north")==0){
		return CellMembrane::north();
	}
	else if(dir.compare("south")==0){
		return CellMembrane::south();
	}
	if(dir.compare("east")==0){
		return CellMembrane::east();
	}
	else if(dir.compare("west")==0){
		return CellMembrane::west();
	}
	if(dir.compare("up")==0){
		return CellMembrane::up();
	}
	else if(dir.compare("down")==0){
		return CellMembrane::down();
	}
	return CellMembrane::north();
}

/**
This function handles the infection spread on the tissue from the specified location and with given infection strength.
After Processing the intial root cell, it calls process_infection(..), which is a recursive module to simulate the
breadth-first exapnsion of infection.
The parameters for infection round are set up and processed with call to process_infection(..)
After return from process_infection(..) , the function calculate variable a and f for the round
**/

void infectionspread(string& cmd, map<string,Tissue::Ptr>& tissues, map<string,TissueReactor*>& reactors){
				string buffer;
				stringstream strm;
				strm.str(cmd);
				string tissue_id, dir;
				
				strm >> buffer >> tissue_id >> buffer;
				int x,y,z, i_strength;
				strm >> x >> y >> z >> dir >> i_strength;
				Cell::Coordinates coord = Cell::Coordinates();
				coord.x =x;
				coord.y =y;
				coord.z =z;

				Cell::Ptr start_cell;

				// Read Input till here

				start_cell = tissues[tissue_id]->cell(coord);
				if (start_cell==Cell::Ptr(0)){
					cerr << " Cell Not Found, Doing Nothing"<< endl;
						return ;
				}
			
				
				// We will print some output in every case now
				int a,b,c,d,e,f,g;
				a = b = c = g = f = 0;
				d = reactors[tissue_id]->num_c_cells();
				e = reactors[tissue_id]->num_h_cells();	

				if(start_cell->health()==Cell::infected()){
					//This cell is already infected
						g=0;
				}

				b++; //attempt will be made
				int diff = ((start_cell->membrane(mapDirToSide(dir)))->antibodyStrength()).value() - i_strength;
				diff = diff*-1;
				c = c + diff;

				if(diff<=0){
					// No infection possible for the root cell
					g =0;
					cerr << "No Infection Possible" << endl;
				}
				else{
					// This cell has been successfully Infected
					// put it in a queue that would be feeded to recursive function that starts from infected cells
					// and ifects other cell in their vicinity in a single pass
					g++;
					start_cell->healthIs(Cell::infected());		//Infect it
					queue<Cell::Ptr> out_queue;
					out_queue.push(start_cell);
					process_infection(out_queue,b,c,g,i_strength,tissues,tissue_id,reactors);
				}
				// Computer f and a here for tissue we already have d,e
				int x_max,y_max,z_max,x_min,y_min,z_min;
				x_max = x_min = x;
				y_min = y_max = y;
				z_min = z_max = z;

				for (Tissue::CellIteratorConst i = tissues[tissue_id]->cellIterConst(); i; ++i) {
					if(i->health()==Cell::infected()){
						a++;
						Cell::Coordinates temp = i->location();
						if (temp.x < x_min){
							x_min = temp.x;
						}
						if (temp.x > x_max){
							x_max = temp.x;
						}
						if (temp.y < y_min){
							y_min = temp.y;
						}
						if (temp.y > y_max){
							y_max = temp.y;
						}
						if (temp.z < z_min){
							z_min = temp.z;
						}
						if (temp.z > z_max){
							z_max = temp.z;
						}
					}
				}

				f = (a==0)?0:((x_max-x_min+1)*(y_max-y_min+1)*(z_max-z_min+1));
				cout << a << " " << b << " " << c << " " << d << " " << e << " " << f << " " << g << endl;
}

/**
This recursive function process all the infected cells received in the parameter 'in_queue' and process all its neigbors
Any neigbors of these infected cells, which are succesfully infected are stored for processing in next call of the
function.
The other parameters for infection round are contibously updated as and when infected attempts take place.
**/

void process_infection(queue<Cell::Ptr>& in_queue,int& b,int& c,int& g,int& i_strength,map<string,Tissue::Ptr>& tissues,string& tissue_id, map<string,TissueReactor*>& reactors){

	queue<Cell::Ptr> out_queue;

	while(!in_queue.empty()){
		// For each element in in_queue we process each each neighbor 
		Cell::Ptr base = in_queue.front();
		Cell::Coordinates base_cood = base->location();
		in_queue.pop();

		string sides[]={"north","south","east","west","up","down"};
		string o_sides[]={"south","north","west","east","down","up"};
		
		for(int i=0;i<6;++i){
			Cell::Ptr current = tissues[tissue_id]->cell(transform(base_cood,sides[i]));
			
			if(current!=Cell::Ptr(0)){
				// skipping cases of Non-existent cells
				
				if(current->health()==Cell::infected()){
					continue;
				}

				b++; //attempt will be made
				int diff = ((current->membrane(mapDirToSide(o_sides[i])))->antibodyStrength()).value() - i_strength;
				diff = diff*-1;
				c = c + diff;

				if(diff>0){
				// We infect this neighbor and add to next round
					current->healthIs(Cell::infected());
					out_queue.push(current);
				}

			}

		}

	}
	if(out_queue.size()>0){
		g++; // We have extended in some direction in breadth plus
		// Recursively call this function
		process_infection(out_queue,b,c,g,i_strength,tissues,tissue_id,reactors);
	}
}

/**
Create a new cell of the specified type as part of the given tissue and given location
Any exceptions raised are caught and default to no action
**/

void makeNewCell(string textLine,map<string,Tissue::Ptr>& tissues,int command){
			string buffer;
			stringstream strm;
			strm.str(textLine);
			string tissue_id;
			strm >> buffer >> tissue_id;
			strm >> buffer;
			int x,y,z;
			strm >> x >> y >> z;
			Cell::Coordinates coord = Cell::Coordinates();
			coord.x =x;
			coord.y =y;
			coord.z =z;
			try{
				if(tissues.count(tissue_id)){
					if(command ==2){
						try{
						(tissues[tissue_id])->cellIs(Cell::CellNew(coord, (tissues[tissue_id]).ptr(), Cell::cytotoxicCell()));
						}
						catch(...){
							cerr << "Exception Caught";
						}
					}
					else{
						try{
						(tissues[tissue_id])->cellIs(Cell::CellNew(coord, (tissues[tissue_id]).ptr(), Cell::helperCell()));
						}
						catch(...){cerr << "Exception Caught";}
					}
				}

			}
			catch(...){
				cerr << "Exception Caught";
			}
}

/**
This function removes the infected cells from the specified tissue.
The Notifier/Notifee constructs ensure the corresponding updates
**/

void removeInfecCells(string textLine,map<string,Tissue::Ptr>& tissues){
			string buffer;
			stringstream strm;
			strm.str(textLine);
			string tissue_id;
			strm >> buffer >> tissue_id;
			Tissue::Ptr tissue = tissues[tissue_id];
			
			for (Tissue::CellIteratorConst i = tissue->cellIterConst(); i; ++i) {
				 for (Cell::MembraneIteratorConst j = i->membraneIterConst(); j; ++j) {
				}
				if(i->health() == Cell::infected()){
					tissue->cellDel(i->name());
				}
			}
}

/**
This function clones a single cell in the specified tissue and in the specified direction
Any exceptions raised are caught and ignored
**/

void cloneSingleCell(string textLine,map<string,Tissue::Ptr>& tissues){
			string buffer;
			stringstream strm;
			strm.str(textLine);
			string tissue_id,dir;
			strm >> buffer >> tissue_id;
			int x,y,z;
			strm >> x >> y >> z;
			Cell::Coordinates coord = Cell::Coordinates();
			coord.x =x;
			coord.y =y;
			coord.z =z;
			strm >> buffer >> dir;
			Cell::Ptr cell1 = (tissues[tissue_id])->cell(coord);
			Cell::Coordinates coordnew = transform(coord,dir);
			try{
			(tissues[tissue_id])->cellIs(Cell::CellNew(coordnew, (tissues[tissue_id]).ptr(), cell1->cellType()));
			Cell::Ptr cell2 = (tissues[tissue_id])->cell(coordnew);
			for (Cell::MembraneIterator i = cell2->membraneIter(); i; ++i) {
				CellMembrane * cm = i.ptr()->ptr(); 
				cm->antibodyStrengthIs(cell1->membrane(cm->side())->antibodyStrength());
			}
			cell2->healthIs(cell1->health());
			}
			catch(...){cerr << "Exception Caught";}
}

/**
This function is used to set the Antibody strength of the soecified membarane of the specified cell to the specified value.
**/

void setSideStrength(string textLine,map<string,Tissue::Ptr>& tissues){
			string buffer;
			stringstream strm;
			strm.str(textLine);
			string tissue_id,dir;
			strm >> buffer >> tissue_id;
			int x,y,z,strength;
			strm >> x >> y >> z;
			Cell::Coordinates coord = Cell::Coordinates();
			coord.x =x;
			coord.y =y;
			coord.z =z;
			strm >> buffer >> dir;
			strm >> buffer >> strength;
			((tissues[tissue_id])->cell(coord))->membrane(mapDirToSide(dir))->antibodyStrengthIs(strength);
}

/**
This function clones all the cells in the specified tissue and in the specified direction
Any exceptions raised are caught and ignored
**/

void cloneCells(string textLine,map<string,Tissue::Ptr>& tissues){
			string buffer;
			stringstream strm;
			strm.str(textLine);
			string tissue_id;
			string dir;
			strm >> buffer >> tissue_id >> buffer >> dir;
			set<string> generated;
				for (Tissue::CellIteratorConst i = tissues[tissue_id]->cellIterConst(); i; ++i) {
				//We need to check that we are not processing a cell generated in current cycle
					Cell::Coordinates coord = i->location();
					Cell::Coordinates coordnew = transform(coord,dir);
					Cell::Ptr cell2;
					if(generated.count(i->name())==0){
						try{
							cell2 = (tissues[tissue_id])->cellIs(Cell::CellNew(coordnew, (tissues[tissue_id]).ptr(), i->cellType()));					
							generated.insert(cell2->name());
							for (Cell::MembraneIterator j = cell2->membraneIter(); j; ++j) {
								CellMembrane * cm = j.ptr()->ptr(); 
								cm->antibodyStrengthIs(i->membrane(cm->side())->antibodyStrength());
							}
							cell2->healthIs(i->health());
						}
						catch(...){}
					} // ends if
				}//end for


}
