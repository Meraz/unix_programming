#include "calc3.h"
extern NBlock* programBlock;
extern int yyparse();

int main(int argc, char **argv)
{
    yyparse();
   printf(programBlock);
    return 0;
}
