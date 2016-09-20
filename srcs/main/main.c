/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/18 15:41:58 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/20 19:41:07 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/argv.h"
#include "ft/ft_printf.h"
#include "ft/libft.h"

#include "raw_socket.h"

#include <stddef.h>

typedef struct s_ping_args		t_ping_args;

/*
** raw_sock_flags	=> raw_socket_create flags
** count			=> number of packet to send (0 means no limit)
** host				=> host
*/
struct			s_ping_args
{
	uint32_t		raw_sock_flags;
	uint32_t		count;
	char const		*host;
};

static struct s_argv_opt const	g_ping_opt[] = {
	ARGV_OPT_FLAG("6", RAW_SOCKET_F_IPV6, offsetof(t_ping_args, raw_sock_flags)),
	ARGV_OPT_VALUE("c", P_UINT, offsetof(t_ping_args, count)),
	ARGV_OPT_ALIAS("count", "c"),
};

bool			parse_argv(t_argv argv, t_ping_args *dst)
{
	t_argv_opt_err	err;
	t_sub			tmp;

	ft_bzero(dst, sizeof(t_ping_args));
	if ((err = ft_argv_argv(&argv, g_ping_opt,
				ARRAY_LEN(g_ping_opt), dst)) != ARGV_OPT_OK)
	{
		ft_argv_arg(&argv, &tmp);
		ft_dprintf(2, "ping: %ts: %ts%n", tmp, g_argv_opt_strerr[err]);
		return (false);
	}
	if (!ft_argv_arg(&argv, &tmp))
	{
		ft_dprintf(2, "ping: Missing host argument%n");
		return (false);
	}
	dst->host = tmp.str;
	if (ft_argv_arg(&argv, &tmp))
	{
		ft_dprintf(2, "ping: %ts: Unexpected argument%n", tmp);
	}
	ft_printf("raw_sock_flags: %b ; count: %u ; host: '%s'%n",
			dst->raw_sock_flags, dst->count, dst->host);
	return (false);
}

int				main(int argc, char **argv)
{
	char			addr_buff[RAW_SOCKET_ADDR_LEN];
	t_raw_socket	*sock;
	t_ping_args		ping_args;

	if (!parse_argv(ARGV(argc, argv), &ping_args))
		return (1);
	if ((sock = raw_socket_create(argv[1], 0)) == NULL)
		return (1);
	raw_socket_addr(sock, addr_buff);
	ft_printf("Connected to %s (%s)%n", argv[1], addr_buff);
	raw_socket_destroy(sock);
	return (0);
}
