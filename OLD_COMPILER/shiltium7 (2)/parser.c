#include "headers.h"

/*
How many statements should we allocate per language block?
Note that sizeof(Statement*), which probably equals 8, is multiplied by this constant and the product of these numbers
is then given to m_alloc as an argument, so setting this to a high value won't use too much memory.
*/
#define DEFAULT_NUMBER_OF_STATEMENTS_TO_ALLOCATE 5

attr_noreturn static void unable_to_allocate_memory_for_statement(void) {
	output_nullt_string("Unable to allocate memory for statement.\n");
	exit(1);
}

attr_noreturn static void unable_to_allocate_memory_to_parse_block(void) {
	output_nullt_string("Unable to allocate memory to parse block.\n");
	exit(1);
}

attr_nonnull
static ErrorNumber parse_statement(DataAndCode* const restrict dac, Statement* statement, Block* block) {
	#if 0
	u64 i;
	#endif
	
	/* just easier names to refer to */
	Token* tokens;
	u64 number_of_tokens;
	
	assert_comparison(statement, !=, NULL);
	assert_comparison(block, !=, NULL);
	
	number_of_tokens = ARROW(statement, length_of_tokens);
	tokens = statement->tokens;
	
	#if 0
	output_nullt_string("Tokens in statement:\n");
	for (i = 0; i < statement->length_of_tokens; ++i) {
		output_nullt_string("    ");
		output_string(tokens[i].string.string, tokens[i].string.length);
		output_newline();
	}
	#endif
	
	/* This branch handles declaring initialized variables, for example:  u i = 0; */
	if (number_of_tokens > 3 && tokens[2].string.length == 1 && tokens[2].string.string[0] == '=') {
		Variable *variable, *already_declared_variable;
		
		if (
				tokens[0].string.length != 1 || (
				tokens[0].string.string[0] != 'u' &&
				tokens[0].string.string[0] != 's' &&
				tokens[0].string.string[0] != 'f' &&
				tokens[0].string.string[0] != 'b')
		) {
			output_nullt_string("Type error on line ");
			output_u64(tokens[0].line_number);
			output_nullt_string(": invalid type: ‘");
			output_string(tokens[0].string.string, tokens[0].string.length);
			
			output_nullt_string(
				"’\nThe valid types are:\n"
				"    u (unsigned 64-bit integer)\n       Range: 0 to 18,446,744,073,709,551,615\n"
				"    s (signed 64-bit integer)\n       Range: -9,223,372,036,854,775,808 to +9,223,372,036,854,775,807\n"
				"    f (double precision floating-point number)\n        Range: about 10^-308 to about 10^308\n"
				"    b (boolean)\n        Possible values: true or false"
				"\n\n");
			
			return ERROR_NUMBER__TYPE_ERROR__INVALID_TYPE;
		}
		
		if (!is_valid_identifier(tokens[1].string.string, tokens[1].string.length)) {
			output_nullt_string("Identifier error on line ");
			output_u64(tokens[1].line_number);
			output_nullt_string(": invalid variable name: ‘");
			output_string(tokens[1].string.string, tokens[1].string.length);
			output_nullt_string("’\n");
			return ERROR_NUMBER__NAME_ERROR__INVALID_VARIABLE_NAME;
		}
		
		already_declared_variable = (Variable*)(*hash_table_get_value_with_string_key(
			&block->variables_in_scope, tokens[1].string.string, tokens[1].string.length
		));
		
		if (already_declared_variable) {
			output_nullt_string("Identifier error on line ");
			output_u64(tokens[1].line_number);
			output_nullt_string(": Variable ‘");
			output_string(tokens[1].string.string, tokens[1].string.length);
			output_nullt_string("’ already declared on line ");
			output_u64(already_declared_variable->line_declared_on);
			output_nullt_string(".\n");
			return ERROR_NUMBER__NAME_ERROR__VARIABLE_ALREADY_DECLARED;
		}
		
		variable = (Variable*)m_alloc(sizeof(Variable));
		if (unlikely(!variable)) {
			output_nullt_string("Unable to allocate memory for variable.\n");
			exit(1);
		}
		
		variable->line_declared_on = tokens[1].line_number;
		variable->name.string = tokens[1].string.string;
		variable->name.length = tokens[1].string.length;
		variable->evaluated = false;
		
		switch (ARRAY_INDEX(ARRAY_INDEX(tokens, 0).string.string, 0)) {
		case 'u': {
			ErrorNumber error_number;
			
			expression_parser_parse_expression(variable, statement, &error_number, block, TYPE_UINT);
			if (error_number != ERROR_NUMBER_NO_ERROR) {
				m_free(variable);
				return error_number;
			}
			break;
		}
		case 'f': {
			ErrorNumber error_number;
			
			expression_parser_parse_expression(variable, statement, &error_number, block, TYPE_FLOAT);
			if (error_number != ERROR_NUMBER_NO_ERROR) {
				m_free(variable);
				return error_number;
			}
			break;
		}
		default:
			output_nullt_string("Not implemented yet. Please fix this.\n");
			exit(1);
		}
		
		assert_comparison(tokens[1].string.string, !=, NULL);
		assert_comparison(tokens[1].string.length, !=, 0);
		
		{
			#if DEBUG
				const bool return_value =
			#endif
			hash_table_insert_string(&block->variables_in_scope, tokens[1].string.string, tokens[1].string.length, variable, false);
			
			assert_comparison(return_value, !=, false);
		}
	}
	else if (number_of_tokens == 2 && strnequal(tokens[0].string.string, "output", tokens[0].string.length, 6)) {
		Variable* variable;
		
		variable = (Variable*)(*hash_table_get_value_with_string_key(&block->variables_in_scope, tokens[1].string.string, tokens[1].string.length));
		if (!variable) {
			output_nullt_string("Name error on line ");
			output_u64(tokens[1].line_number);
			output_nullt_string(": ‘");
			output_string(tokens[1].string.string, tokens[1].string.length);
			output_nullt_string("’ undeclared\n");
			exit(1);
		}
		
		if (ARROW(variable, evaluated)) {
			output_nullt_string("Variable has already been evaluated!\n");
			exit(1);
		} else {
			translate_expression_into_mid_level_instructions(dac, block, variable);
			ARROW_ASSIGN(variable, evaluated) = true;
		}
	}
	/* Unknown syntax error */
	else {
		if (tokens[0].line_number == tokens[number_of_tokens - 1].line_number) {
			output_nullt_string("Syntax error on line ");
			output_u64(tokens[0].line_number);
			output_nullt_string(":\n");
			output_string(statement->statement_line, statement->length_of_statement_line_string);
			output_newline();
		} else {
			output_nullt_string("Syntax error from line ");
			output_u64(tokens[0].line_number);
			output_nullt_string(" to ");
			output_u64(tokens[number_of_tokens - 1].line_number);
			output_nullt_string(":\n");
			output_string(statement->statement_line, statement->length_of_statement_line_string);
			output_newline();
		}
		
		return ERROR_NUMBER__SYNTAX_ERROR__UNCLASSIFIED_SYNTAX_ERROR;
	}
	
	return ERROR_NUMBER_NO_ERROR;
}

