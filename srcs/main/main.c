/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/18 15:41:58 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/22 19:09:13 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/ft_printf.h"
#include "ft/libft.h"
#include "net/icmp.h"

#include "p_main.h"

#include <errno.h>

/*
** ========================================================================== **
** ICMP Proto
*/

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
	t_icmp_header	header;

	header = (t_icmp_header){8, 0, 0, (id << 16) | seq};
	return (icmp_send(sock, &header, payload));
}

/*
** ========================================================================== **
*/

static void		test(t_raw_socket *sock)
{
	uint32_t		len;
	char			buff[512];
	t_ip_info		ip_info;
	t_icmp_header	icmp_header;

	icmp_echo_send(sock, 0, 0, SUBC("OKOKOKOKOK"));

	while (true)
	{
		len = icmp_recv(sock, &ip_info, &icmp_header, buff, sizeof(buff));
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
