/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   p_icmp.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/22 17:43:28 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/28 11:24:19 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef P_ICMP_H
# define P_ICMP_H

# include "net/icmp.h"

typedef union u_ip_header			t_ip_header;
typedef struct s_ipv4_header		t_ipv4_header;
typedef struct s_ipv6_header		t_ipv6_header;

/*
** ========================================================================== **
** TODO: move it to net::utils
*/

struct			s_ipv4_header
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
	struct in_addr	src_addr;
	struct in_addr	dst_addr;
};

struct			s_ipv6_header
{
	uint8_t			version:4;
	uint8_t			traffic_class1:4;
	uint8_t			traffic_class2:4;
	uint32_t		flow_label:20;
	uint16_t		payload_length;
	uint8_t			next_header;
	uint8_t			hop_limit;
	struct in6_addr	src_addr;
	struct in6_addr	dst_addr;
};

union			u_ip_header
{
	t_ipv4_header	v4;
	t_ipv6_header	v6;
};

#endif
