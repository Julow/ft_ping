/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/22 17:40:51 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/22 17:52:15 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "net/icmp.h"
#include "net/utils.h"

#include "p_icmp.h"

#include <errno.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/uio.h>

bool			icmp_send(t_raw_socket *sock, uint8_t type, uint8_t code,
					uint32_t header_data, t_sub payload)
{
	STATIC_ASSERT(sizeof(t_ip_header) == 20);
	STATIC_ASSERT(sizeof(t_icmp_header) == 8);

	ASSERT(!(sock->flags & RAW_SOCKET_F_IPV6), "icmp_send: ipv6 not supported yet");

	char					msg[sizeof(t_icmp_header) + payload.length];
	t_icmp_header *const	header = (t_icmp_header*)msg;

	*header = (t_icmp_header){type, code, 0, header_data};
	ft_memcpy(ENDOF(header), payload.str, payload.length);
	header->checksum = net_checksum(msg, sizeof(msg));

	ASSERT(net_checksum(msg, sizeof(msg)) == 0);
	// print_icmp_packet(header, payload);
	if (sendto(sock->fd, msg, sizeof(msg), 0, sock->addr, sock->addr_len) < 0)
	{
		ft_dprintf(2, "sendto: %s%n", strerror(errno));
		return (false);
	}
	return (true);
}

uint32_t		icmp_recv(t_raw_socket *sock, t_ip_header *ip_header,
					t_icmp_header *icmp_header, void *buff, uint32_t buff_size)
{
	ASSERT(!(sock->flags & RAW_SOCKET_F_IPV6), "icmp_recv: ipv6 not supported yet");

	struct iovec	iovec[3];
	struct msghdr	msg;
	ssize_t			len;

	iovec[0] = (struct iovec){ip_header, sizeof(t_ip_header)};
	iovec[1] = (struct iovec){icmp_header, sizeof(t_icmp_header)};
	iovec[2] = (struct iovec){buff, buff_size};
	msg = (struct msghdr){
		.msg_name = sock->addr,
		.msg_namelen = sock->addr_len,
		.msg_iov = iovec,
		.msg_iovlen = ARRAY_LEN(iovec),
		.msg_control = NULL,
		.msg_controllen = 0,
		.msg_flags = 0,
	};
	if ((len = recvmsg(sock->fd, &msg, 0)) < 0
		|| len < (sizeof(t_ip_header) + sizeof(t_icmp_header)))
	{
		ft_dprintf(2, "recvmsg: %s%n", strerror(errno));
		return (0);
	}
	len -= sizeof(t_ip_header) + sizeof(t_icmp_header);

	// ft_printf("CHECK CHECKSUM: %u%n",
			// (uint16_t)~(uint16_t)(net_checksum(icmp_header, sizeof(t_icmp_header))
			// + net_checksum(buff, len)));
	return (len);
}

