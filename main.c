
#include "assembler.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "symboltable.h"
#include "cmdtable.h"
#include "datatable.h"
#include "saveFiles.h"


static bool handleLine(input_line*);
static bool handleLine2(input_line*);
static addressing getArgWord(const char *str, word *wrd);

FLAG firstRun(FILE *src) {
	input_line *line;
	int lineIndex;
	fseek(src, 0L, SEEK_SET);
	for (lineIndex = 1; true; lineIndex++) { /* Runs through all the lines. */
		if ((line = getLine(src))) {
			if (line->isEOF) {
				freeLine(line);
				break; /* End when EOF is encountered */
			}
			if (line->isEffectless) {
				continue;
			}
			if (!handleLine(line)) {
				if (cmd_list.length + data_list.length >= MAX_MEMORY_SIZE) {
					error(sprintf(errMsg, OUT_OF_STORAGE));
					report(lineIndex);
					return flag;
				}
				report(lineIndex);
			}
		} else {
			report(lineIndex);
		}
		freeLine(line);
	}
	return flag;
}/* End firstRun */

/* Analyses an line of code and operates accordingly */
static bool handleLine(input_line* line) {
	int isOp = false;
	int isExt = false;
	int hasLabel = (line->label ? true : false);
	int wordCounter = 0;
	switch (line->cmd) {
	/* Two operands group */
	case MOV:
	case CMP:
	case ADD:
	case SUB:
	case LEA:
		if (line->args != NULL) {
			if (line->args[1] != NULL) {
				if (!(validReg(line->args[0]) && validReg(line->args[1]))) { /* If not register to register*/
					wordCounter++; /* Two extra words*/
				}
			} else {
				error(sprintf(errMsg, TOO_LESS_ARGS));
				return false;
				/* Expected 2 args get one error */
			}
		} else {
			error(sprintf(errMsg, TOO_LESS_ARGS));
			return false;
			/*Expected 2 args error */
		}
		/* no break */
	case NOT:/* One operands group */
	case CLR:
	case INC:
	case DEC:
	case JMP:
	case BNE:
	case RED:
	case PRN:
	case JSR:
		wordCounter++; /* One extra word*/
		/* no break */
	case RTS: /* No operands group */
	case STOP:
		isOp = true;
		/* Count lines */
		if (hasLabel) {
			printf("addlabel maybe");
			if (!(addLabel(line->label, cmd_list.length, isExt, isOp))) {
				return false;
			}
		}
		increaseCmdLength(++wordCounter); /* Count number of rows in cmd table*/
		break;
	case DOT_DATA:
		/* Save in data table and symbol table*/
		if (line->args != NULL) {
			char** arg = line->args;
			if (hasLabel) {
				if (!(addLabel(line->label, data_list.length, isExt, isOp))) {
					return false;
					/* Failed to add a label error */
				}
			}
			while (*arg != NULL) {
				int num;
				char* rst;
				/* Convert string to int */
				num = (int) strtol(*arg++, &rst, 10);
				if (strcmp(rst, "") == 0) { /* If no extra string*/
					if (!(addData(num))) {
						error(sprintf(errMsg, OUT_OF_RAM));
						return false;
						/* Did't success to addData error */
					}
				} else {
					error(sprintf(errMsg, UNKNOWN_ARG_TYPE));
					return false;
					/* String mix with int error */
				}
			}
		} else {
			error(sprintf(errMsg, TOO_LESS_ARGS));
			return false;
			/* Expected at least 1 arg error */
		}
		break;

	case DOT_STRING:
		/* Save in data table and symbol table*/

		if (line->args != NULL && line->args[1] == NULL) { /* Only one arg*/
			char* c = line->args[0];
			if (hasLabel) {
				
				if (!(addLabel(line->label, data_list.length, isExt, isOp))) {
					return false;/* Error is set by addLabel already */
					/* Did't success to add label error */
				}
			}
			printf("%c\n",*c);
			if (*c == STR_DELIM) {
				while (*c != STR_DELIM) {
					if (!(addData(*c++))) {
						error(sprintf(errMsg, OUT_OF_RAM));
						return false;
						/* Did't success to add char error */
					}
				}
				if (!(addData('\0'))) { /* End of string*/
					error(sprintf(errMsg, OUT_OF_RAM));
					return false;
					/* Did't succeed to add char error */
				}
			} else {
				
				error(sprintf(errMsg, WRONG_STR_FORMAT));
				return false;
				/* Not in a string format error */
			}
		} else {
			error(sprintf(errMsg, WRONG_ARG_COUNT));
			return false;
			/* Expected 1 arg error */
		}
		break;
	case DOT_EXTERN:
		/* Save in symbol table*/
		isExt = true;
		if (line->args[1] == NULL) { /* Only one arg*/
			if (validLabel(line->args[0])) {
				if (addLabel(line->args[0], 0, isExt, isOp)) {

				} else {
					return false;
					/* Did't success to add label error */
				}
			} else {
				error(sprintf(errMsg, ILLEGAL_LABEL, line->args[0]));
				return false;
				/* Extern not a valid label error */
			}
		} else {
			error(sprintf(errMsg, TOO_LESS_ARGS));
			return false;
			/* Expected at least 1 arg error */
		}
		break;
	case DOT_ENTRY:
		/* Do nothing */
		break;
	default:
		/* Not a command error. Probably impossible to reach. */
		error(sprintf(errMsg, UNKNOWN_ERR));
		return false;
		break;
	}
	return true;
}/* End handleLine */




