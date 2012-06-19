#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define OUT_ARRAY_STEP_SIZE 256

#define dprintf(STR, ...) if(debug){ fprintf(stderr, STR , ## __VA_ARGS__); }
#define eprintf(STR, ...)            fprintf(stderr, STR , ## __VA_ARGS__);


void printhelp(FILE *fp);

int main(int argc, char* argv[]){
	
	int isttyi = isatty(fileno(stdin));
	int isttyo = isatty(fileno(stdout));
	int isttye = isatty(fileno(stderr));
	
	int debug = 0;
	int i;
	
	if(argc == 1){
		printhelp(stdout);
		return EXIT_SUCCESS;
	}
	
	for(i = 1; i < argc; i++){
		if(!strcmp(argv[i], "-d") || !strcmp(argv[i], "--debug")){
			debug = 1;
			printf("\n'-d' found, arg #%d\nDebugging enabled\n\n", i + 1);
			break;
		}
	}
	
	dprintf("Are std[in,out,err] ttys? = %d, %d, %d\n\n", isttyi, isttyo, isttye);
	
	for(i = 1; i < argc; i++){
		if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")){
			if(isttyi && !isttye){
				printhelp(stdout);
			} else {
				printhelp(stderr);
			}
			return EXIT_SUCCESS;
		}
	}
	
	FILE* rsFile = fopen("rotors.txt", "r");
	if(rsFile == NULL){
		eprintf("fopen on rotors.txt fail.\n");
		return EXIT_FAILURE;
	}
	
	int numRs = 5;
	int numUsedRs = 3;
	unsigned char **rs   = malloc(numRs * sizeof(unsigned char *));
	unsigned char *rStep = malloc(numRs * sizeof(unsigned char  ));
	unsigned char reflector[26];
	int j;
	for(i = 0; i < numRs; i++){
		rs[i] = malloc(26 * sizeof(unsigned char));
		if(rs[i] == NULL){
			eprintf("malloc fail.\n");
			return EXIT_FAILURE;
		}
	}
	dprintf("From rotors.txt:\n");
	for(i = 0; i < numRs; i++){
		fgets((char *) (rs[i]), 27, rsFile);
		if(fgetc(rsFile) != ' '){
			eprintf("rotors.txt formatted improperly.\n");
			return EXIT_FAILURE;
		}
		rStep[i] = fgetc(rsFile) - 65;
		if(fgetc(rsFile) != 10){
			eprintf("rotors.txt formatted improperly.\n");
			return EXIT_FAILURE;
		}
		dprintf("  Rotor %d: %s, steps at '%c', (%d)\n", i + 1, rs[i], rStep[i] + 65, rStep[i]);
		for(j = 0; j < 26; j++){
			//dprintf("Character beforehand: %c, new numerical value: %d\n", rs[i][i], rs[i][i] - 65);}
			rs[i][j] -= 65;
		}
	}
	fgets((char *) reflector, 27, rsFile);
	if(debug){
		printf("  Reflector string: %s\n", reflector);
	}
	for(i = 0; i < 26; i++){
		//dprintf("Character beforehand: %c, new numerical value: %d\n", reflector[i], reflector[i] - 65);
		reflector[i] -= 65;
	}
	int rOrder = 123;
	char rsUsed[numUsedRs];
	for(i = 0; i < argc - 1; i++){
		//dprintf("  Arg #%d: '%s'\n", i + 1, argv[i]);
		if(!strcmp(argv[i], "-r") || !strcmp(argv[i], "--rotors")){
			dprintf("\n'-r' found, arg #%d\n", i + 1);
			rOrder = atoi(argv[i + 1]);
			dprintf("  Rotor order: %d\n", rOrder);
			break;
		}
	}
	for(i = numUsedRs - 1; i >= 0; i--){
		rsUsed[i] = rOrder % 10;
		dprintf("    Rotor-position %d filled by rotor %d\n", i + 1, rsUsed[i]);
		rOrder = (rOrder - (rOrder % 10)) / 10;
	}
	int inputSource = 0; // 0 = interactive text entry, 1 = file, 2 = command line argument
	int inputArg = 0;
	for(i = 0; i < argc - 1; i++){
		if(!strcmp(argv[i], "-t") || !strcmp(argv[i], "--text")){
			dprintf("\n'-t' found, arg #%d\n", i + 1);
			inputSource = 2;
			inputArg = i + 1;
			dprintf("  text: %s\n", argv[i + 1]);
		}
	}
	char *inputFileName;
	FILE* inputFile = stdin;
	for(i = 0; i < argc - 1; i++){
		//dprintf("  Arg #%d: '%s'\n", i + 1, argv[i]);
		if(!strcmp(argv[i], "-i") || !strcmp(argv[i], "--input")){
			dprintf("\n'-i' found, arg #%d\n", i + 1);
			inputFileName = argv[i + 1];
			inputSource = 1;
			inputFile = fopen(inputFileName, "r");
			if(inputFile == NULL){
				eprintf("Input file open failed.\n");
				return EXIT_FAILURE;
			}
			dprintf("  Input file: %s\n", inputFileName);
			break;
		}
	}
	char *outputFileName;
	int outputToFile = 0;
	FILE* outputFile = stdout;
	for(i = 0; i < argc - 1; i++){
		dprintf("  Arg #%d: '%s'\n", i + 1, argv[i]);
		if(!strcmp(argv[i], "-o") || !strcmp(argv[i], "--output")){
			dprintf("\n'-o' found, arg #%d\n", i + 1);
			outputFileName = argv[i + 1];
			outputToFile = 1;
			outputFile = fopen(outputFileName, "w");
			if(inputFile == NULL){
				eprintf("Output file open failed.\n");
				return EXIT_FAILURE;
			}
			dprintf("  Output file: %s\n", outputFileName);
			break;
		}
	}
	unsigned char rPositions[numUsedRs];
	for(i = 0; i < numUsedRs; i++){
		rPositions[i] = 0;
	}
	for(i = 0; i < argc - 1; i++){
		//dprintf("  Arg #%d: '%s'\n", i + 1, argv[i]);
		if(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--positions")){
			dprintf("\n'-p' found, arg #%d\n", i + 1);
			dprintf("  Starting positions: %s\n", argv[i + 1]);
			for(j = 0; j < numUsedRs; j++){
				rPositions[j] = argv[i + 1][j] - 65;
				dprintf("    Rotor %d starting position: %d\n", j + 1, rPositions[j]);
			}
			break;
		}
	}
	unsigned char cores[numUsedRs];
	for(i = 0; i < numUsedRs; i++){
		cores[i] = 0;
	}
	for(i = 0; i < argc - 1; i++){
		if(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--cores")){
			dprintf("\n'-c' found, arg #%d\n", i + 1);
			for(j = 0; j < numUsedRs; j++){
				if(argv[i + 1][j] < 65 || (argv[i + 1][j] > 90 && argv[i + 1][j] < 97) || argv[i + 1][j] > 122){
					eprintf("Core position argument (arg #%d) not properly formatted (character #%d not allowed).\n", i + 1, argv[i + 1][j]);
					return EXIT_FAILURE;
				}
				if(argv[i + 1][j] >= 65 && argv[i + 1][j] <= 90){
					cores[j] = argv[i + 1][j] - 65;
				}
				if(argv[i + 1][j] >= 97 && argv[i + 1][j] <= 122){
					cores[j] = argv[i + 1][j] - 97;
				}
				dprintf("  Core #%d: %c\n", j + 1, cores[j] + 65);
			}
			break;
		}
	}
	int numSteckers = 0;
	unsigned char steckers[26];
	for(i = 0; i < argc; i++){
		if(!strcmp(argv[i], "-s") || !strcmp(argv[i], "--stecker")){
			dprintf("\n'-s' found, arg #%d\n", i + 1);
			j = 0;
			while(1){
				if(argv[i + 1][j] == '\0'){
					if(numSteckers % 2 == 1){
						eprintf("Odd number of steckers on arg termination, quitting.\n");
						return EXIT_FAILURE;
					}
					break;
				}
				if(argv[i + 1][j] == ','){
					j++;
					continue;
				}
				if(argv[i + 1][j] < 65 || (argv[i + 1][j] > 90 && argv[i + 1][j] < 97) || argv[i + 1][j] > 122){
					eprintf("Steckering argument (arg #%d) not properly formatted (character #%d not allowed).\n", i + 1, argv[i + 1][j]);
					return EXIT_FAILURE;
				}
				if(argv[i + 1][j] >= 65 && argv[i + 1][j] <= 90){
					steckers[numSteckers] = argv[i + 1][j] - 65;
					numSteckers++;
				}
				if(argv[i + 1][j] >= 97 && argv[i + 1][j] <= 122){
					steckers[numSteckers] = argv[i + 1][j] - 97;
					numSteckers++;
				}
				dprintf("  New stecker (#%d): %c\n", numSteckers - 1, steckers[numSteckers - 1] + 65);
				j++;
			}
			break;
		}
	}
	int spaces = 1;
	for(i = 0; i < argc; i++){
		if(!strcmp(argv[i], "-ns") || !strcmp(argv[i], "--nospace")){
			spaces = 0;
			dprintf("\n'-ns' found, arg #%d\n  Spaces disabled.\n", i + 1);
			break;
		}
	}
	int preserveSpacing = 0;
	int curCharCase = 0;
	for(i = 0; i < argc; i++){
		if(!strcmp(argv[i], "-ps") || !strcmp(argv[i], "--preservespacing")){
			preserveSpacing = 1;
			spaces = 0;
			dprintf("\n'-ps' found, arg #%d\n  Spacing will be retained.\n", i + 1);
			break;
		}
	}
	for(i = 0; i < argc; i++){
		if(!strcmp(argv[i], "-q") || !strcmp(argv[i], "--quit")){
			dprintf("\n'-q' found, arg #%d\n  Quitting.\n", i + 1);
			return EXIT_SUCCESS;
		}
	}
	dprintf("\n");
	unsigned char charIn;
	unsigned char curChar;
	unsigned char carry[numUsedRs - 1];
	unsigned char *output;
	output = malloc(sizeof(unsigned char) * OUT_ARRAY_STEP_SIZE);
	int outputLength = 0;
	int outputArrayLength = OUT_ARRAY_STEP_SIZE;
	int k = 0;
	if(!inputSource && isttyi && isttyo){
		printf("Enter text to (en/de)code:\n");
	}
	while(1){
		if(inputSource == 2){
			charIn = argv[inputArg][k];
			k++;
		} else {
			charIn = fgetc(inputFile);
		}
		//if(charIn == 10 && !inputSource){
		//	dprintf("  Return (char 10) detected, quitting.\n");
		//	break;
		//}
		if(charIn == 255){
			dprintf("  EOF (char 255) detected, quitting.\n");
			break;
		}
		if(charIn == 0){
			dprintf("  NULL (char 0) detected, quitting.\n");
			break;
		}
		//dprintf("  CharIn = %c (#%d)\n", charIn, charIn);
		if(outputLength + 1 == outputArrayLength){
			output = realloc(output, outputArrayLength + OUT_ARRAY_STEP_SIZE);
			outputArrayLength += OUT_ARRAY_STEP_SIZE;
			dprintf("Reallocing output array.\n");
		}
		if(output == NULL){
			eprintf("Realloc failed. Buh-bye.\n");
			return EXIT_FAILURE;
		}
		if(charIn < 65 || (charIn > 90 && charIn < 97) || charIn > 122){
			if(preserveSpacing){
				if(outputToFile){
					fprintf(outputFile, "%c", charIn);
				} else {
					output[outputLength] = charIn;
					outputLength++;
				}
				continue;
			}
			dprintf("  Character #%d is not allowed, ignoring.\n", charIn);
			continue;
		}
		if(charIn >= 65 && charIn <= 90){
			curCharCase = 1;
			charIn -= 65;
		}
		if(charIn >= 97 && charIn <= 122){
			curCharCase = 0;
			charIn -= 97;
		}
		//dprintf("    unsigned char numbered: %d\n", charIn);
		//printf("  Old rotor positions: %c%02d", rPositions[0] + 65, rPositions[0]);
		for(i = 1; i < numUsedRs; i++){
			//printf("-%c%02d", rPositions[i] + 65, rPositions[i]);
		}
		//printf(", carries: ");
		for(i = 1; i < numUsedRs; i++){
			if(rPositions[i] == rStep[rsUsed[i] - 1]){
				carry[i - 1] = 1;
			} else {
				carry[i - 1] = 0;
			}
			//printf("%d", carry[i - 1]);
		}
		//printf("\n");
		if(carry[0]){
			rPositions[0] = (rPositions[0] + 1) % 26;
		}
		dprintf("  New rotor positions: %c %02d", rPositions[0] + 65, rPositions[0]);
		for(i = 1; i < numUsedRs - 1; i++){
			if(carry[i - 1] || carry[i]){
				rPositions[i] = (rPositions[i] + 1) % 26;
			}
			dprintf(" - %c %02d", rPositions[i] + 65, rPositions[i]);
		}
		rPositions[numUsedRs - 1] = (rPositions[numUsedRs - 1] + 1) % 26;
		dprintf(" - %c %02d\n", rPositions[numUsedRs - 1] + 65, rPositions[numUsedRs - 1]);
		curChar = charIn;
		if(numSteckers){
			for(i = 0; i < numSteckers; i++){
				if(steckers[i] == curChar){
					dprintf("    Steckering: %c %02d -> ", curChar + 65, curChar);
					if(i % 2 == 0){
						curChar = steckers[i + 1];
					} else {
						curChar = steckers[i - 1];
					}
					dprintf("%c %02d\n", curChar + 65, curChar);
					break;
				}
			}
		}
		for(i = numUsedRs - 1; i >= 0; i--){
			dprintf("    Rotor %d(%d), %c %02d -> ", i + 1, rsUsed[i], curChar + 65, curChar);
			curChar = (26 + rs[rsUsed[i] - 1][(26 + rPositions[i] + curChar - cores[i]) % 26] - rPositions[i] + cores[i]) % 26;
			dprintf("%c %02d\n", curChar + 65, curChar);
		}
		dprintf("    Reflector,  %c %02d -> ", curChar + 65, curChar);
		curChar = reflector[curChar];
		dprintf("%c %02d\n", curChar + 65, curChar);
		for(i = 0; i < numUsedRs; i++){
			dprintf("    Rotor %d(%d), %c %02d -> ", i + 1, rsUsed[i], curChar + 65, curChar);
			for(j = 0; j < 26; j++){
				if((26 + rs[rsUsed[i] - 1][(26 + rPositions[i] + j - cores[i]) % 26] - rPositions[i] + cores[i]) % 26 == curChar){
					curChar = j;
					break;
				}
			}
			dprintf("%c %02d\n", curChar + 65, curChar);
		}
		if(numSteckers){
			for(i = 0; i < numSteckers; i++){
				if(steckers[i] == curChar){
					dprintf("    Steckering: %c %02d -> ", curChar + 65, curChar);
					if(i % 2 == 0){
						curChar = steckers[i + 1];
					} else {
						curChar = steckers[i - 1];
					}
					dprintf("%c %02d\n", curChar + 65, curChar);
					break;
				}
			}
		}
		if(outputToFile){
			if(preserveSpacing && !curCharCase){
				fprintf(outputFile, "%c", curChar + 97);
			} else {
				fprintf(outputFile, "%c", curChar + 65);
			}
			if(spaces && outputLength % 5 == 4){
				fprintf(outputFile, " ");
			}
		}
		if(preserveSpacing && !curCharCase){
			output[outputLength] = curChar + 97;
		} else {
			output[outputLength] = curChar + 65;
		}
		outputLength++;
	}
	if(!outputToFile){
		if(isttyi && isttyo){
			printf("Output:\n");
		}
		for(i = 0; i < outputLength; i++){
			printf("%c", output[i]);
			if(spaces && i % 65 == 64){
				printf("\n");
			} else
			if(spaces && i % 5 == 4){
				printf(" ");
			}
		}
	}
	if(outputToFile){
		//fprintf(outputFile, "\n");
	}
	if(isttyo){
		printf("\n");
	}
	
	return EXIT_SUCCESS;
}

