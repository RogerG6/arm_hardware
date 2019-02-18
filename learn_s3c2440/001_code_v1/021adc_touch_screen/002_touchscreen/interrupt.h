


#ifndef _INTERRUPT_H
#define _INTERRUPT_H
#define INT_NUM	60	

typedef void (*irq_func)(unsigned int irq);
irq_func irq_arr[INT_NUM];

void register_irq(unsigned int irq, irq_func pf);
void timer0_func(unsigned int irq);

#endif  /* _INTERRUPT_H */
