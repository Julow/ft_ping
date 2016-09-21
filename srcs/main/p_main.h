/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   p_main.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/21 11:47:57 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/21 12:51:53 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef P_MAIN_H
# define P_MAIN_H

# include "ft/argv.h"
# include "ft/libft.h"

typedef struct s_ping_args		t_ping_args;

/*
** ========================================================================== **
** Parse argv
*/

/*
** raw_sock_flags	=> raw_socket_create flags
** count			=> number of packet to send (0 means no limit)
** host				=> host
*/
struct			s_ping_args
{
	int				ai_family;
	uint32_t		count;
	char const		*host;
};

/*
** Parse options and fill 'dst'
** Return false on error, true on success
*/
bool			parse_argv(int ac, char **av, t_ping_args *dst);

#endif
