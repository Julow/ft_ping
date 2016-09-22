/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   net_checksum.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/22 17:46:41 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/22 17:47:05 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "net/utils.h"

uint16_t		net_checksum(void const *data, uint32_t length)
{
	uint32_t const	rounded_len = length & ~1;
	uint32_t		i;
	uint32_t		sum;
	uint32_t		tmp;

	i = 0;
	sum = 0;
	while (i < rounded_len)
	{
		sum += *(uint16_t const*)(data + i);
		i += 2;
	}
	if (i < length)
		sum += ((uint32_t)*(uint8_t const*)(data + i)) << 8;
	while ((tmp = sum >> 16) != 0)
		sum = (sum & 0xFFFF) + tmp;
	return (~sum);
}
