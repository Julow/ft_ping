/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   p_main.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/21 11:47:57 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/24 19:18:26 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef P_MAIN_H
# define P_MAIN_H

# include "ft/argv.h"
# include "ft/libft.h"

typedef struct s_ping_args		t_ping_args;

/*
** ========================================================================== **
** ICMP Echo
*/

# include "net/icmp.h"
# include "net/raw_socket.h"

typedef union u_icmp_echo_data		t_icmp_echo_data;

/*
** data			=> Raw data field (see icmp_header)
** id			=> "Identifier" field
** seq			=> "Sequence Number" field
*/
union			u_icmp_echo_data
{
	uint32_t		data;
	struct {
		uint16_t		id;
		uint16_t		seq;
	};
};

# define ICMP_ECHO_DATA(ID, SEQ)	((t_icmp_echo_data){.id=(ID), .seq=(SEQ)})

/*
** Wrapper for icmp_send that send echo-request
*/
bool			icmp_echo_send(t_raw_socket *sock, t_icmp_echo_data data,
					t_sub payload);

/*
** Wrapper for icmp_recv that ignore packets other than echo-reply
*/
uint32_t		icmp_echo_recv(t_raw_socket *sock, t_ip_info *ip_info,
					t_icmp_echo_data *echo_data, void *buff, uint32_t buff_len);

/*
** ========================================================================== **
** Ping
*/

typedef struct s_ping			t_ping;

struct			s_ping
{
	t_raw_socket	*sock;
	char const		*host_name;
	uint32_t const	flags;
	uint32_t const	wait_time;
	uint16_t const	echo_id;
	uint16_t		echo_seq;
	uint32_t		to_receive;
	uint32_t		to_send;
	t_sub const		payload_pattern;
	uint32_t const	payload_size;
};

# define PING_F_PRINT		(1 << 0)
# define PING_F_QUIET		(1 << 1)

void			ping_recvloop(t_ping *ping);

bool			ping_send(t_ping *ping);

/*
** ========================================================================== **
** Parse argv
*/

/*
** ai_family		=> (socket) adress family
** ttl				=> time to live
** preload			=> number of packet to send before the normal ping loop
** flags			=> ping flags
** count			=> number of packet to send (0 means no limit)
** host				=> host
*/
struct			s_ping_args
{
	int				ai_family;
	uint32_t		ttl;
	uint32_t		preload;
	uint32_t		flags;
	uint32_t		count;
	uint32_t		wait;
	t_sub			payload_pattern;
	uint32_t		payload_size;
	char const		*host;
};

/*
** Parse options and fill 'dst'
** Return false on error, true on success
*/
bool			parse_argv(int ac, char **av, t_ping_args *dst);

/*
** ========================================================================== **
** Utils
*/

#define HEXDUMP_DEFAULT		VEC2U(2, 8)

/*
** Show 'size' bytes of 'data'
** 'layout.x' is the number of byte group and 'layout.y' is the size of a group
*/
void			ft_hexdump(void const *data, uint32_t size, t_vec2u layout);

/*
** Call function 'f' in 't' seconds
** The function 'f' take 'data' as param
*/
void			set_timeout(void (*f)(), void *data, uint32_t t);

#endif
