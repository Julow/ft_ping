/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_handlers.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/27 19:26:43 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/28 14:27:33 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "p_main.h"

#include <signal.h>
#include <stdlib.h>

static t_ping	*g_ping_singleton = NULL;

static void		handle_sigint(int sig)
{
	ping_exit(g_ping_singleton);
	(void)sig;
}

static void		handle_sigalrm(int sig)
{
	ping_send(g_ping_singleton);
	(void)sig;
}

static void		handle_siginfo(int sig)
{
	ping_print_stats(g_ping_singleton);
	(void)sig;
}

void			ping_handle_signals(t_ping *ping)
{
	g_ping_singleton = ping;
	signal(SIGINT, &handle_sigint);
#ifdef SIGINFO
	signal(SIGINFO, &handle_siginfo);
#endif
	signal(SIGQUIT, &handle_siginfo);
	signal(SIGALRM, &handle_sigalrm);
}
