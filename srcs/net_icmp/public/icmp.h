/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/22 17:39:13 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/28 11:22:09 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NET__ICMP_H
# define NET__ICMP_H

# include "ft/libft.h"
# include "net/utils.h"

# include "raw_socket.h"

typedef struct s_icmp_header		t_icmp_header;

/*
** ========================================================================== **
** ICMP Proto
*/

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

# define ICMP_MAX_SIZE		65507

/*
** Send an ICMP packet
** The 'checksum' field of 'header' is ignored
*/
bool			icmp_send(t_raw_socket *sock, t_icmp_header const *header,
					t_sub payload);

/*
** Wait for an incoming packet
** 'ip_info', 'icmp_header' and 'buff' are filled accordingly
** 'buff_size' is the maximum size of 'buff'
** Return the number of byte put into 'buff'
*/
uint32_t		icmp_recv(t_raw_socket *sock, t_ip_info *ip_info,
					t_icmp_header *icmp_header, void *buff, uint32_t buff_size);

#endif
