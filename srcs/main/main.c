/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/18 15:41:58 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/21 19:14:45 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/ft_printf.h"
#include "ft/libft.h"

#include "p_main.h"
#include "raw_socket.h"

typedef struct s_icmp_header		t_icmp_header;

struct			s_icmp_header
{
	uint8_t			type;
	uint8_t			code;
	uint16_t		checksum;
	uint32_t		data;
};

/*
** 1's complement internet checksum
** 'length' is in byte
*/
static short	ip_checksum(void const *data, uint32_t length)
{
	uint32_t		i;
	uint32_t		sum;
	uint32_t		tmp;

	i = 0;
	sum = 0;
	while (i < (length & ~1))
	{
		sum += *(short const*)(data + i);
		i += 2;
	}
	if (i < length)
		sum += ((uint32_t)*(uint8_t const*)(data + i)) << 8;
	while ((tmp = sum & 0xFFFF0000) != 0)
		sum = (sum & 0xFFFF) + (tmp >> 16);
	return (~sum);
}

/*
** Send an ICMP packet
** 'type', 'code' and 'header_data' are the corresponding fields in the icmp header
*/
static void		icmp_send(t_raw_socket *sock,
					uint8_t type, uint8_t code,
					uint32_t header_data,
					t_sub payload)
{
	char					msg[sizeof(t_icmp_header) + payload.length];
	t_icmp_header *const	header = (t_icmp_header*)msg;

	*(t_icmp_header*)msg = (t_icmp_header){type, code, 0, header_data};
	ft_memcpy(msg + sizeof(t_icmp_header), payload.str, payload.length);
	((t_icmp_header*)msg)->checksum = ip_checksum(msg, sizeof(msg));

}

// static uint32_t	icmp_recv(t_raw_socket *sock, )

static void		test(t_raw_socket *sock)
{
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