FLAG secondRun(FILE *src) {
	input_line *line;
	int lineIndex;
	fseek(src, 0L, SEEK_SET);
	relocate(cmd_list.length);
	for (lineIndex = 1; true; lineIndex++) { /* Runs through all the lines. */
		if ((line = getLine(src))) {
			if (line->isEOF) {
				freeLine(line);
				break; /* End when EOF is encountered */
			}
			if (line->isEffectless) {
				continue;
			}
			if (!handleLine2(line)) {
				if (cmd_list.length + data_list.length > MAX_MEMORY_SIZE) {
					error(sprintf(errMsg, OUT_OF_STORAGE));
					report(lineIndex);
					return flag;
				}
				report(lineIndex);
			}
		} else {
			report(lineIndex);
		}
		freeLine(line);
	}
	return flag;
}/* End secondRun */


/* Analyses an line of code and operates accordingly */
static bool handleLine2(input_line* line) {
	switch (line->cmd) {
	/* Two operands group */
	case MOV:
	case CMP:
	case ADD:
	case SUB:
	case LEA:
		if (line->args != NULL && line->args[1] != NULL && line->args[2] == NULL) { /* Two args */
			addressing srcAdders, destAdders;
			word srcArg, destArg;
			/* Handle source arg  */
			
			switch (srcAdders = getArgWord(line->args[0], &srcArg)) {
			case REG:
				srcArg.reg.srcOperand = srcArg.reg.destOperand;
				srcArg.reg.destOperand = 0;
			case IMD:
				if (line->cmd == LEA) {
					error(sprintf(errMsg, WRONG_ARG_FOR_FUNC));
					return false;
				}
				break;
			case IREG:

			
			case DIR:
				break;
			default:
				return false;
				break;
			}
			/* handle dest arg  */
			
			switch (destAdders = getArgWord(line->args[1], &destArg)) {
			case IMD:
				if (line->cmd != CMP) {
					error(sprintf(errMsg, WRONG_ARG_FOR_FUNC));
					return false;
					break;
				}
			case IREG:
				return false;
				break;
			case DIR:
			case REG:
				break;
			
			default:
				return false;
				break;
			}

			addCmd(ABS, destAdders, srcAdders, line->cmd, TWO_ARGS, 7);
			if (destAdders == REG && (srcAdders == REG)) {
				word multiReg;
				multiReg.reg.destOperand = destArg.reg.destOperand;
				multiReg.reg.srcOperand = srcArg.reg.srcOperand;
				multiReg.reg.decode = ABS;
				multiReg.reg.zeroBit = 0;
				addArg(multiReg);
			} else {
				destArg.reg.zeroBit = 0;
				srcArg.reg.zeroBit = 0;
				if (srcAdders == DIR && srcArg.num.value == 0) {/* If the arg is external */
					if (!addExt(line->args[0], getCmdLength() + 1)) {
						return false;
					}
				}
				addArg(srcArg);
				if (destAdders == DIR && destArg.num.value == 0) {/* If the arg is external */
					if (!addExt(line->args[1], getCmdLength() + 1)) {
						return false;
					}
				}
				addArg(destArg);
			}
		} else {
			error(sprintf(errMsg, WRONG_ARG_COUNT));
			return false;
			/* Expected 2 arg error */
		}
		break;
		/* One operands group */
	case NOT:
	case CLR:
	case INC:
	case DEC:
	case JMP:
	case BNE:
	case RED:
	case PRN:
	case JSR:
		if (line->args != NULL && line->args[1] == NULL) { /* Only one arg*/
			addressing adders;
			word arg;
			switch (adders = getArgWord(line->args[0], &arg)) {
			case IMD:
				if (line->cmd != PRN) {
					error(sprintf(errMsg, WRONG_ARG_FOR_FUNC));
					return false;
					break;
				}
			case DIR:
			case REG:
			case IREG:
				addCmd(ABS, adders, IMD, line->cmd, ONE_ARG, 7);
				if (adders == DIR && arg.num.value == 0) {/* If the arg is external */
					if (!addExt(line->args[0], getCmdLength() + 1)) {
						return false;
					}
				}
				addArg(arg);
				break;
			
			default:
				return false;
				break;
			}
		} else {
			error(sprintf(errMsg, WRONG_ARG_COUNT));
			return false;
			/* Expected 1 arg error */
		}
		break;
		/* No operands group */
	case RTS:
	case STOP:
		if (line->args == NULL) { /*no args*/
			addCmd(0, IMD, IMD, line->cmd, NO_ARGS, 7);
		} else {
			error(sprintf(errMsg, WRONG_ARG_COUNT));
			return false;
		}
		break;

	case DOT_ENTRY:
		if (line->args != NULL && line->args[1] == NULL) {
			label* entLable;
			/* TODO: check*/
			if ((entLable = getLabel(line->args[0])) != NULL) {
				if (!(addEnt(entLable))) {
					return false;
				}
			} else {
				/* TODO:  */
				error(sprintf(errMsg, ENT_TO_UNDEF, line->args[0]));
				return false;
			}
		}
		break;
	case DOT_DATA:
	case DOT_STRING:
	case DOT_EXTERN:
		/* do nothing */
		break;
	default:
		/* Not a command error. Probably impossible to reach. */
		error(sprintf(errMsg, UNKNOWN_ERR));
		return false;
		break;
	}
	return true;
}/* End handleLine2 */

