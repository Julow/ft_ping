/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_recv_loop.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/27 18:06:58 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/28 11:47:08 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/ft_printf.h"
#include "net/icmp.h"
#include "net/icmp_echo.h"

#include "p_main.h"

#include <netdb.h>
#include <stdlib.h>

static bool		get_host_name(t_ip_info const *ip_info, char *buff,
					uint32_t buff_size)
{
	struct sockaddr_storage	sa;
	uint32_t				sa_len;

	sa_len = sockaddr_of_ipinfo(&sa, ip_info);
	if (getnameinfo(V(&sa), sa_len, buff, buff_size, NULL, 0, NI_NAMEREQD) != 0)
		return (false);
	return (true);
}

static void		print_reply(uint32_t total_size, t_ip_info const *ip_info,
					t_icmp_echo_data const *echo_data, uint64_t delta_t)
{
	char			addr_buff[MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)];
	char			name_buff[128];

	inet_ntop(((ip_info->version == 6) ? AF_INET6 : AF_INET),
			&ip_info->src_addr, addr_buff, sizeof(addr_buff));
	ft_printf("%u bytes from ", total_size);
	if (get_host_name(ip_info, name_buff, sizeof(name_buff)))
		ft_printf("%s (%s)", name_buff, addr_buff);
	else
		ft_printf("%s", addr_buff);
	ft_printf(": icmp_seq=%u ttl=%u time=%u.%0.3u ms%n",
			echo_data->seq, ip_info->max_hop,
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
	ping->total_received++;
	ping->total_time += delta_t;
	if (ping->max_time < delta_t)
		ping->max_time = delta_t;
	if (ping->min_time == 0 || ping->min_time > delta_t)
		ping->min_time = delta_t;
	if (!(ping->flags & PING_F_QUIET))
	{
		print_reply(payload.length + sizeof(t_icmp_header), ip_info,
				echo_data, delta_t);
		if (ping->flags & PING_F_PRINT)
			ft_hexdump(payload.str, payload.length, HEXDUMP_DEFAULT);
	}
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
		if (len == 0)
			continue ;
		if (icmp_is_echo_reply(&icmp_header, &echo_data))
			ping_recved_echo(ping, &ip_info, &echo_data, SUB(buff, len));
	}
}
