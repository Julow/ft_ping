/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raw_socket.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/20 12:23:55 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/22 10:56:13 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "raw_socket.h"

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static t_raw_socket	*try_connect(struct addrinfo const *info)
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
		*sock = (t_raw_socket){fd, 0, ENDOF(sock), info->ai_addrlen};
		if (info->ai_family == AF_INET6)
			sock->flags |= RAW_SOCKET_F_IPV6;
		ft_memcpy(ENDOF(sock), info->ai_addr, info->ai_addrlen);
		return (sock);
	}
	ft_dprintf(2, "socket: %s%n", strerror(err));
	return (NULL);
}

t_raw_socket		*raw_socket_create(char const *host, int ai_family)
{
	struct addrinfo		hints;
	struct addrinfo		*res;
	int					tmp;
	t_raw_socket		*sock;

	hints = (struct addrinfo){
		.ai_family = ai_family,
		.ai_socktype = SOCK_RAW,
		.ai_protocol = IPPROTO_ICMP,
	};
	if ((tmp = getaddrinfo(host, NULL, &hints, &res)) != 0)
	{
		ft_dprintf(2, "getaddrinfo: %s%n", gai_strerror(tmp));
		return (NULL);
	}
	sock = try_connect(res);
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
