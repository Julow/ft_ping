/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/18 15:41:58 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/24 19:18:44 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/ft_printf.h"
#include "ft/libft.h"
#include "net/icmp.h"

#include "p_main.h"

#include <errno.h>
#include <string.h>
#include <unistd.h>

static void		print_status(t_ping const *ping)
{
	char			addr_buff[RAW_SOCKET_ADDR_LEN];

	raw_socket_addr(ping->sock, addr_buff);
	ft_printf("PING %s (%s): %u data bytes%n",
			ping->host_name, addr_buff, ping->payload_size);
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

static bool		ping(t_raw_socket *sock, t_ping_args const *args)
{
	t_ping			ping;

	ping = (t_ping){
		.sock = sock,
		.host_name = args->host,
		.wait_time = args->wait,
		.flags = args->flags,
		.echo_id = getpid(),
		.echo_seq = 0,
		.to_send = (args->count == 0) ? (uint32_t)-1 : args->count,
		.payload_pattern = args->payload_pattern,
		.payload_size = args->payload_size,
	};
	if (args->ttl > 0 && setsockopt(sock->fd, IPPROTO_IP, IP_TTL,
				&args->ttl, sizeof(uint32_t)) < 0)
	{
		ft_dprintf(2, "setsockopt: %s%n", strerror(errno));
		return (false);
	}
	print_status(&ping);
	if (!send_preload(&ping, args->preload))
		return (false);
	ping_recvloop(&ping);
	return (true);
}

int				main(int argc, char **argv)
{
	t_raw_socket	*sock;
	t_ping_args		ping_args;
	bool			r;

	if (!parse_argv(argc, argv, &ping_args))
		return (2);
	if ((sock = raw_socket_create(ping_args.host, ping_args.ai_family)) == NULL)
		return (1);
	r = ping(sock, &ping_args);
	raw_socket_destroy(sock);
	return (r ? 0 : 1);
}
