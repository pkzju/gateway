#ifndef CANMOTOR_H
#define CANMOTOR_H

// A0:up  A1:down
#define gSpeedTorqueRecieveID   0x0C61A1A0
#define gCustomRecieveID        0x0C62A1A0
#define gIdIqRecieveID          0x0C63A1A0
#define gWatchRecieveID         0x0C64A1A0
#define gSpeedIdPIRecieveID     0x0C65A1A0
#define gIqPIRecieveID          0x0C66A1A0
#define gSpeedTorqueSendID      0x0C67A0A1
#define gCustomSendID           0x0C68A0A1
#define gSpeedIdPISendID        0x0C69A0A1
#define gIqPISendID             0x0C6AA0A1

#define gMotorStop          0
#define gMotorRun   		512
#define gMotorStall 		259
#define gMotorOver_speed 	258
#define gMotorOver_Cur  	257
#define gMotorLow_volt   	260
#define gMotorHigh_volt 	261


typedef unsigned short UINT_16;
typedef signed short INT_16;

typedef union
{
    struct
    {
        UINT_16 	SpeedFbk		:16;
        UINT_16 	Status	    	:16;
        UINT_16 	SpeedRef		:16;
        UINT_16 	TargetTorque 	:16;
    } speedTorque;

    struct
    {
        UINT_16 	Reserve		:16;//iq
        UINT_16 	Custom3	    :16;
        UINT_16 	Custom2		:16;
        UINT_16 	Custom1 	:16;//id
    } custom;

    struct
    {
        INT_16 	IqFbk		:16;
        INT_16 	IqRef	    :16;
        INT_16 	IdFbk		:16;
        INT_16 	IdRef 		:16;
    } IdIq;

    struct
    {
        INT_16 	Reserve		:16;
        INT_16 	Watch3	    :16;
        INT_16 	Watch2		:16;//uq
        INT_16 	Watch1 		:16;//ud
    } watch;

    struct
    {
        UINT_16 	IdI		:16;
        UINT_16 	IdP	    :16;
        UINT_16 	SpeedI			:16;
        UINT_16 	SpeedP 			:16;
    } speedIdPI;

    struct
    {
        UINT_16 	Reserve1		:16;
        UINT_16 	Reserve0	    :16;
        UINT_16 	IqI		:16;
        UINT_16 	IqP 	:16;
    } IqPI;

    struct
    {
        UINT_16 	Byte7		:8;
        UINT_16 	Byte6	    :8;
        UINT_16 	Byte5		:8;
        UINT_16 	Byte4 		:8;
        UINT_16 	Byte3		:8;
        UINT_16 	Byte2	    :8;
        UINT_16 	Byte1		:8;
        UINT_16 	Byte0 		:8;
    } Byte;
} can_J1939_frame;

struct canMotor{
  can_J1939_frame speedTorque;	//0x0C61A1A0 0x0C67A0A1
  can_J1939_frame custom;		//0x0C62A1A0 0x0C68A0A1
  can_J1939_frame IdIq;			//0x0C63A1A0
  can_J1939_frame watch;		//0x0C64A1A0
  can_J1939_frame speedIdPI;	//0x0C65A1A0 0x0C69A0A1
  can_J1939_frame IqPI;			//0x0C66A1A0 0x0C6AA0A1

  can_J1939_frame *findFrame(unsigned int id){

      if(id == gSpeedTorqueRecieveID){
          return &speedTorque;
      }

      if(id == gCustomRecieveID)
          return &custom;

      if(id == gIdIqRecieveID)
          return &IdIq;

      if(id == gWatchRecieveID)
          return &watch;

      if(id == gSpeedIdPIRecieveID)
          return &speedIdPI;

      if(id == gIqPIRecieveID)
          return &IqPI;

      return nullptr;
  }

} ;

#endif // CANMOTOR_H
