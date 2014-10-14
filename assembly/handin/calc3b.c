#include <stdio.h>
#include "calc3.h"
#include "y.tab.h"

static int lbl;

#ifndef MYDEBUG
//#define MYDEBUG
#endif

int ex(nodeType *p) {
    int lbl1, lbl2, lbl3;

    if (!p) return 0;
    switch(p->type) 
	{
		case typeCon:       						// Push constants to the stack		ex(p->opr.op[1]);
			#ifdef MYDEBUG 
				printf("typeCon\n"); 
			#endif
//				printf("\tmovl\t$%d, %%eax\n", p->con.value); 
//				printf("\taddl\t$2147483648, %%eax # Add signed byte offset\n"); 
//				printf("\tpushl\t%%eax\n");

				printf("\tpushl\t$%d\n", p->con.value);
			#ifdef MYDEBUG 
				printf("/typeCon\n"); 
			#endif
			break;
		case typeId:        						// Push variables to the stack		ex(p->opr.op[0]);
			#ifdef MYDEBUG 
				printf("typeId\n"); 
			#endif
			printf("\tpushl\t%c\n", p->id.i + 'a'); // The a converts numbers to letters. (+48)

			#ifdef MYDEBUG 
				printf("/typeId\n"); 
			#endif
			break; 	
		case typeOpr:
			#ifdef MYDEBUG 
				printf("typeOpr\n"); 
			#endif
			switch(p->opr.oper) 
			{
				case WHILE:
					#ifdef MYDEBUG 
					printf("typeOpr:WHILE\n"); 
					#endif
					
					printf("L%03d:\n", lbl1 = lbl++);
					ex(p->opr.op[0]);
					printf(" L%03d\n", lbl2 = lbl++);		// Jump
					
					// MY addition
					printf("\tjmp L%03d\n", lbl3 = lbl++);			//
					printf("L%03d:\n", lbl2);		//
			//		printf("This is where I want");
					// /MY addition
			
					ex(p->opr.op[1]);
					printf("jmp L%03d\n", lbl1);
					printf("L%03d:\n", lbl3);
					
					#ifdef MYDEBUG 
					printf("/typeOpr:WHILE\n"); 
					#endif
					break;
				case IF:
					#ifdef MYDEBUG 
					printf("typeOpr:IF\n"); 
					#endif
					ex(p->opr.op[0]);
					if (p->opr.nops > 2) 
					{
						/* if else */
						printf(" L%03d\n", lbl1 = lbl++);
						ex(p->opr.op[1]);
						printf("\tjmp\tL%03d\n", lbl2 = lbl++);
						printf("L%03d:\n", lbl1);
						ex(p->opr.op[2]);
						printf("L%03d:\n", lbl2);
					} 
					else 
					{
						/* if */
						printf(" L%03d\n", lbl1 = lbl++);
						ex(p->opr.op[1]);
						printf("L%03d:\n", lbl1);
					}
					#ifdef MYDEBUG 
					printf("/typeOpr:IF\n"); 
					#endif
					break;
				case PRINT:
					ex(p->opr.op[0]);
					printf("\tcall print\n");
					printf("\taddl\t$%d, %%esp\n", 4);
					break;
				case '=':
					#ifdef MYDEBUG 
					printf("typeOpr:=\n"); 
					#endif
					ex(p->opr.op[1]);
					printf("\tpopl\t%c\n", p->opr.op[0]->id.i + 'a');		// pop from stack to variable
					#ifdef MYDEBUG 
					printf("/typeOpr:=\n"); 
					#endif
					break;
				case UMINUS:
					ex(p->opr.op[0]);
					printf("\tpopl\t%%eax\n");		// pop from stack to variable, second parameter
					printf("\tneg\t%%eax\n");		// pop from stack to variable, second parameter
					
				//	printf("\tneg\n");
						break;
				case FACT:
					ex(p->opr.op[0]);
					printf("\tcall fact\n");
					break;
				case LNTWO:
					ex(p->opr.op[0]);
					printf("\t call lntwo\n");
					break;
				default:
					#ifdef MYDEBUG
						printf("typeOpr:default\n");
					#endif
					ex(p->opr.op[0]);		// Push variable to stack
					ex(p->opr.op[1]);		// Push constant to stack

					switch(p->opr.oper)
					{
						case GCD:
								printf("\tcall gcd\n");
								printf("\taddl $%d, %%esp\n", 8);
								break;
						case '+':   									// +
							printf("\tpopl\t%%eax\n");		// pop from stack to variable, second parameter
							printf("\tpopl\t%%ebx\n");		// pop from stack to variable, first parameter	
							printf("\taddl\t%%eax, %%ebx\n");
							printf("\tpushl\t%%ebx\n");
						break;			
						case '-':   									// -
							#ifdef MYDEBUG 
							printf("typeOpr:default:-\n");
							#endif
							printf("\tpopl\t%%eax\n");		// pop from stack to variable, second parameter
							printf("\tpopl\t%%ebx\n");		// pop from stack to variable, first parameter	
							printf("\tsubl\t%%eax, %%ebx\n");
							printf("\tpushl\t%%ebx\n");

							#ifdef MYDEBUG 
							printf("/typeOpr:default:-\n"); 
							#endif							
							break;
						case '*':   									// *
							printf("\tpopl\t%%eax\n");		// pop from stack to variable, second parameter
							printf("\tpopl\t%%ecx\n");		// pop from stack to variable, first parameter						
							printf("\tmull\t%%ecx\n");
							printf("\tpushl\t%%eax\n");
							break;			
						case '/':   									// /
							printf("\tpopl\t%%eax\n");		// pop from stack to variable, second parameter
							printf("\tpopl\t%%ebx\n");		// pop from stack to variable, first parameter	
							printf("\txorl\t%%edx, %%edx\n");							
							printf("\tdivl\t%%ebx\n");
							printf("\tpushl\t%%ebx\n");
							break;			
						case '<':  // <
							printf("\tpopl\t%%eax\n");		// pop from stack to variable, second parameter
							printf("\tpopl\t%%ebx\n");		// pop from stack to variable, first parameter					
							printf("\tcmpl\t%%eax, %%ebx\n");
							printf("\tjg");
						case '>':   									// >
							#ifdef MYDEBUG 
								printf("typeOpr:default:>\n"); 
							#endif
							printf("\tpopl\t%%eax\n");		// pop from stack to variable, second parameter
							printf("\tpopl\t%%ebx\n");		// pop from stack to variable, first parameter							
							printf("\tcmpl\t%%eax, %%ebx\n");
							printf("\tjl");
							#ifdef MYDEBUG 
							printf("/typeOpr:default:>\n"); 
							#endif
							break;
						case GE:   										// >= in c code, if jge continue. Assembly is usually the other way around */
							printf("\tpopl\t%%eax\n");		// pop from stack to variable, second parameter
							printf("\tpopl\t%%ebx\n");		// pop from stack to variable, first parameter	
							printf("\tcmpl\t%%eax, %%ebx\n");
							printf("\tjge");
							break;
						case LE:    printf("\tLEcompLE\n"); break;		// <= in c code
						case NE:   										// != This is used by the compiler, but for some reason the EQ flag is called after this aswell. And wierdly enough, this dosen't get a jump statement
							#ifdef MYDEBUG 
								printf("typeOpr:default:!=\n"); 
							#endif						
							printf("\tpopl\t%%eax\n");		// pop from stack to variable, second parameter
							printf("\tpopl\t%%ebx\n");		// pop from stack to variable, first parameter						
							printf("\tcmpl\t%%eax, %%ebx\n");	
							printf("\tjne"); // If not equal, continue, otherwise, jump
							#ifdef MYDEBUG 
								printf("/typeOpr:default:!=\n"); 
							#endif					
						case EQ:    									// == && !=
							#ifdef MYDEBUG 
							printf("typeOpr:default:==\n"); 
							#endif
							
							printf("FFFF\n");
							printf("\tpopl\t%%eax\n");		// pop from stack to variable, second parameter
							printf("\tpopl\t%%ebx\n");		// pop from stack to variable, first parameter						
							printf("\tcmpl\t%%eax, %%ebx\n");								
							printf("\tje");
					
							#ifdef MYDEBUG 
							printf("/typeOpr:default:==\n"); 
							#endif
							break;
						default:
						break;
					}
					#ifdef MYDEBUG 
						printf("/typeOpr:default\n"); 
					#endif					
			}
			#ifdef MYDEBUG 
				printf("/typeOpr\n"); 
			#endif
    }
    return 0;
}
