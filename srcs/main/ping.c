/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/24 17:04:41 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/27 14:19:31 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "p_main.h"
#include <ft/ft_printf.h>

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

	while (ping->to_receive > 0)
	{
		len = icmp_echo_recv(ping->sock, &ip_info, &echo_data, buff, sizeof(buff));
		if (echo_data.id != ping->echo_id)
			continue ;
		if (!(ping->flags & PING_F_QUIET))
		{
			print_reply(len + sizeof(t_icmp_header), &ip_info, &echo_data);
			if (ping->flags & PING_F_PRINT)
				ft_hexdump(buff, len, HEXDUMP_DEFAULT);
		}
		ping->to_receive--;
	}
}

static void	fill_payload(t_ping const *ping, char *dst)
{
	uint32_t	i;
	uint32_t	tmp;

	i = 0;
	while (i < ping->payload_size)
	{
		tmp = MIN(ping->payload_size - i, ping->payload_pattern.length);
		ft_memcpy(dst + i, ping->payload_pattern.str, tmp);
		i += tmp;
	}
}

bool		ping_send(t_ping *ping)
{
	char		payload[ping->payload_size];

	fill_payload(ping, payload);
	if (!icmp_echo_send(ping->sock,
			ICMP_ECHO_DATA(ping->echo_id, ping->echo_seq),
			SUB(payload, sizeof(payload))))
		return (false);
	ping->echo_seq++;
	ping->sent++;
	if (ping->sent >= ping->count)
	{
		if (ping->payload_inc != 0)
		{
			if ((ping->payload_size += ping->payload_inc) >= ping->payload_max)
				return (true);
			ping->sent = 0;
		}
		else if (ping->count != 0)
			return (true);
	}
	ping->to_receive++;
	set_timeout(V(&ping_send), ping, ping->wait_time);
	return (true);
}
