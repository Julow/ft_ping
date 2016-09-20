/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raw_socket.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/20 12:23:55 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/20 14:50:24 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "raw_socket.h"

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static t_raw_socket	*try_connect(struct addrinfo const *info, uint32_t flags)
{
	int				fd;
	t_raw_socket	*sock;
	int				err;

	err = 0;
	while (true)
	{
		fd = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
		if (err != 0)
			err = errno;
		if (fd < 0)
		{
			if ((info = info->ai_next) == NULL)
				break ;
			continue ;
		}
		sock = MALLOC(sizeof(t_raw_socket) + info->ai_addrlen);
		*sock = (t_raw_socket){fd, flags, ENDOF(sock)};
		ft_memcpy(ENDOF(sock), info->ai_addr, info->ai_addrlen);
		return (sock);
	}
	ASSERT(!"socket fail");
	// ft_dprintf(2, "socket: %s%n", strerror(err));
	return (NULL);
}

t_raw_socket		*raw_socket_create(char const *host, uint32_t flags)
{
	struct protoent		*proto;
	struct addrinfo		hints;
	struct addrinfo		*res;
	int					tmp;
	t_raw_socket		*sock;

	if ((proto = getprotobyname("icmp")) == NULL)
	{
		ASSERT(!"getprotobyname fail");
		return (NULL);
	}
	hints = (struct addrinfo){
		.ai_family = (flags & RAW_SOCKET_F_IPV6) ? AF_INET6 : AF_INET,
		.ai_socktype = SOCK_RAW,
		.ai_protocol = proto->p_proto, // ? IPPROTO_ICMP
	};
	if ((tmp = getaddrinfo(host, NULL, &hints, &res)) != 0)
	{
		// ft_dprintf(2, "getaddrinfo: %s%n", gai_strerror(tmp));
		ASSERT(!"getaddrinfo fail");
		return (NULL);
	}
	sock = try_connect(res, flags);
	freeaddrinfo(res);
	return (sock);
}

void			raw_socket_addr(t_raw_socket const *s, char *dst)
{
	void const		*addr;

	addr = (s->flags & RAW_SOCKET_F_IPV6)
		? (void const*)&((struct sockaddr_in const*)s->addr)->sin_addr
		: (void const*)&((struct sockaddr_in6 const*)s->addr)->sin6_addr;
	if (inet_ntop(((s->flags & RAW_SOCKET_F_IPV6) ? AF_INET6 : AF_INET),
				addr, dst, RAW_SOCKET_ADDR_LEN) == NULL)
		*dst = '\0';
}

void			raw_socket_destroy(t_raw_socket *s)
{
	close(s->fd);
	free(s);
}
