/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   p_main.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/21 11:47:57 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/29 18:31:14 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef P_MAIN_H
# define P_MAIN_H

# include "ft/argv.h"
# include "ft/libft.h"
# include "ft/oset.h"
# include "ft/utils.h"
# include "net/icmp.h"
# include "net/icmp_echo.h"
# include "net/raw_socket.h"

typedef struct s_ping_args		t_ping_args;

/*
** ========================================================================== **
** Ping
*/

typedef struct s_ping			t_ping;
typedef struct s_ping_packet	t_ping_packet;
typedef struct s_ping_stats		t_ping_stats;
typedef struct s_ping_repeat	t_ping_repeat;

struct			s_ping_stats
{
	uint32_t		total_sent;
	uint32_t		total_received;
	uint32_t		total_timeout;
	uint64_t		total_time;
	uint32_t		max_time;
	uint32_t		min_time;
};

/*
** Increase 'val' by 'inc' every 'count' calls until it reach 'max'
** If 'inc' is 0, never increment
** If 'count' is 0, increment every time if 'inc' > 0 otherwise never end
** -
** PING_REPEAT(COUNT, INITIAL_VAL, INC, MAX)	Constructor
*/
struct			s_ping_repeat
{
	uint32_t		i;
	uint32_t		count;
	uint32_t		val;
	uint32_t		inc;
	uint32_t		max;
};

# define PING_REPEAT(C, VAL, INC, M)	((t_ping_repeat){0, C, VAL, INC, M})

struct			s_ping
{
	t_raw_socket	*sock;
	t_oset			sent_packets;
	char const		*host_name;
	uint32_t		flags;
	uint64_t		timeout;
	uint32_t		wait_time;
	uint16_t		echo_id;
	uint16_t		echo_seq;
	t_sub			payload_pattern;
	t_ping_repeat	payload_size;
	t_ping_stats	stats;
};

struct			s_ping_packet
{
	t_oset_h		set_h;
	uint32_t		seq_number;
	uint64_t		timestamp;
};

# define PING_F_PRINT		(1 << 0)
# define PING_F_QUIET		(1 << 1)
# define PING_F_VERBOSE		(1 << 2)
# define PING_F_NO_LOOKUP	(1 << 3)

# define PING_DANGEROUS_DELTA_T	(T_MSEC)

void			ping_recvloop(t_ping *ping);

void			ping_send(t_ping *ping);

void			ping_show_stats(t_ping const *ping);

void			ping_handle_signals(t_ping *ping);

void			ping_print_reply(t_ping const *ping, t_ip_info const *ip_info,
					t_icmp_echo_data const *echo_data, uint64_t delta_t,
					t_sub payload);
void			ping_print_verbose(t_ping const *ping, t_ip_info const *ip_info,
					t_icmp_header const *icmp_header, t_sub payload);
void			ping_print_stats(t_ping const *ping);

/*
** Increment 'r'
** Return false if it reach the maximum, true otherwise
*/
bool			ping_repeat(t_ping_repeat *r);

/*
** ========================================================================== **
** Parse argv
*/

struct			s_ping_args
{
	int				ai_family;
	uint32_t		ttl;
	uint32_t		preload;
	uint32_t		flags;
	uint32_t		count;
	uint32_t		wait;
	uint32_t		timeout;
	int32_t			inc_size;
	uint32_t		max_size;
	t_sub			payload_pattern;
	uint32_t		payload_size;
	char const		*host;
};

/*
** Parse options and fill 'dst'
** Return false on error, true on success
*/
bool			parse_argv(int ac, char **av, t_ping_args *dst);

#endif
