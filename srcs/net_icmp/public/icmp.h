/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/22 17:39:13 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/22 17:48:28 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NET__ICMP_H
# define NET__ICMP_H

# include "ft/libft.h"
# include "raw_socket.h"

typedef struct s_ip_header			t_ip_header;
typedef struct s_icmp_header		t_icmp_header;

/*
** ========================================================================== **
** ICMP Proto
*/

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

/*
** Send an ICMP packet
** 'type', 'code' and 'header_data' are the corresponding fields in the icmp header
*/
bool			icmp_send(t_raw_socket *sock, uint8_t type, uint8_t code,
					uint32_t header_data, t_sub payload);

/*
** Wait for an incoming packet
** 'dst' is filled with header data
** 'buff' is filled with payload data (max 'buff_size')
** Return the number of byte put into 'buff'
*/
uint32_t		icmp_recv(t_raw_socket *sock, t_ip_header *ip_header,
					t_icmp_header *icmp_header, void *buff, uint32_t buff_size);

#endif
