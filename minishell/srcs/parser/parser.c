#include "minishell.h"

static t_list			*interprets_tokens(t_list **tokens, int cmd_id, int cmd_group, int closed);
static t_list			*handle_command(t_list **tokens, int cmd_id, int cmd_group);
static t_list			*look_for_redir(t_list **tokens, int cmd_id, int closed);
static void				insert_token_in_list(void *instruction, int instr_type);
static t_redirect		*handle_redir(t_list *curr_token, int cmd_id, int before);
static t_cmd			*init_instruction(t_minishell *ms);

t_minishell	*parser(char *line, t_minishell *minishell)
{
	minishell->tokens = calloc_or_exit(1, sizeof(t_list *));
	minishell->instructions = calloc_or_exit(1, sizeof(t_list *));
	minishell->tokens = lexer(line, minishell->tokens);
	if (prog_state(TAKE_STATE) == PROG_OK)
		interprets_tokens(minishell->tokens, 0, 0, 0);
	DEBUG(print_tokens(minishell->tokens);)
	return (minishell);
}

/* always assumes every function call is the first token
 * TODO ensure comparing of ( ) with their types!
 * Legal case: < test-1 | wc !! */
static t_list *interprets_tokens(t_list *curr, int cmd_id, int cmd_group, int closed)
{
	t_token			*curr_token;
	const t_list	*start_pos = curr;

	curr_token = (t_token *)curr->content;
	curr = look_for_redir(curr_token, cmd_id);
	curr_token = (t_token *)curr->content;
	if (prog_state(TAKE_STATE) != PROG_OK)
		return (NULL);
	if (curr_token->type == WORD)
		curr_token = handle_command(curr, cmd_id, cmd_group);
	else if (is_logic_op(curr_token->str))
		;// passing
	else if (ft_strncmp(curr_token->str, "(", 2) == 0)
		curr_token = interprets_tokens(&(*curr)->next, cmd_id + 1, cmd_group + 1, true);
	/* this has the be thought of;
		 * how to handle nesting?
		 * how to avoid empty parens? */
	else if (ft_strncmp(curr_token->str, ")", 2) == 0)
		;// passing
		//curr_token = gets_commands(curr_token->next, cmd_id + 1, cmd_group + 1, true);
	curr = look_for_redir(curr_token, cmd_id);
	curr_token = (t_token *)curr->content;
	if (prog_state(TAKE_STATE) != PROG_OK)
		return (NULL);
	// check if pipe
}

/* initiates an instruction outside of the main
 * handling functions; this ensures allocation is retrievable
 * in case of failure at the handler function */

static t_cmd	*init_instruction(t_minishell *ms)
{
	t_instruction	*instr;
	t_list			*new_instr;

	instr = calloc_or_exit(1, sizeof(t_instruction));
	new_instr = ft_lstnew(instr);
	if (!new_instr)
		ft_error_exit(MEMORY_FAIL);
	ft_lstadd_back(ms->instructions, new_instr);
	instr->cmd = calloc_or_exit(1, sizeof(t_cmd));
	return (instr->cmd);
}

static t_list	*handle_command(t_list **tokens, int cmd_id, int cmd_group)
{
	t_cmd	*cmd;
	int		length;
	int		i;

	cmd = init_instruction(get_minishell(NULL));
	length = take_length_of_command(*tokens);
	cmd->id = cmd_id;
	cmd->group = cmd_group;
	cmd->name = ((t_token *)(*tokens)->content)->str;
	*tokens = (*tokens)->next;
	if (length > 1)
		cmd->args = calloc_or_exit(length, sizeof(char **));
	i = -1;
	while (++i != length - 1)
	{
		cmd->args[i] = ((t_token *)(*tokens)->content)->str;
		*tokens = (*tokens)->next;
	}
	return (*tokens);
}

/* TODO might be shaky regarding "(null) < command" situations
 * the function checks for consecutive redirections, whenever appliable? */
static t_list	*look_for_redir(t_list *curr_token, int cmd_id)
{
	t_token		*token;
	t_redirect	*redir;

	while (curr_token)
	{
		token = (t_token *)curr_token->content;
		if (is_redir_op(token->str))
		{
			curr_token = handle_redir(curr_token, cmd_id, before);
			curr_token = curr_token->next;
		}
		else
			break;
	}
	return (curr_token);
}

/* returns the token with the text, after the redir token
 * merge this with the previous function, for increased sexyness of the code */
static t_list	*handle_redir(t_token *operator, t_token *file_name, int cmd_id)
{
	t_redirect	*redir;
	t_token		*next;

	redir = init_instruction(get_minishell(NULL));
	if (token->type == OPERATOR && ft_strncmp(token->str, "<", 2) == 0)
		redir->type = REDIR_IN;
	else if (token->type == OPERATOR && ft_strncmp(token->str, ">", 2) == 0)
		redir->type = REDIR_OUT;
	else if (token->type == OPERATOR && ft_strncmp(token->str, "<<" , 3) == 0)
		redir->type = REDIR_HERE_DOC;
	else if (token->type == OPERATOR && ft_strncmp(token->str, ">>" , 3) == 0)
		redir->type = REDIR_OUT_APPEND;
	/* no redir found! */
	if (curr_token->next != NULL)
		next = ((t_token *)curr_token->next)->content;
	else if ((t_list *)curr_token->next == NULL || next->type != WORD)
	{
		prog_state(PARSER_ERROR);
		return;
	}
	redir->file_name = ft_strdup(next->str);
	redir->cmd_id = cmd_id;
	return (curr_token->next);
}

/* TODO this function can be generalized to accept all tokens,
 * typecasting (if needed) according to instr_type */
static void		insert_token_in_list(void *instruction, int instr_type)
{
	t_minishell *ms;
	t_list	*new_node;

	ms = get_minishell(NULL);
	if (instr_type == INSTR_CMD)
	{
		new_node = ft_lstnew(instruction);
		if (!(new_node))
			return;
		ft_lstadd_back(ms->instructions, new_node);
	}
}
