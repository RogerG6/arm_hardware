1������ȫ�ֱ�������֤NOR����ֻ������д
2����data�ζ�λ��0x30000000��
3���������ӽű�������data�ε�relocate & load addr
4����data��cp��sdram�У�������ȫ�ֱ�������ʼ����������Ӧ��lds
5����ӡ��Ϊ0��ȫ�ֱ������쿴���Ƿ�Ϊ0
6����lds�ж���bss�ε�start & end, ���bss�Σ��쿴Ч��
7����߶�λЧ�ʣ���4BYTEcp����bss����4ȡ��
8���ض�λ����������SDRAM�У��ض�λǰʹ��λ���޹��루��Ե�ַ�����ض�λ�����ʹ�þ��Ե�ַ
9����Cʵ���ض�λ���ڻ���л�ȡ����
10����Cʵ���ض�λ����C�л�ȡ����
11����ҵ��
(1).ʹ��printf������ӡ������Ϣ:
	volatile unsigned int *dest = (volatile unsigned int *)&__code_start;
	volatile unsigned int *end = (volatile unsigned int *)&__bss_start;
	volatile unsigned int *src = (volatile unsigned int *)0;
a. ��ӡdest, end, src��ֵ
b. ��ӡdest, end, src��ָ��ĵ�ַ��ֵ

(2). �޸����ӽű����ѳ����ض�λ��0x32000000
     �ٹ۲�������ӡ���
	 


�ܽ᣺
1����Cʵ���ض�λ����*.lds���ӽű��л�ȡ������������&����
   ԭ����C������: int a = 100;
		 ���б�����������symbol table�У�symbol table��Ҳ��������100�ĵ�ַ�����ҷ����ڴ��Դ��100��Ҫȡ��a�ĵ�ַ��&
		 ֮ǰ & ȡ��ַԭ��������
		 
		 ���Ƶģ�extern int bss_start; 
		 ���ӽű��е�bss_start��Ҳ������symbol table�У�ֻ������ζ��������ӽű��е�ֵ�������Ǳ����ĵ�ַ����δ�����ڴ�
		 ����Ҫȡ�����е�ֵ������&