#include "pusbcan.h"

UNS8 usbCanReceive(UNS16 index, CANMessage *m)
{

#ifdef _WIN32
    UNS8 ret = 0;
    VCI_CAN_OBJ vco = {0};
    VCI_ERR_INFO errinfo = {0};

    if(VCI_GetReceiveNum(VCI_USBCAN2, 0, index))
        ret = (UNS8)VCI_Receive(VCI_USBCAN2, 0, index, &vco, 1, 100);
    else
        return 0;

    if(ret > 0)
    {
        m->cob_id = vco.ID;
        m->rtr = vco.RemoteFlag;
        m->len = vco.DataLen;
        for (int i = 0 ; i < vco.DataLen ; i++) {
           m->data[i] = vco.Data[i];
        }
    }
    else
    {
        VCI_ReadErrInfo(VCI_USBCAN2, 0, index,&errinfo);
    }
    return ret;
#endif

#ifdef linux
    return 0;
#endif
}

UNS8 usbCanSend(UNS16 index, const CANMessage *m)
{

#ifdef _WIN32
    UNS8 ret = 0;
    VCI_CAN_OBJ pSend = {0};
    pSend.ID = m->cob_id;
    pSend.RemoteFlag = m->rtr;
    pSend.ExternFlag = m->extend || (m->cob_id & 0x1FFFF800U);
    pSend.DataLen = m->len;
    for (int i = 0 ; i < m->len ; i++) {
       pSend.Data[i] = m->data[i];
    }

    ret = (UNS8)VCI_Transmit(VCI_USBCAN2,0,index,&pSend,1);

    return ret;
#endif

#ifdef linux
    return 0;
#endif

}

UNS8 usbCanOpen(UNS16 index)
{

#ifdef _WIN32
    VCI_INIT_CONFIG init_config = {0};
    init_config.AccCode = 0;
    init_config.AccMask = 0xffffffff;  //all pass
    init_config.Filter = 1;  //one filter
    init_config.Mode = 0;    //normel mode
    init_config.Timing0 = 0x01; // 0x03 0x1c 125kbps
    init_config.Timing1 = 0x1c; //defualt baud rate set 250kbps


    if(VCI_OpenDevice(VCI_USBCAN2,0,0) == STATUS_OK)
    {
        if(VCI_InitCAN(VCI_USBCAN2,0,index,&init_config) == STATUS_OK)
        {
            if(VCI_StartCAN(VCI_USBCAN2, 0, index) == 1)
            {
                return 1;
            }
        }
        VCI_CloseDevice(VCI_USBCAN2,0);
    }
    return 0;
#endif

#ifdef linux
    return 0;
#endif
}

UNS8 usbCanClose(UNS16 index)
{

#ifdef _WIN32
    int ret = 0;

    ret = VCI_ResetCAN(VCI_USBCAN2, 0, index);

    if(ret)
        ret = VCI_CloseDevice(VCI_USBCAN2,0);

    return ret;
#endif

#ifdef linux
    return 0;
#endif
}

UNS8 usbCanChangeBaudrate(UNS16 index, UNS32 baud)
{

#ifdef _WIN32
    int ret = 0;
    if(baud = 125)
        ret = VCI_SetReference(VCI_USBCAN2, 0, index, 0, (PVOID)0x1C0011);
    else if(baud = 250)
        ret = VCI_SetReference(VCI_USBCAN2, 0, index, 0, (PVOID)0x1C0008);
    else if(baud = 500)
        ret = VCI_SetReference(VCI_USBCAN2, 0, index, 0, (PVOID)0x060007);

    return ret;
#endif

#ifdef linux
    return 0;
#endif
}

