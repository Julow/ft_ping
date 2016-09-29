/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_print.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/28 14:03:21 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/29 17:59:32 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/ft_printf.h"
#include "p_main.h"

#include <netdb.h>

static char const *const	g_icmp_type_names[255] = {
	[0] = "Echo Reply",
	[3] = "Destination Unreachable",
	[4] = "Source Quench",
	[5] = "Redirect Message",
	[8] = "Echo Request",
	[9] = "Router Advertisement",
	[10] = "Router Solicitation",
	[11] = "Time Exceeded",
	[12] = "Bad IP header",
	[13] = "Timestamp",
	[14] = "Timestamp Reply",
	[15] = "Information Request",
	[16] = "Information Reply",
	[17] = "Address Mask Request",
	[18] = "Address Mask Reply",
	[30] = "Traceroute",
};

static void		print_host_name(t_ping const *ping, t_ip_info const *ip_info)
{
	char					addr_buff[MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)];
	char					name_buff[256];
	struct sockaddr_storage	sa;
	uint32_t				sa_len;

	inet_ntop(((ip_info->version == 6) ? AF_INET6 : AF_INET),
			&ip_info->src_addr, addr_buff, sizeof(addr_buff));
	sa_len = sockaddr_of_ipinfo(&sa, ip_info);
	if (!(ping->flags & PING_F_NO_LOOKUP) && getnameinfo(V(&sa), sa_len,
			name_buff, sizeof(name_buff), NULL, 0, NI_NAMEREQD) == 0)
		ft_printf("%s (%s)", name_buff, addr_buff);
	else
		ft_printf("%s", addr_buff);
}

#define USEC_TO_MSEC_VEC(U)	((uint32_t)((U)/T_MSEC)), ((uint32_t)((U)%T_MSEC))

void			ping_print_reply(t_ping const *ping, t_ip_info const *ip_info,
					t_icmp_echo_data const *echo_data, uint64_t delta_t,
					t_sub payload)
{
	ft_printf("%u bytes from ", payload.length + sizeof(t_icmp_header));
	print_host_name(ping, ip_info);
	ft_printf(": icmp_seq=%u ttl=%u time=%u.%0.3u ms%n",
			echo_data->seq, ip_info->max_hop, USEC_TO_MSEC_VEC(delta_t));
	if (ping->flags & PING_F_PRINT)
		ft_hexdump(payload.str, payload.length, HEXDUMP_DEFAULT);
}

void			ping_print_verbose(t_ping const *ping, t_ip_info const *ip_info,
					t_icmp_header const *icmp_header, t_sub payload)
{
	ft_printf("[v] %u bytes from ", payload.length);
	print_host_name(ping, ip_info);
	ft_printf(": type=%u", icmp_header->type);
	if (g_icmp_type_names[icmp_header->type] != NULL)
		ft_printf(" \"%s\"", g_icmp_type_names[icmp_header->type]);
	ft_printf(" code=%u data=%u ttl=%u%n",
			icmp_header->code, icmp_header->data, ip_info->max_hop);
	if (ping->flags & PING_F_PRINT)
		ft_hexdump(payload.str, payload.length, HEXDUMP_DEFAULT);
}

void			ping_print_stats(t_ping const *ping)
{
	t_ping_stats const *const	s = &ping->stats;

	ft_printf("--- %s ping statistics ---\n", ping->host_name);
	ft_printf("%u packets transmitted, %u packets received, %u%% packet loss%n",
			s->total_sent, s->total_received,
			100 - ((s->total_received == 0) ? 0 :
							s->total_received * 100 / s->total_sent));
	if (s->total_received != s->total_sent)
		ft_printf("    (%u timeout, %u pending)%n", s->total_timeout,
				s->total_sent - s->total_received - s->total_timeout);
	if (s->total_received > 0)
		ft_printf("round-trip min/avg/max = %u.%0.3u/%u.%0.3u/%u.%0.3u ms%n",
				USEC_TO_MSEC_VEC(s->min_time),
				USEC_TO_MSEC_VEC(s->total_time / s->total_received),
				USEC_TO_MSEC_VEC(s->max_time));
}
