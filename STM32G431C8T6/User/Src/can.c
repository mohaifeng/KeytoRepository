/*
 * can.c
 *
 *  Created on: Sep 19, 2025
 *      Author: 莫海峰
 */
#include "can.h"
#include "fdcan.h"
#include "dev.h"
#include <string.h>

// 定义对象字典结构体
OD_ListTypedef od_stu;
FDCAN_TxHeaderTypeDef CanTxHeader;
FDCAN_RxHeaderTypeDef CanRxHeader;
uint8_t CanRxData[8];
uint8_t CanTxBuff[8];
uint8_t can_sernum = 0;

//配置对象字典结构体函数
static void OD_Config_Callback(uint16_t index, uint8_t sub, RW_Handle_t access, int32_t Minval, int32_t Maxval,
		int32_t Deftval)
{
	if (od_stu.tail < OD_SIZE)
	{
		od_stu.od_arr[od_stu.tail].index = index;
		od_stu.od_arr[od_stu.tail].sub_index = sub;
		od_stu.od_arr[od_stu.tail].access = access;
		od_stu.od_arr[od_stu.tail].Minval = Minval;
		od_stu.od_arr[od_stu.tail].Maxval = Maxval;
		od_stu.od_arr[od_stu.tail].Deftval = Deftval;
		od_stu.od_arr[od_stu.tail].data = Deftval;
		od_stu.tail++;
	}
}

void OD_Init(void)
{
	od_stu.tail = 0;
	OD_Config_Callback(0x4000, 0, WRITE_ONLY, 10, 1000, 500); //置零过程中速度,单位：uL/s。
	OD_Config_Callback(0x4000, 1, READ_WRITE, 0, 100, 100); //置零过程中功率百分比，单位：%
	OD_Config_Callback(0x4000, 2, READ_WRITE, 0, 2, 0); //置零过程中退TIP模式

	OD_Config_Callback(0x4001, 0, WRITE_ONLY, 1, 104000, 0); //吸液量,单位：0.01uL。
	OD_Config_Callback(0x4001, 1, READ_WRITE, 1, 2000, 500); //吸液速度,单位：uL/s。
	OD_Config_Callback(0x4001, 2, READ_WRITE, 0, 2000, 10); //截流速度,单位：uL/s。
	OD_Config_Callback(0x4001, 3, READ_WRITE, 0, 2, 0); //TIP补偿

	OD_Config_Callback(0x4002, 0, WRITE_ONLY, 1, 104000, 0); //排液量,单位：0.01uL。
	OD_Config_Callback(0x4002, 1, READ_WRITE, 0, 1000, 0); //回吸体积,单位：0.01uL。
	OD_Config_Callback(0x4002, 2, READ_WRITE, 1, 2000, 500); //排液速度,单位：uL/s。
	OD_Config_Callback(0x4002, 3, READ_WRITE, 0, 2000, 10); //停止速度,单位：uL/s。

	OD_Config_Callback(0x4003, 0, WRITE_ONLY, 0, 250880, 0); //位置值，单位P
	OD_Config_Callback(0x4003, 1, READ_WRITE, 0, 500000, 128000); //运行速度，单位P/s
	OD_Config_Callback(0x4003, 2, READ_WRITE, 0, 256000, 32000); //停止速度，单位P/s

}

//uint8_t OD_Idex_Count(uint16_t index)
//{
//	uint8_t count = 0;
//	for (const OD_Typedef *entry = od_stu.od_arr; entry->index != 0xFFFF; entry++)
//	{
//		if (entry->index == index)
//		{
//			count++;
//		}
//	}
//	return count;
//}

// 通过索引和子索引查找字典条目
static OD_Typedef* OD_Find_Entry(uint16_t index, uint8_t sub_index)
{
	for (uint8_t i = 0; i < od_stu.tail; i++)
	{
		if (od_stu.od_arr[i].index == index && od_stu.od_arr[i].sub_index == sub_index)
		{
			return &od_stu.od_arr[i];
		}
	}
	return NULL;
}

// 读取对象字典值
Sys_Status_t OD_Read(uint16_t index, uint8_t sub_index, int32_t *pdata)
{
	const OD_Typedef *entry;
	Sys_Status_t state = PARAMETER_ERROR;
	RegValue_Handle_t regval;
	if (index == 0x2000)
	{
		state = Read_Register(sub_index, &regval);
		if (state == EXECUTE_SUCCESS)
		{
			*pdata = regval.value.i32v;
		}
		return state;
	}
	else
	{
		entry = OD_Find_Entry(index, sub_index);
		if (entry)
		{
			if (entry->access & WRITE_ONLY)
			{
				state = READ_WRITE_ONLY; // 只写
			}
			else
			{
				*pdata = entry->data;
				state = EXECUTE_SUCCESS;
			}
		}
	}
	return state;
}

// 写入对象字典值
Sys_Status_t OD_Write(uint16_t index, uint8_t sub_index, int32_t value)
{
	OD_Typedef *entry;
	Sys_Status_t state = PARAMETER_ERROR;
	if (index == 0x2000)
	{
		return Write_Register(sub_index, value);
	}
	entry = OD_Find_Entry(index, sub_index);
	if (entry)
	{

		if (value < entry->Minval || value > entry->Maxval)
		{
			state = OVER_LIMIT;
		}
		else
		{
			entry->data = value;
			state = EXECUTE_SUCCESS;
		}
	}
	return state;
}

