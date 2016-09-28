/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_recv_loop.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/27 18:06:58 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/28 18:01:02 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/ft_printf.h"
#include "net/icmp.h"
#include "net/icmp_echo.h"

#include "p_main.h"

#include <netdb.h>
#include <stdlib.h>

/*
** Retrieve a sent packet by it's sequence number
** Return it's time delta on success, 0 if not found
*/
static uint64_t	ping_pop_packet(t_ping *ping, uint32_t seq)
{
	t_ping_packet		*p;
	uint64_t			delta_t;

	p = ft_set_get(&ping->sent_packets.set, &seq);
	if (p == NULL)
		return (0);
	delta_t = ft_clock(p->timestamp);
	if (delta_t >= ping->timeout - PING_DANGEROUS_DELTA_T)
		return (0);
	ft_oset_remove(&ping->sent_packets, p);
	free(p);
	return (delta_t);
}

static bool		ping_recved_echo(t_ping *ping, t_ip_info const *ip_info,
					t_icmp_echo_data const *echo_data, t_sub payload)
{
	uint64_t			delta_t;

	if (echo_data->id != ping->echo_id)
		return (false);
	if ((delta_t = ping_pop_packet(ping, echo_data->seq)) == 0)
		return (false);
	ping->total_received++;
	ping->total_time += delta_t;
	if (ping->max_time < delta_t)
		ping->max_time = delta_t;
	if (ping->min_time == 0 || ping->min_time > delta_t)
		ping->min_time = delta_t;
	if (!(ping->flags & PING_F_QUIET))
		ping_print_reply(ping, ip_info, echo_data, delta_t, payload);
	return (true);
}

__attribute__ ((noreturn))
void			ping_recvloop(t_ping *ping)
{
	uint32_t			len;
	char				buff[ICMP_MAX_SIZE];
	t_ip_info			ip_info;
	t_icmp_header		icmp_header;
	t_icmp_echo_data	echo_data;

	while (true)
	{
		len = icmp_recv(ping->sock, &ip_info, &icmp_header, buff, sizeof(buff));
		if (icmp_is_echo_reply(&icmp_header, &echo_data)
			&& ping_recved_echo(ping, &ip_info, &echo_data, SUB(buff, len)))
			;
		else if (ping->flags & PING_F_VERBOSE)
			ping_print_verbose(ping, &ip_info, &icmp_header, SUB(buff, len));
	}
}
