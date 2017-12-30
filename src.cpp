#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;
//define: global consts
#define NUM_MAX_LEN 8//NUM here means UINT
#define SRC_FILE_NAME_MAX_LEN 255
#define SRC_MAX_LEN 10000
#define MAX_ERROR_NUM 500
#define IDF_MAX_LEN 20
#define STR_MAX_LEN 255
#define RESERVER_NUM 13
#define MAX_STR_NUM 100
#define MAX_TAB_LEN 500
#define MAX_MID_LEN 3000
#define MAX_LABEL_NUM 999
#define MAX_TMP_NUM 1000
#define MAX_PARAM_NUM 256
#define DATA_BASE 0x10010000//correspond to "default" mem config of mars!
//define: words
#define END 0
#define IDENTIFIER	1
#define CONSTFLAG	2
#define INTFLAG	3
#define CHARFLAG	4
#define VOIDFLAG	5
#define IFFLAG	6
#define ELSEFLAG	7
#define DOFLAG	8
#define WHILEFLAG	9
#define FORFLAG	10
#define RETURNFLAG	11
#define LES	12
#define LEQ	13
#define GRT	14
#define GEQ	15
#define EQS	16
#define ASSIGN	17
#define STRING	18
#define CHAR	19
#define UINT	20
#define NEQ	21
#define ZERO	22
#define ADD	23
#define SUB	24
#define MUL	25
#define DIV	26
#define COMMA	27
#define SEMICOLON	28
#define LPAREN	29
#define RPAREN	30
#define LBRACK	31
#define RBRACK	32
#define LBRACE	33
#define RBRACE	34
#define MAINFLAG	35
#define PRINTFLAG	36
#define SCANFLAG    37
//id-str mapping for debug
char *word_strs[100] = {
	"",
	"IDENTIFIER  ",
	"CONSTFLAG   ",
	"INTFLAG     ",
	"CHARFLAG    ",
	"VOIDFLAG    ",
	"IFFLAG      ",
	"ELSEFLAG    ",
	"DOFLAG      ",
	"WHILEFLAG   ",
	"FORFLAG     ",
	"RETURNFLAG  ",
	"LES         ",
	"LEQ         ",
	"GRT         ",
	"GEQ         ",
	"EQS         ",
	"ASSIGN      ",
	"STRING      ",
	"CHAR        ",
	"UINT        ",
	"NEQ         ",
	"ZERO        ",
	"ADD         ",
	"SUB         ",
	"MUL         ",
	"DIV         ",
	"COMMA       ",
	"SEMICOLON   ",
	"LPAREN      ",
	"RPAREN      ",
	"LBRACK      ",
	"RBRACK      ",
	"LBRACE      ",
	"RBRACE      ",
	"MAINFLAG    ",
	"PRINTFLAG   ",
	"SCANFLAG    "
};

//reservers and reserver-ids
char *reservers[RESERVER_NUM] = { "const","int","char","void","if","else","do","while","for","return","main","printf","scanf" };
int reserver_ids[RESERVER_NUM] = { CONSTFLAG,INTFLAG,CHARFLAG,VOIDFLAG,IFFLAG,ELSEFLAG,DOFLAG,WHILEFLAG,FORFLAG,RETURNFLAG,MAINFLAG,PRINTFLAG,SCANFLAG };
//define: messages
#define LEX_SUB_DETECT_SUCCESS				1
#define LEX_SUB_DETECT_FAILURE				2
#define LEX_REACHING_END				3
#define STX_SUB_DETECT_SUCCESS			4
#define STX_SUB_DETECT_FAILURE          5

#define LEX_SUB_ILLEGAL_NUMBER				-1
#define LEX_SUB_ILLEGAL_STRING				-2
#define LEX_SUB_UNCLOSED_CHAR				-3
#define LEX_SUB_ILLEGAL_CHAR				-4
#define LEX_SUB_INCOMPLETE_NEQ				-5
#define LEX_SUB_UNCLOSED_STRING				-6
#define LEX_SUB_STRING_TOO_LONG				-7
#define LEX_SUB_IDF_TOO_LONG				-8
#define LEX_SUB_NUM_TOO_LONG				-9
#define LEX_ILLEGAL_CHAR				-10

#define STX_SUB_NEED_UINT            -11
#define STX_SUB_ARY_INCOMPLETE			   -12
#define STX_SUB_CONST_ITEM_ERR			-13
#define STX_SUB_NEED_TYPE_FLAG			-14
#define STX_SUB_NEED_INT				-15
#define STX_SUB_NEED_IDF	-16
#define STX_SUB_NEED_ASSIGN - 17
#define STX_SUB_NEED_CHR -18
#define STX_SUB_NEED_SEMICOLON	-19
#define STX_SUB_NEED_RBRACK	-20
#define STX_SUB_EXP_ITEM_ERROR -21
#define STX_SUB_EXP_FACTOR_ERROR	-22
#define STX_SUB_NEED_EXP	-23
#define STX_SUB_NEED_RPAREN - 24
#define STX_SUB_VAL_PAR_LIST_ERROR	-25
#define STX_SUB_NEED_LPAREN	-26
#define STX_SUB_NEED_STMT	-27
#define STX_SUB_NEED_WHILE	-28
#define STX_SUB_NEED_ADDOP	-29
#define STX_SUB_NEED_RBRACE	-30
#define STX_SUB_NEED_LBRACE	-31
#define GEN_MULTIPLE_DEFS	-32
#define GEN_NO_DEF	-33
#define GEN_NOT_ARY	-34
#define GEN_TOO_MANY_PARAMS	-35
#define GEN_PARAM_NOT_ENOUGH	-36
#define GEN_NO_POINTER	-37
#define GEN_FUNC_NO_RET	-38
#define GEN_CANT_CHANGE_CONST	-39
#define STX_SUB_NEED_VOID_RET -40
#define STX_SUB_NEED_EXP_RET -41

//messages end
//module: file io
char src_file_name[SRC_FILE_NAME_MAX_LEN] = { 0 };

char src[SRC_MAX_LEN+1] = { 0 };
int src_len = 0;
char *src_cursor = src;

int src_line_mark[SRC_MAX_LEN + 1] = { 0 };

void get_src_file_name() {
	cout << "Enter src file path: " << endl;
	scanf("%s", src_file_name);
}


void get_src() {
	ifstream input;
	input.open(src_file_name, std::ifstream::in);
	int line_count = 1;
	char c = input.get();
	while (input.good()) {
		if (src_len < SRC_MAX_LEN) {
			src_line_mark[src_len] = line_count;
			src[src_len] = c;
			if (c == '\n') {
				line_count++;
			}
			c = input.get();
			src_len++;
		}
		else {
			cout << "get_src: warning: Input file is too long. Exceeding chars are ignored." << endl;
			break;
		}
	}
}
//module end: file io
//module: error
struct ErrorItem {
	int id;// = 0;//>0 is normal, <0 is error
	bool halt;// = false;
	int row;// = 0;
	int col;// = 0;
	ErrorItem() {
		id = 0;
		halt = false;
		row = 0;
		col = 0;
	}
};

#define word(code) sym.type==code
#define skip_to_word(sym_condition) {do{get_sym();}while(!(sym_condition));}

void get_position(char *ptr,int *p_row, int *p_col) {
	int offset = ptr - src;
	int row = src_line_mark[offset];
	int col = 0;
	while (src_line_mark[offset] == row) {
		col++;
		offset--;
	}
	(*p_row) = row;
	(*p_col) = col;
}

int error_count = 0;
ErrorItem error_list[MAX_ERROR_NUM];

void print_error(ErrorItem *item) {
	cout << "Error at row " << item->row << " col " << item->col << ": ";
	switch(item->id){
	case LEX_SUB_ILLEGAL_NUMBER: cout << "illegal number format;" << endl; break;
	case LEX_SUB_ILLEGAL_STRING: cout << "illegal char in <string>;" << endl; break;
	case LEX_SUB_UNCLOSED_CHAR: cout << "missing ' after <char>;" << endl; break;
	case LEX_SUB_ILLEGAL_CHAR: cout << "illegal char in <char>;" << endl; break;
	case LEX_SUB_INCOMPLETE_NEQ: cout << "should be != ;" << endl; break;
	case LEX_SUB_UNCLOSED_STRING: cout << "string unclosed;" << endl; break;
	case LEX_SUB_STRING_TOO_LONG: cout << "string is too long(>255 chs);" << endl; break;
	case LEX_SUB_IDF_TOO_LONG: cout << "identifier is too long(>20 chs);" << endl; break;
	case LEX_SUB_NUM_TOO_LONG: cout << "number is too long(>8 chs);" << endl; break;
	case LEX_ILLEGAL_CHAR: cout << "char is not the beginning of any word" << endl; break;
	case STX_SUB_NEED_UINT:cout << "need a uint;" << endl; break;
	case STX_SUB_ARY_INCOMPLETE:cout << "incomplete array;" << endl; break;
	case STX_SUB_CONST_ITEM_ERR:cout << "something wrong with const define item;" << endl; break;
	case STX_SUB_NEED_TYPE_FLAG:cout << "need type flag;" << endl; break;
	case STX_SUB_NEED_INT:cout << "need an integer;" << endl; break;
	case STX_SUB_NEED_IDF:cout << "need an identifier;" << endl; break;
	case STX_SUB_NEED_ASSIGN:cout << "need a '=';" << endl; break;
	case STX_SUB_NEED_CHR:cout << "need a char;" << endl; break;
	case STX_SUB_NEED_SEMICOLON:cout << "need a ';';" << endl; break;
	case STX_SUB_NEED_RBRACK:cout << "need a ']';" << endl; break;
	case STX_SUB_EXP_ITEM_ERROR:cout << "something wrong with this item;" << endl; break;
	case STX_SUB_EXP_FACTOR_ERROR:cout << "something wrong with this factor;" << endl; break;
	case STX_SUB_NEED_EXP:cout << "need expression;" << endl; break;
	case STX_SUB_NEED_RPAREN:cout << "need ')';" << endl; break;
	case STX_SUB_VAL_PAR_LIST_ERROR:cout << "something wrong with value param list;" << endl; break;
	case STX_SUB_NEED_LPAREN:cout << "need '(';" << endl; break;
	case STX_SUB_NEED_STMT:cout << "need statement;" << endl; break;
	case STX_SUB_NEED_WHILE:cout << "need 'while';" << endl; break;
	case STX_SUB_NEED_ADDOP:cout << "need '+' or '-';" << endl; break;
	case STX_SUB_NEED_RBRACE:cout << "need '}';" << endl; break;
	case STX_SUB_NEED_LBRACE:cout << "need '{';" << endl; break;
	case GEN_MULTIPLE_DEFS:cout << "multiple defs;" << endl; break;
	case GEN_NO_DEF:cout << "undefined identifier;" << endl; break;
	case GEN_NOT_ARY: cout << "this isn't an array;" << endl; break;
	case GEN_TOO_MANY_PARAMS: cout << "too many params;" << endl; break;
	case GEN_PARAM_NOT_ENOUGH: cout << "param not enough;" << endl; break;
	case GEN_NO_POINTER: cout << "shouldn't use array name here;" << endl; break;
	case GEN_FUNC_NO_RET: cout << "this func doesn't have return value;" << endl; break;
	case GEN_CANT_CHANGE_CONST: cout << "can't change const value;" << endl; break;
	case STX_SUB_NEED_VOID_RET: cout << "void func can't return value;" << endl; break;
	case STX_SUB_NEED_EXP_RET: cout << "have to return a value;" << endl; break;
	}
}

void print_errors_and_exit() {
	for (int i = 0; i < error_count; i++) {
		print_error(error_list + i);
	}
	system("pause");
	exit(1);
}

void log_error(int err_id, char *src_ptr) {
	ErrorItem *p_cur_error = error_list + error_count;
	p_cur_error->id = err_id;//fill in id
	get_position(src_ptr, &(p_cur_error->row), &(p_cur_error->col));//fill in position
	if (false) {//halt or not
		p_cur_error->halt = true;
	}
	error_count++;
	if (error_count == MAX_ERROR_NUM) {
		print_errors_and_exit();
	}
}
//module end: error
//module: lex analysis
struct SymItem {
	int type;
	bool jammed;// = false;
	int num_value;
	char str_value[STR_MAX_LEN + 1];// = { 0 };
	int str_value_len;
	char chr_value;
	SymItem() {
		type = END;
		jammed = false;
		num_value = 0;
		memset(str_value, 0, STR_MAX_LEN + 1);
		str_value_len = 0;
		chr_value = 0;
	}
};

struct ReportItem {
	int message_id;// = 0;//like reaching end, sub_detect_success/failure
	int error_num;// = 0;
	bool halt;// = false;
	ReportItem() {
		message_id = 0;
		error_num = 0;
		halt = false;
	}
};

SymItem sym;

void reset_sym() {
	sym.type = END;
	sym.jammed = false;
	sym.num_value = 0;
	sym.chr_value = 0;
	sym.str_value_len = 0;
	char *p_str = sym.str_value;
	memset(p_str, 0, STR_MAX_LEN);
}