void printhelp(FILE *fp){
	//          12345678901234567890123456789012345678901234567890123456789012345678901234567890
	fprintf(fp, "\nUsage: enigma [OPTION]... [-i FILE] [-o FILE]\n"
	            "\nNote: currently, this only supports the Army/Air Force style single-stepping\n"
	            "  rotors, as opposed to the naval double-stepping rotors.\n"
	            "\nOptions:\n"
	            "  -d, --debug         Prints out all sorts of fun debug messages.\n"
	            "  -h, --help          Prints this help text and exits.\n"
	            "  -r, --rotors 123    Sets rotor order, e.g. if the following argument was 415,\n"
	            "                        it would place rotor 4 from the rotor file into\n"
	            "                        rotor position 1, rotor 1 into 2, 5 into 3.\n"
	            "                        Currently, this program only supports 3-rotor enigmas.\n"
	            "                        Thus, the argument must be a 3 digit number with each\n"
	            "                        digit not exceeding the number of rotors in the rotor\n"
	            "                        file (rotors.txt).\n"
	            "  -p, --positions ABC Sets the initial rotor positions. The argument is a three\n"
	            "                        letter string, each A-Z.\n"
	            "  -c, --cores ABC     Sets the core (pin and wiring assembly) offset from the\n"
	            "                        outer ring and stepping mechanism. An advanced,\n"
	            "                        optional option.\n"
	            "  -s, --stecker AB,CD,EF...\n"
	            "                      Performs a substitution between the given letter pairs\n"
	            "                        before and after the rest of the cipher.\n"
	            "  -i, --input file    Reads input from given file (as opposed to default stdin).\n"
	            "  -o, --output file   Puts output to given file (as opposed to default stdout).\n"
	            "  -ns,--nospaces      Prints output, to file or stdout, without default spaces.\n"
	            "  -t, --text ABC...   Accepts the following argument as the text to be\n"
	            "                        (en/de)coded.\n"
	            "  -q, --quit          Quits before actually (en/de)coding, i.e., only\n"
	            "                        initializes everything. Use with --debug. Will open\n"
	            "                        files.\n"
	            "\nWritten by Felix Handte <w@felixhandte.com>.\n");
	//          12345678901234567890123456789012345678901234567890123456789012345678901234567890
	printf("\n");
}