/*
Parses the statement parameter's statement_line member and sets the statement parameter appropriately.
This function makes very good use of memory; it only uses the amount it needs -- no more, no less.
*/
attr_nonnull static ErrorNumber parse_statement_string_into_statement(Statement* const statement, u64 line_number_at_start_of_statement) {
	u64 i, token_number, line_number = line_number_at_start_of_statement, start_of_token_offset;
	/* This variable is used to test whether or not the parser is in a comment and also to store the starting line for the block comment. */
	u64 block_comment_starting_line;
	
	assert_comparison(statement, !=, NULL);
	assert_comparison(line_number_at_start_of_statement, !=, 0);
	
	block_comment_starting_line = 0;
	statement->length_of_tokens = 0;
	
	for (i = 0; i < statement->length_of_statement_line_string; ++i) {
		if (statement->statement_line[i] == '\n') {
			++line_number;
		}
		
		if (block_comment_starting_line) {
			if (statement->statement_line[i] == '/' && statement->statement_line[i - 1] == '*') {
				block_comment_starting_line = 0;
			}
			
			/* Continue regardless of whether the parser is still in the comment or just got out of it. */
			continue;
		}
		
		if (statement->statement_line[i] == '/' && i + 1 != statement->length_of_statement_line_string && statement->statement_line[i + 1] == '*') {
			block_comment_starting_line = line_number;
		}
		
		if (
			(statement->statement_line[i] == ' ' ||
			statement->statement_line[i] == '\t' ||
			statement->statement_line[i] == '\n' ||
			statement->statement_line[i] == ';') &&
			i && (
			statement->statement_line[i - 1] != ' ' &&
			statement->statement_line[i - 1] != '\t' &&
			statement->statement_line[i - 1] != '\n')
		) {
			if (!(i > 1 && statement->statement_line[i - 2] == '*' && statement->statement_line[i - 1] == '/')) {
				++statement->length_of_tokens;
			}
		}
	}
	
	if (block_comment_starting_line) {
		output_nullt_string("Syntax error: unclosed block comment");
		if (block_comment_starting_line != line_number) {
		 	output_nullt_string(" (it was opened on line ");
			output_u64(block_comment_starting_line);
			output_char(')');
		}
		output_char('\n');
		return ERROR_NUMBER__SYNTAX_ERROR__UNCLOSED_BLOCK_COMMENT_NOT_AT_BEGINNING_OF_LINE;
	}
	
	if (!statement->length_of_tokens) {
		output_nullt_string("Syntax error on line ");
		output_u64(line_number_at_start_of_statement);
		output_nullt_string(": unlike C, C++, Java, C#, D and other C-like languages, in the Shilto Language an empty statement is invalid.\n");
		return ERROR_NUMBER__SYNTAX_ERROR__EMPTY_STATEMENT;
	}
	
	statement->tokens = (Token*)m_alloc(sizeof(Token) * statement->length_of_tokens);
	if (unlikely(!statement->tokens)) {
		output_nullt_string("Unable to allocate memory for token.\n");
		exit(1);
	}
	
	for (i = 0, token_number = 0, start_of_token_offset = 0, block_comment_starting_line = 0, line_number = line_number_at_start_of_statement
		;;
		++i
	) {
		if (statement->statement_line[i] == '\n') {
			++line_number;
		}
		
		if (block_comment_starting_line) {
			if (statement->statement_line[i] == '/' && statement->statement_line[i - 1] == '*') {
				block_comment_starting_line = 0;
			}
			
			++start_of_token_offset;
			
			/* Continue regardless of whether the parser is still in the comment or just got out of it. */
			continue;
		}
		
		if (statement->statement_line[i] == '/' && i + 1 < statement->length_of_statement_line_string && statement->statement_line[i + 1] == '*') {
			block_comment_starting_line = line_number;
			++start_of_token_offset;
			continue;
		}
		
		if (
			statement->statement_line[i] == ' ' ||
			statement->statement_line[i] == '\t' ||
			statement->statement_line[i] == '\n' ||
			statement->statement_line[i] == ';'
		) {
			if (
				i && (
				statement->statement_line[i - 1] == ' ' ||
				statement->statement_line[i - 1] == '\t' ||
				statement->statement_line[i - 1] == '\n')
			) {
				++start_of_token_offset;
			} else if (!(i > 1 && statement->statement_line[i - 2] == '*' && statement->statement_line[i - 1] == '/')) {
				statement->tokens[token_number].string.string = statement->statement_line + start_of_token_offset;
				statement->tokens[token_number].string.length = i - start_of_token_offset;
				statement->tokens[token_number].line_number = line_number;
				
				++token_number;
				if (token_number >= statement->length_of_tokens) break;
				start_of_token_offset = i + 1;
			} else {
				++start_of_token_offset;
			}
		}
	}
	
	return ERROR_NUMBER_NO_ERROR;
}

