 #ifndef buzzer_included
#define buzzer_included

#define A 2273
#define B 2025
#define CN 1911
#define D 1703
#define E 659
#define F 716
#define G 391
#define REST 1


void buzzer_init();
void buzzer_advance_frequency();
void buzzer_set_period(short cycles);

#endif // included