bool is_letter(char c) {
	return (c == '_') || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

bool is_digit(char c) {
	return ('0' <= c&&c <= '9');
}

bool is_non_zero_digit(char c) {
	return ('1' <= c&&c <= '9');
}

bool is_str_char(char c) {
	return c == 32 || c == 33 || (35 <= c && c <= 126);
}

bool is_white(char c) {
	return c == ' ' || c == '\t' || c == '\n';
}

bool is_chr_char(char c) {
	return is_letter(c) || is_digit(c) || c == '+' || c == '-' || c == '*' || c == '/';
}

void skip_white(char **p_src_cursor) {
	char *internal_cursor = *p_src_cursor;
	while (is_white(*internal_cursor)) {
		internal_cursor++;
	}
	//now points to the nearest non-white char
	//move external cursor
	*p_src_cursor = internal_cursor;
}

int get_idf_type(char *idf) {
	for (int i = 0; i < RESERVER_NUM; i++) {
		if (strcmp(idf, reservers[i]) == 0) {
			return reserver_ids[i];
		}
	}
	return IDENTIFIER;
}

int get_single_chr_type(char c) {
	switch (c) {
	case '0': 	return ZERO; break;
	case '+': 	return ADD; break;
	case '-': 	return SUB; break;
	case '*': 	return MUL; break;
	case '/': 	return DIV; break;
	case ',': 	return COMMA; break;
	case ';': 	return SEMICOLON; break;
	case '(': 	return LPAREN; break;
	case ')': 	return RPAREN; break;
	case '[': 	return LBRACK; break;
	case ']': 	return RBRACK; break;
	case '{': 	return LBRACE; break;
	case '}': 	return RBRACE; break;
	default:    return -1; break;
	}
}

ReportItem detect_identifier(char **p_src_cursor) {
	ReportItem report;
	char *internal_cursor = *p_src_cursor;
	while (is_digit(*internal_cursor) || is_letter(*internal_cursor)) {	
		internal_cursor++;
	}
	int idf_len = internal_cursor - (*p_src_cursor);
	if (idf_len > IDF_MAX_LEN) {//idf too long, ignore its name, report error, continue
		log_error(LEX_SUB_IDF_TOO_LONG, *p_src_cursor);
		report.error_num = 1;
		report.halt = false;
		report.message_id = LEX_SUB_DETECT_SUCCESS;
		sym.type = IDENTIFIER;
		sym.jammed = true;
		//move external cursor
		(*p_src_cursor) = internal_cursor;
		return report;
	}
	else {
		for (int i = 0; i < idf_len;i++) {
			sym.str_value[i] = (*p_src_cursor)[i];
		}
		sym.str_value_len = idf_len;
		sym.type = get_idf_type(sym.str_value);
		sym.jammed = false;
		report.error_num = 0;
		report.halt = false;
		report.message_id = LEX_SUB_DETECT_SUCCESS;
		(*p_src_cursor) = internal_cursor;
		return report;
	}
}

ReportItem detect_uint(char **p_src_cursor) {
	ReportItem report;
	char *internal_cursor = (*p_src_cursor);
	while (is_digit(*internal_cursor)) {
		internal_cursor++;
	}
	//now internal_cursor points to a non-digit char
	int uint_len = internal_cursor - (*p_src_cursor);
	if (uint_len > NUM_MAX_LEN) {// too long
		sym.type = UINT;
		sym.jammed = true;//sym
		log_error(LEX_SUB_NUM_TOO_LONG, (*p_src_cursor));//error
		report.error_num = 1;
		report.message_id = LEX_SUB_DETECT_SUCCESS;
		report.halt = false;//report
		(*p_src_cursor) = internal_cursor;//skip jammed uint
		return report;
	}else{//good
		memcpy(sym.str_value, (*p_src_cursor), uint_len);
		sym.num_value = atoi(sym.str_value);
		sym.type = UINT;
		sym.jammed = false;//sym
		report.error_num = 0;
		report.message_id = LEX_SUB_DETECT_SUCCESS;
		report.halt = false;//report
		(*p_src_cursor) = internal_cursor;//move external cursor
		return report;
	}
}

ReportItem detect_string(char **p_src_cursor) {
	ReportItem report;
	char *internal_cursor = *p_src_cursor;//now internal cursor points to "
	char *content_begin = internal_cursor + 1;
	do {
		internal_cursor++;
	} while (is_str_char(*internal_cursor));
	//now internal cursor points to a non-str-char
	if ((*internal_cursor) == '"') {//enclosed
		int str_len = internal_cursor - content_begin;// >=0
		if (str_len > STR_MAX_LEN) {//enclosed str too long
			sym.type = STRING;
			sym.jammed = true;//sym
			log_error(LEX_SUB_STRING_TOO_LONG, (*p_src_cursor));//error
			report.error_num = 1;
			report.message_id = LEX_SUB_DETECT_SUCCESS;//report;
			(*p_src_cursor) = internal_cursor + 1;//skip jammed str
			return report;
		}
		else {// good
			memcpy(sym.str_value, content_begin, str_len);
			sym.str_value_len = str_len;
			sym.type = STRING;
			sym.jammed = false;//sym
			report.message_id = LEX_SUB_DETECT_SUCCESS;//report
			(*p_src_cursor) = internal_cursor + 1;//move ext cur
			return report;
		}
	}
	else {//the first illegal str char is not "
		sym.type = STRING;
		sym.jammed = true;//sym
		log_error(LEX_SUB_UNCLOSED_STRING, (*p_src_cursor));//error
		report.error_num = 1;
		report.message_id = LEX_SUB_DETECT_SUCCESS;//report;
		(*p_src_cursor) = internal_cursor;//skip jammed str
		return report;
	}
}

ReportItem get_sym() {
	reset_sym();
	ReportItem report;
	skip_white(&src_cursor);
	if (src_cursor == src + src_len) {
		report.message_id = LEX_REACHING_END;
		report.halt = true;
		return report;
		//cout << "Abnormally reaching end." << endl;
		//print_errors_and_exit();
	}
	char c = *src_cursor;
	if (is_letter(c)) {//identifier
		report = detect_identifier(&src_cursor);
		return report;
	}else if (is_non_zero_digit(c)) {//uint
		report = detect_uint(&src_cursor);
		return report;
	}
	else if (c == '<') {// < <=
		if ((*(src_cursor + 1)) == '=') {
			sym.type = LEQ;// <=
			sym.jammed = false;
			src_cursor += 2;
			return report;
		}
		else {
			sym.type = LES;//<
			sym.jammed = false;
			src_cursor += 1;
			return report;
		}
	}
	else if (c == '>') {// > >=
		if ((*(src_cursor + 1)) == '=') {
			sym.type = GEQ;// >=
			sym.jammed = false;
			src_cursor += 2;
			return report;
		}
		else {
			sym.type = GRT;// >
			sym.jammed = false;
			src_cursor += 1;
			return report;
		}
	}
	else if (c == '=') {// = ==
		if ((*(src_cursor + 1)) == '=') {
			sym.type = EQS;// ==
			sym.jammed = false;
			src_cursor += 2;
			return report;
		}
		else {
			sym.type = ASSIGN;// =
			sym.jammed = false;
			src_cursor += 1;
			return report;
		}
	}
	else if (c == '"') {//"..."
		report = detect_string(&src_cursor);
		return report;
	}
	else if (c == '\'') {//'c'  'a':good char 'a:unenclosed good char '#:illegal char, equivalent to a single '
		char ch;
		char *internal_cursor = src_cursor+1;
		if (is_chr_char(*internal_cursor)) {//good char
			ch = *internal_cursor;
			internal_cursor++;
			if ((*internal_cursor) == '\'') {// enclosed good char
				sym.type = CHAR;
				sym.jammed = false;
				sym.chr_value = ch;//sym
				report.message_id = LEX_SUB_DETECT_SUCCESS;//report
				src_cursor = internal_cursor + 1;//move ext cur
				return report;
			}
			else {// unenclosed good char
				sym.type = CHAR;
				sym.jammed = true;
				sym.chr_value = ch;//sym
				log_error(LEX_SUB_UNCLOSED_CHAR, src_cursor);//error
				report.message_id = LEX_SUB_DETECT_SUCCESS;//report
				report.error_num = 1;
				src_cursor = internal_cursor;//move ext cur
				return report;
			}
		}
		else {// illegal chr char
			internal_cursor++;
			sym.type = CHAR;
			sym.jammed = true;//sym
			log_error(LEX_SUB_ILLEGAL_CHAR, src_cursor);//error
			if ((*internal_cursor) == '\'') {// enclosed bad char
				report.message_id = LEX_SUB_DETECT_SUCCESS;
				report.error_num = 1;//report
				src_cursor = internal_cursor + 1;//skip enclosed bad char
				return report;
			}
			else {// unenclosed bad char
				log_error(LEX_SUB_UNCLOSED_CHAR, src_cursor);//error
				report.message_id = LEX_SUB_DETECT_SUCCESS;
				report.error_num = 2;//report
				src_cursor = internal_cursor+1;//skip unenclosed bad char
				return report;
			}
		}
	}
	else if (c == '!') {// !=
		if ((*(src_cursor + 1)) != '=') {//incomplete
			sym.type = NEQ;
			sym.jammed = true;//sym
			log_error(LEX_SUB_INCOMPLETE_NEQ, src_cursor);//error
			report.message_id = LEX_SUB_DETECT_SUCCESS;//report
			report.error_num = 1;
			src_cursor++;//skip this jammed neq
			return report;
		}
		else {//good
			sym.type = NEQ;
			sym.jammed = false;//sym
			report.message_id = LEX_SUB_DETECT_SUCCESS;//report
			src_cursor+=2;//move cur
			return report;
		}
	}
	else {//single char
		int type = get_single_chr_type(c);
		if (type > 0) {
			sym.type = type;
			sym.jammed = false;//sym
			report.message_id = LEX_SUB_DETECT_SUCCESS;//report
			src_cursor += 1;//move cur
			return report;
		}
		else {//a char that is not the beginning of any word, halt
			log_error(LEX_ILLEGAL_CHAR, src_cursor);
			report.error_num = 1;//report
			report.message_id = LEX_REACHING_END;
			report.halt = true;
			return report;
		}
	}
}

struct Word {
	int type;
	int num_value_id;
	int str_value_id;
	char *position;
	Word() {
		type = 0;
		num_value_id = -1;
		str_value_id = -1;
		position = 0;
	}
};
int word_num_values[2000];
int word_num_values_len = 0;
char *word_str_values[2000];
int word_str_values_len = 0;
Word words[2000];
int words_len = 0;
Word *word_cursor = words;

void move(Word **pp) {
	Word *p = (*pp);
	if (p - words <= words_len - 1) {
	//if (p - words < words_len - 1) {
		(*pp) = p + 1;
	}
	else {
		cout << "reaching end." << endl;
		print_errors_and_exit();
	}
}

void add_word_with_str_value(char *position) {
	Word *p_word = words+words_len;
	p_word->type = sym.type;
	p_word->position = position;
	if (!sym.jammed) {
		p_word->str_value_id = word_str_values_len;
		word_str_values[word_str_values_len] = (char*)malloc(sizeof(char)*sym.str_value_len);
		strcpy(word_str_values[word_str_values_len], sym.str_value);
		word_str_values_len++;
	}
	else {
		p_word->str_value_id = -1;
	}
	words_len++;
}

void add_word_with_num_value(char *position) {
	Word *p_word = words+words_len;
	p_word->type = sym.type;
	p_word->position = position;
	if (!sym.jammed) {
		p_word->num_value_id = word_num_values_len;
		if (sym.type == UINT) {
			word_num_values[word_num_values_len] = sym.num_value;
		}
		else {
			word_num_values[word_num_values_len] = sym.chr_value;
		}
		word_num_values_len++;
	}
	else {
		p_word->num_value_id = -1;
	}
	words_len++;
}

void add_word_with_no_value(char *position) {
	Word *p_word = &(words[words_len]);
	p_word->type = sym.type;
	p_word->position = position;
	words_len++;
}

void get_words() {
	while (true) {
		char *sym_begin = src_cursor;
		ReportItem rep = get_sym();
		if (rep.halt) { break; }
		else {
			if (sym.type == UINT || sym.type == CHAR) { add_word_with_num_value(sym_begin); }
			else if (sym.type == IDENTIFIER || sym.type == STRING) { add_word_with_str_value(sym_begin); }
			else { add_word_with_no_value(sym_begin); }
		}
	}
	//reaching end, halted
}

void print_word(ostream &os, Word *p_word) {
	switch (p_word->type) {
	case IDENTIFIER: os << "IDENTIFIER " << word_str_values[p_word->str_value_id]; break;
	case CONSTFLAG: os << "CONSTFLAG  "; break;
	case INTFLAG: os << "INTFLAG    "; break;
	case CHARFLAG: os << "CHARFLAG   "; break;
	case VOIDFLAG: os << "VOIDFLAG   "; break;
	case IFFLAG: os << "IFFLAG     "; break;
	case ELSEFLAG: os << "ELSEFLAG   "; break;
	case DOFLAG: os << "DOFLAG     "; break;
	case WHILEFLAG: os << "WHILEFLAG  "; break;
	case FORFLAG: os << "FORFLAG    "; break;
	case RETURNFLAG: os << "RETURNFLAG "; break;
	case LES: os << "LES        "; break;
	case LEQ: os << "LEQ        "; break;
	case GRT: os << "GRT        "; break;
	case GEQ: os << "GEQ        "; break;
	case EQS: os << "EQS        "; break;
	case ASSIGN: os << "ASSIGN     "; break;
	case STRING: os << "STRING     " << word_str_values[p_word->str_value_id]; break;
	case CHAR: os << "CHAR       " << (char)word_num_values[p_word->num_value_id]; break;
	case UINT: os << "UINT       " << word_num_values[p_word->num_value_id]; break;
	case NEQ: os << "NEQ        "; break;
	case ZERO: os << "ZERO       "; break;
	case ADD: os << "ADD        "; break;
	case SUB: os << "SUB        "; break;
	case MUL: os << "MUL        "; break;
	case DIV: os << "DIV        "; break;
	case COMMA: os << "COMMA      "; break;
	case SEMICOLON: os << "SEMICOLON  "; break;
	case LPAREN: os << "LPAREN     "; break;
	case RPAREN: os << "RPAREN     "; break;
	case LBRACK: os << "LBRACK     "; break;
	case RBRACK: os << "RBRACK     "; break;
	case LBRACE: os << "LBRACE     "; break;
	case RBRACE: os << "RBRACE     "; break;
	}
	os << endl;
}
//module end: lex analysis
//module: syntax analysis
//module: tab
#define TVAR 1
#define TCONST 2
#define TFUNC 3
#define TINT 1
#define TCHAR 2
#define TVOID 3

#define USE true
#define DEF false

struct TabItem {
	char *name;
	int obj;//TVAR, TCONST, TFUNC
	int type;//TINT, TCHAR, TVOID
	//0 for simple var and const; 
	//positive for array, recording its length
	//>=0 for func, recording how many word it uses for local vars(including params) 
	int len;
	bool is_global;
	int offset;//to gp for globals, to fp for locals
	int const_val;//const init val
	int param_num;//param num for funcs; param tab items are just after the func item
	int def_link;//to check if there's multiple defs
	int use_link;//to check if defined when using
	TabItem() {
		//memset(name, 0, IDF_MAX_LEN + 1);
		obj = -1;
		type = -1;
		len = 0;
		is_global = false;
		offset = 0;
		const_val = 0;
		param_num = 0;
		def_link = -1;
		use_link = -1;
	}
};

char *const_strs[MAX_STR_NUM+1];//const_str id starts from 1
int const_str_len = 1;
int new_const_str(char *src) {
	if (const_str_len== MAX_STR_NUM) {
		cout << "fatal: too many strs;" << endl;
		print_errors_and_exit();
	}
	const_strs[const_str_len] = src;
	const_str_len++;
	return const_str_len - 1;
}

TabItem tab[MAX_TAB_LEN];
int tab_len = 0;
//field nodes
int global_end_id = -1;
int prev_nonfunc_id = -1;
int prev_func_id = -1;
bool is_in_func = false;//status bools for specifying the link connection schema of tab item
bool got_first_obj_in_func = false;
int dx = 0;//offset to gp/fp
int global_size = 0;
int main_size = 0;
int main_tmp_size = 0;
int main_entry_label_id = -1;

int tab_add_item() {
	//returns idx of new item
	if (tab_len + 1 > MAX_TAB_LEN) {
		cout << "fatal: tab overflow" << endl;
		print_errors_and_exit();
	}
	else {
		tab_len++;
		return tab_len - 1;
	}
}
int tab_query_nonfunc(Word *p_name, bool mode) {
	//mode: USE or DEF
	//return: name's idx in tab; 
	//in DEF mode, returning positive value means multiple definition;
	//in USE mode, returning -1 means undefined idf.
	char *name = word_str_values[p_name->str_value_id];
	if (mode == USE) {//USE mode
		int itr = prev_nonfunc_id;
		while (itr != -1) {
			if (strcmp(tab[itr].name, name) == 0) {//found
				return itr;
			}
			//not found yet
			itr = tab[itr].use_link;
		}
		//not found
		return -1;
	}
	else {//DEF mode
		int itr = prev_nonfunc_id;
		while (itr != -1) {
			if (strcmp(tab[itr].name, name) == 0) {//found
				return itr;
			}
			//not found yet
			itr = tab[itr].def_link;
		}
		//not found
		return -1;
	}
}
int tab_query_func(Word *p_name) {
	//return: name's idx in tab
	char *name = word_str_values[p_name->str_value_id];
	int itr = prev_func_id;
	while (itr != -1) {
		if (strcmp(tab[itr].name, name) == 0) {//found
			return itr;
		}
		//not found yet
		itr = tab[itr].use_link;
	}
	return -1;
}
int tab_insert_nonfunc(Word *p_name, int obj, int type, int len, int const_val) {
	char *name = word_str_values[p_name->str_value_id];
	int query_id;// = tab_query_nonfunc(name, DEF);//query for multiple defs
	if (is_in_func && !got_first_obj_in_func) query_id = -1; //if is first obj in func, then no multiple defs.
	else query_id = tab_query_nonfunc(p_name, DEF);
	if (query_id == -1) {//no multiple defs
		int tab_id = tab_add_item();
		tab[tab_id].name = name;
		tab[tab_id].obj = obj;
		tab[tab_id].type = type;
		tab[tab_id].const_val = const_val;
		tab[tab_id].len = len;
		tab[tab_id].offset = dx;
		if (obj != TCONST) {//const values are hard-encoded
			if (len == 0) { dx++; }
			else { dx += len; }
		}
		if (!is_in_func) {//mid: global stuff
			tab[tab_id].is_global = true;
			tab[tab_id].use_link = prev_nonfunc_id;
			tab[tab_id].def_link = prev_nonfunc_id;
			prev_nonfunc_id = tab_id;
			global_end_id = tab_id;
		}
		else if (is_in_func && !got_first_obj_in_func) {
			got_first_obj_in_func = true; //is first obj in func
			tab[tab_id].is_global = false;
			tab[tab_id].use_link = global_end_id;
			tab[tab_id].def_link = -1;
			prev_nonfunc_id = tab_id;
		}
		else {//not first obj in func
			tab[tab_id].is_global = false;
			tab[tab_id].use_link = prev_nonfunc_id;
			tab[tab_id].def_link = prev_nonfunc_id;
			prev_nonfunc_id = tab_id;
		}
		return tab_id;
	}
	else {//multiple def
		log_error(GEN_MULTIPLE_DEFS, p_name->position);
		return -1;
	}
}
int tab_insert_func(Word *p_name, int type, int len, int param_num) {//len, param_num may need to be filled back later
	char *name = word_str_values[p_name->str_value_id];
	int query_id = tab_query_func(p_name);
	if (query_id == -1) {//good, no multiple defs
		int tab_id = tab_add_item();
		tab[tab_id].name = name;
		tab[tab_id].obj = TFUNC;
		tab[tab_id].type = type;
		tab[tab_id].len = len;
		tab[tab_id].param_num = param_num;
		tab[tab_id].use_link = prev_func_id;
		tab[tab_id].def_link = prev_func_id;
		prev_func_id = tab_id;
		return tab_id;
	}
	else {// multiple defs
		log_error(GEN_MULTIPLE_DEFS, p_name->position);
		return -1;
	}
}
void tab_print() {
	printf("%10s %10s %10s %10s %10s %10s %10s %10s %10s %10s %10s \n", "tab_id", "name", "obj", "type", "len", "is_global", "offset", "const_val", "param_num", "def_link", "use_link");
	for (int i = 0; i < tab_len; i++) {
		printf("%10d ", i);
		printf("%10s", tab[i].name);
		switch (tab[i].obj) {
		case TCONST:  printf("%10s ", "const"); break;
		case TVAR:  printf("%10s ", "var"); break;
		case TFUNC:  printf("%10s ", "func"); break;
		}
		switch (tab[i].type) {
		case TCHAR:  printf("%10s ", "char"); break;
		case TINT:  printf("%10s ", "int"); break;
		case TVOID: printf("%10s ", "void"); break;
		}
		printf("%10d ", tab[i].len);
		if (tab[i].is_global) printf("%10s ", "yes");
		else printf("%10s ", "no");
		printf("%10d ", tab[i].offset);
		if (tab[i].type == TCHAR) printf("%10c ", tab[i].const_val);
		else printf("%10d ", tab[i].const_val);
		printf("%10d ", tab[i].param_num);
		printf("%10d ", tab[i].def_link);
		printf("%10d \n", tab[i].use_link);
	}
}  
//module end: tab
//module: mid
#define OPADD	1
#define OPSUB	2
#define OPMUL	3
#define OPDIV	4
#define OPGT	5
#define OPGE	6
#define OPLS	7
#define OPLE	8
#define OPEQ	9
#define OPNE	10
#define OPBEQ	11
#define OPJ	12
#define OPCAL	13
#define OPPAS	14
#define OPENDPAS	15
#define OPFUNCBEG	16
#define OPFUNCEND 22
#define OPGET	17
#define OPSET	18
#define OPSCANF	19
#define OPPRINTF	20
#define OPRET	21
#define OPGETRET 23

struct MidItem {
	int label_id;
	int type;
	int op1;
	int op2;
	int target;
	bool op1_is_const;
	bool op2_is_const;
	MidItem() {
		label_id = -1;
		type = 0;
		op1 = 0;
		op2 = 0;
		target = 0;
		op1_is_const = false;
		op2_is_const = false;
	}
};
MidItem mids[MAX_MID_LEN];
int mids_len = 0;
int mid_add_item() {
	if (mids_len == MAX_MID_LEN) {
		cout << "fatal: too many mids" << endl;
		print_errors_and_exit();
	}
	mids_len++;
	return mids_len - 1;
}
int mid_insert(int type, int op1, int op2, int target, bool op1_is_const, bool op2_is_const) {
	int mid_id = mid_add_item();
	mids[mid_id].type = type;
	mids[mid_id].op1 = op1;
	mids[mid_id].op2 = op2;
	mids[mid_id].target = target;
	mids[mid_id].op1_is_const = op1_is_const;
	mids[mid_id].op2_is_const = op2_is_const;
	return mid_id;
}
void mid_print_calc(int i) {
	if (mids[i].target >= 0) { printf("%10s = ", tab[mids[i].target].name); }
	else { printf("    t%05d = ", -mids[i].target); }
	if (mids[i].op1_is_const) {
		printf("%10d", mids[i].op1);
	}
	else {
		if (mids[i].op1 >= 0) { printf("%10s", tab[mids[i].op1].name); }
		else { printf("    t%05d", -mids[i].op1); }
	}
	switch (mids[i].type) {
	case OPADD: printf("  +  "); break;
	case OPSUB: printf("  -  "); break;
	case OPMUL: printf("  *  "); break;
	case OPDIV: printf("  /  "); break;
	case OPGT: printf("  >  "); break;
	case OPGE: printf("  >= "); break;
	case OPLS: printf("  <  "); break;
	case OPLE: printf("  <= "); break;
	case OPEQ: printf("  == "); break;
	case OPNE: printf("  != "); break;
	}
	if (mids[i].op2_is_const) {
		printf("%10d", mids[i].op2);
	}
	else {
		if (mids[i].op2 >= 0) { printf("%10s", tab[mids[i].op2].name); }
		else { printf("    t%05d", -mids[i].op2); }
	}
	printf("\n");
}
void mid_fprint_calc(FILE *out,int i) {
	if (mids[i].target >= 0) { fprintf(out,"%10s = ", tab[mids[i].target].name); }
	else { fprintf(out,"    t%05d = ", -mids[i].target); }
	if (mids[i].op1_is_const) {
		fprintf(out,"%10d", mids[i].op1);
	}
	else {
		if (mids[i].op1 >= 0) { fprintf(out,"%10s", tab[mids[i].op1].name); }
		else { fprintf(out,"    t%05d", -mids[i].op1); }
	}
	switch (mids[i].type) {
	case OPADD: fprintf(out,"  +  "); break;
	case OPSUB: fprintf(out,"  -  "); break;
	case OPMUL: fprintf(out,"  *  "); break;
	case OPDIV: fprintf(out,"  /  "); break;
	case OPGT: fprintf(out,"  >  "); break;
	case OPGE: fprintf(out,"  >= "); break;
	case OPLS: fprintf(out,"  <  "); break;
	case OPLE: fprintf(out,"  <= "); break;
	case OPEQ: fprintf(out,"  == "); break;
	case OPNE: fprintf(out,"  != "); break;
	}
	if (mids[i].op2_is_const) {
		fprintf(out,"%10d", mids[i].op2);
	}
	else {
		if (mids[i].op2 >= 0) { fprintf(out,"%10s", tab[mids[i].op2].name); }
		else { fprintf(out,"    t%05d", -mids[i].op2); }
	}
	fprintf(out,"\n");
}


#define LAB_NAME_LEN 20
struct LabelItem {
	char *name;
	int mid_id;
};
LabelItem labels[MAX_LABEL_NUM];
int labels_len = 0;
int get_new_label() {//return id of new label in labels tab
	if (labels_len == MAX_LABEL_NUM) {
		cout << "fatal: too many labels" << endl;
		print_errors_and_exit();
	}
	LabelItem *p_lab = labels + labels_len;
	p_lab->mid_id = -1;
	//p_lab->name = (char*)malloc(sizeof(char) * LAB_NAME_LEN);
	//sprintf(p_lab->name, "l%d", labels_len);
	labels_len++;
	return labels_len - 1;
}
int simple_label_cnt = 0;
int get_new_simple_label() {
	int lab_id = get_new_label();
	labels[lab_id].name = (char*)malloc(sizeof(char) * LAB_NAME_LEN);
	sprintf(labels[lab_id].name, "l%d", simple_label_cnt);
	simple_label_cnt++;
	return lab_id;
}
int else_label_cnt = 0;
int get_new_else_label() {
	int lab_id = get_new_label();
	labels[lab_id].name = (char*)malloc(sizeof(char) * LAB_NAME_LEN);
	sprintf(labels[lab_id].name, "else%d", else_label_cnt);
	else_label_cnt++;
	return lab_id;
}
int ifend_label_cnt = 0;
int get_new_ifend_label() {
	int lab_id = get_new_label();
	labels[lab_id].name = (char*)malloc(sizeof(char) * LAB_NAME_LEN);
	sprintf(labels[lab_id].name, "ifend%d", ifend_label_cnt);
	ifend_label_cnt++;
	return lab_id;
}
int dobody_label_cnt = 0;
int get_new_dobody_label() {
	int lab_id = get_new_label();
	labels[lab_id].name = (char*)malloc(sizeof(char) * LAB_NAME_LEN);
	sprintf(labels[lab_id].name, "dobody%d", dobody_label_cnt);
	dobody_label_cnt++;
	return lab_id;
}
int forbody_label_cnt = 0;
int get_new_forbody_label() {
	int lab_id = get_new_label();
	labels[lab_id].name = (char*)malloc(sizeof(char) * LAB_NAME_LEN);
	sprintf(labels[lab_id].name, "forbody%d", forbody_label_cnt);
	forbody_label_cnt++;
	return lab_id;
}
int forend_label_cnt = 0;
int get_new_forend_label() {
	int lab_id = get_new_label();
	labels[lab_id].name = (char*)malloc(sizeof(char) * LAB_NAME_LEN);
	sprintf(labels[lab_id].name, "forend%d", forend_label_cnt);
	forend_label_cnt++;
	return lab_id;
}
int get_new_funcbegin_label(int func_id) {
	//func_id: -1 for main func
	int lab_id = get_new_label();
	labels[lab_id].name = (char*)malloc(sizeof(char) * LAB_NAME_LEN);
	if (func_id == -1) {//main
		sprintf(labels[lab_id].name, "f_%s", "main");
	}
	else {
		sprintf(labels[lab_id].name, "f_%s", tab[func_id].name);
	}
	return lab_id;
}
void mid_fprint_single(FILE *out, int i) {
	if (mids[i].label_id != -1) { fprintf(out,"%10s:| ", labels[mids[i].label_id].name); }
	else { fprintf(out,"           | "); }
	switch (mids[i].type) {
	case OPADD:
	case OPSUB:
	case OPMUL:
	case OPDIV:
	case OPGT:
	case OPGE:
	case OPLS:
	case OPLE:
	case OPEQ:
	case OPNE:
		mid_fprint_calc(out,i);
		break;
	case OPBEQ:
		fprintf(out,"        if ( ");
		if (mids[i].op1_is_const) {
			fprintf(out,"%10d", mids[i].op1);
		}
		else {
			if (mids[i].op1 >= 0) { fprintf(out,"%10s", tab[mids[i].op1].name); }
			else { fprintf(out,"    t%05d", -mids[i].op1); }
		}
		fprintf(out,"  == ");
		if (mids[i].op2_is_const) {
			fprintf(out,"%10d", mids[i].op2);
		}
		else {
			if (mids[i].op2 >= 0) { fprintf(out,"%10s", tab[mids[i].op2].name); }
			else { fprintf(out,"    t%05d", -mids[i].op2); }
		}
		fprintf(out," )  goto%10s\n", labels[mids[i].target].name);
		break;
	case OPJ:
		fprintf(out,"      goto   %10s\n", labels[mids[i].target].name);
		break;
	case OPCAL:
		fprintf(out,"      call   %s\n", tab[mids[i].target].name);
		break;
	case OPPAS:
		if (mids[i].target >= 0) { fprintf(out,"      pass   %10s\n", tab[mids[i].target].name); }
		else { fprintf(out,"      pass       t%05d\n", -mids[i].target); }
		break;
	case OPENDPAS:
		fprintf(out,"   endpass   %s\n", tab[mids[i].target].name);
		break;
	case OPFUNCBEG:
		fprintf(out," funcbegin   ");
		if (mids[i].target == -1) { fprintf(out,"      main"); }
		else { fprintf(out,"%10s", tab[mids[i].target].name); }
		fprintf(out,"-------------------------\n");
		break;
	case OPFUNCEND:
		fprintf(out,"   funcend   ");
		if (mids[i].target == -1) { fprintf(out,"      main"); }
		else { fprintf(out,"%10s", tab[mids[i].target].name); }
		fprintf(out,"-------------------------\n");
		break;
	case OPGET:
		if (mids[i].op1 >= 0) { fprintf(out,"%10s = ", tab[mids[i].op1].name); }
		else { fprintf(out,"    t%05d = ", -mids[i].op1); }
		fprintf(out,"%10s  [  ", tab[mids[i].target].name);
		if (mids[i].op2 >= 0) { fprintf(out,"%10s]\n", tab[mids[i].op2].name); }
		else { fprintf(out,"    t%05d ]\n", -mids[i].op2); }
		break;
	case OPSET:
		fprintf(out,"%10s [ ", tab[mids[i].target].name);
		if (mids[i].op2 >= 0) { fprintf(out,"%10s  ]= ", tab[mids[i].op2].name); }
		else { fprintf(out,"    t%05d  ]= ", -mids[i].op2); }
		if (mids[i].op1 >= 0) { fprintf(out,"%10s\n", tab[mids[i].op1].name); }
		else { fprintf(out,"    t%05d\n", -mids[i].op1); }
		break;
	case OPSCANF:
		fprintf(out,"     scanf   %10s\n", tab[mids[i].target].name);
		break;
	case OPPRINTF:
		fprintf(out,"    printf   ");
		if (mids[i].op1_is_const) { fprintf(out,"\"%s\"", const_strs[mids[i].target]); }
		else {
			if (mids[i].target >= 0) { fprintf(out,"%10s", tab[mids[i].target].name); }
			else { fprintf(out,"    t%05d", -mids[i].target); }
		}
		fprintf(out,"\n");
		break;
	case OPRET:
		fprintf(out,"RETURNVALUE= ");
		if (mids[i].op1_is_const) {
			fprintf(out,"%10d", mids[i].op1);
		}
		else {
			if (mids[i].op1 >= 0) { fprintf(out,"%10s", tab[mids[i].op1].name); }
			else { fprintf(out,"    t%05d", -mids[i].op1); }
		}
		fprintf(out,"\n");
		break;
	case OPGETRET:
		if (mids[i].target >= 0) { fprintf(out,"%10s = RETURNVALUE\n", tab[mids[i].target].name); }
		else { fprintf(out,"    t%05d = RETURNVALUE\n", -mids[i].target); }
		break;
	}
}
void mid_print() {
	for (int i = 0; i < mids_len; i++) {
		if (mids[i].label_id != -1) {printf("%10s:| ", labels[mids[i].label_id].name);}
		else {printf("           | ");}
		switch (mids[i].type) {
		case OPADD:
		case OPSUB:
		case OPMUL:
		case OPDIV:
		case OPGT:
		case OPGE:
		case OPLS:
		case OPLE:
		case OPEQ:
		case OPNE:
			mid_print_calc(i);
			break;
		case OPBEQ:
			printf("        if ( ");
			if (mids[i].op1_is_const) {
				printf("%10d", mids[i].op1);
			}
			else {
				if (mids[i].op1 >= 0) { printf("%10s", tab[mids[i].op1].name); }
				else { printf("    t%05d", -mids[i].op1); }
			}
			printf("  == ");
			if (mids[i].op2_is_const) {
				printf("%10d", mids[i].op2);
			}
			else {
				if (mids[i].op2 >= 0) { printf("%10s", tab[mids[i].op2].name); }
				else { printf("    t%05d", -mids[i].op2); }
			}
			printf(" )  goto%10s\n", labels[mids[i].target].name);
			break;
		case OPJ: 
			printf("      goto   %10s\n", labels[mids[i].target].name);
			break;
		case OPCAL:
			printf("      call   %s\n", tab[mids[i].target].name);
			break;
		case OPPAS:
			if (mids[i].target >= 0) { printf("      pass   %10s\n", tab[mids[i].target].name); }
			else { printf("      pass       t%05d\n", -mids[i].target); }
			break;
		case OPENDPAS:
			printf("   endpass   %s\n", tab[mids[i].target].name);
			break;
		case OPFUNCBEG: 
			printf(" funcbegin   ");
			if (mids[i].target == -1) { printf("      main"); }
			else { printf("%10s", tab[mids[i].target].name); }
			printf("-------------------------\n");
			break;
		case OPFUNCEND: 
			printf("   funcend   ");
			if (mids[i].target == -1) { printf("      main"); }
			else { printf("%10s", tab[mids[i].target].name); }
			printf("-------------------------\n");
			break;
		case OPGET:
			if (mids[i].op1 >= 0) { printf("%10s = ", tab[mids[i].op1].name); }
			else { printf("    t%05d = ", -mids[i].op1); }
			printf("%10s  [  ", tab[mids[i].target].name);
			if (mids[i].op2 >= 0) { printf("%10s]\n", tab[mids[i].op2].name); }
			else { printf("    t%05d ]\n", -mids[i].op2); }
			break;
		case OPSET:
			printf("%10s [ ", tab[mids[i].target].name);
			if (mids[i].op2 >= 0) { printf("%10s  ]= ", tab[mids[i].op2].name); }
			else { printf("    t%05d  ]= ", -mids[i].op2); }
			if (mids[i].op1 >= 0) { printf("%10s\n", tab[mids[i].op1].name); }
			else { printf("    t%05d\n", -mids[i].op1); }
			break;
		case OPSCANF: 
			printf("     scanf   %10s\n", tab[mids[i].target].name);
			break;
		case OPPRINTF: 
			printf("    printf   ");
			if (mids[i].op1_is_const) {printf("\"%s\"", const_strs[mids[i].target]);}
			else {
				if (mids[i].target >= 0) {printf("%10s", tab[mids[i].target].name);}
				else {printf("    t%05d", -mids[i].target);}
			}
			printf("\n");
			break;
		case OPRET: 
			printf("RETURNVALUE= ");
			if (mids[i].op1_is_const) {
				printf("%10d", mids[i].op1);
			}
			else {
				if (mids[i].op1 >= 0) { printf("%10s", tab[mids[i].op1].name); }
				else { printf("    t%05d", -mids[i].op1); }
			}
			printf("\n");
			break;
		case OPGETRET:
			if (mids[i].target >= 0) { printf("%10s = RETURNVALUE\n", tab[mids[i].target].name); }
			else { printf("    t%05d = RETURNVALUE\n", -mids[i].target); }
			break;
		}
	}
}
void mid_mark_next(int label_id) {
	mids[mids_len].label_id = label_id;
	//just for showing, all labels pointing to a same position should be of same name
	for (int i = 0; i < labels_len; i++) {
		if (labels[i].mid_id == mids_len) {
			strcpy(labels[i].name, labels[label_id].name);
		}
	}
	labels[label_id].mid_id = mids_len;
}

int cur_tmp_id = 0;//tmp id is negative!
int get_new_tmp() {
	cur_tmp_id--;
	return cur_tmp_id;
}
void reset_tmp() {
	cur_tmp_id = 0;
}
//module end: mid

#define STX_DEBUG(content) if(false) cout<<content<<endl;

bool is_type_flag(int type) {
	return (type == CHARFLAG || type == INTFLAG);
}

bool is_int_begin(int type) {
	return (type == ADD || type == SUB ||
		type == UINT || type == ZERO);
}

bool is_exp_begin(int type) {
	return (type == ADD || type == SUB ||
		type == IDENTIFIER || type == UINT ||
		type == ZERO || type == CHAR ||
		type == LPAREN);
}

bool is_factor_begin(int type) {
	return (type==IDENTIFIER||type==LPAREN||type==CHAR||is_int_begin(type));
}

bool is_item_begin(int type) {
	return is_factor_begin(type);
}

bool is_stat_begin(int type) {
	return (type == IFFLAG || type == DOFLAG ||
		type == FORFLAG || type == LBRACE ||
		type == IDENTIFIER || type == PRINTFLAG ||
		type == SCANFLAG || type == SEMICOLON ||
		type == RETURNFLAG);
}

bool is_compare_op(int type) {
	return (type == LES || type == LEQ ||
		type == GRT || type == GEQ ||
		type == EQS || type == NEQ);
}

//detect success return true, else return false;

bool stx_type(Word **p_word_cur, int *typ) {
	Word *p_word = *(p_word_cur);
	if ((p_word->type) == INTFLAG || (p_word->type) == CHARFLAG) {
		if (p_word->type == INTFLAG) {//mid: out:type
			*typ = TINT;
		}
		else {
			*typ = TCHAR;
		}
		move(&p_word);
		STX_DEBUG("detected <type flag>");
		*(p_word_cur) = p_word;
		return true;
	}
	else { 
		log_error(STX_SUB_NEED_TYPE_FLAG,p_word->position);//shouldn't get here
		return false; 
	}
}

bool stx_int(Word **p_word_cur, int *val) {
	Word *p_word = (*p_word_cur);
	int sign = 1;
	int int_val;
	if (p_word->type == ADD || p_word->type == SUB) {
		sign = (p_word->type == ADD) ? (1) : (-1);
		move(&p_word);
		if (p_word->type == UINT) {//good signed int
			int_val = sign*word_num_values[p_word->num_value_id];
			(*val) = int_val;//mid: value
			STX_DEBUG("detected <int> value: " << int_val);
			move(&p_word);
			(*p_word_cur) = p_word;
			return true;
		}
		else {//bad signed int
			log_error(STX_SUB_NEED_UINT, p_word->position);
			return false;
		}
	}
	else if(p_word->type==UINT){// good uint
		int_val = word_num_values[p_word->num_value_id];
		(*val) = int_val;//mid: value
		STX_DEBUG("detected <int> value: " << int_val);
		move(&p_word);
		(*p_word_cur) = p_word;
		return true;
	}
	else if (p_word->type == ZERO) {// good zero				
		int_val = 0;
		(*val) = int_val;//mid: value
		STX_DEBUG("detected <int> value: " << int_val);
		move(&p_word);
		(*p_word_cur) = p_word;
		return true;
	}
	else {
		log_error(STX_SUB_NEED_INT, p_word->position);//shouldn't get here
		return false;
	}
}

bool stx_const_int_item(Word **p_word_cur) {
	Word *p_word = (*p_word_cur);
	if (p_word->type == IDENTIFIER) {
		Word *p_name = p_word;//mid: const name
		move(&p_word);
		if (p_word->type == ASSIGN) {
			move(&p_word);
			if (is_int_begin(p_word->type)) {
				int int_val = 0;//mid: int val
				if (stx_int(&p_word,&int_val)) {//success
					STX_DEBUG("detected <const int item>");
					tab_insert_nonfunc(p_name, TCONST, TINT, 0, int_val);//mid: insert to tab
					(*p_word_cur) = p_word;					
					return true;
				}
				else {//something wrong in stx_int
					return false;
				}
			}
			else {//need integer
				log_error(STX_SUB_NEED_INT, p_word->position);
				return false;
			}
		}
		else {//need assign
			log_error(STX_SUB_NEED_ASSIGN, p_word->position);
			return false;
		}
	}
	else {//need idf
		log_error(STX_SUB_NEED_IDF, p_word->position);
		return false;
	}
}

bool stx_const_chr_item(Word **p_word_cur) {
	Word *p_word = (*p_word_cur);
	if (p_word->type == IDENTIFIER) {
		Word *p_name = p_word;//mid: name
		move(&p_word);
		if (p_word->type == ASSIGN) {
			move(&p_word);
			if (p_word->type==CHAR) {//success
				int chr_val = word_num_values[p_word->num_value_id];
				STX_DEBUG("detected <const chr item> value: '"<<(char)chr_val<<"'");
				tab_insert_nonfunc(p_name, TCONST, TCHAR, 0, chr_val);//mid: insert to tab
				move(&p_word);
				(*p_word_cur) = p_word;
				return true;
			}
			else {//need char
				log_error(STX_SUB_NEED_CHR, p_word->position);
				return false;
			}
		}
		else {//need assign
			log_error(STX_SUB_NEED_ASSIGN, p_word->position);
			return false;
		}
	}
	else {//need idf
		log_error(STX_SUB_NEED_IDF, p_word->position);
		return false;
	}
}

bool stx_const_define(Word **p_word_cur) {
	Word *p_word = (*p_word_cur);
	if (p_word->type == INTFLAG) {
		move(&p_word);
		if (p_word->type == IDENTIFIER) {
			if (stx_const_int_item(&p_word)) {
				while (true) {
					if (p_word->type == COMMA) {
						move(&p_word);
						if (p_word->type == IDENTIFIER) {
							if (stx_const_int_item(&p_word)) {
								continue;
							}
							else {//something wrong with repeating item
								return false;
							}
						}
						else {
							log_error(STX_SUB_NEED_IDF, p_word->position);
							return false;
						}
					}
					else {//no more repeating items
						break;
					}
				}
				//success:
				STX_DEBUG("detected <const define>");
				(*p_word_cur) = p_word;
				return true;
			}
			else {//something wrong with first item
				return false;
			}
		}
		else {//need idf(need const int item)
			log_error(STX_SUB_NEED_IDF, p_word->position);
			return false;
		}
	}
	else if(p_word->type==CHARFLAG){
		move(&p_word);
		if (p_word->type == IDENTIFIER) {
			if (stx_const_chr_item(&p_word)) {
				while (true) {
					if (p_word->type == COMMA) {
						move(&p_word);
						if (p_word->type == IDENTIFIER) {
							if (stx_const_chr_item(&p_word)) {
								continue;
							}
							else {//something wrong with repeating item
								return false;
							}
						}
						else {
							log_error(STX_SUB_NEED_IDF, p_word->position);
							return false;
						}
					}
					else {//no more repeating items
						break;
					}
				}
				//success:
				STX_DEBUG("detected <const define>");
				(*p_word_cur) = p_word;
				return true;
			}
			else {//something wrong with first item
				return false;
			}
		}
		else {//need idf(need const int item)
			log_error(STX_SUB_NEED_IDF, p_word->position);
			return false;
		}
	}
	else {// need type flag
		log_error(STX_SUB_NEED_TYPE_FLAG, p_word->position);//shouldn't get here
		return false;
	}
}

bool stx_const_declare(Word **p_word_cur) {
	Word *p_word = (*p_word_cur);
	while (true) {
		if (p_word->type == CONSTFLAG) {
			move(&p_word);
			if (p_word->type == INTFLAG || p_word->type == CHARFLAG) {
				if (stx_const_define(&p_word)) {
					if (p_word->type == SEMICOLON) {//good, repeat
						move(&p_word);
						continue;
					}
					else {//need semicolon, skip after ;,  continue
						log_error(STX_SUB_NEED_SEMICOLON, p_word->position);
						while (p_word->type != SEMICOLON) {
							move(&p_word);
						}
						move(&p_word);
						continue;
					}
				}
				else {//something wrong with repeating define; skip after ;
					while (p_word->type != SEMICOLON) {
						move(&p_word);
					}
					move(&p_word);
					continue;
				}
			}
			else {//need type flag; skip after ;
				log_error(STX_SUB_NEED_TYPE_FLAG, p_word->position);
				while (p_word->type != SEMICOLON) {
					move(&p_word);
				}
				move(&p_word);
				continue;
			}
		}
		else {//no more repeatings
			break;
		}
	}
	//success, or skipped because of error
	STX_DEBUG("detected <const declare>");
	(*p_word_cur) = p_word;
	return true;
}

bool stx_var_define_item(Word **p_word_cur,int var_type) {
	Word *p_word = (*p_word_cur);
	if (p_word->type == IDENTIFIER) {
		Word *p_name = p_word;//mid: var name		
		move(&p_word);
		if (p_word->type == LBRACK) {// ary[
			move(&p_word);
			if (p_word->type == UINT) {// ary[12
				int ary_len = word_num_values[p_word->num_value_id];//mid: ary len
				move(&p_word);
				if (p_word->type == RBRACK) {//success: ary[12]
					STX_DEBUG("detected<var define item>, array");
					tab_insert_nonfunc(p_name, TVAR, var_type, ary_len, 0);//mid: insert to tab
					move(&p_word);
					(*p_word_cur) = p_word;
					return true;
				}
				else {//need rbrack
					log_error(STX_SUB_NEED_RBRACK,p_word->position);			
					return false;
				}
			}
			else {//need uint
				log_error(STX_SUB_NEED_UINT, p_word->position);
				return false;
			}
		}
		else {//success: simple var
			STX_DEBUG("detected <var define item>, simple var");
			tab_insert_nonfunc(p_name, TVAR, var_type, 0, 0);//mid: insert simple var to tab
			(*p_word_cur) = p_word;
			if (is_in_func && !got_first_obj_in_func) {//mid: is first obj in func?
				got_first_obj_in_func = true;
				//is first obj in func
			}
			else {
				//not first obj in func
			}
			return true;
		}
	}
	else {// need idf
		log_error(STX_SUB_NEED_IDF, p_word->position);
		return false;
	}
}

bool stx_var_define(Word **p_word_cur) {
	Word *p_word = (*p_word_cur);
	if (p_word->type==INTFLAG||p_word->type==CHARFLAG) {
		int var_type = (p_word->type == INTFLAG) ? (TINT) : (TCHAR);//mid: var type
		move(&p_word);
		if (p_word->type == IDENTIFIER) {
			if (stx_var_define_item(&p_word,var_type)) {
				while (true) {
					if (p_word->type == COMMA) {
						move(&p_word);
						if (p_word->type == IDENTIFIER) {
							if (stx_var_define_item(&p_word,var_type)) {
								continue;
							}else{//something wrong with repeating item
								return false;
							}
						}
						else {//need idf
							log_error(STX_SUB_NEED_IDF, p_word->position);
							return false;
						}
					}
					else {//no more repeating items
						break;
					}
				}
				//success:
				STX_DEBUG("detected <var define>");
				(*p_word_cur) = p_word;
				return true;
			}
			else {//something wrong with beginning item
				return false;
			}
		}
		else {//need idf
			log_error(STX_SUB_NEED_IDF, p_word->position);
			return false;
		}
	}
	else {
		log_error(STX_SUB_NEED_TYPE_FLAG, p_word->position);//shouldn't get here
		return false;
	}
}

bool stx_var_declare(Word **p_word_cur) {
	Word *p_word = (*p_word_cur);
	while (true) {
		if (is_type_flag(p_word->type)&&((p_word+2)->type!=LPAREN)) {
			if (stx_var_define(&p_word)) {
				if (p_word->type == SEMICOLON) {
					move(&p_word);
					continue;
				}
				else {//need semicolon
					log_error(STX_SUB_NEED_SEMICOLON, p_word->position);
					continue;
				}
			}
			else {//something wrong with defines; skip after ;
				while (p_word->type != SEMICOLON) {
					move(&p_word);
				}
				move(&p_word);
			}
		}
		else {//no more repeating defines
			break;
		}
	}
	//success:
	STX_DEBUG("detected <var declare>");
	(*p_word_cur) = p_word;
	return true;
}

bool stx_define_func_ret_head(Word **p_word_cur, int *p_func_tab_id) {
	Word *p_word = (*p_word_cur);
	if (is_type_flag(p_word->type)) {
		int ret_type = (p_word->type == INTFLAG) ? (TINT) : (TCHAR);//mid: get func return type
		move(&p_word);
		if (p_word->type == IDENTIFIER) {
			(*p_func_tab_id) = tab_insert_func(p_word, ret_type, 0, 0);//mid: insert func to tab; report tab id
			move(&p_word);
			(*p_word_cur) = p_word;
			STX_DEBUG("detected <define func ret head>");
			return true;
		}
		else {//need idf
			log_error(STX_SUB_NEED_IDF, p_word->position);
			return false;
		}
	}
	else {//need type
		log_error(STX_SUB_NEED_TYPE_FLAG, p_word->position);
		return false;
	}
}

bool stx_param_list(Word **p_word_cur, int *p_param_num) {
	Word *p_word = (*p_word_cur);
	int param_num = 0;//mid: param num counter
	if (is_type_flag(p_word->type)) {
		int param_type = (p_word->type == INTFLAG) ? (TINT) : (TCHAR);//mid: param type
		move(&p_word);
		if (p_word->type == IDENTIFIER) {
			//got_first_obj_in_func = true;//mid: the first param must be the first obj in a function.
			tab_insert_nonfunc(p_word, TVAR, param_type, 0, 0);//mid: insert param to tab
			param_num++;//mid: increase param cnt
			move(&p_word);
			while (true) {
				if (p_word->type == COMMA) {
					move(&p_word);
					if (is_type_flag(p_word->type)) {
						int param_type1 = (p_word->type == INTFLAG) ? (TINT) : (TCHAR);//mid: param type(following repeats)
						move(&p_word);
						if (p_word->type == IDENTIFIER) {
							tab_insert_nonfunc(p_word, TVAR, param_type1, 0, 0);//mid: insert following param to tab
							param_num++;//mid: increase param cnt
							move(&p_word);
							continue;
						}
						else {//need idf
							log_error(STX_SUB_NEED_IDF, p_word->position);
							return false;
						}
					}
					else {//need type
						log_error(STX_SUB_NEED_TYPE_FLAG, p_word->position);
						return false;
					}
				}
				else {//no more repeats
					break;
				}
			}
			//not empty, break;
			STX_DEBUG("detected <param list>,not empty");
			(*p_param_num) = param_num;//mid: report param num
			(*p_word_cur) = p_word;
			return true;
		}
		else {//need idf
			log_error(STX_SUB_NEED_IDF, p_word->position);
			return false;
		}
	}
	else {//empty param list
		STX_DEBUG("detected <param list>, empty");
		(*p_param_num) = param_num;//mid: no params
		(*p_word_cur) = p_word;
		return true;
	}
}

bool stx_expression(Word **p_word_cur) { return false; }//stub
bool stx_expression(Word **p_word_cur, int *val_id);//pre-define

bool stx_param_val(Word **p_word_cur, int func_tab_id, int **p_param_id_vec) {
	Word *p_word = (*p_word_cur);
	//GENBEGIN
	int param_num = 0;
	int *param_id_vec = (int*)malloc(sizeof(int)*MAX_PARAM_NUM);
	(*p_param_id_vec) = param_id_vec;
	//GENEND
	if (is_exp_begin(p_word->type)) {
		//GENBEGIN
		int exp_tmp_id = 0;
		param_num = 0;
		//GENEND
		if (stx_expression(&p_word,&exp_tmp_id)) {//beginning expression
			//GENBEGIN
			param_id_vec[param_num] = exp_tmp_id;
			param_num++;
			if (param_num >= MAX_PARAM_NUM) { cout << "fatal: param overflow;" << endl; print_errors_and_exit(); }
			//mid_insert(OPPAS, 0, 0, exp_tmp_id, false, false);
			//GENEND
			while (true) {
				if (p_word->type == COMMA) {
					move(&p_word);
					if (is_exp_begin(p_word->type)) {
						if (stx_expression(&p_word,&exp_tmp_id)) {//repeating expression
							//GENBEGIN
							param_id_vec[param_num] = exp_tmp_id;
							param_num++;
							if (param_num >= MAX_PARAM_NUM) { cout << "fatal: param overflow;" << endl; print_errors_and_exit(); }
							//mid_insert(OPPAS, 0, 0, exp_tmp_id, false, false);
							//GENEND
							continue;
						}
						else {//something wrong with repeating expressions
							return false;
						}
					}
					else {//need exp after comma
						log_error(STX_SUB_NEED_EXP, p_word->position);
						return false;
					}
				}
				else {//no repeatings					
					break;
				}
			}
			//success
			STX_DEBUG("detected <param val>, not empty");
			//GENBEGIN
			if (param_num > tab[func_tab_id].param_num) {
				log_error(GEN_TOO_MANY_PARAMS, p_word->position);
			}
			else if (param_num < tab[func_tab_id].param_num) {
				log_error(GEN_PARAM_NOT_ENOUGH, p_word->position);
			}
			//(*p_param_id_vec) = param_id_vec;
			//GENEND			
			(*p_word_cur) = p_word;
			return true;
		}
		else {//something wrong with beginning expression
			return false;
		}
	}
	else {//epsilon
		STX_DEBUG("detected <param val>, empty");
		//GENBEGIN
		if (param_num > tab[func_tab_id].param_num) {
			log_error(GEN_TOO_MANY_PARAMS, p_word->position);
		}
		else if (param_num < tab[func_tab_id].param_num) {
			log_error(GEN_PARAM_NOT_ENOUGH, p_word->position);
		}
		//(*p_param_id_vec) = param_id_vec;
		//GENEND
		return true;
	}
}


bool stx_exp_factor(Word **p_word_cur, int *val_id) {
	Word *p_word = (*p_word_cur);
	if (p_word->type == IDENTIFIER) {
		//GENBEGIN
		Word *p_name = p_word;//mid: get name
		//GENEND
		move(&p_word);
		if (p_word->type == LBRACK) {
			move(&p_word);
			if (is_exp_begin(p_word->type)) {			
				//GENBEGIN
				int index_id = 0;//mid: ary index tmp/tab id
				//GENEND
				if (stx_expression(&p_word,&index_id)) {
					if (p_word->type == RBRACK) {//success
						STX_DEBUG("detected <exp_factor>, array item");
						//GENBEGIN
						int tmp_id = get_new_tmp();//mid: gen a tmp for result
						int ary_tab_id = tab_query_nonfunc(p_name, USE);//mid: find ary in tab
						if (ary_tab_id != -1) {//mid: found
							if (tab[ary_tab_id].len > 0) {//is ary
								mid_insert(OPGET, tmp_id, index_id, ary_tab_id, false, false);//mid: gen mid
								(*val_id) = tmp_id;//mid: report tmp id for ary[idx]
							}
							else {//not ary
								log_error(GEN_NOT_ARY, p_name->position);
							}
						}
						else {//mid: not found
							log_error(GEN_NO_DEF, p_name->position);
						}
						//GENEND
						move(&p_word);
						(*p_word_cur) = p_word;
						return true;
					}
					else {//need rbrack
						log_error(STX_SUB_NEED_RBRACK, p_word->position);
						return false;
					}
				}
				else {//something wrong with expression
					return false;
				}
			}
			else {//need exp
				log_error(STX_SUB_NEED_EXP, p_word->position);
				return false;
			}
		}
		else if (p_word->type == LPAREN) {
			//GENBEGIN
			int *param_id_vec;
			int func_param_num = 0;
			int func_tab_id = tab_query_func(p_name);//mid: query func in tab
			if (func_tab_id != -1) {//found
				if (tab[func_tab_id].type==TVOID) {
					log_error(GEN_FUNC_NO_RET, p_name->position);
				}
				func_param_num = tab[func_tab_id].param_num;
				//mid_insert(OPCAL, 0, 0, func_tab_id,false,false);//mid: gen cal
			}
			else {//undefined
				log_error(GEN_NO_DEF, p_name->position);
			}
			//GENEND
			move(&p_word);
			if (stx_param_val(&p_word,func_tab_id,&param_id_vec)) {				
				if (p_word->type == RPAREN) {//success
					STX_DEBUG("detected <exp_factor>, func call");
					//GENBEGIN
					mid_insert(OPCAL, 0, 0, func_tab_id, false, false);//mid: gen cal
					for (int i = 0; i < func_param_num; i++) {
						mid_insert(OPPAS, 0, 0, param_id_vec[i], false, false);//mid: gen pass
					}
					mid_insert(OPENDPAS, 0, 0, func_tab_id, false, false);//gen endpass
					int tmp_id = get_new_tmp();
					mid_insert(OPGETRET, 0, 0, tmp_id, false, false);
					(*val_id) = tmp_id;
					//GENEND
					move(&p_word);
					(*p_word_cur) = p_word;
					return true;
				}
				else {//need rparen
					log_error(STX_SUB_NEED_RPAREN, p_word->position);
					return false;
				}
			}
			else {//something wrong with val params
				log_error(STX_SUB_VAL_PAR_LIST_ERROR, p_word->position);
				return false;
			}
			//GENBEGIN
			free(param_id_vec);
			//GENEND
		}
		else {
			STX_DEBUG("detected <exp factor>, simple var");
			//GENBEGIN
			int tab_id = tab_query_nonfunc(p_name,USE);//mid: find idf in tab
			if (tab_id != -1) {//mid: found
				if (tab[tab_id].len > 0) {
					log_error(GEN_NO_POINTER, p_name->position);
				}
				//int tmp_id = get_new_tmp();
				//mid_insert(OPADD, 0, tab_id, tmp_id, true, false);
				//(*val_id) = tmp_id;//report idf id
				(*val_id) = tab_id;//if is in tab, report tab id
			}
			else {//undefined
				log_error(GEN_NO_DEF, p_name->position);
			}
			//GENEND
			(*p_word_cur) = p_word;
			return true;
		}
	}
	else if (p_word->type == CHAR) {
		STX_DEBUG("detected <exp factor>, char");
		//GENBEGIN
		int tmp_id = get_new_tmp();
		mid_insert(OPADD, 0, word_num_values[p_word->num_value_id], tmp_id, true, true);
		(*val_id) = tmp_id;
		//GENEND
		move(&p_word);
		(*p_word_cur) = p_word;
		return true;
	}
	else if (p_word->type == LPAREN) {
		move(&p_word);
		if (is_exp_begin(p_word->type)) {
			//GENBEGIN
			int exp_tmp_id = 0;
			//GENEND
			if (stx_expression(&p_word,&exp_tmp_id)) {
				if (p_word->type == RPAREN) {//success
					STX_DEBUG("detected <exp factor>, (exp)");
					//GENBEGIN
					(*val_id) = exp_tmp_id;
					//GENEND
					move(&p_word);
					(*p_word_cur) = p_word;
					return true;
				}
				else {//need rpar
					log_error(STX_SUB_NEED_RPAREN, p_word->position);
					return false;
				}
			}
			else {//something wrong with expression
				return false;
			}
		}
		else {//need an exp
			log_error(STX_SUB_NEED_EXP, p_word->position);
			return false;
		}
	}
	else if (is_int_begin(p_word->type)) {
		int int_val = 0;//mid: int_val
		if (stx_int(&p_word,&int_val)) {
			STX_DEBUG("detected <exp factor>, int");
			//GENBEGIN
			int tmp_id = get_new_tmp();
			mid_insert(OPADD,0,int_val,tmp_id,true,true);
			(*val_id) = tmp_id;
			//GENEND
			(*p_word_cur) = p_word;
			return true;
		}
		else {//something wrong with int
			return false;
		}
	}
	else {// not a factor
		return false;
	}
}

bool stx_exp_item(Word **p_word_cur,int *val_id) {
	Word *p_word = (*p_word_cur);
	if (is_factor_begin(p_word->type)) {
		//GENBEGIN
		int begin_factor_tmp_tab_id = 0;//beginning factor tmp id, accumulate following factors to it
		int accumulator_tmp_id = 0;//generate and return this tmp if there's more than one factor
		bool accumulator_generated = false;
		//GENEND
		if (stx_exp_factor(&p_word,&begin_factor_tmp_tab_id)) {//beginning factor
			if (begin_factor_tmp_tab_id < 0) {//use the tmp of first factor as accumulator, if first factor reports a tmp
				accumulator_tmp_id = begin_factor_tmp_tab_id;
				accumulator_generated = true;
			}
			while (true) {
				if (p_word->type == MUL || p_word->type == DIV) {
					int optype = (p_word->type==MUL)?(OPMUL):(OPDIV);
					move(&p_word);
					if (is_factor_begin(p_word->type)) {
						//GENBEGIN
						int repeat_factor_tmp_id = 0;
						//GENEND
						if (stx_exp_factor(&p_word, &repeat_factor_tmp_id)) {
							//GENBEGIN
							if (!accumulator_generated) {//if the first factor reports a tab_id and there exists following factors, gen A new accumulator
								accumulator_tmp_id = get_new_tmp();
								accumulator_generated = true;
								mid_insert(OPADD, 0, begin_factor_tmp_tab_id, accumulator_tmp_id, true, false);//accumulator init as first factor
							}
							mid_insert(optype, accumulator_tmp_id, repeat_factor_tmp_id, accumulator_tmp_id, false, false);
							//GENEND
							continue;
						}
						else {//something wrong with repeating factor
							return false;
						}
					}
					else {//need factor after mul op
						log_error(STX_SUB_EXP_FACTOR_ERROR, p_word->position);
						return false;
					}
				}
				else {//no more repeatings
					break;
				}
			}
			//success
			STX_DEBUG("detected <exp item>");
			//GENBEGIN
			if (accumulator_generated) {//one tmp factor; or more than one factor
				(*val_id) = accumulator_tmp_id;
			}
			else {//only one tab_id factor
				(*val_id) = begin_factor_tmp_tab_id;
			}
			//GENEND
			(*p_word_cur) = p_word;
			return true;
		}
		else {//something wrong with beginning factor;
			return false;
		}
	}
	else {//need beginning factor
		log_error(STX_SUB_EXP_FACTOR_ERROR, p_word->position);
		return false;
	}
}

bool stx_expression(Word **p_word_cur, int *val_id) {
	Word *p_word = (*p_word_cur);
	//GENBEGIN
	int begin_item_tmp_tab_id = 0;
	int repeat_item_tmp_id = 0;
	int accumulator_tmp_id = 0;
	bool accumulator_generated = false;
	//GENEND
	int optype = OPADD;
	if (p_word->type == ADD || p_word->type == SUB) {
		optype = (p_word->type == ADD) ? (OPADD) : (OPSUB);
		move(&p_word);
	}
	if (is_item_begin(p_word->type)) {
		if (stx_exp_item(&p_word,&begin_item_tmp_tab_id)) {//beginning item
			//GENBEGIN
			if (begin_item_tmp_tab_id < 0) {//if first item is tmp, use it as accumulator
				accumulator_tmp_id = begin_item_tmp_tab_id;
				accumulator_generated = true;
				if (optype == OPSUB) {//if negative, reverse
					mid_insert(OPSUB, 0, accumulator_tmp_id, accumulator_tmp_id, true, false);
				}
			}
			else {//if first item reports tab_id
				if (optype == OPSUB) {//if reverse, still need a new tmp
					accumulator_tmp_id = get_new_tmp();
					accumulator_generated = true;
					mid_insert(OPSUB, 0, begin_item_tmp_tab_id, accumulator_tmp_id, true, false);//init accumulator
				}
			}
			//GENEND
			while (true) {
				if (p_word->type == ADD || p_word->type == SUB) {
					optype = (p_word->type == ADD) ? (OPADD) : (OPSUB);
					move(&p_word);
					if (is_item_begin(p_word->type)) {
						if (stx_exp_item(&p_word,&repeat_item_tmp_id)) {//repeating items
							//GENBEGIN
							if (!accumulator_generated) {//if first item reports tab_id and there's following items, the still need a new tmp for accumulation
								accumulator_tmp_id = get_new_tmp();
								accumulator_generated = true;
								mid_insert(OPADD, 0, begin_item_tmp_tab_id, accumulator_tmp_id, true, false);//it should be opadd here.
							}
							mid_insert(optype, accumulator_tmp_id, repeat_item_tmp_id, accumulator_tmp_id, false, false);
							//GENEND
							continue;
						}
						else {//something wrong with repeating item
							return false;
						}
					}
					else {//need exp item after add op
						log_error(STX_SUB_EXP_ITEM_ERROR, p_word->position);
						return false;
					}
				}
				else {//no following add items
					break;
				}
			}
			//success
			STX_DEBUG("detected <expression>");
			//GENBEGIN
			if (accumulator_generated) {
				(*val_id) = accumulator_tmp_id;
			}
			else {
				(*val_id) = begin_item_tmp_tab_id;
			}
			//GENEND
			(*p_word_cur) = p_word;
			return true;
		}
		else {//something wrong with beginning item
			return false;
		}
	}
	else {//need a beginning exp item
		log_error(STX_SUB_EXP_ITEM_ERROR, p_word->position);
		return false;
	}
}

bool stx_assign_stat(Word **p_word_cur) {
	Word *p_word = (*p_word_cur);
	if (p_word->type == IDENTIFIER) {
		//GENBEGIN
		Word *p_name = p_word;
		int assign_to_tab_id = tab_query_nonfunc(p_name,USE);
		if (assign_to_tab_id == -1) {
			log_error(GEN_NO_DEF, p_name->position);
		}
		else if(tab[assign_to_tab_id].obj==TCONST){
			log_error(GEN_CANT_CHANGE_CONST, p_name->position);
		}
		int ary_index_tmp_id = 0;
		//GENEND
		move(&p_word);
		bool ary_elem = false;
		if (p_word->type == LBRACK) {//ary elem assign
			move(&p_word);
			if (is_exp_begin(p_word->type)) {
				//GENBEGIN
				if (tab[assign_to_tab_id].len == 0) {
					log_error(GEN_NOT_ARY, p_name->position);
				}
				//GENEND
				if (stx_expression(&p_word,&ary_index_tmp_id)) {
					if (p_word->type == RBRACK) {//success on ary indexing
						ary_elem = true;
						move(&p_word);
					}
					else {
						log_error(STX_SUB_NEED_RBRACK, p_word->position);
						return false;
					}
				}
				else {//someting wrong with index expression
					return false;
				}
			}
			else {//need exp
				log_error(STX_SUB_NEED_EXP, p_word->position);
				return false;
			}		
		}
		//nothing bad in ary indexing, continue; or not ary at all.
		if (p_word->type == ASSIGN) {
			move(&p_word);
			if (is_exp_begin(p_word->type)) {
				//GENBEGIN
				int assign_val_tmp_id = 0;
				//GENEND
				if (stx_expression(&p_word,&assign_val_tmp_id)) {//success
					if (ary_elem) { 
						STX_DEBUG("detected <assign stat>, ary element"); 
						//GENBEGIN
						mid_insert(OPSET, assign_val_tmp_id, ary_index_tmp_id, assign_to_tab_id,false,false);
						//GENEND
					}
					else { 
						STX_DEBUG("detected <assign stat>, simple var"); 
						//GENBEGIN
						mid_insert(OPADD, 0, assign_val_tmp_id, assign_to_tab_id, true, false);
						//GENEND
					}
					(*p_word_cur) = p_word;
					return true;
				}
				else {//something wrong with assign expression
					return false;
				}
			}
			else {// need value expression
				log_error(STX_SUB_NEED_EXP, p_word->position);
				return false;
			}
		}
		else {//need assign
			log_error(STX_SUB_NEED_ASSIGN, p_word->position);
			return false;
		}
	}
	else {//need idf
		log_error(STX_SUB_NEED_IDF, p_word->position);
		return false;
	}
}

bool need_ret_with_exp = false;
bool got_ret_with_exp = false;

bool stx_ret_stat(Word **p_word_cur) {
	Word *p_word = (*p_word_cur);
	//GENBEGIN
	int ret_exp_tmp_id = 0;
	//GENEND
	if (p_word->type == RETURNFLAG) {
		move(&p_word);
		//cout << word_strs[(p_word)->type] << endl;//debug
		if (p_word->type == LPAREN) {
			move(&p_word);
			if (is_exp_begin(p_word->type)) {
				if (stx_expression(&p_word,&ret_exp_tmp_id)) {
					if (p_word->type == RPAREN) {//success
						STX_DEBUG("detected <ret stat>, with value");
						got_ret_with_exp = true;
						if (!need_ret_with_exp) {
							log_error(STX_SUB_NEED_VOID_RET, p_word->position);
						}
						//GENBEGIN
						mid_insert(OPRET, ret_exp_tmp_id, 0, 0, false, false);
						//GENEND
						move(&p_word);
						(*p_word_cur) = p_word;
						return true;
					}
					else {//need rparen
						log_error(STX_SUB_NEED_RPAREN, p_word->position);
						return false;
					}					
				}
				else {//something wrong with ret exp
					return false;
				}
			}
			else {//need expression
				log_error(STX_SUB_NEED_EXP, p_word->position);
				return false;
			}
		}
		else {//return without (exp)
			STX_DEBUG("detected <ret stat>, without value");
			//GENBEGIN
			mid_insert(OPRET, 0, 0, 0, true, false);
			//GENEND
			(*p_word_cur) = p_word;
			return true;
		}
	}
	else {//not return stat
		return false;//shouldn't get here
	}
}

bool stx_read_stat(Word **p_word_cur) {
	Word *p_word = (*p_word_cur);
	//GENBEGIN
	int target_tab_id = 0;
	//GENEND
	if (p_word->type == SCANFLAG) {
		move(&p_word);
		if (p_word->type == LPAREN) {
			move(&p_word);
			if (p_word->type == IDENTIFIER) {//begining idf
				//GENBEGIN
				target_tab_id = tab_query_nonfunc(p_word, USE);
				if (target_tab_id == -1) {//not found
					log_error(GEN_NO_DEF, p_word->position);
				}
				else {//found
					if (tab[target_tab_id].obj == TCONST) {
						log_error(GEN_CANT_CHANGE_CONST, p_word->position);
					}
					if (tab[target_tab_id].len != 0) {
						log_error(GEN_NO_POINTER, p_word->position);
					}
				}
				mid_insert(OPSCANF, 0, 0, target_tab_id, false, false);
				//GENEND
				move(&p_word);
				while (true) {
					if (p_word->type == COMMA) {
						move(&p_word);
						if (p_word->type == IDENTIFIER) {
							//GENBEGIN
							target_tab_id = tab_query_nonfunc(p_word, USE);
							if (target_tab_id == -1) {//not found
								log_error(GEN_NO_DEF, p_word->position);
							}
							else {//found
								if (tab[target_tab_id].obj == TCONST) {
									log_error(GEN_CANT_CHANGE_CONST, p_word->position);
								}
								if (tab[target_tab_id].len != 0) {
									log_error(GEN_NO_POINTER, p_word->position);
								}
							}
							mid_insert(OPSCANF, 0, 0, target_tab_id, false, false);
							//GENEND
							move(&p_word);
							continue;
						}
						else {//need idf
							log_error(STX_SUB_NEED_IDF, p_word->position);
							return false;
						}
					}
					else {//no more repeating idfs
						break;
					}
				}
				if (p_word->type == RPAREN) {//success
					STX_DEBUG("detected <read stat>");
					move(&p_word);
					(*p_word_cur) = p_word;
					return true;
				}
				else {//need rparen
					log_error(STX_SUB_NEED_RPAREN, p_word->position);
					return false;
				}
			}
			else {//need beginning idf
				log_error(STX_SUB_NEED_IDF, p_word->position);
				return false;
			}
		}
		else {//need lparen
			log_error(STX_SUB_NEED_LPAREN, p_word->position);
			return false;
		}
	}
	else {
		return false;//shouldn't get here
	}
}

bool smart_print_check_char(Word *p_word) {
	//check whether printf needs to print a char or int
	//return true for print char
	//p_word points to the beginning word of the <exp> in printf("str",<exp) or printf(<exp>)
	if (p_word->type == CHAR) {//begin with char like 'a'
		move(&p_word);
		if (p_word->type == RPAREN) {//char type 1: printf('a');
			return true;
		}
		else {//followed by other stuff
			return false;
		}
	}
	else if (p_word->type == IDENTIFIER) {
		Word *p_name = p_word;
		move(&p_word);
		if (p_word->type == RPAREN) {//like printf(<idf>)
			int tab_id = tab_query_nonfunc(p_name,USE);
			if (tab_id != -1) {
				if (tab[tab_id].type == TCHAR) {//char type 2: char c; printf(c);
					return true;
				}
			}
			return false;
		}
		else if (p_word->type == LPAREN) {//like printf(<idf>(...)...);
			int paren_cnt = 1;
			while (true) {
				move(&p_word);
				if (p_word->type == LPAREN) {
					paren_cnt++;
				}
				else if (p_word->type == RPAREN) {
					paren_cnt--;
				}
				if (paren_cnt == 0) {
					break;
				}
			}
			//if no syntax error, p_word is now at:
			//printf(<idf>(...)..);
			//                ^
			move(&p_word);
			if (p_word->type == RPAREN) {
				//printf(<idf>(...));
				//                 ^
				int tab_id = tab_query_func(p_name);
				if (tab[tab_id].type == TCHAR) {//char type 3: char func(); printf(func());
					return true;
				}
			}
			return false;
		}
		else if (p_word->type == LBRACK) {
			int brack_cnt = 1;
			while (true) {
				move(&p_word);
				if (p_word->type == LBRACK) {
					brack_cnt++;
				}
				else if (p_word->type == RBRACK) {
					brack_cnt--;
				}
				if (brack_cnt == 0) {
					break;
				}
			}
			//if no syntax error, p_word is now at:
			//printf(<idf>[...]..);
			//                ^
			move(&p_word);
			if (p_word->type == RPAREN) {
				//printf(<idf>[...]);
				//                 ^
				int tab_id = tab_query_nonfunc(p_name,USE);
				if (tab[tab_id].type == TCHAR) {//char type 4: char ary[3]; printf(ary[1]);
					return true;
				}
			}
			return false;
		}
	}
	else {//not char
		return false;
	}
}

bool stx_write_stat(Word **p_word_cur) {
	Word *p_word = (*p_word_cur);
	//GENBEGIN
	int print_exp_tmp_id = 0;
	//GENEND
	if (p_word->type == PRINTFLAG) {
		move(&p_word);
		if (p_word->type == LPAREN) {
			move(&p_word);
			if (p_word->type == STRING) {
				//GENBEGIN
				int str_id = new_const_str(word_str_values[p_word->str_value_id]);
				mid_insert(OPPRINTF, 0, 0, str_id, true, false);
				//GENEND
				move(&p_word);
				if (p_word->type == COMMA) {
					move(&p_word);
					if (is_exp_begin(p_word->type)) {
						//call smart_print_check_char(); if returns true, we needs to print char.
						bool force_char = smart_print_check_char(p_word);
						if (stx_expression(&p_word,&print_exp_tmp_id)) {
							//GENBEGIN
							mid_insert(OPPRINTF, 0, 0, print_exp_tmp_id, false, force_char);
							//GENEND
							//now need an rpar
						}
						else {//something wrong with expression
							return false;
						}
					}
					else {//need exp
						log_error(STX_SUB_NEED_EXP, p_word->position);
						return false;
					}
				}
				//else there's no exp.
			}
			else if (is_exp_begin(p_word->type)) {
				//call smart_print_check_char() here
				bool force_char = smart_print_check_char(p_word);
				if (stx_expression(&p_word,&print_exp_tmp_id)) {
					//GENBEGIN
					mid_insert(OPPRINTF, 0, 0, print_exp_tmp_id, false, force_char);
					//GENEND
					//now need an rpar
				}
				else {//something wrong with exp
					return false;
				}
			}
			else {//need exp
				log_error(STX_SUB_NEED_EXP, p_word->position);
				return false;
			}
			//now need an rpar
			if (p_word->type == RPAREN) {//success
				STX_DEBUG("detected <write stat>");
				move(&p_word);
				(*p_word_cur) = p_word;
				return true;
			}
			else {//need rpar
				log_error(STX_SUB_NEED_RPAREN, p_word->position);
				return false;
			}
		}
		else {// need lpar
			log_error(STX_SUB_NEED_LPAREN, p_word->position);
			return false;
		}
	}
	else {
		return false;//shouldn't get here
	}
}

bool stx_call_func(Word **p_word_cur) {
	Word *p_word = (*p_word_cur);
	//GENBEGIN
	int func_tab_id = 0;
	int func_param_num = 0;
	int *param_id_vec;
	//GENEND
	if (p_word->type == IDENTIFIER) {
		//GENBEGIN
		func_tab_id = tab_query_func(p_word);
		if (func_tab_id == -1) {//not found
			log_error(GEN_NO_DEF, p_word->position);
		}
		else {//found
			func_param_num = tab[func_tab_id].param_num;
		}
		//mid_insert(OPCAL, 0, 0, func_tab_id, false, false);
		//GENEND
		move(&p_word);
		if (p_word->type == LPAREN) {
			move(&p_word);
			if (stx_param_val(&p_word,func_tab_id,&param_id_vec)) {
				if (p_word->type == RPAREN) {
					STX_DEBUG("detected <call func>");
					//GENBEGIN
					mid_insert(OPCAL, 0, 0, func_tab_id, false, false);
					for (int i = 0; i < func_param_num; i++) {
						mid_insert(OPPAS, 0, 0, param_id_vec[i], false, false);
					}
					mid_insert(OPENDPAS, 0, 0, func_tab_id, false, false);
					//GENEND
					move(&p_word);
					(*p_word_cur) = p_word;
					return true;
				}
				else {//need rpar
					log_error(STX_SUB_NEED_RPAREN, p_word->position);
					return true;
				}
			}
			else {//something wrong with val params
				return false;
			}
			//GENBEGIN
			free(param_id_vec);
			//GENEND
		}
		else {//need lpar
			log_error(STX_SUB_NEED_LPAREN, p_word->position);
			return false;
		}
	}
	else {
		return false;//shouldn't get here
	}
}

bool stx_statement(Word **p_word_cur);//pre define

bool stx_stats(Word **p_word_cur) {
	Word *p_word = (*p_word_cur);
	while (true) {
		//int type = p_word->type;
		if (is_stat_begin(p_word->type)) {
			if (stx_statement(&p_word)) {
				continue;
			}
			else {//error in stat, skip after ; or } and continue
				while (!(p_word->type == SEMICOLON || p_word->type == RBRACE)) {
					move(&p_word);
				}
				move(&p_word);
				continue;
			}			
		}
		else {//no more repeating stats
			break;
		}
	}
	STX_DEBUG("detected <stats>");
	(*p_word_cur) = p_word;
	return true;
}

bool stx_condition(Word **p_word_cur) { return false; }//stub
bool stx_condition(Word **p_word_cur, int *p_val_id) {
	Word *p_word = (*p_word_cur);
	//GENBEGIN
	int exp1_tmp_id = 0;
	//GENEND
	if (is_exp_begin(p_word->type)) {
		if (stx_expression(&p_word,&exp1_tmp_id)) {
			if (is_compare_op(p_word->type)) {
				//GENBEGIN
				int optype = 0;
				switch (p_word->type) {
				case LES:optype = OPLS; break;
				case LEQ:optype = OPLE; break;
				case GRT:optype = OPGT; break;
				case GEQ:optype = OPGE; break;
				case EQS:optype = OPEQ; break;
				case NEQ:optype = OPNE; break;
				}
				//GENEND
				move(&p_word);
				if (is_exp_begin(p_word->type)) {
					//GENBEGIN
					int exp2_tmp_id = 0;
					//GENEND
					if (stx_expression(&p_word,&exp2_tmp_id)) {//success: double exp
						STX_DEBUG("detected <condition>, double exp");
						//GENBEGIN
						int tmp_id = get_new_tmp();
						mid_insert(optype, exp1_tmp_id, exp2_tmp_id, tmp_id, false, false);
						(*p_val_id) = tmp_id;
						//GENEND
						(*p_word_cur) = p_word;
						return true;
					}
					else {//somehing wrong with second exp
						return false;
					}
				}
				else {//need exp
					log_error(STX_SUB_NEED_EXP, p_word->position);
					return false;
				}
			}
			else {//success: single exp
				STX_DEBUG("detected <condition>, single exp");
				//GENBEGIN
				int tmp_id = get_new_tmp();
				mid_insert(OPNE,0,exp1_tmp_id,tmp_id,true,false);
				(*p_val_id) = tmp_id;
				//(*p_val_id) = exp1_tmp_id;
				//GENEND
				(*p_word_cur) = p_word;
				return true;
			}
		}
		else {//something wrong with beginning exp
			return false;
		}
	}
	else {//need beginning exp
		log_error(STX_SUB_NEED_EXP, p_word->position);
		return false;
	}
}

bool stx_condition_stat(Word **p_word_cur) {
	Word *p_word = (*p_word_cur);
	//GENBEGIN
	int cond_tmp_id = 0;
	//GENEND
	if (p_word->type == IFFLAG) {
		move(&p_word);
		if (p_word->type == LPAREN){
			move(&p_word);
		}
		else {//need lparen
			log_error(STX_SUB_NEED_LPAREN, p_word->position);
		}
		if (is_exp_begin(p_word->type)) {
			if (stx_condition(&p_word,&cond_tmp_id)) {
				if (p_word->type == RPAREN) {
					move(&p_word);
				}
				else {//need rparen
					log_error(STX_SUB_NEED_RPAREN, p_word->position);
				}
			}
			else {//condition failure, skip after )
				while (!(p_word->type == RPAREN)) {
					move(&p_word);
				}
				move(&p_word);
			}
		}
		else {//condition not found, skip after )
			log_error(STX_SUB_NEED_EXP, p_word->position);
			while (!(p_word->type == RPAREN)) {
				move(&p_word);
			}
			move(&p_word);
		}		
		//now p_word after )
		//GENBEGIN
		int else_label_id = get_new_else_label();
		int beq_ins_id = mid_insert(OPBEQ, 0, cond_tmp_id, else_label_id, true, false);
		//GENEND
		if (is_stat_begin(p_word->type)) {
			stx_statement(&p_word);
			//GENBEGIN
			int ifend_label_id = get_new_ifend_label();
			mid_insert(OPJ, 0, 0, ifend_label_id, false, false);
			//GENEND
			if (p_word->type == ELSEFLAG) {
				move(&p_word);
				if (is_stat_begin(p_word->type)) {
					//GENBEGIN
					mid_mark_next(else_label_id);
					//GENEND
					stx_statement(&p_word);
					STX_DEBUG("detected <condition stat>, with else");
					//GENBEGIN
					mid_mark_next(ifend_label_id);
					//GENEND
					(*p_word_cur) = p_word;
					return true;
				}
				else {//need stmt; skip after ; or }
					log_error(STX_SUB_NEED_STMT, p_word->position);
					while (!(p_word->type == SEMICOLON || p_word->type == RBRACE)) {
						move(&p_word);
					}
					move(&p_word);
					(*p_word_cur) = p_word;
					return true;
				}
			}
			else {//success: no else
				STX_DEBUG("detected <condition stat>, no else");
				//GENBEGIN
				mids[beq_ins_id].target = ifend_label_id;//else label is dumped
				mid_mark_next(ifend_label_id);				
				//GENEND
				(*p_word_cur) = p_word;
				return true;
			}
		}
		else {//need stmt; skip after ; or }
			log_error(STX_SUB_NEED_STMT, p_word->position);
			while (!(p_word->type == SEMICOLON || p_word->type == RBRACE)) {
				move(&p_word);
			}
			move(&p_word);
			(*p_word_cur) = p_word;
			return true;
		}
	}
	else {
		return false;//shouldn't get here
	}
}

bool stx_do_loop(Word **p_word_cur) {
	Word *p_word = (*p_word_cur);
	if (p_word->type == DOFLAG) {
		move(&p_word);
		//GENBEGIN
		int dobody_label_id = get_new_dobody_label();
		mid_mark_next(dobody_label_id);
		//GENEND
		if (is_stat_begin(p_word->type)) {
			stx_statement(&p_word);
		}
		else {//need stmt; skip after ; or }
			log_error(STX_SUB_NEED_STMT, p_word->position);
			while (!(p_word->type == SEMICOLON || p_word->type == RBRACE)) {
				move(&p_word);
			}
			move(&p_word);
		}
		if (p_word->type == WHILEFLAG) {
			move(&p_word);
		}
		else {//need while
			log_error(STX_SUB_NEED_WHILE, p_word->position);
		}
		if (p_word->type == LPAREN) {
			move(&p_word);
		}else {//need lparen
			log_error(STX_SUB_NEED_WHILE, p_word->position);
		}
		if (is_exp_begin(p_word->type)) {
			//GENBEGIN
			int cond_tmp_id = 0;
			//GENEND
			if (stx_condition(&p_word,&cond_tmp_id)) {
				//GENBEGIN
				mid_insert(OPEQ, 0, cond_tmp_id, cond_tmp_id, true, false);
				mid_insert(OPBEQ, 0, cond_tmp_id, dobody_label_id, true, false);
				//GENEND
				if (p_word->type == RPAREN) {//success
					STX_DEBUG("detected <do loop>");
					move(&p_word);
					(*p_word_cur) = p_word;
					return true;
				}
				else {//need rparen
					log_error(STX_SUB_NEED_RPAREN, p_word->position);
					(*p_word_cur) = p_word;
					return true;
				}
			}
			else {//condition failure, skip after ) and return true
				while (!(p_word->type == RPAREN)) {
					move(&p_word);
				}
				move(&p_word);
				(*p_word_cur) = p_word;
				return true;
			}
		}
		else {//need condition(exp); skip after ) and return true
			log_error(STX_SUB_NEED_IDF, p_word->position);
			while (!(p_word->type == RPAREN)) {
				move(&p_word);
			}
			move(&p_word);
			(*p_word_cur) = p_word;
			return true;
		}
	}
	else {
		return false;//shouldn't come here
	}
}

bool stx_for_loop(Word **p_word_cur) {
	Word *p_word = (*p_word_cur);
	//GENBEGIN
	int loop_var_tab_id = 0;
	int loop_var_init_exp_tmp_id = 0;
	int loop_cond_tmp_id = 0;
	int loop_inc_var1_tab_id = 0;
	int loop_inc_var2_tab_id = 0;
	int loop_pace = 1;
	int forbody_label_id = 0;
	int forend_label_id = 0;
	//GENEND
	if (p_word->type == FORFLAG) {
		move(&p_word);
		if (p_word->type == LPAREN) {
			move(&p_word);
		}
		else {//need lparen
			log_error(STX_SUB_NEED_LPAREN, p_word->position);
		}
		if (p_word->type == IDENTIFIER) {
			//GENBEGIN
			loop_var_tab_id = tab_query_nonfunc(p_word, USE);
			if (loop_var_tab_id == -1) {//not found
				log_error(GEN_NO_DEF, p_word->position);
			}
			else {//found
				if (tab[loop_var_tab_id].obj == TCONST) {
					log_error(GEN_CANT_CHANGE_CONST, p_word->position);
				}
				if (tab[loop_var_tab_id].len != 0) {//can't use ary name here
					log_error(GEN_NO_POINTER, p_word->position);
				}
			}
			//GENEND
			move(&p_word);
		}
		else {//need idf
			log_error(STX_SUB_NEED_IDF, p_word->position);
		}
		if (p_word->type == ASSIGN) {
			move(&p_word);
		}
		else {//need assign
			log_error(STX_SUB_NEED_ASSIGN, p_word->position);
		}
		if (is_exp_begin(p_word->type)) {
			if (stx_expression(&p_word,&loop_var_init_exp_tmp_id)) {
				if (p_word->type == SEMICOLON) {
					move(&p_word);
				}
				else {//need ;
					log_error(STX_SUB_NEED_SEMICOLON, p_word->position);
				}
			}
			else {//exp failure, skip after ;
				while (p_word->type != SEMICOLON) {
					move(&p_word);
				}
				move(&p_word);
			}
		}
		else {//need exp, skip after ;
			log_error(STX_SUB_NEED_EXP, p_word->position);
			while (p_word->type != SEMICOLON) {
				move(&p_word);
			}
			move(&p_word);
		}
		//p_word after ; anyway
		//GENBEGIN
		mid_insert(OPADD, 0, loop_var_init_exp_tmp_id, loop_var_tab_id, true, false);
		int forbody_label_id = get_new_forbody_label();
		mid_mark_next(forbody_label_id);
		//GENEND
		if (is_exp_begin(p_word->type)) {
			if (stx_condition(&p_word,&loop_cond_tmp_id)) {
				//GENBEGIN
				forend_label_id = get_new_forend_label();
				mid_insert(OPBEQ, 0, loop_cond_tmp_id, forend_label_id, true, false);
				//GENEND
				if (p_word->type == SEMICOLON) {
					move(&p_word);
				}
				else {//need ;
					log_error(STX_SUB_NEED_SEMICOLON, p_word->position);
				}
			}
			else {//condition failure, skip after ;
				while (p_word->type != SEMICOLON) {
					move(&p_word);
				}
				move(&p_word);
			}
		}
		else {//need exp, skip after ;
			log_error(STX_SUB_NEED_EXP, p_word->position);
			while (p_word->type != SEMICOLON) {
				move(&p_word);
			}
			move(&p_word);
		}
		//p_word after second ; anyway
		if (p_word->type == IDENTIFIER) {
			//GENBEGIN
			loop_inc_var1_tab_id = tab_query_nonfunc(p_word, USE);
			if (loop_inc_var1_tab_id == -1) {//not found
				log_error(GEN_NO_DEF, p_word->position);
			}
			else {//found
				if (tab[loop_inc_var1_tab_id].obj == TCONST) {
					log_error(GEN_CANT_CHANGE_CONST, p_word->position);
				}
				if (tab[loop_inc_var1_tab_id].len != 0) {//can't use ary name here
					log_error(GEN_NO_POINTER, p_word->position);
				}
			}
			//GENEND
			move(&p_word);
		}
		else {//need idf
			log_error(STX_SUB_NEED_IDF, p_word->position);
		}
		if (p_word->type == ASSIGN) {
			move(&p_word);
		}
		else {//need assign
			log_error(STX_SUB_NEED_ASSIGN, p_word->position);
		}
		if (p_word->type == IDENTIFIER) {
			//GENBEGIN
			loop_inc_var2_tab_id = tab_query_nonfunc(p_word, USE);
			if (loop_inc_var2_tab_id == -1) {//not found
				log_error(GEN_NO_DEF, p_word->position);
			}
			else {//found
				if (tab[loop_inc_var2_tab_id].len != 0) {//can't use ary name here
					log_error(GEN_NO_POINTER, p_word->position);
				}
			}
			//GENEND
			move(&p_word);
		}
		else {//need idf
			log_error(STX_SUB_NEED_IDF, p_word->position);
		}
		if (p_word->type == ADD || p_word->type == SUB) {
			//GENBEGIN
			loop_pace = (p_word->type == ADD) ? (1) : (-1);//pace sign
			//GENEND
			move(&p_word);
		}
		else {// need addop
			log_error(STX_SUB_NEED_ADDOP, p_word->position);
			move(&p_word);//special
		}
		if (p_word->type == UINT) {
			//GENBEGIN
			loop_pace *= word_num_values[p_word->num_value_id];
			//GENEND
			move(&p_word);
		}
		else {//need uint
			log_error(STX_SUB_NEED_UINT, p_word->position);
		}
		if (p_word->type == RPAREN) {
			move(&p_word);
		}
		else {//need rparen
			log_error(STX_SUB_NEED_RPAREN, p_word->position);
		}
		if (is_stat_begin(p_word->type)) {//success
			stx_statement(&p_word);
			STX_DEBUG("detected <for loop>");
			//GENBEGIN
			mid_insert(OPADD, loop_inc_var2_tab_id, loop_pace, loop_inc_var1_tab_id, false, true);//gen increase
			mid_insert(OPJ, 0, 0, forbody_label_id, false, false);
			mid_mark_next(forend_label_id);
			//GENEND
			(*p_word_cur) = p_word;
			return true;
		}
		else {//stmt not found, skip after ; or }
			while (!(p_word->type == SEMICOLON||p_word->type==RBRACE)) {
				move(&p_word);
			}
			move(&p_word);
			(*p_word_cur) = p_word;
			return true;
		}
	}
	else {
		return false;//shouldn't reach here
	}
}

bool stx_loop_stat(Word **p_word_cur) {
	Word *p_word = (*p_word_cur);
	if (p_word->type == DOFLAG) {
		stx_do_loop(&p_word);
		(*p_word_cur) = p_word;
		return true;
	}
	else if (p_word->type == FORFLAG) {
		stx_for_loop(&p_word);
		(*p_word_cur) = p_word;
		return true;
	}
	else {
		return false;//shouldn't get here
	}
}

bool stx_statement(Word **p_word_cur) {
	Word *p_word = (*p_word_cur);
	bool need_semicolon = false;
	bool no_problem = true;
	if (p_word->type == IFFLAG) {
		stx_condition_stat(&p_word);
	}
	else if (p_word->type == FORFLAG ||
		p_word->type == DOFLAG) {
		stx_loop_stat(&p_word);
	}
	else if (p_word->type == LBRACE) {
		move(&p_word);
		stx_stats(&p_word);
		if (p_word->type == RBRACE) {
			move(&p_word);//goto success
		}
		else {
			log_error(STX_SUB_NEED_RBRACE, p_word->position);
			(*p_word_cur) = p_word;
			return true;
		}
	}
	else if (p_word->type == IDENTIFIER) {
		move(&p_word);
		need_semicolon = true;
		if (p_word->type == LPAREN) {
			p_word--;
			no_problem = stx_call_func(&p_word);
		}
		else { // not call, should be assign
			p_word--;
			no_problem = stx_assign_stat(&p_word);
		}
	}
	else if (p_word->type == SCANFLAG) {
		need_semicolon = true;
		no_problem = stx_read_stat(&p_word);
	}
	else if (p_word->type == PRINTFLAG) {
		need_semicolon = true;
		no_problem = stx_write_stat(&p_word);
	}
	else if (p_word->type == SEMICOLON) {
		need_semicolon = false;
		move(&p_word);
	}
	else if (p_word->type == RETURNFLAG) {
		need_semicolon = true;
		no_problem = stx_ret_stat(&p_word);
	}
	else {
		return false;//shouldn't get here
	}
	if (!no_problem) {//skip after ; or }
		need_semicolon = false;
		while (!(p_word->type == SEMICOLON || p_word->type == RBRACE)) {
			move(&p_word);
		}
		move(&p_word);
	}
	if (need_semicolon) {
		if (p_word->type == SEMICOLON) {
			move(&p_word);//goto success
		}
		else {
			log_error(STX_SUB_NEED_SEMICOLON, p_word->position);
			(*p_word_cur) = p_word;
			return true;
		}
	}
	//success if reaches here
	STX_DEBUG("detected <statement>");
	(*p_word_cur) = p_word;
	return true;
}

bool stx_compound_stats(Word **p_word_cur) {
	Word *p_word = (*p_word_cur);
	if (p_word->type == CONSTFLAG) {
		stx_const_declare(&p_word);
	}
	if (p_word->type == INTFLAG || p_word->type == CHARFLAG) {
		stx_var_declare(&p_word);
	}
	stx_stats(&p_word);
	STX_DEBUG("detected <compound stats>");
	(*p_word_cur) = p_word;
	return true;
}

bool stx_define_func_ret(Word **p_word_cur) {
	Word *p_word = (*p_word_cur);
	dx = 0;//mid: init dx
	int func_tab_id = 0, param_num = 0;//mid: tabid, for filling len(data size) and param num later
	if (stx_define_func_ret_head(&p_word,&func_tab_id)) {

	}
	else {//skip to lpar
		while (p_word->type != LPAREN) move(&p_word);
	}
	if (p_word->type == LPAREN) {
		move(&p_word);
		if (stx_param_list(&p_word,&param_num)) {
			tab[func_tab_id].param_num = param_num;//mid: fill back param num
		}
		else {//skip to rpar
			while (p_word->type != RPAREN) move(&p_word);
		}
	}
	else {
		log_error(STX_SUB_NEED_LPAREN, p_word->position);
	}
	if (p_word->type == RPAREN) {
		move(&p_word);
	}
	else {//need rpar
		log_error(STX_SUB_NEED_RPAREN, p_word->position);
	}
	if (p_word->type == LBRACE) {
		move(&p_word);
	}
	else {//need lbrace
		log_error(STX_SUB_NEED_LBRACE, p_word->position);
	}
	//GENBEGIN
	int funcbeg_lab_id = get_new_funcbegin_label(func_tab_id);
	mid_mark_next(funcbeg_lab_id);
	mid_insert(OPFUNCBEG, 0, 0, func_tab_id, false, false);
	//GENEND
	stx_compound_stats(&p_word);
	//GENBEGIN
	mid_insert(OPFUNCEND, 0, 0, func_tab_id, false, false);
	//GENEND
	tab[func_tab_id].len = dx;//mid: fill back len(data size)
	if (p_word->type == RBRACE) {
		move(&p_word);
	}
	else {//need rbrace
		log_error(STX_SUB_NEED_RBRACE, p_word->position);
	}
	STX_DEBUG("detected <define func ret>");
	(*p_word_cur) = p_word;
	got_first_obj_in_func = false;//mid: exiting function
	return true;
}

bool stx_define_func_void(Word **p_word_cur) {
	Word *p_word = (*p_word_cur);
	dx = 0;//mid: init dx
	int func_tab_id = 0, param_num = 0;//mid: tabid, for filling len(data size) and param num later
	if (p_word->type == VOIDFLAG) {
		move(&p_word);
		if (p_word->type == IDENTIFIER) {
			func_tab_id = tab_insert_func(p_word, TVOID, 0, 0);//mid: insert func to tab
			move(&p_word);
		}
		else {//need idf
			log_error(STX_SUB_NEED_IDF, p_word->position);
		}
		if (p_word->type == LPAREN) {
			move(&p_word);
			if (stx_param_list(&p_word,&param_num)) {
				tab[func_tab_id].param_num = param_num;//mid: fill back param num
			}
			else {//skip to rpar
				while (p_word->type != RPAREN) move(&p_word);
			}
		}
		else {
			log_error(STX_SUB_NEED_LPAREN, p_word->position);
		}
		if (p_word->type == RPAREN) {
			move(&p_word);
		}
		else {//need rpar
			log_error(STX_SUB_NEED_RPAREN, p_word->position);
		}
		if (p_word->type == LBRACE) {
			move(&p_word);
		}
		else {//need lbrace
			log_error(STX_SUB_NEED_LBRACE, p_word->position);
		}
		//GENBEGIN
		int funcbeg_lab_id = get_new_funcbegin_label(func_tab_id);
		mid_mark_next(funcbeg_lab_id);
		mid_insert(OPFUNCBEG, 0, 0, func_tab_id, false, false);
		//GENEND
		stx_compound_stats(&p_word);
		//GENBEGIN
		mid_insert(OPFUNCEND, 0, 0, func_tab_id, false, false);
		//GENEND
		tab[func_tab_id].len = dx;//mid: fill back len(data size)
		if (p_word->type == RBRACE) {
			move(&p_word);
		}
		else {//need rbrace
			log_error(STX_SUB_NEED_RBRACE, p_word->position);
		}
		STX_DEBUG("detected <define func void>");
		(*p_word_cur) = p_word;
		got_first_obj_in_func = false;//mid: exiting function
		return true;
	}
	else {
		return false;//shouldn't reach here
	}
}

bool stx_main_func(Word **p_word_cur) {
	Word *p_word = (*p_word_cur);
	dx = 0;//mid: init dx
	if (p_word->type == VOIDFLAG) {
		move(&p_word);
		if (p_word->type == MAINFLAG) {
			move(&p_word);
			if (p_word->type == LPAREN) {
				move(&p_word);
			}
			else {
				log_error(STX_SUB_NEED_LPAREN, p_word->position);
			}
			if (p_word->type == RPAREN) {
				move(&p_word);
			}
			else {
				log_error(STX_SUB_NEED_RPAREN, p_word->position);
			}
			if (p_word->type == LBRACE) {
				move(&p_word);
			}
			else {//need lbrace
				log_error(STX_SUB_NEED_LBRACE, p_word->position);
			}
			//GENBEGIN
			int funcbeg_lab_id = get_new_funcbegin_label(-1);
			main_entry_label_id = funcbeg_lab_id;
			mid_mark_next(funcbeg_lab_id);
			mid_insert(OPFUNCBEG, 0, 0, -1, false, false);
			//GENEND
			stx_compound_stats(&p_word);
			//GENBEGIN
			main_size = dx;
			mid_insert(OPFUNCEND, 0, 0, -1, false, false);
			//GENEND
			if (p_word->type == RBRACE) {
				move(&p_word);
			}
			else {//need rbrace
				log_error(STX_SUB_NEED_RBRACE, p_word->position);
			}
			STX_DEBUG("detected <main func>");
			(*p_word_cur) = p_word;
			return true;		
		}
		else {
			return false;//shouldn't get here
		}
	}
	else {
		return false;//shouldn't get here
	}
}

bool stx_program(Word **p_word_cur) {
	Word *p_word = (*p_word_cur);
	if (p_word->type == CONSTFLAG) {
		stx_const_declare(&p_word);
	}
	if (p_word->type == INTFLAG || p_word->type == CHARFLAG) {
		move(&p_word);
		move(&p_word);
		if (p_word->type != LPAREN) {
			p_word -= 2;
			stx_var_declare(&p_word);
		}
		else {
			p_word -= 2;
		}
	}
	global_size = dx;
	is_in_func = true;//mid: all global stuff are done here
	while (true) {
		if (p_word->type == INTFLAG || p_word->type == CHARFLAG) {
			Word *p_func_begin = p_word;
			need_ret_with_exp = true;
			stx_define_func_ret(&p_word);
			if (need_ret_with_exp) {
				if (!got_ret_with_exp) {
					log_error(STX_SUB_NEED_EXP_RET, p_func_begin->position);
				}
			}
			got_ret_with_exp = false;
			need_ret_with_exp = false;
			continue;
		}
		else if (p_word->type == VOIDFLAG) {
			move(&p_word);
			if (p_word->type != MAINFLAG) {
				p_word--;
				stx_define_func_void(&p_word);
				continue;
			}
			else {
				p_word--;
				break;
			}
		}
		else {// not function head, error
			break;//temp
		}
	}
	stx_main_func(&p_word);
	STX_DEBUG("detected <program>");
	(*p_word_cur) = p_word;
	if (p_word - words != words_len) {
		cout << "redundant content after main, ignored;" << endl;
	}
	return true;
}
//module end:syntax analysis
//module: mips
struct TmpMapItem {
	int tmp_id;
	int dx;
	TmpMapItem() {
		tmp_id = 0;
		dx = 0;
	}
};
TmpMapItem tmp_map[MAX_TMP_NUM];
int tmp_map_len = 0;
void reset_tmp_map() {
	for (int i = 0; i < tmp_map_len; i++) {
		tmp_map[i].dx = 0;
		tmp_map[i].tmp_id = 0;
	}
	tmp_map_len = 0;
}
void count_tmp(int tmp_id, int func_size) {
	for (int i = 0; i < tmp_map_len; i++) {
		if (tmp_map[i].tmp_id == tmp_id) {
			return;//already counted
		}
	}
	//never counted this tmp
	tmp_map[tmp_map_len].tmp_id = tmp_id;
	tmp_map[tmp_map_len].dx = func_size + tmp_map_len;
	tmp_map_len++;
}
void count_tmp_in_mid(int mid_id, int func_size) {
	MidItem *p_mid = mids + mid_id;
	switch (p_mid->type) {
	case OPADD:
	case OPSUB:
	case OPMUL:
	case OPDIV:
	case OPGT:
	case OPGE:
	case OPLS:
	case OPLE:
	case OPEQ:
	case OPNE:
	case OPGETRET:
		if (p_mid->target < 0) {
			count_tmp(p_mid->target,func_size);
		}
		break;
	case OPGET:
		if (!(p_mid->op1_is_const)) {
			if (p_mid->op1 < 0) {
				count_tmp(p_mid->op1, func_size);
			}
		}
		break;
	}
}
int count_tmp_in_func(int begin_mid_id) {
	//begin mid should be of OPFUNCBEG; 
	//count till an OPFUNCEND
	//return the number of tmps in this func
	reset_tmp_map();
	int func_tab_id = mids[begin_mid_id].target;
	int func_size = (func_tab_id == -1) ? (main_size) : (tab[func_tab_id].len);
	int i = begin_mid_id;
	while (mids[i].type != OPFUNCEND) {
		count_tmp_in_mid(i,func_size);
		i++;
	}
	return tmp_map_len;
}
void count_tmp_in_program() {
	int i = 0;
	while (i < mids_len) {
		if (mids[i].type == OPFUNCBEG) {
			if (mids[i].target == -1) {//main
				main_tmp_size = count_tmp_in_func(i);
			}
			else {//not main
				tab[mids[i].target].offset = count_tmp_in_func(i);
			}
		}
		i++;
	}
}
int tmp_get_offset(int tmp_id) {
	for (int i = 0; i < tmp_map_len; i++) {
		if (tmp_map[i].tmp_id == tmp_id) {
			return tmp_map[i].dx;
		}
	}
	return -1;
}
int gp_offset = 256;//256: there'are 128 chrs, and each chr is followed by a '\0', so all chrs need 256 Bytes of space
void get_gp_offset() {
	for (int i = 1; i < const_str_len; i++) {//const str id starts from 1
		gp_offset += (strlen(const_strs[i]) + 1);
	}
	gp_offset = gp_offset / 4 + 1;
	gp_offset = gp_offset * 4;//we hope to start our stack at a brand-new-clean 32-bit word
	gp_offset += DATA_BASE;//(base of .data) + (all chrs and const strs)
}
#define MIPS_DEBUG(out,content) if(true){fprintf(out,"%s",content);}

//mips_instructions
#define i_add	1
#define i_addi	2
#define i_sub	3
#define i_mult	4
#define i_div	5
#define i_mflo	6
#define i_lw	7
#define i_sw	8
#define i_syscall	9
#define i_j	10
#define i_beq	11
#define i_slt	12
#define i_or	13
#define i_xori	14
#define i_jal   15
#define i_jr    16

//mips_regs
#define r_zero	0
#define r_at	1
#define r_v0	2
#define r_v1	3
#define r_a0	4
#define r_a1	5
#define r_a2	6
#define r_a3	7
#define r_t0	8
#define r_t1	9
#define r_t2	10
#define r_t3	11
#define r_t4	12
#define r_t5	13
#define r_t6	14
#define r_t7	15
#define r_s0	16
#define r_s1	17
#define r_s2	18
#define r_s3	19
#define r_s4	20
#define r_s5	21
#define r_s6	22
#define r_s7	23
#define r_t8	24
#define r_t9	25
#define r_k0	26
#define r_k1	27
#define r_gp	28
#define r_sp	29
#define r_fp	30
#define r_ra	31
void mips_print_op(FILE *out, int inst) {
	switch (inst) {
	case i_add:	fprintf(out, "%10s", "add     "); break;
	case i_addi:	fprintf(out, "%10s", "addi    "); break;
	case i_sub:	fprintf(out, "%10s", "sub     "); break;
	case i_mult:	fprintf(out, "%10s", "mult    "); break;
	case i_div:	fprintf(out, "%10s", "div     "); break;
	case i_mflo:	fprintf(out, "%10s", "mflo    "); break;
	case i_lw:	fprintf(out, "%10s", "lw      "); break;
	case i_sw:	fprintf(out, "%10s", "sw      "); break;
	case i_syscall:   fprintf(out, "%10s", "syscall "); break;
	case i_j:	fprintf(out, "%10s", "j       "); break;
	case i_beq:	fprintf(out, "%10s", "beq     "); break;
	case i_slt:	fprintf(out, "%10s", "slt     "); break;
	case i_or:	fprintf(out, "%10s", "or      "); break;
	case i_xori:	fprintf(out, "%10s", "xori    "); break;
	case i_jal:	fprintf(out, "%10s", "jal     "); break;
	case i_jr:	fprintf(out, "%10s", "jr      "); break;
	}
}
void mips_print_reg(FILE *out, int reg_id) {
	switch (reg_id) {
	case r_zero:	    fprintf(out, "%6s", "$zero"); break;
	case r_at:		fprintf(out, "%6s", "$at  "); break;
	case r_v0:		fprintf(out, "%6s", "$v0  "); break;
	case r_v1:		fprintf(out, "%6s", "$v1  "); break;
	case r_a0:		fprintf(out, "%6s", "$a0  "); break;
	case r_a1:		fprintf(out, "%6s", "$a1  "); break;
	case r_a2:		fprintf(out, "%6s", "$a2  "); break;
	case r_a3:		fprintf(out, "%6s", "$a3  "); break;
	case r_t0:		fprintf(out, "%6s", "$t0  "); break;
	case r_t1:		fprintf(out, "%6s", "$t1  "); break;
	case r_t2:		fprintf(out, "%6s", "$t2  "); break;
	case r_t3:		fprintf(out, "%6s", "$t3  "); break;
	case r_t4:		fprintf(out, "%6s", "$t4  "); break;
	case r_t5:		fprintf(out, "%6s", "$t5  "); break;
	case r_t6:		fprintf(out, "%6s", "$t6  "); break;
	case r_t7:		fprintf(out, "%6s", "$t7  "); break;
	case r_s0:		fprintf(out, "%6s", "$s0  "); break;
	case r_s1:		fprintf(out, "%6s", "$s1  "); break;
	case r_s2:		fprintf(out, "%6s", "$s2  "); break;
	case r_s3:		fprintf(out, "%6s", "$s3  "); break;
	case r_s4:		fprintf(out, "%6s", "$s4  "); break;
	case r_s5:		fprintf(out, "%6s", "$s5  "); break;
	case r_s6:		fprintf(out, "%6s", "$s6  "); break;
	case r_s7:		fprintf(out, "%6s", "$s7  "); break;
	case r_t8:		fprintf(out, "%6s", "$t8  "); break;
	case r_t9:		fprintf(out, "%6s", "$t9  "); break;
	case r_k0:		fprintf(out, "%6s", "$k0  "); break;
	case r_k1:		fprintf(out, "%6s", "$k1  "); break;
	case r_gp:		fprintf(out, "%6s", "$gp  "); break;
	case r_sp:		fprintf(out, "%6s", "$sp  "); break;
	case r_fp:		fprintf(out, "%6s", "$fp  "); break;
	case r_ra:		fprintf(out, "%6s", "$ra  "); break;
	}
}
void mips_print_bar(FILE *out, bool with_comma) {
	if (with_comma) {
		fprintf(out, ",  ");
	}else{
		fprintf(out, "   ");
	}
}
void mips_print_label(FILE *out, int label_id) {
	fprintf(out, "%s:\n", labels[label_id].name);
}
void mips_gen_usual_ins(FILE *out, int operation, int reg0, int reg1, int reg2) {//pass -1 if not using that reg
	mips_print_op(out, operation);
	mips_print_bar(out, false);
	if (reg0 != -1) {
		mips_print_reg(out,reg0);
		mips_print_bar(out, true);
	}
	if (reg1 != -1) {
		mips_print_reg(out, reg1);
		mips_print_bar(out, true);
	}
	if (reg2 != -1) {
		mips_print_reg(out, reg2);
	}
	fprintf(out, "\n");
}
void mips_gen_instant_ins(FILE *out, int operation, int reg0, int reg1, int instant) {
	mips_print_op(out, operation);
	mips_print_bar(out, false);
	if (reg0 != -1) {
		mips_print_reg(out, reg0);
		mips_print_bar(out, true);
	}
	if (reg1 != -1) {
		mips_print_reg(out, reg1);
		mips_print_bar(out, true);
	}
	fprintf(out, "%d", instant);
	fprintf(out, "\n");
}
void mips_gen_ls_ins(FILE *out, int operation, int reg0, int offset, int reg1) {
	mips_print_op(out, operation);
	mips_print_bar(out, false);
	if (reg0 != -1) {
		mips_print_reg(out, reg0);
		mips_print_bar(out, true);
	}
	fprintf(out, "%d(", offset);
	if (reg1 != -1) {
		mips_print_reg(out, reg1);
	}
	fprintf(out, ")\n");
}
void mips_gen_j_ins(FILE *out, int operation, int reg0, int reg1, int label_id) {
	mips_print_op(out, operation);
	mips_print_bar(out, false);
	if (reg0 != -1) {
		mips_print_reg(out, reg0);
		mips_print_bar(out, true);
	}
	if (reg1 != -1) {
		mips_print_reg(out, reg1);
		mips_print_bar(out, true);
	}
	fprintf(out,"%s\n", labels[label_id].name);
}

#define add(r0,r1,r2) mips_gen_usual_ins(out, i_add, r_##r0,r_##r1,r_##r2)
#define addi(r0,r1,inst) mips_gen_instant_ins(out, i_addi,r_##r0,r_##r1,inst)
#define sub(r0,r1,r2) mips_gen_usual_ins(out, i_sub, r_##r0,r_##r1,r_##r2)
#define mult(r0,r1) mips_gen_usual_ins(out, i_mult,-1,r_##r0,r_##r1)
#define div(r0,r1) mips_gen_usual_ins(out, i_div,-1,r_##r0,r_##r1)
#define mflo(r0) mips_gen_usual_ins(out,i_mflo,-1,-1,r_##r0)
#define lw(r0,offset,r1) mips_gen_ls_ins(out,i_lw,r_##r0,offset,r_##r1)
#define sw(r0,offset,r1) mips_gen_ls_ins(out,i_sw,r_##r0,offset,r_##r1)
#define syscall mips_gen_usual_ins(out,i_syscall,-1,-1,-1)
#define j(label_id) mips_gen_j_ins(out,i_j,-1,-1,label_id)
#define beq(r0,r1,label_id) mips_gen_j_ins(out,i_beq,r_##r0,r_##r1,label_id)
#define slt(r0,r1,r2) mips_gen_usual_ins(out,i_slt,r_##r0,r_##r1,r_##r2)
#define or(r0,r1,r2) mips_gen_usual_ins(out,i_or,r_##r0,r_##r1,r_##r2)
#define xori(r0,r1,inst) mips_gen_instant_ins(out, i_xori,r_##r0,r_##r1,inst)
#define jal(label_id) mips_gen_j_ins(out,i_jal,-1,-1,label_id)
#define jr(r0) mips_gen_usual_ins(out,i_jr,-1,-1,r_##r0)
void mips_gen_header(FILE *out) {
	//generate const strs and chars
	fprintf(out, ".data\n");
	/*
	MIPS_DEBUG(out, "#chars display\n");//chars
	for (int i = 0; i < 128; i++) {
		char c = (char)i;
		if (is_chr_char(c)) {
			fprintf(out, "ch%d: .asciiz \"%c\"\n", i, c);
		}
		else {
			fprintf(out, "ch%d: .asciiz \" \"\n", i);
		}
	}
	*/
	MIPS_DEBUG(out, "#const strs\n");//const strs
	for (int i = 1; i < const_str_len; i++) {
		fprintf(out, "str%d: .asciiz \"%s\"\n", i, const_strs[i]);
	}
}
void mips_gen_global_main_init(FILE *out) {
	fprintf(out, ".text\n");
	addi(gp, zero, gp_offset);//init gp to gp_base
	addi(fp, gp, global_size * 4);//alloc global var space
	addi(sp, fp, (main_size + main_tmp_size) * 4);//alloc main var+tmp size
	j(main_entry_label_id);//jump to main entry
}

void mips_get_op_into_reg(FILE *out, int mid_id, int op_id,int reg) {
	MidItem *mid = mids + mid_id;
	if (op_id == 1) {//op1
		if (mid->op1_is_const) {
			mips_gen_instant_ins(out, i_addi, reg, r_zero, mid->op1);
		}
		else {
			if (mid->op1 >= 0) {//in tab
				if (tab[mid->op1].obj == TCONST) {
					mips_gen_instant_ins(out, i_addi, reg, r_zero, tab[mid->op1].const_val);
				}
				else {
					if (tab[mid->op1].is_global) {
						mips_gen_ls_ins(out, i_lw, reg, 4 * (tab[mid->op1].offset), r_gp);
					}
					else {
						mips_gen_ls_ins(out, i_lw, reg, 4 * (tab[mid->op1].offset), r_fp);
					}
				}
			}
			else {//tmp
				mips_gen_ls_ins(out, i_lw, reg, 4 * (tmp_get_offset(mid->op1)), r_fp);
			}
		}		
	}
	else {//op2
		if (mid->op2_is_const) {
			mips_gen_instant_ins(out, i_addi, reg, r_zero, mid->op2);
		}
		else {
			if (mid->op2 >= 0) {//in tab
				if (tab[mid->op2].obj == TCONST) {
					mips_gen_instant_ins(out, i_addi, reg, r_zero, tab[mid->op2].const_val);
				}
				else {
					if (tab[mid->op2].is_global) {
						mips_gen_ls_ins(out, i_lw, reg, 4 * (tab[mid->op2].offset), r_gp);
					}
					else {
						mips_gen_ls_ins(out, i_lw, reg, 4 * (tab[mid->op2].offset), r_fp);
					}
				}
			}
			else {//tmp
				mips_gen_ls_ins(out, i_lw, reg, 4 * (tmp_get_offset(mid->op2)), r_fp);
			}
		}
	}
}
void mips_save_result_to_mem(FILE *out, int mid_id,int result_reg) {
	MidItem *mid = mids + mid_id;
	if (mid->target >= 0) {//in tab
		if (tab[mid->target].is_global) {
			mips_gen_ls_ins(out, i_sw, result_reg, 4 * (tab[mid->target].offset), r_gp);
		}
		else {
			mips_gen_ls_ins(out, i_sw, result_reg, 4 * (tab[mid->target].offset), r_fp);
		}
	}
	else {//is tmp
		mips_gen_ls_ins(out, i_sw, result_reg, 4 * (tmp_get_offset(mid->target)), r_fp);
	}
}
void mips_translate_mid(FILE *out) {
	get_gp_offset();//because there's const strs in memory, gp does not start from .DATA_BASE; so we need to calculate it.
	//count how many tmps are there in each function. 
	//for non-main funcs, result is saved at tab.offset;
	//for main func, result is saved at main_tmp_size.
	count_tmp_in_program();
	mips_gen_header(out);//chars and const strs
	//init gp fp sp, allocate global var and main function space, 
	//and generate a J instruction towards main entry.
	mips_gen_global_main_init(out);
	int cal_func_id = 0;
	int cur_param_id = 0;
	bool is_in_main = false;//some solutions are different for main function(such as return), so need a marker.
	for (int i = 0; i < mids_len; i++) {
		MidItem *mid = mids + i;
		if (true) {//print mid code as comment
			fprintf(out, "#");
			mid_fprint_single(out,i);
		}
		if (mid->label_id != -1) {//gen label
			mips_print_label(out, mid->label_id);
		}
		switch (mid->type) {
		case OPFUNCBEG:
			count_tmp_in_func(i);//get tmp mem map
			if (mid->target != -1) {//no return addr for main function
				//save ret addr
				//at this phase, fp is pointing to the base of new stack frame.
				sw(ra, 4 * (-2), fp);
			}
			else {
				is_in_main = true;
			}
			break;
		case OPFUNCEND:
			if (mid->target != -1) {
				add(sp, zero, fp); //sp=fp
				sw(zero, 4 * (-1), sp);//force return 0
				lw(fp, 4 * (-3), sp);//restore fp
				lw(ra, 4 * (-2), sp);//get return addr
				addi(sp, sp, 4 * (-3));//restore sp
				jr(ra);//return
			}			
			break;
		case OPPRINTF:
			if (mid->op1_is_const) {//target is const str
				addi(v0, zero, 4);//select print str service
				fprintf(out, "        la   $a0,  str%d\n", mid->target);
				syscall;
			}
			else {//target is in tab or is tmp
				//select service
				if (mid->op2_is_const) {//force print char
					addi(v0, zero, 11);//select print char service
				}
				else {//print int
					addi(v0, zero, 1);//select print int service
				}
				//get stuff into reg
				if (mid->target >= 0) {//target is in tab
					int tab_id = mid->target;
					if (tab[tab_id].obj == TCONST) {//const
						addi(a0, zero, tab[tab_id].const_val);
					}
					else {//var
						if (tab[tab_id].is_global) {//global var, gp offset
							lw(a0, 4 * (tab[tab_id].offset), gp);
						}
						else {//local var, fp offset
							lw(a0, 4 * (tab[tab_id].offset), fp);
						}
					}
				}
				else {//target is tmp
					lw(a0, 4 * (tmp_get_offset(mid->target)), fp);
				}
				syscall;
			}
			break;
		case OPSCANF: 
		{
			//select service
			int tab_id = mid->target;
			if (tab[tab_id].type == TINT) {
				addi(v0, zero, 5);//select read int service
			}
			else {
				addi(v0, zero, 12);//select read char service
			}
			syscall;
			if (tab[tab_id].is_global) {//global, gp
				sw(v0, 4 * (tab[tab_id].offset), gp);
			}
			else {//local , fp
				sw(v0, 4 * (tab[tab_id].offset), fp);
			}
		}
			break;
		case OPJ:
			j(mid->target);
			break;
		case OPCAL:
			cal_func_id = mid->target;
			cur_param_id = 0;
			addi(sp, sp, 4 * 3);
			break;
		case OPPAS:
			cur_param_id++;
			if (mid->target >= 0) {//param in tab
				if (tab[mid->target].obj == TCONST) {//const
					addi(t0, zero, tab[mid->target].const_val);
				}
				else {//var
					if (tab[mid->target].is_global) {
						lw(t0, 4 * (tab[mid->target].offset), gp);
					}
					else {
						lw(t0, 4 * (tab[mid->target].offset), fp);
					}
				}
				//at current phase, sp points to the base of new stack frame.
				sw(t0, 4 * (tab[cal_func_id + cur_param_id].offset), sp);
			}
			else {//param is tmp
				lw(t0, 4 * (tmp_get_offset(mid->target)), fp);
				//at current phase, sp points to the base of new stack frame.
				sw(t0, 4 * (tab[cal_func_id + cur_param_id].offset), sp);
			}
			break;
		case OPENDPAS:
			sw(fp, 4 * (-3), sp);//save prev fp
			add(fp, zero, sp);//fp in place
			addi(sp, sp, 4 * (tab[cal_func_id].len + tab[cal_func_id].offset));//alloc func space(var+tmp)
			fprintf(out, "   jal     f_%s\n", tab[cal_func_id].name);//jump to function
			break;
		case OPRET:
			if (!is_in_main) {//solution for ordinary funcs
				mips_get_op_into_reg(out, i, 1, r_k0);
				//now retv is in k0
				add(sp, zero, fp);//sp=fp;
				sw(k0, 4 * (-1), sp);//save return val
				lw(fp, 4 * (-3), sp);//restore fp
				lw(ra, 4 * (-2), sp);//get return addr
				addi(sp, sp, 4 * (-3));//restore sp;
				jr(ra);//return
			}
			else {//if meets return in main func, exit immediately
				addi(v0, zero, 10);//set exit service
				syscall;//exit immediately
			}			 
			break;
		case OPGETRET:
			lw(k0, 4 * 2, sp);//get retv into k0
			mips_save_result_to_mem(out, i, r_k0);
			break;
		case OPGET:
			mips_get_op_into_reg(out, i, 2, r_t0);//t0 is index value
			add(t0, t0, t0);
			add(t0, t0, t0);//index should *4 
			if (tab[mid->target].is_global) {//global array
				addi(t1, gp, 4*(tab[mid->target].offset));
			}
			else {//local array
				addi(t1, fp, 4*(tab[mid->target].offset));
			}
			add(t1, t1, t0);//t1 is absolute addr of target ary element
			lw(t1, 0, t1);//t1 is target elem value;
			if (mid->op1 >= 0) {//in tab
				if (tab[mid->op1].is_global) {
					sw(t1, 4 * (tab[mid->op1].offset), gp);
				}
				else {
					sw(t1, 4 * (tab[mid->op1].offset), fp);
				}
			}
			else {//is tmp
				sw(t1, 4 * (tmp_get_offset(mid->op1)), fp);
			}
			break;
		case OPSET:
			mips_get_op_into_reg(out, i, 2, r_t0);//t0 is index value
			add(t0, t0, t0);
			add(t0, t0, t0);//index should *4 
			if (tab[mid->target].is_global) {//global array
				addi(t1, gp, 4 * (tab[mid->target].offset));
			}
			else {//local array
				addi(t1, fp, 4 * (tab[mid->target].offset));
			}
			add(t1, t1, t0);//t1 is absolute addr of target ary element
			mips_get_op_into_reg(out, i, 1, r_t2);//the value for assign is in t2
			sw(t2, 0, t1);
			break;
		case OPBEQ:
			mips_get_op_into_reg(out, i, 1, r_t0);
			mips_get_op_into_reg(out, i, 2, r_t1);
			beq(t0, t1, mid->target);
			break;
		case OPADD:
			mips_get_op_into_reg(out, i, 1, r_t0);
			mips_get_op_into_reg(out, i, 2, r_t1);
			add(t2, t0, t1);
			mips_save_result_to_mem(out, i, r_t2);
			break;
		case OPSUB:
			mips_get_op_into_reg(out, i, 1, r_t0);
			mips_get_op_into_reg(out, i, 2, r_t1);
			sub(t2, t0, t1);
			mips_save_result_to_mem(out, i, r_t2);
			break;
		case OPMUL:
			mips_get_op_into_reg(out, i, 1, r_t0);
			mips_get_op_into_reg(out, i, 2, r_t1);
			mult(t0, t1);
			mflo(t2);
			mips_save_result_to_mem(out, i, r_t2);
			break;
		case OPDIV:
			mips_get_op_into_reg(out, i, 1, r_t0);
			mips_get_op_into_reg(out, i, 2, r_t1);
			div(t0, t1);
			mflo(t2);
			mips_save_result_to_mem(out, i, r_t2);
			break;
		case OPGT:
			mips_get_op_into_reg(out, i, 1, r_t0);
			mips_get_op_into_reg(out, i, 2, r_t1);
			slt(t2, t1, t0);
			mips_save_result_to_mem(out, i, r_t2);
			break;
		case OPGE:
			mips_get_op_into_reg(out, i, 1, r_t0);
			mips_get_op_into_reg(out, i, 2, r_t1);
			slt(t2, t0, t1);
			xori(t2, t2, 1);
			mips_save_result_to_mem(out, i, r_t2);
			break;
		case OPLS:
			mips_get_op_into_reg(out, i, 1, r_t0);
			mips_get_op_into_reg(out, i, 2, r_t1);
			slt(t2, t0, t1);
			mips_save_result_to_mem(out, i, r_t2);
			break;
		case OPLE:
			mips_get_op_into_reg(out, i, 1, r_t0);
			mips_get_op_into_reg(out, i, 2, r_t1);
			slt(t2, t1, t0);
			xori(t2, t2, 1);
			mips_save_result_to_mem(out, i, r_t2);
			break;
		case OPEQ:
			mips_get_op_into_reg(out, i, 1, r_t0);
			mips_get_op_into_reg(out, i, 2, r_t1);
			slt(t2, t0, t1);
			slt(t0, t1, t0);
			or (t2, t2, t0);
			xori(t2, t2, 1);
			mips_save_result_to_mem(out, i, r_t2);
			break;
		case OPNE:
			mips_get_op_into_reg(out, i, 1, r_t0);
			mips_get_op_into_reg(out, i, 2, r_t1);
			slt(t2, t0, t1);
			slt(t0, t1, t0);
			or (t2, t2, t0);
			mips_save_result_to_mem(out, i, r_t2);
			break;
		}
	}
}
//module end: mips
//module: opt
#define MAX_BLK_NUM 3000;
MidItem mid_draft[MAX_MID_LEN];

class BlkItem {
public: 
	MidItem *p_first_mid;
	MidItem *p_last_mid;
	vector<BlkItem*> *prev_blks;
	vector<BlkItem*> *next_blks;
	bool is_entry;
	bool is_end;
	int id;
	BlkItem(MidItem *p_first_mid, MidItem *p_last_mid, int id) {
		this->p_first_mid = p_first_mid;
		this->p_last_mid = p_last_mid;
		prev_blks = new vector<BlkItem*>();
		next_blks = new vector<BlkItem*>();
		this->is_entry = false;
		this->is_end = false;
		this->id = id;
	}
	BlkItem(bool is_entry,bool is_end) {
		p_first_mid = NULL;
		p_last_mid = NULL;
		this->is_entry = is_entry;
		this->is_end = is_end;
		prev_blks = new vector<BlkItem*>();
		next_blks = new vector<BlkItem*>();
		id = -1;
	}
	~BlkItem() {
		delete prev_blks;
		delete next_blks;
	}

	int mids_num() {
		return (p_last_mid - p_first_mid) + 1;
	}

	void print_name() {
		if (is_entry) {
			printf("B_entry");
		}
		else if (is_end) {
			printf("B_exit");
		}
		else {
			printf("B_%d", id);
		}
	}

	void print_links() {
		printf("prevs: ");
		for (int i = 0; i < prev_blks->size(); i++) {
			(*prev_blks)[i]->print_name();
			printf(" ");
		}
		printf("\nnexts: ");
		for (int i = 0; i < next_blks->size(); i++) {
			(*next_blks)[i]->print_name();
			printf(" ");
		}
		printf("\n");
	}

	void print_body() {
		if (is_entry || is_end) {
			return;
		}
		for (MidItem *p = p_first_mid; p <= p_last_mid; p++) {
			mid_fprint_single(stdout, p - mids);
		}
	}

	void print() {
		print_name();printf(":\n");
		print_links();
		print_body();
		printf("\n");
	}
};

class FlowGraph {
public:
	MidItem *p_first_mid;
	MidItem *p_last_mid;
	vector<BlkItem*> *blks;
	BlkItem *entry_blk;
	BlkItem *end_blk;
	int id;
private:
	bool blks_deleted;
public:
	FlowGraph(MidItem *p_first_mid, MidItem *p_last_mid, int id) {
		this->p_first_mid = p_first_mid;
		this->p_last_mid = p_last_mid;
		this->id = id;
		split_blks();
		weave();
	}

	void split_blks() {
		//clear old blks
		delete_blks();
		blks = new vector<BlkItem*>();
		blks_deleted = false;
		//begin split
		bool *is_blk_entry = (bool*)malloc(sizeof(bool)*mids_num());
		bool *is_blk_end   = (bool*)malloc(sizeof(bool)*mids_num());
		for (int i = 0; i < mids_num(); i++) {
			is_blk_entry[i] = false;
			is_blk_end[i] = false;
		}
		//find blk begin mids
		for (MidItem *p = p_first_mid; p <= p_last_mid; p++) {
			switch (p->type)
			{
			case OPFUNCBEG:
				is_blk_entry[p - p_first_mid] = true; break;
			case OPBEQ:
			case OPJ:
			case OPRET:
				is_blk_entry[p - p_first_mid + 1] = true; break;
			}
			if (p->label_id != -1) {
				is_blk_entry[p - p_first_mid] = true;
			}
		}
		//find blk end mids
		for (int i = 0; i < mids_num(); i++) {
			if(is_blk_entry[i]){
				if (i-1>=0) {
					is_blk_end[i-1] = true;
				}
			}
		}
		is_blk_end[mids_num() - 1] = true;
		//create common blk
		MidItem *cur_blk_first;
		MidItem *cur_blk_last;
		int blk_id = 0;
		for (int i = 0; i < mids_num(); i++) {
			if (is_blk_entry[i]) {
				cur_blk_first = p_first_mid + i;
			}
			if (is_blk_end[i]) {
				cur_blk_last = p_first_mid + i;
				BlkItem *tmp = new BlkItem(cur_blk_first, cur_blk_last,blk_id);
				blk_id++;
				blks->push_back(tmp);
			}
		}
		//create entry and end blk
		entry_blk = new BlkItem(true, false);
		end_blk = new BlkItem(false, true);
		blks->push_back(entry_blk);
		blks->push_back(end_blk);
		//split completed
		free(is_blk_entry);
		free(is_blk_end);		
	}

	void weave() {
		//weave prevs and nexts
		for (int i = 0; i < blks->size(); i++) {
			//for each block:
			//1. if first mid is funcbegin, then it's the only 'next' of entry blk;
			//2. if last mid is j/beq, then it's the 'prev' of the target blk of j/beq;
			//3. if last mid is return/funcend, then it's the 'prev' of end blk;
			BlkItem *tmp = (*blks)[i];
			if (tmp->is_end || tmp->is_entry) {
				break;
			}
			if (tmp->p_first_mid->type == OPFUNCBEG) {
				tmp->prev_blks->push_back(entry_blk);
				entry_blk->next_blks->push_back(tmp);
			}
			int type_last = tmp->p_last_mid->type;
			if (type_last == OPBEQ || type_last == OPJ) {
				BlkItem *target_blk = locate_label(tmp->p_last_mid->target);
				tmp->next_blks->push_back(target_blk);
				target_blk->prev_blks->push_back(tmp);
			}
			else if (type_last == OPRET || type_last == OPFUNCEND) {
				tmp->next_blks->push_back(end_blk);
				end_blk->prev_blks->push_back(tmp);
			}
			else {
				//shouldn't get here
			}
		}
	}

	//the number of mids in this function
	int mids_num() {
		return (p_last_mid - p_first_mid) + 1;
	}
	//find out which block does the label points to
	BlkItem *locate_label(int label_id) {
		int mid_id = labels[label_id].mid_id;//external access: label
		MidItem *target_mid = mids + mid_id;//external access: mids
		for (int i = 0; i < blks->size(); i++) {
			BlkItem *tmp = (*blks)[i];
			if ((tmp->p_first_mid) <= target_mid&&target_mid <= (tmp->p_last_mid)) {
				return tmp;
			}
		}
		return NULL;
	}

	void print() {
		printf("########FlowGraph%3d########\n",id);
		for (int i = 0; i < blks->size(); i++) {
			(*blks)[i]->print();
		}
		printf("########FlowGraph%3d########\n", id);
	}

	~FlowGraph() {
		delete_blks();
	}
	void delete_blks() {
		if (!blks_deleted) {
			for (int i = 0; i < blks->size(); i++) {
				delete (*blks)[i];
			}
			delete blks;
			blks_deleted = true;
		}
	}
};

class MidsView{
public:
	vector<FlowGraph*> *flow_graphs;
	MidsView() {
		flow_graphs = new vector<FlowGraph*>();
		split_flow_graphs();
	}

	void split_flow_graphs() {
		MidItem *cur_funcbegin;
		MidItem *cur_funcend;
		int flow_graph_id = 0;
		for (int i = 0; i < mids_len; i++) {
			MidItem *p = mids + i;
			if (p->type == OPFUNCBEG) {
				cur_funcbegin = p;
			}
			else if(p->type== OPFUNCEND){
				cur_funcend = p;
				FlowGraph *tmp = new FlowGraph(cur_funcbegin, cur_funcend, flow_graph_id);
				flow_graphs->push_back(tmp);
				flow_graph_id++;
			}
		}
	}

	void print() {
		for (int i = 0; i < flow_graphs->size(); i++) {
			(*flow_graphs)[i]->print();
		}
	}

	~MidsView() {
		delete_flow_graphs();
	}
	void delete_flow_graphs() {
		for (int i = 0; i < flow_graphs->size(); i++) {
			delete (*flow_graphs)[i];
		}
		delete flow_graphs;
	}
};

//module end: opt

///* for other temporary test
int main() {
	//read source code from file
	get_src_file_name();
	//strcpy(src_file_name, "D:\\Compiler\\Project\\优化\\test\\optimize_test.txt");
	get_src();
	get_words();
	stx_program(&word_cursor);
	//NORMAL
	//--tab_print();
	//printf("global size: %d, main size: %d\n", global_size, main_size);
	//mid_print();
	//NORMAL
	
	//OPT
	//MidsView *view = new MidsView();
	//view->print();
	//OPT
	if (error_count == 0) {
		FILE *out;
		//out = fopen("D:\\Compiler\\Project\\优化\\mips生成\\test_mips_out.asm", "w");
		out = fopen("test_mips_out.asm", "w");
		mips_translate_mid(out);
		//tab_print();
		fclose(out);
		cout << "done." << endl;
	}
	else {
		cout << "terminated with error(s)." << endl;
		print_errors_and_exit();
	}			
	system("pause");
	return 0;
}
//*/


//vector test
//int main() {
	/*
	vector<int>* v = new vector<int>();
	for (int i = 0; i < 20; i++) {
		//v->push_back(i);
		v->insert(v->begin()+i, i);
		//(*v)[i] = i;//out of bound!!
	}
	//v->erase(v->begin()+3);//v.removeAt(3)
	for (int i = 0; i < v->size(); i++) {
		cout << (*v)[i] << endl;
	}
	*/	
//}