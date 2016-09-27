/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_recv_loop.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/27 18:06:58 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/27 18:07:39 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/ft_printf.h"
#include "net/icmp.h"
#include "net/icmp_echo.h"

#include "p_main.h"

#include <stdlib.h>

static void		print_reply(uint32_t total_size, t_ip_info const *ip_info,
					t_icmp_echo_data const *echo_data, uint64_t delta_t)
{
	char			addr_buff[MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)];

	inet_ntop(((ip_info->version == 6) ? AF_INET6 : AF_INET),
			&ip_info->src_addr, addr_buff, sizeof(addr_buff));
	ft_printf("%u bytes from %s: icmp_seq=%u ttl=%u time=%u.%0.3u ms%n",
			total_size, addr_buff, echo_data->seq, ip_info->max_hop,
			delta_t / 1000, delta_t % 1000);
}

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

static void		ping_recved_echo(t_ping *ping, t_ip_info const *ip_info,
					t_icmp_echo_data const *echo_data, t_sub payload)
{
	uint64_t			delta_t;

	if (echo_data->id != ping->echo_id)
		return ;
	if ((delta_t = ping_pop_packet(ping, echo_data->seq)) == 0)
		return ;
	if (!(ping->flags & PING_F_QUIET))
	{
		print_reply(payload.length + sizeof(t_icmp_header), ip_info,
				echo_data, delta_t);
		if (ping->flags & PING_F_PRINT)
			ft_hexdump(payload.str, payload.length, HEXDUMP_DEFAULT);
	}
	ping->to_receive--;
}

void			ping_recvloop(t_ping *ping)
{
	uint32_t			len;
	char				buff[ICMP_MAX_SIZE];
	t_ip_info			ip_info;
	t_icmp_header		icmp_header;
	t_icmp_echo_data	echo_data;

	while (ping->to_receive > 0)
	{
		len = icmp_recv(ping->sock, &ip_info, &icmp_header, buff, sizeof(buff));
		if (len == 0)
			continue ;
		if (icmp_is_echo_reply(&icmp_header, &echo_data))
			ping_recved_echo(ping, &ip_info, &echo_data, SUB(buff, len));
	}
}
