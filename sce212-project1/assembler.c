#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

/******************************************************
 * Structure Declaration
 *******************************************************/

struct Inst {
	char *name;
	char *op;
	char type;
	char *funct;
};

struct Data {
	int value;
	struct Data *next;
};

struct Text {
	int idx;
	char *d;
	char *s;
	char *t;
	unsigned int imm;
	unsigned int address;
	struct Text *next;
};

struct Sym {
	char *name;
	int size;
	unsigned int address;
	struct Data *first;
	struct Sym *next;
	struct Data *last;
};


/******************************************************
 * Global Variable Declaration
 *******************************************************/

struct Inst inst[20] = {
	{"addiu",   "001001", 'I', ""},
	{"addu",    "000000", 'R', "100001"},
	{"and",     "000000", 'R', "100100"},
	{"andi",    "001100", 'I', ""},
	{"beq",     "000100", 'I', ""},
	{"bne",     "000101", 'I', ""},
	{"j",       "000010", 'J', ""},
	{"jal",     "000011", 'J', ""},
	{"jr",      "000000", 'R', "001000"},
	{"lui",     "001111", 'I', ""},
	{"lw",      "100011", 'I', ""},
	{"nor",     "000000", 'R', "100111"},
	{"or",      "000000", 'R', "100101"},
	{"ori",     "001101", 'I', ""},
	{"sltiu",   "001011", 'I', ""},
	{"sltu",    "000000", 'R', "101011"},
	{"sll",     "000000", 'S', "000000"},
	{"srl",     "000000", 'S', "000010"},
	{"sw",      "101011", 'I', ""},
	{"subu",    "000000", 'R', "100011"}
};
struct Sym *Symbols;
struct Sym *Symbols_Text;
struct Text *Texts;
int datasize, textsize;


/******************************************************
 * Function Declaration
 *******************************************************/

/* You may need the following functions */
char* change_file_ext(char *str);
char* num_to_bits(unsigned int num, int len);

/* You may fill the following functions */
void read_asm();
void subst_asm_to_num();
void print_bits();


/******************************************************
 * Function: main
 *
 * Parameters:
 *  int
 *      argc: the number of argument
 *  char*
 *      argv[]: array of a sting argument
 *
 * Return:
 *  return success exit value
 *
 * Info:
 *  The typical main function in C language.
 *  It reads system arguments from terminal (or commands)
 *  and parse an assembly file(*.s).
 *  Then, it converts a certain instruction into
 *  object code which is basically binary code.
 *
 *******************************************************/
