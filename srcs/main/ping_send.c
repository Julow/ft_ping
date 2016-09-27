/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_send.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/27 18:06:52 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/27 18:08:14 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "net/icmp.h"
#include "net/icmp_echo.h"

#include "p_main.h"

#include <stdlib.h>

static void		fill_payload(t_ping const *ping, char *dst)
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

static void		ping_push_packet(t_ping *ping)
{
	t_ping_packet *const	p = NEW(t_ping_packet);

	*p = (t_ping_packet){
		.set_h = OSET_HEAD(),
		.seq_number = ping->echo_seq,
		.timestamp = ft_clock(0),
	};
	ft_oset_insert(&ping->sent_packets, p, &p->seq_number);
}

static void		ping_pop_timeout(t_ping *ping)
{
	t_ping_packet	*p;

	while ((p = ping->sent_packets.first) != NULL)
	{
		if (ft_clock(p->timestamp) >= ping->timeout)
		{
			TRACE("TIMEOUT");
			ft_oset_remove(&ping->sent_packets, p);
			free(p);
		}
	}
}

bool			ping_send(t_ping *ping)
{
	char		payload[ping->payload_size];

	ping_pop_timeout(ping);
	fill_payload(ping, payload);
	if (!icmp_echo_send(ping->sock,
			ICMP_ECHO_DATA(ping->echo_id, ping->echo_seq),
			SUB(payload, sizeof(payload))))
		return (false);
	ping_push_packet(ping);
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