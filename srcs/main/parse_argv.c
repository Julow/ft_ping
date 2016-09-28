/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_argv.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/21 11:46:56 by jaguillo          #+#    #+#             */
/*   Updated: 2016/09/28 16:25:47 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/argv.h"
#include "ft/ft_printf.h"
#include "ft/ft_vprintf.h"
#include "net/raw_socket.h"

#include "p_main.h"

#include <limits.h>
#include <stddef.h>
#include <stdlib.h>

static t_argv_opt_err	opt_help(t_argv *argv, void *dst)
{
	ft_printf("Usage: %s [options] host\n"
"Options:\n"
"  -4                    Ipv4 only\n"
"  -6                    Ipv6 only\n"
"  -u                    Ipv4 or Ipv6 (default)\n"
"  -c <count>\n"
"  --count=<count>       Stop after <count> packets (default: unlimited)\n"
"  -i <wait>\n"
"  --wait=<wait>         Wait <wait> second between each packets (default: 1)\n"
"  -P <data>\n"
"  --payload=<data>      Set the payload pattern\n"
"                          (repeated until it match the size of a packet)\n"
"  -s <size>\n"
"  --size=<size>         Set the size of a packet (default: 48)\n"
"  -h <inc>\n"
"  --increment=<inc>     Increment the packet size by <inc> after each packet\n"
"                          or after <count> packets if the -c option is used\n"
"  -G <max_size>\n"
"  --max-increment=<max_size>\n"
"                        Stop at <max_size> bytes when using the -h option\n"
"                          (default: 96, max: 65507)\n"
"  -l <preload>\n"
"  --preload=<preload>   Send <preload> packet at the begining (default: 1)\n"
"  -m <ttl>\n"
"  --ttl=<ttl>           Set the Time-To-Live value\n"
"  -t <timeout>\n"
"  --timeout=<timeout>   Timeout in second to wait for a packet (default: 5)\n"
"  -n\n"
"  --no-lookup           Do not try to resolve host name\n"
"  -p\n"
"  --print               Print packet content in hexdump format\n"
"  -q\n"
"  --quiet               Do not print message when receiving a packet\n"
"  -v\n"
"  --verbose             Show packets that would be ignored\n"
"  -?\n"
"  --help                Show help\n"
		"%!", argv->argv[0]);
	exit(0);
	(void)dst;
}

static struct s_argv_opt const	g_ping_opt[] = {
	ARGV_OPT_SET("4", AF_INET, offsetof(t_ping_args, ai_family)),
	ARGV_OPT_SET("6", AF_INET6, offsetof(t_ping_args, ai_family)),
	ARGV_OPT_SET("u", AF_UNSPEC, offsetof(t_ping_args, ai_family)),
	ARGV_OPT_STR("P", false, offsetof(t_ping_args, payload_pattern)),
	ARGV_OPT_UINT("s", (0, UINT_MAX), offsetof(t_ping_args, payload_size)),
	ARGV_OPT_INT("h", (INT_MIN, INT_MAX), offsetof(t_ping_args, inc_size)),
	ARGV_OPT_UINT("G", (0, ICMP_MAX_SIZE), offsetof(t_ping_args, max_size)),
	ARGV_OPT_UINT("c", (1, UINT_MAX), offsetof(t_ping_args, count)),
	ARGV_OPT_FLAG("p", PING_F_PRINT, offsetof(t_ping_args, flags)),
	ARGV_OPT_FLAG("q", PING_F_QUIET, offsetof(t_ping_args, flags)),
	ARGV_OPT_UINT("l", (1, UINT_MAX), offsetof(t_ping_args, preload)),
	ARGV_OPT_UINT("i", (1, UINT_MAX), offsetof(t_ping_args, wait)),
	ARGV_OPT_UINT("t", (1, UINT_MAX), offsetof(t_ping_args, timeout)),
	ARGV_OPT_UINT("m", (1, 255), offsetof(t_ping_args, ttl)),
	ARGV_OPT_FLAG("n", PING_F_NO_LOOKUP, offsetof(t_ping_args, flags)),
	ARGV_OPT_FLAG("v", PING_F_VERBOSE, offsetof(t_ping_args, flags)),
	ARGV_OPT_FUNC("?", &opt_help, 0),
	ARGV_OPT_ALIAS("count", "c"),
	ARGV_OPT_ALIAS("quiet", "q"),
	ARGV_OPT_ALIAS("print", "p"),
	ARGV_OPT_ALIAS("preload", "l"),
	ARGV_OPT_ALIAS("payload", "P"),
	ARGV_OPT_ALIAS("size", "s"),
	ARGV_OPT_ALIAS("increment", "h"),
	ARGV_OPT_ALIAS("max-increment", "G"),
	ARGV_OPT_ALIAS("wait", "i"),
	ARGV_OPT_ALIAS("timeout", "t"),
	ARGV_OPT_ALIAS("ttl", "m"),
	ARGV_OPT_ALIAS("no-lookup", "n"),
	ARGV_OPT_ALIAS("verbose", "v"),
	ARGV_OPT_ALIAS("help", "?"),
};

static bool		argv_error(t_argv const *argv, char const *fmt, ...)
{
	va_list			ap;

	ft_dprintf(2, "%s: ", argv->argv[0]);
	va_start(ap, fmt);
	ft_vdprintf(2, fmt, ap);
	va_end(ap);
	ft_dprintf(2, "\nType `%s -?` for help%n", argv->argv[0]);
	return (false);
}

#define PING_DEFAULT_PAYLOAD	"abcdefghijklmnopqrstuvwxyz"

bool			parse_argv(int ac, char **av, t_ping_args *dst)
{
	t_argv			argv;
	t_argv_opt_err	err;
	t_sub			tmp;

	argv = ARGV(ac, av);
	*dst = (t_ping_args){
		.ai_family = AF_UNSPEC,
		.ttl = 0,
		.preload = 1,
		.flags = 0,
		.count = 0,
		.wait = 1,
		.timeout = 5,
		.inc_size = 0,
		.max_size = 96,
		.payload_pattern = SUBC(PING_DEFAULT_PAYLOAD),
		.payload_size = 48,
		.host = NULL,
	};
	if ((err = ft_argv_argv(&argv, g_ping_opt,
				ARRAY_LEN(g_ping_opt), dst)) != ARGV_OPT_OK)
	{
		ft_argv_arg(&argv, &tmp);
		return (argv_error(&argv, "%ts: %ts", tmp, g_argv_opt_strerr[err]));
	}
	if (!ft_argv_arg(&argv, &tmp))
		return (argv_error(&argv, "Missing host argument"));
	dst->host = tmp.str;
	if (ft_argv_arg(&argv, &tmp))
		return (argv_error(&argv, "%ts: Unexpected argument", tmp));
	return (true);
}
