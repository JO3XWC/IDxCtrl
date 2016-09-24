
unsigned char linear2ulaw(short pcm_val);
short ulaw2linear(unsigned char	u_val);

void ulaw2linear(short* ppcm_val, unsigned char* pu_val, int count);
void linear2ulaw(unsigned char* pu_val, short* ppcm_val, int count);