static void CAN_Filter_Config(void)
{
	FDCAN_FilterTypeDef filterconfig;
	filterconfig.IdType = FDCAN_EXTENDED_ID; //扩展帧
	filterconfig.FilterIndex = 0;
	filterconfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	filterconfig.FilterType = FDCAN_FILTER_MASK;
	filterconfig.FilterID1 = sysconfig.CommunicationConfig.Add & 0xFF;
	filterconfig.FilterID2 = 0xFF;
	if (HAL_FDCAN_ConfigFilter(&hfdcan1, &filterconfig) != HAL_OK)
	{
		Error_Handler();
	}
	//设置为只允许过滤列表ID通过
	if (HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE)
			!= HAL_OK)
	{
		Error_Handler();
	}
}

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
	hfdcan1.Init.ExtFiltersNbr = 1;
	hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
	if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
	{
		Error_Handler();
	}
	CAN_Filter_Config();
	HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
	HAL_FDCAN_Start(&hfdcan1);
}

void CAN_ResolutionGeneral_Protocol(uint32_t idtype, uint8_t *data, uint8_t len)
{
	CANFrame_Handle_t canframe;
	Sys_Status_t state;
	uint32_t resp_id;
	canframe.cmd = (idtype >> 16) & 0xFFFF;
	canframe.target_addr = (idtype >> 8) & 0xFF;
	canframe.ser_num = data[0];
	canframe.idex = (((uint16_t) data[1] << 8) | (uint16_t) data[2]) & 0xFFFF;
	canframe.sub_idex = data[3];
	canframe.data = (((int32_t) (data[4]) << 24) | ((int32_t) (data[5]) << 16) | ((int32_t) (data[6]) << 8)
			| ((int32_t) (data[7])));
	switch (canframe.cmd)
	{
		case CMD_WRITE:
			state = OD_Write(canframe.idex, canframe.sub_idex, canframe.data);
			CanTxBuff[4] = 0;
			CanTxBuff[5] = 0;
			CanTxBuff[6] = 0;
			CanTxBuff[7] = state;
			break;
		case CMD_READ:
			int32_t data;
			state = OD_Read(canframe.idex, canframe.sub_idex, &data);
			if (state == EXECUTE_SUCCESS)
			{
				CanTxHeader.Identifier = 0x00400000; //报文ID
				CanTxBuff[4] = (data >> 24) & 0xFF;
				CanTxBuff[5] = (data >> 16) & 0xFF;
				CanTxBuff[6] = (data >> 8) & 0xFF;
				CanTxBuff[7] = data & 0xFF;
			}
			else
			{
				CanTxBuff[4] = 0;
				CanTxBuff[5] = 0;
				CanTxBuff[6] = 0;
				CanTxBuff[7] = state;
			}
			break;
	}
	CanTxBuff[0] = canframe.ser_num;
	CanTxBuff[1] = (canframe.idex >> 8) & 0xFF;
	CanTxBuff[2] = canframe.idex & 0xFF;
	CanTxBuff[3] = canframe.sub_idex;
	resp_id = ((uint32_t) CMD_RESPONSE << 16) | (uint32_t) (sysconfig.CommunicationConfig.Add << 8)
			| canframe.target_addr;
	CAN_Transmit(resp_id);
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
	if (hfdcan->Instance == FDCAN1)
	{
		uint8_t FillLevel = HAL_FDCAN_GetRxFifoFillLevel(hfdcan, FDCAN_RX_FIFO0);
		for (uint8_t i = 0; i < FillLevel; i++)
		{
			if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &CanRxHeader, CanRxData) != HAL_OK)
			{
				/* Reception Error */
				return;
			}
//			if (CanRxHeader.IdType == FDCAN_EXTENDED_ID)
//				startBootloader(CanRxHeader.Identifier, CanRxData);
//			else
			CAN_ResolutionGeneral_Protocol(CanRxHeader.Identifier, CanRxData, CanRxHeader.DataLength);
		}
	}
}

void CAN_Transmit(uint32_t id)
{
	uint8_t retry = 5;
	CanTxHeader.Identifier = id;
	CanTxHeader.TxFrameType = FDCAN_DATA_FRAME; //数据帧
	CanTxHeader.FDFormat = FDCAN_CLASSIC_CAN; //经典CAN帧，最大8字节
	CanTxHeader.BitRateSwitch = FDCAN_BRS_OFF; //仲裁段和数据段使用相同速率
	CanTxHeader.DataLength = FDCAN_DLC_BYTES_8; //数据长度代码 (DLC)
	CanTxHeader.IdType = FDCAN_EXTENDED_ID; //扩展帧
	while (retry--)
	{
		if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &CanTxHeader, CanTxBuff) == HAL_OK)
			return;
	}
}

void CAN_Heart_Transmit(void)
{
	CanTxBuff[0] = can_sernum++;
	memset(&CanTxBuff[1], 0, 6);
	CanTxBuff[7] = sysconfig.RunStatus;
	CAN_Transmit(((uint32_t) CMD_HEART << 16) | (uint32_t) (sysconfig.CommunicationConfig.Add << 8));
}

