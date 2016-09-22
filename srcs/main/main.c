/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/18 15:41:58 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/22 16:29:14 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/ft_printf.h"
#include "ft/libft.h"

#include "p_main.h"
#include "raw_socket.h"

#include <errno.h>

/*
** ========================================================================== **
** ICMP Proto
*/

typedef struct s_ip_header			t_ip_header;
typedef struct s_icmp_header		t_icmp_header;

/*
** IP header
*/
struct			s_ip_header
{
	uint8_t			version:4;
	uint8_t			ihl:4;
	uint8_t			dscp:6;
	uint8_t			ecn:2;
	uint16_t		length;
	uint16_t		id;
	uint16_t		flags:3;
	uint16_t		offset:13;
	uint8_t			ttl;
	uint8_t			protocol;
	uint16_t		checksum;
	uint32_t		src_addr;
	uint32_t		dst_addr;
};

/*
** ICMP header
*/
struct			s_icmp_header
{
	uint8_t			type;
	uint8_t			code;
	uint16_t		checksum;
	uint32_t		data;
};

static void		print_icmp_packet(t_icmp_header const *header, t_sub payload)
{
	uint32_t		i;
	uint32_t		tmp;

	ft_printf("| %2u | %2u | %^6u |\n",
			header->type, header->code, header->checksum);
	ft_printf("| %^6u | %^6u |\n",
			(header->data & 0xFFFF0000) >> 16, header->data & 0xFFFF);
	i = 0;
	while (i < payload.length)
	{
		ft_printf("| ");
		tmp = i + 4;
		while (i < MIN(tmp, payload.length))
		{
			if (IS(payload.str[i], IS_PRINT))
				ft_printf(" \\%c ", (uint8_t)payload.str[i]);
			else
				ft_printf(" %.2x ", (uint8_t)payload.str[i]);
			i++;
		}
		while (i < tmp)
		{
			ft_printf("    ");
			i++;
		}
		ft_printf("  |%n");
	}
}

/*
** 1's complement internet checksum
** 'length' is in byte
*/
static uint16_t	ip_checksum(void const *data, uint32_t length)
{
	uint32_t const	rounded_len = length & ~1;
	uint32_t		i;
	uint32_t		sum;
	uint32_t		tmp;

	i = 0;
	sum = 0;
	while (i < rounded_len)
	{
		sum += *(uint16_t const*)(data + i);
		i += 2;
	}
	if (i < length)
		sum += ((uint32_t)*(uint8_t const*)(data + i)) << 8;
	while ((tmp = sum >> 16) != 0)
		sum = (sum & 0xFFFF) + tmp;
	return (~sum);
}

/*
** Send an ICMP packet
** 'type', 'code' and 'header_data' are the corresponding fields in the icmp header
*/
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
	header->checksum = ip_checksum(msg, sizeof(msg));

	ASSERT(ip_checksum(msg, sizeof(msg)) == 0);
	print_icmp_packet(header, payload);
	if (sendto(sock->fd, msg, sizeof(msg), 0, sock->addr, sock->addr_len) < 0)
	{
		ft_dprintf(2, "sendto: %s%n", strerror(errno));
		return (false);
	}
	return (true);
}

#include <unistd.h>

#include <sys/types.h>
#include <sys/uio.h>

/*
** Wait for an incoming packet
** 'dst' is filled with header data
** 'buff' is filled with payload data (max 'buff_size')
** Return the number of byte put into 'buff'
*/
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
			// (uint16_t)~(uint16_t)(ip_checksum(icmp_header, sizeof(t_icmp_header))
			// + ip_checksum(buff, len)));
	return (len);
}

/*
** ========================================================================== **
** ICMP Echo Proto
*/

/*
** Wrapper for icmp_send that send echo request
** 'id' and 'seq' are "Identifier" and "Sequence number" header fields
*/
bool			icmp_echo_send(t_raw_socket *sock, uint16_t id, uint16_t seq,
					t_sub payload)
{
	return (icmp_send(sock, 8, 0, (id << 16) | seq, payload));
}

/*
** ========================================================================== **
*/

static void		test(t_raw_socket *sock)
{
	uint32_t		len;
	char			buff[512];
	t_ip_header		ip_header;
	t_icmp_header	icmp_header;

	icmp_echo_send(sock, 0, 0, SUBC("OKOKOKOKOK"));

	while (true)
	{
		len = icmp_recv(sock, &ip_header, &icmp_header, buff, sizeof(buff));
		ft_printf("RECV %u bytes\n", len);
		print_icmp_packet(&icmp_header, SUB(buff, len));
	}

}

int				main(int argc, char **argv)
{
	char			addr_buff[RAW_SOCKET_ADDR_LEN];
	t_raw_socket	*sock;
	t_ping_args		ping_args;

	if (!parse_argv(argc, argv, &ping_args))
		return (1);
	ft_printf("ai_family: %d ; count: %u ; host: '%s'%n",
			ping_args.ai_family, ping_args.count, ping_args.host);
	if ((sock = raw_socket_create(ping_args.host, ping_args.ai_family)) == NULL)
		return (1);
	raw_socket_addr(sock, addr_buff);
	ft_printf("Connected to %s (%s)%n", ping_args.host, addr_buff);

	test(sock);

	raw_socket_destroy(sock);
	return (0);
}
