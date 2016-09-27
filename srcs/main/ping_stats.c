/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_stats.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/27 18:58:37 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/27 19:31:27 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/ft_printf.h"
#include "p_main.h"

#define USEC_TO_F(USEC, BASE)	(((float)(USEC)) / (BASE))

void			ping_show_stats(t_ping const *ping)
{
	ft_printf("--- %s ping statistics ---\n", ping->host_name);
	ft_printf("%u packets transmitted, %u packets received, %u%% packet loss%n",
			ping->total_sent, ping->total_received,
			100 - ((ping->total_received == 0) ? 0 :
							ping->total_sent * 100 / ping->total_received));
	if (ping->total_received > 0)
		ft_printf("round-trip min/avg/max = %0.3f/%0.3f/%0.3f ms%n",
				USEC_TO_F(ping->min_time, T_MSEC),
				USEC_TO_F(ping->total_time / ping->total_received, T_MSEC),
				USEC_TO_F(ping->max_time, T_MSEC));
}
