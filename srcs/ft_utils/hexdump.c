/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hexdump.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/24 17:03:19 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/27 18:00:36 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/ft_printf.h"
#include "ft/utils.h"

static void		hexdump_hex(uint8_t const *data, uint32_t size)
{
	uint32_t		i;

	i = 0;
	ft_printf("%0.2x", data[i++]);
	while (i < size)
		ft_printf(" %0.2x", data[i++]);
}

static void		hexdump_ascii(uint8_t const *data, uint32_t size)
{
	uint32_t		i;

	i = 0;
	while (i < size)
	{
		ft_printf("%c", IS(data[i], IS_PRINT) ? data[i] : '.');
		i++;
	}
}

static void		hexdump_group(t_vec2u layout, uint8_t const *data,
					uint32_t i, uint32_t size)
{
	if (size >= layout.y + i)
	{
		hexdump_hex(data + i, layout.y);
	}
	else if (size > i)
	{
		hexdump_hex(data + i, size - i);
		ft_printf("%.*c", (layout.y - (size - i)) * 3, ' ');
	}
	else
	{
		ft_printf("%.*c", layout.y * 3 - 1, ' ');
	}
}

void			ft_hexdump(void const *data, uint32_t size, t_vec2u layout)
{
	uint32_t		i;
	uint32_t		group_i;

	i = 0;
	while (i < size)
	{
		ft_printf("%0.5x  ", i);
		group_i = 0;
		while (group_i < layout.x)
		{
			hexdump_group(layout, data, layout.y * group_i + i, size);
			group_i++;
			ft_printf("  ");
		}
		ft_printf("|");
		hexdump_ascii(data + i, MIN(size - i, layout.x * layout.y));
		ft_printf("|%n");
		i += layout.x * layout.y;
	}
	ft_printf("%0.5x%n", size);
}
