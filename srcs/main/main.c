/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/18 15:41:58 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/24 17:11:38 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/ft_printf.h"
#include "ft/libft.h"
#include "net/icmp.h"

#include "p_main.h"

#include <errno.h>

/*
** ========================================================================== **
*/

#include <unistd.h>

static void		print_status(t_ping const *ping)
{
	char			addr_buff[RAW_SOCKET_ADDR_LEN];

	raw_socket_addr(ping->sock, addr_buff);
	ft_printf("PING %s (%s): %u data bytes%n",
			ping->host_name, addr_buff, ping->payload.length);
}

static void		ping(t_raw_socket *sock, t_ping_args const *args)
{
	t_sub const		payload = SUBC("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuv");
	t_ping			ping;

	ping = (t_ping){
		.sock = sock,
		.host_name = args->host,
		.flags = args->flags,
		.echo_id = getpid(),
		.echo_seq = 0,
		.to_send = (args->count == 0) ? (uint32_t)-1 : args->count,
		.payload = payload,
	};
	print_status(&ping);
	ping_send(&ping);
	ping_recvloop(&ping);
}

int				main(int argc, char **argv)
{
	t_raw_socket	*sock;
	t_ping_args		ping_args;

	if (!parse_argv(argc, argv, &ping_args))
		return (1);
	if ((sock = raw_socket_create(ping_args.host, ping_args.ai_family)) == NULL)
		return (1);
	ping(sock, &ping_args);
	raw_socket_destroy(sock);
	return (0);
}