/* Parses a block, a list of statements surrounded by curly braces. Blocks include functions, if statements and loops. */
Block* parse_block(FILE* restrict file, DataAndCode* restrict dac, ErrorNumber* error_number, GlobalData* global_data) {
	Block* block;
	char* buffer;
	int char_, last_char;
	bool before_statement = true;
	u64 buffer_allocated_length, i, block_comment_starting_line, number_of_slash_stars_in_block_comment;
	u64 block_allocated_length, line_number, line_number_at_start_of_statement;
	
	(void)global_data;
	
	assert_comparison(file, !=, NULL);
	assert_comparison(dac, !=, NULL);
	assert_comparison(error_number, !=, NULL);
	assert_comparison(global_data, !=, NULL);
	
	block = (Block*)m_zero_initialized_alloc(sizeof(Block));
	if (unlikely(!block)) unable_to_allocate_memory_to_parse_block();
	buffer = m_alloc(100);
	if (unlikely(!buffer)) unable_to_allocate_memory_to_parse_block();
	
	block->statement_or_block_array = (Statement**)m_alloc(sizeof(Statement*) * DEFAULT_NUMBER_OF_STATEMENTS_TO_ALLOCATE);
	if (unlikely(!buffer)) unable_to_allocate_memory_to_parse_block();
	
	line_number = 1;
	line_number_at_start_of_statement = 1;
	block_allocated_length = DEFAULT_NUMBER_OF_STATEMENTS_TO_ALLOCATE;
	buffer_allocated_length = 100;
	number_of_slash_stars_in_block_comment = 0;
	block->length = 0;
	
	while (1) {
		block_comment_starting_line = 0;
		last_char = '\0';
		
		while (1) {
			char_ = fgetc(file);
			if (char_ == EOF) goto after;
			
			if (char_ == '\n') {
				++line_number;
			}
			
			if (last_char == '*' && char_ == '/') {
				block_comment_starting_line = 0;
				continue;
			}
			
			if (char_ == '/') {
				int next_char = fgetc(file);
				if (next_char == EOF) {
					output_nullt_string("EOF\n");
					exit(1);
				}
				
				if (next_char == '*') {
					if (number_of_slash_stars_in_block_comment) {
						output_nullt_string("Warning on line ");
						output_u64(line_number);
						output_nullt_string(": ");
						if (number_of_slash_stars_in_block_comment == 1) {
							output_nullt_string("‘/*’ in block comment\n");
						} else if (number_of_slash_stars_in_block_comment == 2) {
							output_nullt_string("another ‘/*’ in block comment\n");
						} else {
							output_nullt_string("a ");
							output_u64(number_of_slash_stars_in_block_comment);
							if (number_of_slash_stars_in_block_comment == 3) {
								output_nullt_string("rd");
							} else {
								output_nullt_string("th");
							}
							output_nullt_string(" ‘/*’ in block comment\n");
						}
					} else {
						block_comment_starting_line = line_number;
					}
					++number_of_slash_stars_in_block_comment;
					continue;
				} else {
					if (ungetc(next_char, file) == EOF) {
						output_nullt_string("Unable to ungetc.\n");
						exit(1);
					}
				}
				continue;
			}
			
			if (!block_comment_starting_line && char_ != ' ' && char_ != '\t' && char_ != '\n') break;
			last_char = char_;
		}
		
		if (number_of_slash_stars_in_block_comment > 1) {
			if (number_of_slash_stars_in_block_comment == 2) {
				output_nullt_string("Style warning: remove the ‘/*’ that are is in the block comment!\n");
			} else {
				output_nullt_string("Style warning: remove all the ‘/*’s that are in the block comment!\n");
			}
		}
		
		line_number_at_start_of_statement = line_number;
		
		i = 0;
		
		while (char_ != EOF) {
			buffer[i] = char_;
			
			if (char_ == '\n') {
				++line_number;
			}
			
			if (i == buffer_allocated_length - 1) {
				buffer = m_realloc(buffer, buffer_allocated_length *= 2);
				if (unlikely(!buffer)) unable_to_allocate_memory_to_parse_block();
			}
			
			if (char_ == ';') {
				ErrorNumber result;
				
				if (block->length == block_allocated_length) {
					block->statement_or_block_array =
						(Statement**)m_realloc(block->statement_or_block_array, sizeof(Statement*) * (block_allocated_length *= 2));
					if (unlikely(!buffer)) unable_to_allocate_memory_to_parse_block();
				}
				block->statement_or_block_array[block->length] = (Statement*)m_alloc(sizeof(Statement));
				if (unlikely(!buffer)) unable_to_allocate_memory_for_statement();
				block->statement_or_block_array[block->length]->length_of_statement_line_string = i + 1;
				block->statement_or_block_array[block->length]->statement_line = m_alloc(i + 1);
				if (unlikely(!buffer)) unable_to_allocate_memory_for_statement();
				memcpy(block->statement_or_block_array[block->length]->statement_line, buffer, i + 1);
				block->statement_or_block_array[block->length]->type = STATEMENT_OR_BLOCK_TYPE__STATEMENT;
				
				result = parse_statement_string_into_statement(block->statement_or_block_array[block->length], line_number_at_start_of_statement);
				if (result != ERROR_NUMBER_NO_ERROR) {
					m_free(buffer);
					DEREF_ASSIGN(error_number) = result;
					return block;
				}
				
				line_number_at_start_of_statement = line_number;
				++block->length;
				before_statement = true;
				break;
			}
			before_statement = false;
			++i;
			char_ = fgetc(file);
		}
		
		if (char_ == EOF) {
			u64 j = --i;
			
			while (j) {
				if (buffer[j] != ' ' && buffer[j] != '\t' && buffer[j] != '\n') break;
				--j;
			}
			
			output_nullt_string("Syntax error: missing semicolon at the end of line ");
			output_u64(line_number - 1);
			output_nullt_string(":\n");
			output_string(buffer, i);
			
			/* Nice unicode arrow, isn't it? :) */
			output_nullt_string(" ◀――― ';'\n");
			
			m_free(buffer);
			DEREF_ASSIGN(error_number) = ERROR_NUMBER__SYNTAX_ERROR__MISSING_SEMICOLON;
			return block;
		}
	}
	
after:
	m_free(buffer);
	
	if (block_comment_starting_line) {
		output_nullt_string("Syntax error: unclosed block comment");
		if (block_comment_starting_line != line_number - 1) {
		 	output_nullt_string(" (it was opened on line ");
			output_u64(block_comment_starting_line);
			output_char(')');
		}
		output_char('\n');
		DEREF_ASSIGN(error_number) = ERROR_NUMBER__SYNTAX_ERROR__UNCLOSED_BLOCK_COMMENT_AT_BEGINNING_OF_LINE;
		return block;
	}
	
	hash_table_new(&block->variables_in_scope, 20, 4, 3.0);
	mid_level_instructions_init(&block->mid_level_instructions);
	
	for (i = 0; i < block->length; ++i) {
		ErrorNumber result = parse_statement(dac, block->statement_or_block_array[i], block);
		if (result != ERROR_NUMBER_NO_ERROR) {
			DEREF_ASSIGN(error_number) = result;
			return block;
		}
	}
	
	translate_mid_level_instructions_to_cpu_instructions(dac, &block->mid_level_instructions);
	DEREF_ASSIGN(error_number) = ERROR_NUMBER_NO_ERROR;
	return block;
}
