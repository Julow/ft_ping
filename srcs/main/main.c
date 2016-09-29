/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/18 15:41:58 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/29 18:19:11 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/ft_printf.h"
#include "ft/libft.h"
#include "net/icmp.h"
#include "net/icmp_echo.h"

#include "p_main.h"

#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void		print_status(t_ping const *ping)
{
	char			addr_buff[RAW_SOCKET_ADDR_LEN];

	raw_socket_addr(ping->sock, addr_buff);
	ft_printf("PING %s (%s): ", ping->host_name, addr_buff);
	if (ping->payload_size.inc == 0)
		ft_printf("%u", ping->payload_size.val);
	else
		ft_printf("(%u ... %u)", ping->payload_size.val,
			(ping->payload_size.inc > 0) ? ping->payload_size.max : 0);
	ft_printf(" data bytes%n");
}

static bool		send_preload(t_ping *ping, uint32_t p)
{
	while (p-- > 0)
	{
		ping_send(ping);
	}
	return (true);
}

static int		ping_packet_cmp(t_ping_packet const *a, uint32_t const *key)
{
	if (a->seq_number < *key)
		return (-1);
	else
		return ((a->seq_number > *key) ? 1 : 0);
}

static bool		init_ping(t_ping_args const *args, t_ping *dst)
{
	ft_bzero(dst, sizeof(t_ping));
	*dst = (t_ping){
		.sent_packets = OSET(&ping_packet_cmp, 0),
		.host_name = args->host,
		.timeout = args->timeout * T_SEC,
		.wait_time = args->wait,
		.flags = args->flags,
		.echo_id = getpid(),
		.payload_pattern = args->payload_pattern,
		.payload_size = PING_REPEAT(args->count, args->payload_size,
				args->inc_size, args->max_size),
	};
	if ((dst->sock = raw_socket_create(args->host, args->ai_family)) == NULL)
		return (false);
	if (args->ttl > 0 && setsockopt(dst->sock->fd, IPPROTO_IP, IP_TTL,
				&args->ttl, sizeof(uint32_t)) < 0)
	{
		ft_dprintf(2, "setsockopt: %s%n", strerror(errno));
		return (false);
	}
	return (true);
}

int				main(int argc, char **argv)
{
	t_ping_args		ping_args;
	t_ping			ping;

	if (!parse_argv(argc, argv, &ping_args))
		return (2);
	if (!init_ping(&ping_args, &ping))
		return (1);
	ping_handle_signals(&ping);
	print_status(&ping);
	send_preload(&ping, ping_args.preload);
	ping_recvloop(&ping);
}
