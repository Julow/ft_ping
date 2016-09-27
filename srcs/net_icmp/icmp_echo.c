/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp_echo.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/27 17:56:55 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/27 17:57:19 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "net/icmp_echo.h"

bool			icmp_echo_send(t_raw_socket *sock, t_icmp_echo_data data,
					t_sub payload)
{
	t_icmp_header	header;

	header = (t_icmp_header){8, 0, 0, data.data};
	return (icmp_send(sock, &header, payload));
}

bool			icmp_is_echo_reply(t_icmp_header const *header,
					t_icmp_echo_data *echo_data)
{
	if (header->type != 0 || header->code != 0)
		return (false);
	echo_data->data = header->data;
	return (true);
}
