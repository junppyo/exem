#include "utils.h"

int	ft_strlen(const char *s)
{
	int	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}


char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*ret;
	int		n;
	int		m;
	int		i;

	if (!s1)
	{
		ret = malloc(sizeof(char) * (ft_strlen(s2) + 1));
		i = -1;
		while (s2[++i])
			ret[i] = s2[i];
		ret[i] = '\0';
	}
	else
	{
		n = ft_strlen(s1);
		m = ft_strlen(s2);
		ret = malloc(sizeof(char) * (n + m + 1));
		if (ret == NULL)
			return (0);
		i = -1;
		while (++i < n)
			ret[i] = s1[i];
		i = -1;
		while (++i < m)
			ret[n + i] = s2[i];
		ret[n + i] = '\0';
	}
	return (ret);
}


char	*ft_strdup(const char *src)
{
	char	*ret;
	int		len;

	if (!src || ft_strlen(src) == 0)
	{
		ret = malloc(sizeof(char));
		ret[0] = '\0';
	}
	len = 0;
	while (src[len])
		len++;
	ret = (char *)malloc(sizeof(char) * len + 1);
	if (ret == NULL)
		return (0);
	len = 0;
	while (src[len])
	{
		ret[len] = src[len];
		len++;
	}
	ret[len] = '\0';
	return (ret);
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*ret;
	size_t	i;

	if (s == NULL)
		return (0);
	if (start >= ft_strlen(s))
		return (ft_strdup(""));
	ret = malloc(sizeof(char) * (len + 1));
	if (ret == 0)
		return (0);
	i = 0;
	while (i < len)
	{
		ret[i] = s[start + i];
		i++;
	}
	ret[i] = '\0';
	return (ret);
}

static char	**do_malloc(int n)
{
	char	**ret;

	ret = malloc(sizeof(char *) * (n + 1));
	if (ret == NULL)
		return (0);
	ret[n] = 0;
	return (ret);
}

static int	get_row(char const *str, char c)
{	
	int		cnt;
	int		i;
	int		chk;

	if (str == NULL)
		return (0);
	i = -1;
	cnt = 0;
	chk = 1;
	while (str[++i])
	{
		if (str[i] == c)
			chk = 1;
		else if (chk == 1)
		{
			chk = 0;
			cnt++;
		}
	}
	return (cnt);
}

static char	**chk_null(char **s, int cnt)
{
	int	i;
	int	chk;

	if (s == 0)
		return (0);
	i = 0;
	chk = 0;
	while (i < cnt)
	{
		if (s[i++] == NULL)
			chk = 1;
	}
	i = 0;
	if (chk == 1)
	{
		while (i < cnt)
			free(s[i++]);
		free(s);
		s = NULL;
	}
	return (s);
}

static char	**do_split(char **res, char const *s, char c, int cnt)
{
	int	i;
	int	row;
	int	tmp;

	i = -1;
	row = 0;
	tmp = 0;
	while (s[++i])
	{
		if (s[i] == c)
		{
			if (i > tmp)
				res[row++] = ft_substr(s, tmp, i - tmp);
			tmp = i + 1;
		}
	}
	if (row < cnt)
		res[row] = ft_substr(s, tmp, i - tmp);
	res = chk_null(res, cnt);
	return (res);
}

char	**ft_split(char const *s, char c)
{
	char	**ret;
	int		row;

	if (s == NULL)
		return (0);
	row = get_row(s, c);
	ret = do_malloc(row);
	if (ret == NULL)
		return (0);
	ret = do_split(ret, s, c, row);
	return (ret);
}

int rowcnt(char **matrix)
{
	if (!matrix)
		return 0;
	int i = 0;
	while (matrix[i])
	{
		i++;
	}
	return (i);
}