static addressing getArgWord(const char *str, word *wrd) {
	int num, i;
	label *lbl;
	int first_reg,second_reg;
	
	if (str[2] == '[' && str[5] == ']') {
			sscanf(str, "r%d[r%d]", &first_reg, &second_reg);
			wrd->reg.destOperand = first_reg;
			wrd->reg.srcOperand = second_reg;
			wrd->reg.decode = ABS;
			wrd->reg.zeroBit = 0;
		

			return IREG;
	}
	 else if (str[0] == IMD_FLAG) {/* Is immediate number */
		if (!strToInt(str + 1, &num)) {
			error(sprintf(errMsg, SYNTAX_ERROR UNKNOWN_ARG_TYPE));
			return -1;
		}
		wrd->num.decode = ABS;
		wrd->num.value = num % (int) pow(2, VALUE_SIZE);
		
		return IMD;
	} else if (validReg(str)) {/* Is register name */
		wrd->reg.zeroBit = 0;
		wrd->reg.decode = ABS;
		wrd->reg.destOperand = str[1] - '0';
		wrd->reg.srcOperand = 0;
		
		return REG;
	} else if (validLabel(str)) {/* Is label name */
		if (!(lbl = getLabel(str))) {
			error(sprintf(errMsg, UNKNOWN_LABEL, str));
			return -1;
		}
		wrd->num.value = lbl->address;
		wrd->num.decode = lbl->isExt ? EXT : RLC;
		return DIR;
	}
	error(sprintf(errMsg, SYNTAX_ERROR INVALID_ARG, str));/* Syntax error */
	return -1;
}



char *currFileName;

int main(int argc, char* argv[]) {
		int i;
	FILE *current;
	srand(time(NULL)); /* Generate seed */
	if (argc < 2) {
		fprintf(stderr, TOO_LESS_FILES);
		return EXIT_FAILURE;
	} else {
		for (i = 1; i < argc; i++) {
			
			currFileName = malloc(strlen(argv[i]) + EXT_LEN + 1);
			
			sprintf(currFileName, "%s%s", argv[i], INPUT_EXT);
			if ((current = fopen(currFileName, "r")) != NULL) { /* Opens the file */
				if (assembler(current) == ASM_FAILURE) {
					printf("failllll\n");
					fprintf(stderr, ASSEMBLER_ERR, argv[i], INPUT_EXT);
				}else{
					fprintf(stdout, ASSEMBLER_SUCCESS, argv[i], INPUT_EXT);
				}
				fclose(current); /* Closes the file */
			} else {
				fprintf(stderr, "Invalid file name\n");/* Here comes error handler for invalid input file */
			}
			free(currFileName);
		}
		return EXIT_SUCCESS;
	}
}


/* Main assembler function. */
FLAG assembler(FILE *src) {
	flag = ASM_SUCCESS;
	firstRun(src);
	if (flag == ASM_SUCCESS)
		secondRun(src);
	if (flag == ASM_SUCCESS)
		saveFiles();

	resetCmdTable();
	resetDataTable();
	freeSymbolTable();
	return flag;
}