/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/22 17:42:48 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/28 11:37:53 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NETWORK_UTILS_H
# define NETWORK_UTILS_H

# include "ft/libft.h"
# include <arpa/inet.h>

typedef struct s_ip_info			t_ip_info;

/*
** ========================================================================== **
** Ip info
*/

/*
** Some info that come with the IP header
** version		=> ip version
** max_hop		=> hop limit (ttl)
** protocol		=> protocol id
** size			=> total packet size
*/
struct			s_ip_info
{
	uint8_t			version;
	uint8_t			max_hop;
	uint8_t			protocol;
	uint16_t		size;
	union {
		struct in_addr	v4;
		struct in6_addr	v6;
	}				src_addr;
};

/*
** Build a struct sockaddr from ip_info
** Return the length of the addr
*/
uint32_t		sockaddr_of_ipinfo(struct sockaddr_storage *dst,
					t_ip_info const *ip_info);
/*
** ========================================================================== **
** Internet checksum
*/

/*
** 1's complement internet checksum
** 'length' is in byte
*/
uint16_t		net_checksum(void const *data, uint32_t length);

#endif
