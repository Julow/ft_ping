/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/22 17:42:48 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/22 17:47:09 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NETWORK_UTILS_H
# define NETWORK_UTILS_H

# include "ft/libft.h"

/*
** ========================================================================== **
*/

/*
** 1's complement internet checksum
** 'length' is in byte
*/
uint16_t		net_checksum(void const *data, uint32_t length);

#endif
