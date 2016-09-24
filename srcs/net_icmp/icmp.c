/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/22 17:40:51 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/24 14:20:14 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "net/icmp.h"
#include "net/utils.h"

#include "p_icmp.h"

#include <errno.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/uio.h>

bool			icmp_send(t_raw_socket *sock, t_icmp_header const *header,
					t_sub payload)
{
	char					msg[sizeof(t_icmp_header) + payload.length];
	t_icmp_header *const	dst_header = (t_icmp_header*)msg;

	ASSERT(!(sock->flags & RAW_SOCKET_F_IPV6), "icmp send do not support ipv6 yet");

	*dst_header = *header;
	dst_header->checksum = 0;
	ft_memcpy(ENDOF(dst_header), payload.str, payload.length);
	dst_header->checksum = net_checksum(msg, sizeof(msg));

	ASSERT(net_checksum(msg, sizeof(msg)) == 0);
	// print_icmp_packet(dst_header, payload);
	if (sendto(sock->fd, msg, sizeof(msg), 0, sock->addr, sock->addr_len) < 0)
	{
		ft_dprintf(2, "sendto: %s%n", strerror(errno));
		return (false);
	}
	return (true);
}

/*
** Load ip_info from a given ip header
** Return the size of the header
*/
static void		unpack_ip_info(t_ip_header const *header, t_ip_info *dst)
{
	if (header->v4.version == 6)
	{
		*dst = (t_ip_info){
			.version = header->v6.version,
			.max_hop = header->v6.hop_limit,
			.protocol = header->v6.next_header,
			.size = header->v6.payload_length + sizeof(t_ipv6_header),
			.src_addr.v6 = header->v6.src_addr,
		};
	}
	else
	{
		*dst = (t_ip_info){
			.version = header->v4.version,
			.max_hop = header->v4.ttl,
			.protocol = header->v4.protocol,
			.size = header->v4.length,
			.src_addr.v4 = header->v4.src_addr,
		};
	}
}

uint32_t		icmp_recv(t_raw_socket *sock, t_ip_info *ip_info,
					t_icmp_header *icmp_header, void *buff, uint32_t buff_size)
{
	t_ip_header		ip_header;
	struct iovec	iovec[3];
	struct msghdr	msg;
	ssize_t			len;

	iovec[0] = (sock->flags & RAW_SOCKET_F_IPV6) ?
			(struct iovec){&ip_header.v6, sizeof(t_ipv6_header)} :
			(struct iovec){&ip_header.v4, sizeof(t_ipv4_header)};
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
	if ((len = recvmsg(sock->fd, &msg, 0)) < 0)
	{
		ft_dprintf(2, "recvmsg: %s%n", strerror(errno));
		return (0);
	}
	if ((len -= (iovec[0].iov_len + iovec[1].iov_len)) < 0)
	{
		TRACE("NEGATIVE LENGTH PACKET LOL");
		return (0);
	}
	unpack_ip_info(&ip_header, ip_info);
	return (len);
	STATIC_ASSERT(sizeof(t_ipv4_header) == 20);
	STATIC_ASSERT(sizeof(t_ipv6_header) == 40);
	STATIC_ASSERT(sizeof(t_icmp_header) == 8);
}
