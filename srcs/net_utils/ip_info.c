/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ip_info.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/28 11:21:01 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/28 11:41:32 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "net/utils.h"

static uint32_t	sockaddr_of_ipinfo4(struct sockaddr_in *dst,
					t_ip_info const *info)
{
	ft_bzero(dst, sizeof(struct sockaddr_in));
	dst->sin_family = AF_INET;
	dst->sin_addr = info->src_addr.v4;
	return (sizeof(struct sockaddr_in));
}

static uint32_t	sockaddr_of_ipinfo6(struct sockaddr_in6 *dst,
					t_ip_info const *info)
{
	ft_bzero(dst, sizeof(struct sockaddr_in6));
	dst->sin6_family = AF_INET6;
	dst->sin6_addr = info->src_addr.v6;
	return (sizeof(struct sockaddr_in6));
}

uint32_t		sockaddr_of_ipinfo(struct sockaddr_storage *dst,
					t_ip_info const *ip_info)
{
	return ((ip_info->version == 6)
			? sockaddr_of_ipinfo6(V(dst), ip_info)
			: sockaddr_of_ipinfo4(V(dst), ip_info));
}
