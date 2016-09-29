/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_send.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/27 18:06:52 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/29 18:28:02 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/ft_printf.h"
#include "net/icmp.h"
#include "net/icmp_echo.h"

#include "p_main.h"

#include <stdlib.h>
#include <unistd.h>

static void		fill_payload(t_ping const *ping, char *dst, uint32_t size)
{
	uint32_t	i;
	uint32_t	tmp;

	i = 0;
	while (i < size)
	{
		tmp = MIN(size - i, ping->payload_pattern.length);
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
		if (ft_clock(p->timestamp) < ping->timeout)
			break ;
		ft_oset_remove(&ping->sent_packets, p);
		ft_printf("Request timeout for icmp_seq=%u%n", p->seq_number);
		ping->stats.total_timeout++;
		free(p);
	}
}

bool			ping_repeat(t_ping_repeat *r)
{
	if (r->i >= r->count)
	{
		if (r->inc != 0)
		{
			if ((r->val + r->inc) >= r->max)
				return (false);
			r->val += r->inc;
			r->i = 0;
		}
		else if (r->count != 0)
			return (false);
	}
	r->i++;
	return (true);
}

void			ping_send(t_ping *ping)
{
	uint32_t const	payload_size = ping->payload_size.val;
	char			payload[payload_size];

	ping_pop_timeout(ping);
	if (ping_repeat(&ping->payload_size))
	{
		fill_payload(ping, payload, payload_size);
		ping_push_packet(ping);
		if (!icmp_echo_send(ping->sock,
				ICMP_ECHO_DATA(ping->echo_id, ping->echo_seq),
				SUB(payload, sizeof(payload))))
			exit(1);
		ping->echo_seq++;
		ping->stats.total_sent++;
	}
	else if (ping->sent_packets.set.count == 0)
	{
		ping_print_stats(ping);
		exit((ping->stats.total_received == 0) ? 1 : 0);
	}
	alarm(ping->wait_time);
}
