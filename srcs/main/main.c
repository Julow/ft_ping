/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/18 15:41:58 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/19 17:53:27 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/ft_printf.h"
#include "ft/libft.h"

#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#define ADDR_DST_LENGTH		MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)

static int		try_connect(struct addrinfo const *info, char *addr_dst)
{
	int				fd;
	void const		*addr;

	while (info != NULL)
	{
		fd = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
		if (fd >= 0)
		{
			addr = (info->ai_family == AF_INET)
				? (void const*)&((struct sockaddr_in const*)info->ai_addr)->sin_addr
				: (void const*)&((struct sockaddr_in6 const*)info->ai_addr)->sin6_addr;
			if (inet_ntop(info->ai_family, addr, addr_dst, ADDR_DST_LENGTH) == NULL)
				*addr_dst = '\0';
			if (connect(fd, info->ai_addr, info->ai_addrlen) >= 0)
				return (fd);
			ft_dprintf(2, "connect: %s%n", strerror(errno));
			close(fd);

		}
		info = info->ai_next;
	}
	ft_dprintf(2, "socket: Can't create socket%n");
	return (-1);
}

int				connect_raw(bool ipv6, char const *addr, char *addr_dst)
{
	struct protoent		*proto;
	struct addrinfo		hints;
	struct addrinfo		*res;
	int					tmp;

	if ((proto = getprotobyname("icmp")) == NULL)
	{
		ASSERT(!"getprotobyname fail");
		return (-1);
	}
	hints = (struct addrinfo){
		.ai_family = ipv6 ? AF_INET6 : AF_INET,
		.ai_socktype = SOCK_RAW,
		.ai_protocol = proto->p_proto,
	};
	if ((tmp = getaddrinfo(addr, NULL, &hints, &res)) != 0)
	{
		ft_dprintf(2, "getaddrinfo: %s%n", gai_strerror(tmp));
		return (-1);
	}
	tmp = try_connect(res, addr_dst);
	freeaddrinfo(res);
	return (tmp);
}

#include <stdio.h>

int				main(int argc, char **argv)
{
	char			addr_buff[ADDR_DST_LENGTH];
	int				sock_fd;

	if (argc <= 1)
		return (1);
	if ((sock_fd = connect_raw(false, argv[1], addr_buff)) < 0)
		return (1);
	ft_printf("Connected to %s (%s)%n", argv[1], addr_buff);
	close(sock_fd);
	return (0);
}
