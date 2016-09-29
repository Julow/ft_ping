/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_handlers.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/27 19:26:43 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/29 17:58:09 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "p_main.h"

#include <signal.h>
#include <stdlib.h>

static t_ping	*g_ping_singleton = NULL;

static void		handle_sigint(int sig)
{
	ping_print_stats(g_ping_singleton);
	exit((g_ping_singleton->stats.total_received == 0) ? 1 : 0);
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

#ifdef SIGINFO
# define HANDLE_SIGINFO		true
#else
# define HANDLE_SIGINFO		false
# define SIGINFO			0
#endif

void			ping_handle_signals(t_ping *ping)
{
	g_ping_singleton = ping;
	signal(SIGINT, &handle_sigint);
	if (HANDLE_SIGINFO)
		signal(SIGINFO, &handle_siginfo);
	signal(SIGQUIT, &handle_siginfo);
	signal(SIGALRM, &handle_sigalrm);
}
