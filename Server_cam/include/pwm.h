#ifndef _PWM_H
#define _PWM_H


#define FSPWM_MAGIC	'f'
#define ARRAY_SIZE(a)	(sizeof(a) / sizeof(a[0]))
#define PWM_DEV "/dev/pwm"
#define BEAT	250000
#define FSPWM_START	_IO(FSPWM_MAGIC, 0)
#define FSPWM_STOP	_IO(FSPWM_MAGIC, 1)
#define FSPWM_SET_FREQ	_IOW(FSPWM_MAGIC, 2, unsigned int)

#endif
