/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_timeout.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/24 17:05:08 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/27 18:00:26 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/utils.h"

#include <signal.h>
#include <unistd.h>

static t_callback	g_set_timeout_callback = CALLBACK(NULL, NULL);

static void		set_timeout_handler(int sig)
{
	CALL(void, g_set_timeout_callback);
	(void)sig;
}

void			set_timeout(void (*f)(), void *data, uint32_t t)
{
	g_set_timeout_callback = CALLBACK(f, data);
	signal(SIGALRM, &set_timeout_handler);
	alarm(t);
}
