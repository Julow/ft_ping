/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/18 15:41:58 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/28 11:47:27 by jaguillo         ###   ########.fr       */
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
	if (ping->payload_inc == 0)
		ft_printf("%u", ping->payload_size);
	else
		ft_printf("(%u ... %u)", ping->payload_size,
			(ping->payload_inc > 0) ? ping->payload_max : 0);
	ft_printf(" data bytes%n");
}

static bool		send_preload(t_ping *ping, uint32_t p)
{
	while (p-- > 0)
	{
		if (!ping_send(ping))
			return (false);
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

__attribute__ ((noreturn))
static void		ping(t_raw_socket *sock, t_ping_args const *args)
{
	t_ping			ping;

	ping = (t_ping){
		.sock = sock,
		.sent_packets = OSET(&ping_packet_cmp, 0),
		.host_name = args->host,
		.timeout = args->timeout * T_SEC,
		.wait_time = args->wait,
		.flags = args->flags,
		.echo_id = getpid(),
		.echo_seq = 0,
		.count = args->count,
		.sent = 0,
		.payload_pattern = args->payload_pattern,
		.payload_size = args->payload_size,
		.payload_inc = args->inc_size,
		.payload_max = args->max_size,
		.total_sent = 0,
		.total_received = 0,
		.total_time = 0,
	};
	ping_handle_signals(&ping);
	if (args->ttl > 0 && setsockopt(sock->fd, IPPROTO_IP, IP_TTL,
				&args->ttl, sizeof(uint32_t)) < 0)
	{
		ft_dprintf(2, "setsockopt: %s%n", strerror(errno));
		exit(1);
	}
	print_status(&ping);
	send_preload(&ping, args->preload);
	ping_recvloop(&ping);
}

int				main(int argc, char **argv)
{
	t_raw_socket	*sock;
	t_ping_args		ping_args;

	if (!parse_argv(argc, argv, &ping_args))
		return (2);
	if ((sock = raw_socket_create(ping_args.host, ping_args.ai_family)) == NULL)
		return (1);
	ping(sock, &ping_args);
}
