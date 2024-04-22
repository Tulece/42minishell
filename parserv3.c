#include "minishell.h"

int		ft_isshelloperator(char c)
{
	if (c == '<' || c == '>' || c == '|' || c == '&')
		return (1);
	return (0);
}

void    ft_add_char_to_buffer(char **buffer, char c, size_t *len)
{
	char    *new_buffer;

	new_buffer = malloc(*len + 2);
	if (!new_buffer)
	{
		ft_printf("Error: Allocation failed\n");
		exit(EXIT_FAILURE);
	}
	ft_memcpy(new_buffer, *buffer, *len);
	new_buffer[*len] = c;
	(*len)++;
	new_buffer[*len] = '\0';
	free(*buffer);
	*buffer = new_buffer;
}

void	set_quotes_state(t_tkn_info *tkn_info)
{
	if (*tkn_info->curr_char == '\'' && (tkn_info->curr_char == tkn_info->input || *(tkn_info->curr_char - 1) != '\\'))
	{
		if (tkn_info->state == reg)
			tkn_info->state = quote;
		else if (tkn_info->state == quote)
			tkn_info->state = reg;
	}
	else if (*tkn_info->curr_char == '"' && (tkn_info->curr_char == tkn_info->input || *(tkn_info->curr_char - 1) != '\\'))
	{
		if (tkn_info->state == reg)
			tkn_info->state = dquote;
		else if (tkn_info->state == dquote)
			tkn_info->state = reg;
	}
}

void	space_quotes(t_tkn_info *tkn_info)
{
	if (tkn_info->state == reg)
		while (ft_isspace(*(tkn_info->curr_char)))
			tkn_info->curr_char++;
	set_quotes_state(tkn_info);
}

int		break_token(t_tkn_info *tkn_info)
{
	t_sm	state;

	state = tkn_info->state;
	if (ft_isalnum(tkn_info->curr_char))
		return (0);
	if (state == reg)
	{
		if (ft_isshelloperator(tkn_info->curr_char))
			return (1);
	}
	else
		return (0);
}

void	set_token_text(t_tkn_info *tkn_info, t_token_lst *token)
{
	int		go;
	char	buffer[1024];

	go = 1;
	while (go)
	{
		ft_strcat(buffer, tkn_info->curr_char);
		if (break_token(tkn_info))
			go = 0;
	}
}

t_token_lst	*redir_token(t_tkn_info *tkn_info)
{
	t_token_lst *token;

	token = (t_token_lst *)malloc(sizeof(t_token_lst));
	space_quotes(tkn_info);
	if (*tkn_info->curr_char == '>')
	{
		tkn_info->curr_char++;
		if (*tkn_info->curr_char == '>')
		{
			tkn_info->curr_char++;
			token->type = redir_app;
		}
		else
			token->type = redir_out;
	}
	else if (*tkn_info->curr_char == '<')
	{
		tkn_info->curr_char++;
		if (*tkn_info->curr_char == '<')
		{
			tkn_info->curr_char++;
			token->type = heredoc;
		}
		else
			token->type = redir_in;
	}
	space_quotes(tkn_info);
	set_token_text(tkn_info, token);
}

t_token_lst *next_token(t_tkn_info *tkn_info)
{
	t_token_lst	*new_token;
	t_token_lst	*last;

	last = token_last(tkn_info->token_lst);
	if (*tkn_info->curr_char == '\0' || *tkn_info->curr_char == '\n')
		return (token_new(eol, NULL));
	if (*tkn_info->curr_char == '|')
	{
		tkn_info->curr_char++;
		if (*tkn_info->curr_char == '|')
		{
			tkn_info->curr_char++;
			return (token_new(or_op, NULL));
		}
		else
			return (token_new(pipe_op, NULL));
	}
	else if (*tkn_info->curr_char == '&' && *(tkn_info->curr_char + 1) == '&')
	{
		tkn_info->curr_char += 2;
		return (token_new(and_op, NULL));
	}
	if (*tkn_info->curr_char == '>' || *tkn_info->curr_char == '<')
		return (redir_token(tkn_info));
	space_quotes(tkn_info);
	return (cmd_token(tkn_info));
}

t_token_lst	*tokenize(char *input)
{
	t_token_lst	*last;
	t_tkn_info	tkn_info;

	tkn_info.input = ft_strdup(input);
	tkn_info.curr_char = NULL;
	tkn_info.token_lst = NULL;
	tkn_info.state = reg;
	tokenaddback(&tkn_info.token_lst, next_token(&tkn_info));
	last = token_last(tkn_info.token_lst);
	while (last && last->type != eol)
	{
		tokenaddback(&tkn_info.token_lst, next_token(&tkn_info));
		last = token_last(tkn_info.token_lst);
	}
	return (tkn_info.token_lst);
}
