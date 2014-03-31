
#ifndef PARSER_H_
#include "parser.h"
#endif

void MainLoop();

int
main(void)
{
    // initialize binary precedence table
    BinopPrecedenceCreate();

    // create ready-state
    std::cout << "ready> ";
    getNextToken();

    // enter driver, and let driver handle rest
    try{
	MainLoop();
    }
    catch(Error& m){
	m.print();
    }

    return 0;
}
