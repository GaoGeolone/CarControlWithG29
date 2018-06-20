#ifndef SERIAL_ADAPTER_H
#define SERIAL_ADAPTER_H
#include <sys/types.h>

class SerialAdapter
{
public:
    SerialAdapter();
    void Initialize(const char*dev, int baudrate);
    int Read(u_char *rcv_buf, int data_len);
    int ReadNonBlock(u_char *rcv_buf, int data_len);
    int Write(u_char *send_buf, int data_len);

private:
    int fd_;
};

#endif // SERIAL_ADAPTER_H
