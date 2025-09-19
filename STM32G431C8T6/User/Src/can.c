/*
 * can.c
 *
 *  Created on: Sep 19, 2025
 *      Author: 莫海峰
 */
#include "can.h"
#include "fdcan.h"
#include "dev.h"

void User_CAN_Init(void)
{
	uint32_t Prescaler, SyncJumpWidth, TimeSeg1, TimeSeg2;
	switch (sysconfig.CommunicationConfig.CAN_BaudRate)
	{
		case 100: //0k
			Prescaler = 68;
			SyncJumpWidth = 1;
			TimeSeg1 = 20;
			TimeSeg2 = 4;
			break;
		case 250: //OK
			Prescaler = 34;
			SyncJumpWidth = 1;
			TimeSeg1 = 15;
			TimeSeg2 = 4;
			break;
		case 1000:
			Prescaler = 17;
			SyncJumpWidth = 1;
			TimeSeg1 = 7;
			TimeSeg2 = 2;
			break;
		default: //500K
			Prescaler = 17;
			SyncJumpWidth = 1;
			TimeSeg1 = 15;
			TimeSeg2 = 4;
			break;
	}
	hfdcan1.Instance = FDCAN1;
	hfdcan1.Init.ClockDivider = FDCAN_CLOCK_DIV1;
	hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
	hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
	hfdcan1.Init.AutoRetransmission = ENABLE;
	hfdcan1.Init.TransmitPause = DISABLE;
	hfdcan1.Init.ProtocolException = DISABLE;
	hfdcan1.Init.NominalPrescaler = Prescaler;
	hfdcan1.Init.NominalSyncJumpWidth = SyncJumpWidth;
	hfdcan1.Init.NominalTimeSeg1 = TimeSeg1;
	hfdcan1.Init.NominalTimeSeg2 = TimeSeg2;
	hfdcan1.Init.DataPrescaler = 17;
	hfdcan1.Init.DataSyncJumpWidth = 1;
	hfdcan1.Init.DataTimeSeg1 = 15;
	hfdcan1.Init.DataTimeSeg2 = 4;
	hfdcan1.Init.StdFiltersNbr = 0;
	hfdcan1.Init.ExtFiltersNbr = 0;
	hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
	if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
	{
		Error_Handler();
	}
	HAL_FDCAN_Start(&hfdcan1);
}

uint8_t tx_buff[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
FDCAN_TxHeaderTypeDef TxHeader;
void CAN_Transmit(void)
{
	TxHeader.TxFrameType = FDCAN_DATA_FRAME; //数据帧
	TxHeader.FDFormat = FDCAN_CLASSIC_CAN; //经典CAN帧，最大8字节
	TxHeader.BitRateSwitch = FDCAN_BRS_OFF; //仲裁段和数据段使用相同速率
	TxHeader.DataLength = FDCAN_DLC_BYTES_8; //数据长度代码 (DLC)
	TxHeader.IdType = FDCAN_EXTENDED_ID; //扩展帧
	TxHeader.Identifier = 0x00400000; //报文ID

	HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, tx_buff);

}

