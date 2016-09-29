/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp_echo.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/27 17:55:41 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/29 17:20:44 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ICMP_ECHO_H
# define ICMP_ECHO_H

# include "net/icmp.h"
# include "net/raw_socket.h"

typedef struct s_icmp_echo_data		t_icmp_echo_data;

/*
** ========================================================================== **
** ICMP Echo
*/

/*
** id			=> "Identifier" field
** seq			=> "Sequence Number" field
*/
struct			s_icmp_echo_data
{
	uint16_t		id;
	uint16_t		seq;
};

# define ICMP_ECHO_DATA(ID, SEQ)	((t_icmp_echo_data){.id=(ID), .seq=(SEQ)})

/*
** Wrapper for icmp_send that send echo-request
*/
bool			icmp_echo_send(t_raw_socket *sock, t_icmp_echo_data data,
					t_sub payload);

/*
** Check if 'header' correspond to an echo reply and fill 'echo_data'
** Return true on success, false otherwise
*/
bool			icmp_is_echo_reply(t_icmp_header const *header,
					t_icmp_echo_data *echo_data);

#endif
