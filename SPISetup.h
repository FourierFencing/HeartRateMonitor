#ifdef __cplusplus
extern "C" {
#endif

int SPIGetFd     (int channel) ;
int SPIDataRW    (int channel, unsigned char *data, int len) ;
int SPISetupMode (int channel, int speed, int mode) ;
int SPISetup     (int channel, int speed) ;

#ifdef __cplusplus
}
#endif
