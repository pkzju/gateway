#ifndef PUSBCAN_H
#define PUSBCAN_H

#include "../canopen/ControlCAN.H"

struct CANFrame{
  UNS32 cob_id:29;	/**< message's ID */
  UNS8 extend:1;/**< extend frame flag. (0 if not extend frame, 1 if extend frame) */
  UNS8 rtr:2;		/**< remote transmission request. (0 if not rtr message, 1 if rtr message) */
  UNS8 len;		    /**< message's length (0 to 8) */
  UNS8 data[8];     /**< message's datas */
} ;
using CANMessage = CANFrame;

UNS8 usbCanReceive(UNS16 index, CANMessage *m);
UNS8 usbCanSend(UNS16 index, const CANMessage *m);
UNS8 usbCanOpen(UNS16 index);
UNS8 usbCanClose(UNS16 index);
UNS8 usbCanChangeBaudrate(UNS16 index, UNS32 baud);


#endif // PUSBCAN_H
