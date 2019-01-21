
# include "my_printf.h"


//========================================
typedef char * va_list;
# define  INTSIZEOF(n)	    ((sizeof(n) + (sizeof(int) - 1)) & ~(sizeof(int) - 1))    //ȷ������ʱ�����ڴ�4�ֽڶ���
# define  va_start(ap, v)	(ap = (va_list)&v + INTSIZEOF(v))                         //����ָ���һ����ε�ָ��
# define  va_arg(ap, t)		(*(t *)((ap += INTSIZEOF(t)) - INTSIZEOF(t)))             //ָ���Ƶ���һ����Σ���ȡ��֮ǰָ��ָ���ֵ����������Ϊt
# define  va_end(ap)		(ap = (va_list)0)										  //��β��������ָ�븳ΪNULL������Ұָ��
//========================================

unsigned char hex[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

static int out_num(long n, char symbol, char lead, int maxlen, int base)
{
	unsigned long m ;   //kΪ��%d���ָ���ʱʹ��
	char tmp[TMPSIZE], * s = tmp + sizeof(tmp);
	int count = 0, i = 0;
	*--s = '\0';
	
	if (n < 0 && symbol == 's')
		m = -n;
	else	
		m = n;
	do
	{
		*--s = hex[m % base];	
		count++; 
	} while ((m /= base) != 0);
	
	if (maxlen && count < maxlen)
		for (i = maxlen - count; i; i--)
			*--s = lead;
	
	if (n < 0 && symbol == 's')
		*--s = '-';
	
	return puts(s);
}

static void my_printf(const char * fmt, va_list ap)
{
	char lead = ' ';   //��ӡ��ʽ�У������ǿ���
	int maxlen = 0;     //��ӡ�����ַ����ĳ���
	for (; *fmt != '\0'; fmt++)
	{
		
		if (*fmt != '%')
		{
			putchar(*fmt);
			continue;
		}
		fmt++;
		
		if (*fmt == '0')
		{
			lead = '0';
			fmt++;
		}
		
		while ((*fmt >= '0') && (*fmt <= '9'))
		{
			maxlen *= 10;
			maxlen += (*fmt - '0');
			fmt++;
		}
		
		switch(*fmt)
		{
			case 'd': out_num(va_arg(ap, int),          's', lead, maxlen, 10); break;
			case 'u': out_num(va_arg(ap, unsigned int), 'u', lead, maxlen, 10 ); break;
			case 'o': out_num(va_arg(ap, unsigned int), 'u',lead, maxlen, 8); break;
			case 'x': out_num(va_arg(ap, unsigned int),	'u',lead, maxlen, 16); break;
			case 'c': putchar(va_arg(ap, int)); break;
			case 's': puts(va_arg(ap, char *)); break;
			default: putchar(*fmt);
		}
		lead = ' ';
		maxlen = 0;
	}
}

 int printf(const char * format, ...)
{
	va_list ap;
	my_printf(format, va_start(ap, format));
	va_end(ap);
	return 0;
}





