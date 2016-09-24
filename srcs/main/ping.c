/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/24 17:04:41 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/24 17:10:27 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "p_main.h"

static void		print_reply(uint32_t total_size, t_ip_info const *ip_info,
					t_icmp_echo_data const *echo_data)
{
	char			addr_buff[MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)];

	inet_ntop(((ip_info->version == 6) ? AF_INET6 : AF_INET), &ip_info->src_addr,
			addr_buff, sizeof(addr_buff));
	ft_printf("%u bytes from %s: icmp_seq=%u ttl=%u ...%n",
			total_size, addr_buff, echo_data->seq, ip_info->max_hop);
}

void			ping_recvloop(t_ping *ping)
{
	uint32_t			len;
	char				buff[512];
	t_ip_info			ip_info;
	t_icmp_echo_data	echo_data;

	while (ping->to_send > 0 || ping->to_receive > 0)
	{
		len = icmp_echo_recv(ping->sock, &ip_info, &echo_data, buff, sizeof(buff));
		if (len == 0)
			return ;
		if (echo_data.id != ping->echo_id)
			continue ;
		if (ping->flags & PING_F_PRINT)
			ft_hexdump(buff, len, HEXDUMP_DEFAULT);
		print_reply(len + sizeof(t_icmp_header), &ip_info, &echo_data);
		ping->to_receive--;
	}
}

void		ping_send(t_ping *ping)
{
	if (ping->to_send == 0)
		return ;
	// icmp_echo_send(ping->sock, 0, 0, SUBC("lolololol")); // WTF
	icmp_echo_send(ping->sock, ICMP_ECHO_DATA(ping->echo_id, ping->echo_seq),
		ping->payload);
	ping->to_send--;
	ping->to_receive++;
	ping->echo_seq++;
	set_timeout(&ping_send, ping, 1);
}
