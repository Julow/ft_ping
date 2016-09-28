/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_print.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/28 14:03:21 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/28 14:11:34 by jaguillo         ###   ########.fr       */
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

static void		print_host_name(t_ip_info const *ip_info)
{
	char					addr_buff[MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)];
	char					name_buff[128];
	struct sockaddr_storage	sa;
	uint32_t				sa_len;

	inet_ntop(((ip_info->version == 6) ? AF_INET6 : AF_INET),
			&ip_info->src_addr, addr_buff, sizeof(addr_buff));
	sa_len = sockaddr_of_ipinfo(&sa, ip_info);
	if (getnameinfo(V(&sa), sa_len, name_buff, sizeof(name_buff),
				NULL, 0, NI_NAMEREQD) == 0)
		ft_printf("%s (%s)", name_buff, addr_buff);
	else
		ft_printf("%s", addr_buff);
}

void			ping_print_reply(t_ping const *ping, t_ip_info const *ip_info,
					t_icmp_echo_data const *echo_data, uint64_t delta_t,
					t_sub payload)
{
	ft_printf("%u bytes from ", payload.length + sizeof(t_icmp_header));
	print_host_name(ip_info);
	ft_printf(": icmp_seq=%u ttl=%u time=%u.%0.3u ms%n",
			echo_data->seq, ip_info->max_hop,
			(uint32_t)(delta_t / 1000), (uint32_t)(delta_t % 1000));
	if (ping->flags & PING_F_PRINT)
		ft_hexdump(payload.str, payload.length, HEXDUMP_DEFAULT);
}

void			ping_print_verbose(t_ping const *ping, t_ip_info const *ip_info,
					t_icmp_header const *icmp_header, t_sub payload)
{
	ft_printf("[v] %u bytes from ", payload.length);
	print_host_name(ip_info);
	ft_printf(": type=%u", icmp_header->type);
	if (g_icmp_type_names[icmp_header->type] != NULL)
		ft_printf(" \"%s\"", g_icmp_type_names[icmp_header->type]);
	ft_printf(" code=%u data=%u ttl=%u%n",
			icmp_header->code, icmp_header->data, ip_info->max_hop);
	if (ping->flags & PING_F_PRINT)
		ft_hexdump(payload.str, payload.length, HEXDUMP_DEFAULT);
}
