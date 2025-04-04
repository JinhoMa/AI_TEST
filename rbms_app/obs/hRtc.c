int32_t hRtcOpen(void);
void hRtcSetClock(int32_t fd);
void hRtcSetStop(int32_t fd);
void hRtcSetStart(int32_t fd);
int32_t hRtcGetTime(uint8_t *clock, uint8_t size);
void hRtcClose(int32_t fd);

