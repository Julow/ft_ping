/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp_echo.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/24 17:07:58 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/24 17:08:12 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "p_main.h"

bool			icmp_echo_send(t_raw_socket *sock, t_icmp_echo_data data,
					t_sub payload)
{
	t_icmp_header	header;

	header = (t_icmp_header){8, 0, 0, data.data};
	return (icmp_send(sock, &header, payload));
}

uint32_t		icmp_echo_recv(t_raw_socket *sock, t_ip_info *ip_info,
					t_icmp_echo_data *echo_data, void *buff, uint32_t buff_len)
{
	uint32_t		len;
	t_icmp_header	icmp_header;

	while (true)
	{
		len = icmp_recv(sock, ip_info, &icmp_header, buff, buff_len);
		if (len == 0)
			return (0);
		if (icmp_header.type != 0 || icmp_header.code != 0)
			continue ;
		echo_data->data = icmp_header.data;
		return (len);
	}
}
