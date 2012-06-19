#include <stdio.h>

int main(int argc, char *argv[]){
	int charCount[26];
	char curChar = 0;
	while(curChar < 26){
		charCount[curChar] = 0;
		curChar++;
		}
	char sourceFromFile = 0;
	if(argc == 2){
		sourceFromFile = 1;
		}
	FILE* in;
	if(sourceFromFile){
		in = fopen(argv[1], "r");
		while((curChar = fgetc(in)) != EOF){
			//printf("%d\n", curChar);
			if(curChar >= 65 && curChar <= 90){
				charCount[curChar - 65]++;
				}
			if(curChar >= 97 && curChar <= 122){
				charCount[curChar - 97]++;
				}
			}
		}
	curChar = 0;
	while(curChar < 26){
		printf("%c - %5d\n", curChar + 65, charCount[curChar]);
		curChar++;
		}
}
