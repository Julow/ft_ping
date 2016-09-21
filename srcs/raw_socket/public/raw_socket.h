/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raw_socket.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/20 12:15:40 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/21 12:45:11 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RAW_SOCKET_H
# define RAW_SOCKET_H

# include "ft/libft.h"

# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/types.h>

typedef struct s_raw_socket			t_raw_socket;

/*
** ========================================================================== **
** Raw socket
*/

struct			s_raw_socket
{
	uint32_t		fd;
	uint32_t		flags;
	struct sockaddr	*addr;
};

/*
** Flags
** RAW_SOCKET_IPV6		=> Use ipv6
*/
# define RAW_SOCKET_F_IPV6		(1 << 0)

/*
** Create a raw socket to 'host'
** ai_family can be AF_INET, AF_INET6 or AF_UNSPEC
** Return NULL on error
*/
t_raw_socket	*raw_socket_create(char const *host, int ai_family);

/*
** Write the address of 's' in human readable format into 'dst'
** 'dst' should be at least of size RAW_SOCKET_ADDR_LEN
*/
void			raw_socket_addr(t_raw_socket const *s, char *dst);

/*
** Destroy (and free) 's'
*/
void			raw_socket_destroy(t_raw_socket *s);

/*
** -
*/
# define RAW_SOCKET_ADDR_LEN	MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)

#endif