int main(int argc, char* argv[]) {

	FILE *input, *output;
	char *filename;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <*.s>\n", argv[0]);
		fprintf(stderr, "Example: %s sample_input/example?.s\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	// reading the input file
	input = freopen(argv[1], "r", stdin);
	if (input == NULL) {
		perror("ERROR");
		exit(EXIT_FAILURE);
	}

	// creating the output file (*.o)
	filename = strdup(argv[1]); // strdup() is not a standard C library but fairy used a lot.
	if(change_file_ext(filename) == NULL) {
		fprintf(stderr, "'%s' file is not an assembly file.\n", filename);
		exit(EXIT_FAILURE);
	}

	output = freopen(filename, "w", stdout);
	if (output == NULL) {
		perror("ERROR");
		exit(EXIT_FAILURE);
	}

	Symbols = (struct Sym *)malloc(sizeof(struct Sym));
	Symbols_Text = (struct Sym *)malloc(sizeof(struct Sym));
	Texts = (struct Text *)malloc(sizeof(struct Text));
	/******************************************************
	 *  Let's complete the below functions!
	 *
	 *  read_asm(): Read the assmebly code.
	 *  subst_asm_to_num(): Convert the assembly code to num.
	 *  print_bits(): Print the results of assemble.
	 *
	 *******************************************************/
	read_asm();
	subst_asm_to_num();
	print_bits();

	// freeing the memory
	free(filename);
	free(Symbols);
	free(Texts);

	fclose(input);
	fclose(output);
	exit(EXIT_SUCCESS);
}

/******************************************************
 * Function: change_file_ext
 *
 * Parameters:
 *  char
 *      *str: a raw filename (without path)
 *
 * Return:
 *  return NULL if a file is not an assembly file.
 *  return empty string
 *
 * Info:
 *  This function reads filename and converts it into
 *  object extention name, *.o
 *
 *******************************************************/
char* change_file_ext(char *str) {
	char *dot = strrchr(str, '.');

	if (!dot || dot == str || (strcmp(dot, ".s") != 0))
		return NULL;

	str[strlen(str) - 1] = 'o';
	return "";
}

/******************************************************
 * Function: num_to_bits
 *
 * Parameters:
 *  unsigned int
 *      num: a decimal number to be converted to binary bits
 *  int
 *      len: a length of bits
 *
 * Return:
 *  return string of binary bits
 *
 * Info:
 *  This function reads an integer number and converts it into
 *  binary bits represented by string.
 *
 *******************************************************/
char* num_to_bits(unsigned int num, int len) {
	char* bits = (char *) malloc(len+1);
	int idx = len-1, i;

	while (num > 0 && idx >= 0) {
		if (num % 2 == 1) {
			bits[idx--] = '1';
		} else { 
			bits[idx--] = '0';
		}
		num /= 2;
	}

	for (i = idx; i >= 0; i--){ 
		bits[i] = '0';
	}

	return bits;
}


/******************************************************
 * Function: read_asm
 *
 * Info:
 *  This function reads assembly code and saves the
 *  information into struct Sym, Data, Text, respectively.
 *
 *******************************************************/
void read_asm() {
	int tmp, i;
	unsigned int address;
	char temp[0x1000] = {0};
	struct Sym *temp_sym = NULL;
	struct Data *temp_data = NULL;
	struct Text *temp_text = NULL;

	//Read .data region
	address = 0x10000000;

	struct Sym *ttsym;
	struct Data *ttdata;
	tmp = 0;
	for(i = 0; scanf("%s", temp) == 1;){
		if (strcmp(temp, ".text") == 0) {
			break;
		} 
		else if(temp[strlen(temp)-1] == ':') {
			temp_sym = (struct Sym*)malloc(sizeof(struct Sym));
			temp_sym->name = (char*)malloc(sizeof(char)*strlen(temp-1));
			strtok(temp, ":");
			strcpy(temp_sym->name, temp);
			temp_sym->address = address;

			if(i == 0){
				Symbols->next = temp_sym;
				ttsym = temp_sym;
			}
			else{
				ttsym->next = temp_sym;
				ttsym = temp_sym;
			}
			tmp = 0;
			i++;
		} 
		else if(strcmp(temp, ".word") == 0) {
			scanf("%s",temp);
			temp_sym->size += 4;
			address += 4;
			temp_data = (struct Data*)malloc(sizeof(struct Data));

			if(strchr(temp, 'x') != NULL){
				temp_data->value = strtol(temp, NULL, 16);
			}
			else temp_data->value = strtol(temp, NULL, 10);

			if(tmp == 0){
				temp_sym->first = temp_data;
				temp_sym->last = temp_data;
				ttdata = temp_data;
			}
			else{
				ttdata->next = temp_data;
				ttdata = temp_data;
				temp_sym->last = temp_data;
			}       
			tmp++;
		}
	}
	datasize = address - 0x10000000;

	//Read .text region, textsym

	struct Text *tttext;
	char* ta;
	int j = 0;
	address = 0x400000;
	for (i = 0; scanf("%s", temp) == 1;){
		if (temp[strlen(temp)-1] == ':'){
			temp_sym = (struct Sym*)malloc(sizeof(struct Sym));
			temp_sym->name = (char*)malloc(sizeof(char)*strlen(temp));
			strtok(temp, ":");
			strcpy(temp_sym->name, temp);
			temp_sym->address = address;

			if(i == 0){
				Symbols_Text->next = temp_sym;
				ttsym = temp_sym;
			}
			else{
				ttsym->next = temp_sym;
				ttsym = temp_sym;
			}
			i++;
		} 

		else{   //I'm so happy because i'm a gummy bear
			temp_text = (struct Text*)malloc(sizeof(struct Text));
			temp_text->address = address;
			address += 4;

			if(strcmp(temp, "la") == 0){
				char reg[5];
				scanf("%s",temp);
				strtok(temp, ",");
				strcpy(reg, temp+1);
				scanf("%s", temp);
				for(temp_sym = Symbols->next; temp_sym != NULL; temp_sym = temp_sym->next){
					if(strcmp(temp_sym->name, temp) == 0){
						break;
					}
				}
				temp_text->idx = 9;

				temp_text->s = (char*)malloc(sizeof(char));
				strcpy(temp_text->s, "0");

				temp_text->t = (char*)malloc(sizeof(char)*strlen(reg)); 
				strcpy(temp_text->t, reg);
					
				temp_text->imm = 0x1000;
				
				if(temp_sym->address > 0x10000000){
					tttext->next = temp_text;
					tttext = temp_text;
					
					temp_text = (struct Text*)malloc(sizeof(struct Text));
					
					temp_text->address = address;
					address += 4;
					temp_text->idx = 13;

					temp_text->s = (char*)malloc(sizeof(char)*strlen(reg));
					strcpy(temp_text->s, reg);

					temp_text->t = (char*)malloc(sizeof(char)*strlen(reg)); 
					strcpy(temp_text->t, reg);
					
					temp_text->imm = temp_sym->address-0x10000000;
				}

			}
			else{
				for(tmp = 0; tmp<20; tmp++){
					if(strcmp(temp, inst[tmp].name) == 0){
						if(inst[tmp].type == 'R'){
							temp_text->idx = tmp;
							if(tmp == 8){
							    scanf("%s", temp);
							    temp_text->s = (char*)malloc(sizeof(char)*strlen(temp-1));
							    strcpy(temp_text->s, temp+1);
							    
							    temp_text->d = (char*)malloc(sizeof(char));
							    strcpy(temp_text->d, "0");

							    temp_text->t = (char*)malloc(sizeof(char));
							    strcpy(temp_text->t, "0");


							}
							else{
							    scanf("%s", temp);
							    temp_text->d = (char*)malloc(sizeof(char)*(strlen(temp)-2));
							    strtok(temp, ",");
							    strcpy(temp_text->d, temp+1);
							

							    scanf("%s", temp);
							    temp_text->s = (char*)malloc(sizeof(char)*(strlen(temp)-2));
							    strtok(temp, ",");
							    strcpy(temp_text->s, temp+1);

							    scanf("%s", temp);
							    temp_text->t = (char*)malloc(sizeof(char)*(strlen(temp)-2));
							    strcpy(temp_text->t, temp+1);
							}
						}
						else if(inst[tmp].type == 'S'){
							temp_text->idx = tmp;

							scanf("%s", temp);
							temp_text->d = (char*)malloc(sizeof(char)*(strlen(temp)-2));
							strtok(temp, ",");
							strcpy(temp_text->d, temp+1);

							scanf("%s", temp);
							temp_text->s = (char*)malloc(sizeof(char)*(strlen(temp)-2));
							strtok(temp, ",");
							strcpy(temp_text->s, temp+1);

							scanf("%s", temp);
							temp_text->imm = strtol(temp, NULL, 10);


						}
						else if(inst[tmp].type == 'I'){
							temp_text->idx = tmp;
							scanf("%s", temp);
							temp_text->t = (char*)malloc(sizeof(char)*(strlen(temp)-2));
							strtok(temp, ",");
							strcpy(temp_text->t, temp+1);
							if(tmp == 9){
							    temp_text->s = (char*)malloc(sizeof(char));
							    strcpy(temp_text->s, "0");
							}
							else if(tmp == 10 || tmp == 18){
							    scanf("%s", temp);
							    temp_text->s = (char*)malloc(sizeof(char)*strlen(temp));
							    ta = (char*)malloc(sizeof(char)*strlen(temp));
							    strcpy(temp_text->s, temp);
							    strcpy(ta, temp);
							    strtok(ta, ")");
							    ta = strchr(ta, '(');
							    
							    strcpy(temp_text->s, ta+2);

							    strtok(temp, "(");
							    temp_text->imm = atoi(temp);
							}
							else{
							    scanf("%s", temp);
							    temp_text->s = (char*)malloc(sizeof(char)*(strlen(temp)-2));
							    strtok(temp, ",");
							    strcpy(temp_text->s, temp+1);
							
							}
							if(tmp == 13 || tmp == 9){
								scanf("%s", temp);
								ta = (char*)malloc(sizeof(char)*strlen(temp));
								strncpy(ta, temp, 2);
								if(strcmp(ta, "0x") == 0) temp_text->imm = strtol(temp, NULL, 16);
								else {
									temp_text->imm = atoi(temp);
								}
							}
							else if(tmp == 10 || tmp == 18);
							else{
								scanf("%s", temp);
								temp_text->d = (char*)malloc(sizeof(char)*strlen(temp));
								strcpy(temp_text->d, temp);
							}
						}
						else if(inst[tmp].type == 'J'){
							temp_text->idx = tmp;

							scanf("%s", temp);
							temp_text->s = (char*)malloc(sizeof(char)*strlen(temp));
							strcpy(temp_text->s, temp);
						}			
					}
				}
			}
			if(j == 0){
				Texts->next = temp_text;
				tttext = temp_text;
			}
			else{
				tttext->next = temp_text;
				tttext = temp_text;
			}
			j++;
		}
	}	
	textsize = address - 0x400000;
}


/******************************************************
 * Function: subst_asm_to_num
 *
 * Info:
 *  This function converts assembly code to numbers.
 *
 *******************************************************/
void subst_asm_to_num() {
	struct Text *text;
	struct Sym *temp;
	char* ta;
	unsigned int offset;

	for (text = Texts->next; text != NULL; text = text->next) {
		if(text->idx == 4 || text->idx == 5){
			for(temp = Symbols_Text->next; temp != NULL; temp = temp->next){
				if(strcmp(text->d, temp->name) == 0) break;
			}
		offset = (temp->address - (text->address+4)) >> 2;
		text->imm = offset;
		ta = (char*)malloc(sizeof(char)*strlen(text->s));
		strcpy(ta, text->s);
		strcpy(text->s, text->t);
		strcpy(text->t, ta);
		}
		else if(text->idx == 0 || text->idx == 3 || text->idx == 14){
			ta = (char*)malloc(sizeof(char)*strlen(text->d));
			strncpy(ta, text->d, 2);
			if(strcmp(ta, "0x") == 0) text->imm = strtol(text->d, NULL, 16);
			else {
				text->imm = atoi(text->d);
			}
		}
		
		else if(text->idx == 6 || text->idx == 7){
			for(temp = Symbols_Text->next; temp != NULL; temp = temp->next){
				if(strcmp(text->s, temp->name) == 0) break;
			}
			text->imm = (temp->address)/4;
		}
	}

}


/******************************************************
 * Function: print_bits
 *
 * Info:
 *  This function prints binary bit string converted by
 *  assembler. It will make binary object file of assembly code.
 *
 *******************************************************/
void print_bits() {
	struct Text* text;
	struct Sym* sym;
	struct Data* data;

	// print the header
	printf("%s", num_to_bits(textsize, 32));
	printf("%s", num_to_bits(datasize, 32));

	// print the body
	for (text = Texts->next ; text != NULL ; text = text->next) {
		printf("%s", inst[text->idx].op);

		if (inst[text->idx].type == 'R') { // d s t -> s t d
			printf("%s%s%s%s%s", 
			num_to_bits(atoi(text->s), 5), 
			num_to_bits(atoi(text->t), 5), 
			num_to_bits(atoi(text->d), 5), 
			num_to_bits(0, 5), 
			num_to_bits(strtol(inst[text->idx].funct, NULL, 2), 6));

		} else if(inst[text->idx].type == 'I') {
			printf("%s%s%s", 
			num_to_bits(atoi(text->s), 5), 
			num_to_bits(atoi(text->t), 5),  
			num_to_bits(text->imm, 16));

		} else if(inst[text->idx].type == 'S') {
			printf("%s%s%s%s%s", num_to_bits(0, 5), 
			num_to_bits(atoi(text->s), 5), 
			num_to_bits(atoi(text->d), 5), 
			num_to_bits(text->imm, 5), 
			num_to_bits(strtol(inst[text->idx].funct, NULL, 2), 6));

		} else {
			printf("%s", num_to_bits(text->imm, 26));

		}
	}

	for (sym = Symbols->next; sym != NULL; sym = sym->next) {
		if(sym->first != sym->last){
			for(data = sym->first; data != NULL; data = data->next){
				printf("%s", num_to_bits(data->value, 32));
			}
		}
		else printf("%s", num_to_bits(sym->first->value, 32));
	}

	printf("\n"); // To exclude "No newline at end of file"
}
