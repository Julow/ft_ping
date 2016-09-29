/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/27 17:59:22 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/29 16:36:03 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
# define UTILS_H

# include "ft/libft.h"

/*
** ========================================================================== **
** Hexdump
*/

# define HEXDUMP_DEFAULT	VEC2U(2, 8)

/*
** Show 'size' bytes of 'data'
** 'layout.x' is the number of byte group and 'layout.y' is the size of a group
*/
void			ft_hexdump(void const *data, uint32_t size, t_vec2u layout);

#endif
