
/* Includes ------------------------------------------------------------------*/
#define _COM1010_GLOBAL
#include "includes.h"
#include <stdlib.h>

/**
  * @brief  check sum(cumulative sum)
  * @param  u8 *sendbuf,u8 length.
  * @retval u8 result
  */
u8 Check_Sum(u8 *buf,u8 length)
{
	u8 i;
	u8 result = *buf++;
	for(i = 1;i < length;i++)
	{
		result ^= *buf++;
	}
	return result;
}

/**
  * @brief  receive data deal
  * @param  none
  * @retval none
  */
void rev_deal(void)
{
	u8 temp,i;
	u8 check_sum;
	temp = (u8)USART_ReceiveData(USART2);
	Usart2_Rece_Buf[Usart2_Rec_Cnt] = temp;
	Usart2_Rec_Cnt++;
	switch(Usart2_Rec_Cnt)
	{
		case 1:
			if ((temp != 0xEE) && (temp != 0xDD))	Usart2_Rec_Cnt = 0;
			break;
		case 2:
			if ((temp != 0xEE) && (temp != 0xDD) && (temp != 0xAA)) Usart2_Rec_Cnt = 0;
			break;
		case 3:
			//if (!temp)				rev_count = 0;
			break;
		case 4:
			//if (!temp)				rev_count = 0;
			break;
		case 5:
			//if (!temp)				rev_count = 0;
			break;
		default:
			if (Usart2_Rec_Cnt > 30)//防止接收错误后溢出
			{
				Usart2_Rec_Cnt = 0;
			}
			if ((Usart2_Rece_Buf[0] == 0xEE)&&((Usart2_Rece_Buf[1] == 0xEE)||(Usart2_Rece_Buf[1] == 0xAA)))
			{
				if (Usart2_Rec_Cnt > Usart2_Rece_Buf[5] + 2)//接收数据完成
				{
					Usart2_Rec_Cnt = 0;
					check_sum = Check_Sum(Usart2_Rece_Buf+2,Usart2_Rece_Buf[5]);
					
					if (check_sum == Usart2_Rece_Buf[Usart2_Rece_Buf[5] + 2])//校验正确
					{
						usart2_rev_success = 1;
						for (i = 0;i < 100;i++)
						{
							sicp_buf[i] = Usart2_Rece_Buf[i];
						}
					}
					else
					{
						Usart2_Rec_Cnt = 0;
					}
				}
			}
			else if((Usart2_Rece_Buf[0] == 0xDD)&&(Usart2_Rece_Buf[1] == 0xDD))
			{
				if (Usart2_Rec_Cnt > Usart2_Rece_Buf[3] + 2)//接收数据完成
				{
					Usart2_Rec_Cnt = 0;
					usart2_rev_success = 1;
					for (i = 0;i < 30;i++)
					{
						sicp_buf[i] = Usart2_Rece_Buf[i];
					}
				}
			}
			else
			{
				Usart2_Rec_Cnt = 0;
			}
			break;
	}
}
//清除USART2_Send_Buf
void clear_sicp_buf(void)
{
	u8 i;
	for(i = 0; i < 100;i++)	sicp_buf[i] = 0x00;
}
//十六进制数转换为字符
char hex2ascii(u8 x)
{
	if(x>0x0f)
	{
		return 0;  
	}
	if(x>9)
	{
		return ('A'+x-10);
	}
	return ('0'+x); 
}
//字符转换为十六进制数
u8 ascii2hex(char bChar)
{
	if((bChar>=0x30)&&(bChar<=0x39))  
	{  
			bChar -= 0x30;  
	}  
	else if((bChar>=0x41)&&(bChar<=0x46)) // Capital  
	{  
			bChar -= 0x37;  
	}  
	else if((bChar>=0x61)&&(bChar<=0x66)) //littlecase  
	{  
			bChar -= 0x57;  
	}  
	else   
	{  
			bChar = 0xff;  
	}  
	return bChar; 
}

/*************SICP接收数据分析**********************/
void rev_anaylze(void)
{
	u8 i;
	u8 	rev_message_id;
	u16 rev_mesh_id;
	if(usart2_rev_success){
		usart2_rev_success = 0;
	if ((sicp_buf[0] == 0xEE) && ((sicp_buf[1]== 0xEE) || (sicp_buf[1]== 0xAA))){
		ble_data_frame = 1;
	}
	else if ((sicp_buf[0] == 0xDD) && (sicp_buf[1]== 0xDD)){//Network	Status	Reporting	{
		ble_ctrl_frame = 1;
	}
	
	if (ble_data_frame){
		ble_data_frame = 0;
		rev_message_id = sicp_buf[2];
		rev_mesh_id		 = (((u16)sicp_buf[3])<<8) + (u16)sicp_buf[4];
		switch(sicp_buf[6]){
			case 0x20://ST回复传感器数据
				rev_20 = 1;
				ss.st[rev_message_id].sense.ambient_light = sicp_buf[7];
				ss.st[rev_message_id].sense.color_sense_H = sicp_buf[8];
				ss.st[rev_message_id].sense.color_sense_M = sicp_buf[9];
				ss.st[rev_message_id].sense.color_sense_L = sicp_buf[10];
			break;
			case 0x06:
				deal_rev_status();
			break;
			case 0xB1:
			case 0xB2:
			case 0xB3:
			case 0xB4:
				deal_device_info(sicp_buf[6]);
			break;
			case 0xAA:
				if((rev_message_id >= 75) && (rev_message_id <= 79)){//说明接收到action cmd回复
					if(sicp_buf[7] == 0x05){//接收成功
						for(i = 0; i < 15;i++){
							if(ss.sc[rev_message_id-75].slc[i].MDID == sicp_buf[8]){
								ss.sc[rev_message_id-75].slc[i].ch1_status = sicp_buf[9];
								ss.sc[rev_message_id-75].slc[i].ch2_status = sicp_buf[10];
								ss.sc[rev_message_id-75].slc[i].ch3_status = sicp_buf[11];
								ss.sc[rev_message_id-75].slc[i].ch4_status = sicp_buf[12];
								ss.sc[rev_message_id-75].slc[i].status._flag_bit.bit1 = 1;
								rev_action_success_cnt++;
								break;	
							}
						}
						for(i = 0; i < 15;i++){
							if(ss.sc[rev_message_id-75].spc[i].MDID == sicp_buf[8]){
								ss.sc[rev_message_id-75].spc[i].ch1_status = sicp_buf[9];
								ss.sc[rev_message_id-75].spc[i].ch2_status = sicp_buf[10];
								ss.sc[rev_message_id-75].spc[i].ch3_status = sicp_buf[11];
								ss.sc[rev_message_id-75].spc[i].ch4_status = sicp_buf[12];
								ss.sc[rev_message_id-75].spc[i].status._flag_bit.bit1 = 1;
								rev_action_success_cnt++;
								break;
							}
						}
					}
					else if(sicp_buf[7] == 0x03){//执行失败
						for(i = 0; i < 15;i++){
							if(ss.sc[rev_message_id-75].slc[i].MDID == sicp_buf[8]){
								ss.sc[rev_message_id-75].slc[i].status._flag_bit.bit2 = 1;
								rev_action_fail_cnt++;
								break;
							}
						}
						for(i = 0; i < 15;i++){
							if(ss.sc[rev_message_id-75].spc[i].MDID == sicp_buf[8]){
								ss.sc[rev_message_id-75].spc[i].status._flag_bit.bit2 = 1;
								rev_action_fail_cnt++;
								break;
							}
						}
					}
					if(rev_action_success_cnt + rev_action_fail_cnt >= ss_ap.qos - 2){
						rev_action_done = 1;
					}
				}
				else if((rev_message_id >= 80) && (rev_message_id < 100)){//说明接收到的qe action的回复 ST
					if(sicp_buf[7] == 0x05){//执行成功
						ss.st[rev_message_id-80].ch1_status = sicp_buf[8];
						ss.st[rev_message_id-80].ch2_status = sicp_buf[9];
						ss.st[rev_message_id-80].ch3_status = sicp_buf[10];
						ss.st[rev_message_id-80].ch4_status = sicp_buf[11];
						ss.st[rev_message_id-80].status._flag_bit.bit1 = 1;
						rev_action_success_cnt++;
						rev_action_done2 = 1;
					}
					else if(sicp_buf[7] == 0x03){//执行失败
						ss.st[rev_message_id-80].status._flag_bit.bit2 = 1;
						rev_action_fail_cnt++;
						rev_action_done2 = 1;
					}

				}
				else if((rev_message_id >= 100) && (rev_message_id < 105)){//说明接收到的qe action的回复 SLC/SPC
					if(sicp_buf[7] == 0x05){//执行成功
						for(i = 0; i < 15;i++){
							if(ss.sc[rev_message_id-100].slc[i].MDID == sicp_buf[8]){
								ss.sc[rev_message_id-100].slc[i].ch1_status = sicp_buf[9];
								ss.sc[rev_message_id-100].slc[i].ch2_status = sicp_buf[10];
								ss.sc[rev_message_id-100].slc[i].ch3_status = sicp_buf[11];
								ss.sc[rev_message_id-100].slc[i].ch4_status = sicp_buf[12];
								ss.sc[rev_message_id-100].slc[i].status._flag_bit.bit1 = 1;
								rev_action_done2 = 1;
								break;	
							}
						}
						for(i = 0; i < 15;i++){
							if(ss.sc[rev_message_id-100].spc[i].MDID == sicp_buf[8]){
								ss.sc[rev_message_id-100].spc[i].ch1_status = sicp_buf[9];
								ss.sc[rev_message_id-100].spc[i].ch2_status = sicp_buf[10];
								ss.sc[rev_message_id-100].spc[i].ch3_status = sicp_buf[11];
								ss.sc[rev_message_id-100].spc[i].ch4_status = sicp_buf[12];
								ss.sc[rev_message_id-100].spc[i].status._flag_bit.bit1 = 1;
								rev_action_done2 = 1;								
								break;
							}
						}
					}
					else if(sicp_buf[7] == 0x03){//执行失败
						for(i = 0; i < 15;i++){
							if(ss.sc[rev_message_id-100].slc[i].MDID == sicp_buf[8]){
								ss.sc[rev_message_id-100].slc[i].status._flag_bit.bit2 = 1;	
								rev_action_fail_cnt++;
								rev_action_done2 = 1;
								break;
							}
						}
						for(i = 0; i < 15;i++){
							if(ss.sc[rev_message_id-100].spc[i].MDID == sicp_buf[8]){
								ss.sc[rev_message_id-100].spc[i].status._flag_bit.bit2 = 1;
								rev_action_fail_cnt++;
								rev_action_done2 = 1;
								break;
							}
						}
					}
					
				}
				else if((rev_message_id >= 130) && (rev_message_id < 140)){//收到st alert cmd回复
					
				}
				else if((rev_message_id >= 140) && (rev_message_id < 145)){//收到sc alert cmd回复
					
				}
				break;
			case 0x0A://故障汇报
				deal_mal_cmd();
				break;
			case 0x2A://SC上报电量
				deal_sc_data_cmd();
				break;
			case 0x29://ST汇报手势
				for(i = 0; i < 20; i++){
					if(ss.st[i].meshid == rev_mesh_id){
						ss.st[i].sense.handgusture_H = sicp_buf[7];
						ss.st[i].sense.handgusture_L = sicp_buf[8];
						sicp_receipt(0x02,rev_message_id,rev_mesh_id);
						break;
					}
				}
				break;
			case 0x35://ST汇报触摸按键
				for(i = 0; i < 20; i++){
					if(ss.st[i].meshid == rev_mesh_id){
						if(sicp_buf[7] == 1)	ss.st[i].ch1_status = sicp_buf[8];
						else if(sicp_buf[7] == 2)	ss.st[i].ch2_status = sicp_buf[8];
						else if(sicp_buf[7] == 3)	ss.st[i].ch3_status = sicp_buf[8];
						sicp_receipt(0x02,rev_message_id,rev_mesh_id);
						break;
					}
				}
				break;
			default:
				break;
		}
	}
	else if (ble_ctrl_frame){
		ble_ctrl_frame = 0;
		switch(sicp_buf[4]){
			case 0x01://网络状态帧
				ns.signal = sicp_buf[5];
				ns.connect = sicp_buf[6];
				ns.phones = sicp_buf[7];
				ss.meshid = ((u16)sicp_buf[8]<<4) | ((u16)sicp_buf[9]);
				ns.host_meshid_H = sicp_buf[10];
				ns.host_meshid_L = sicp_buf[11];
				break;
			case 0x02:
				system_init();
				clear_sicp_buf();
				break;
			case 0x03:
				system_init();
				clear_sicp_buf();
				ns.signal = 0x00;
				ns.connect = 0x00;
				ns.phones = 0x00;
				ss.meshid = 0x80FF;
				ns.host_meshid_H = 0x00;
				ns.host_meshid_L = 0x00;
				break;
		}
	}
	}
}



/*******************接收处理函数************************/
//处理device info指令
void deal_device_info(u8 type)
{
	u8  rev_deviceid[8];
	u8  i,j;
	u8 	acceptable = 0;
	switch(type)
	{
		case 0xB1://SC
			rev_B1 = 1;
			
			rev_deviceid[0] = hex2ascii(((sicp_buf[7]&0xF0)>>4));
			rev_deviceid[1] = hex2ascii((sicp_buf[7]&0x0F));
			rev_deviceid[2] = hex2ascii(((sicp_buf[8]&0xF0)>>4));
			rev_deviceid[3] = hex2ascii((sicp_buf[8]&0x0F));
			rev_deviceid[4] = hex2ascii(((sicp_buf[9]&0xF0)>>4));
			rev_deviceid[5] = hex2ascii((sicp_buf[9]&0x0F));
			rev_deviceid[6] = hex2ascii(((sicp_buf[10]&0xF0)>>4));
			rev_deviceid[7] = hex2ascii((sicp_buf[10]&0x0F));
			for(i = 0;i<5;i++){
				if(strncmp(ss.sc[i].deviceid,rev_deviceid,8)==0){//找到对应device id的sc，说明device info合法
					ss.sc[i].meshid = (((u16)sicp_buf[3])<<8) + (u16)sicp_buf[4];
					acceptable = 1;
					if(sicp_buf[11] < 0x0F){
						ss.sc[i].model[0] = hex2ascii(sicp_buf[11]);
					}
					else{
						ss.sc[i].model[0] = hex2ascii(((sicp_buf[11]&0xF0)>>4));
						ss.sc[i].model[1] = hex2ascii((sicp_buf[11]&0x0F));
					}
					if(sicp_buf[12] < 0x0F){
						ss.sc[i].firmware[0] = hex2ascii(sicp_buf[12]);
					}
					else{
						ss.sc[i].firmware[0] = hex2ascii(((sicp_buf[12]&0xF0)>>4));
						ss.sc[i].firmware[1] = hex2ascii((sicp_buf[12]&0x0F));
					}
					ss.sc[i].HWTtest = sicp_buf[13];
					ss.sc[i].Ndevice = sicp_buf[14];
					sicp_receipt(0x02,sicp_buf[2],ss.sc[i].meshid);
					ready_sc_post = 1;
				}		
			}
			if(!acceptable)	{
				sicp_receipt(0x04,sicp_buf[2],ss.sc[i].meshid);
				sicp_ble_cmd(0x02,sicp_buf[2],ss.sc[i].meshid);
			}
			
			break;
		case 0xB2://SLC
			rev_B2 = 1;
			rev_deviceid[0] = hex2ascii(((sicp_buf[7]&0xF0)>>4));
			rev_deviceid[1] = hex2ascii((sicp_buf[7]&0x0F));
			rev_deviceid[2] = hex2ascii(((sicp_buf[8]&0xF0)>>4));
			rev_deviceid[3] = hex2ascii((sicp_buf[8]&0x0F));
			rev_deviceid[4] = hex2ascii(((sicp_buf[9]&0xF0)>>4));
			rev_deviceid[5] = hex2ascii((sicp_buf[9]&0x0F));
			rev_deviceid[6] = hex2ascii(((sicp_buf[10]&0xF0)>>4));
			rev_deviceid[7] = hex2ascii((sicp_buf[10]&0x0F));
			for(i = 0;i < 5;i++){
				for(j = 0;j < 15;j++){
					if(strncmp(ss.sc[i].slc[j].deviceid,rev_deviceid,8)==0){//找到对应device id的slc，说明device info合法
						acceptable = 1;
						if(sicp_buf[11] < 0x0F){
							ss.sc[i].slc[j].model[0] = hex2ascii(sicp_buf[11]);
						}
						else{
							ss.sc[i].slc[j].model[0] = hex2ascii(((sicp_buf[11]&0xF0)>>4));
							ss.sc[i].slc[j].model[1] = hex2ascii((sicp_buf[11]&0x0F));
						}
						if(sicp_buf[12] < 0x0F){
							ss.sc[i].slc[j].firmware[0] = hex2ascii(sicp_buf[12]);
						}
						else{
							ss.sc[i].slc[j].firmware[0] = hex2ascii(((sicp_buf[12]&0xF0)>>4));
							ss.sc[i].slc[j].firmware[1] = hex2ascii((sicp_buf[12]&0x0F));
						}
						ss.sc[i].slc[j].HWTtest = sicp_buf[13];
						ss.sc[i].slc[j].MDID = sicp_buf[14];
						sicp_receipt(0x02,sicp_buf[2],ss.sc[i].meshid);
						ready_slc_post = 1;
					}
				}
			}
			if(!acceptable)	{
				sicp_receipt(0x04,sicp_buf[2],ss.sc[i].meshid);
				sicp_ble_cmd(0x02,sicp_buf[2],ss.sc[i].meshid);
			}
			
			break;
		case 0xB3://SPC
			rev_B3 = 1;
			rev_deviceid[0] = hex2ascii(((sicp_buf[7]&0xF0)>>4));
			rev_deviceid[1] = hex2ascii((sicp_buf[7]&0x0F));
			rev_deviceid[2] = hex2ascii(((sicp_buf[8]&0xF0)>>4));
			rev_deviceid[3] = hex2ascii((sicp_buf[8]&0x0F));
			rev_deviceid[4] = hex2ascii(((sicp_buf[9]&0xF0)>>4));
			rev_deviceid[5] = hex2ascii((sicp_buf[9]&0x0F));
			rev_deviceid[6] = hex2ascii(((sicp_buf[10]&0xF0)>>4));
			rev_deviceid[7] = hex2ascii((sicp_buf[10]&0x0F));
			for(i = 0;i < 5;i++){
				for(j = 0;j < 15;j++){
					if(strncmp(ss.sc[i].spc[j].deviceid,rev_deviceid,8)==0){//找到对应device id的slc，说明device info合法
						acceptable = 1;
						if(sicp_buf[11] < 0x0F){
							ss.sc[i].spc[j].model[0] = hex2ascii(sicp_buf[11]);
						}
						else{
							ss.sc[i].spc[j].model[0] = hex2ascii(((sicp_buf[11]&0xF0)>>4));
							ss.sc[i].spc[j].model[1] = hex2ascii((sicp_buf[11]&0x0F));
						}
						if(sicp_buf[12] < 0x0F){
							ss.sc[i].spc[j].firmware[0] = hex2ascii(sicp_buf[12]);
						}
						else{
							ss.sc[i].spc[j].firmware[0] = hex2ascii(((sicp_buf[12]&0xF0)>>4));
							ss.sc[i].spc[j].firmware[1] = hex2ascii((sicp_buf[12]&0x0F));
						}
						ss.sc[i].spc[j].HWTtest = sicp_buf[13];
						ss.sc[i].spc[j].MDID = sicp_buf[14];
						sicp_receipt(0x02,sicp_buf[2],ss.sc[i].meshid);
						ready_spc_post = 1;
					}
				}
			}
			if(!acceptable)	{
				sicp_receipt(0x04,sicp_buf[2],ss.sc[i].meshid);
				sicp_ble_cmd(0x02,sicp_buf[2],ss.sc[i].meshid);
			}
			break;
		case 0xB4://ST
			rev_B4 = 1;
			rev_deviceid[0] = hex2ascii(((sicp_buf[7]&0xF0)>>4));
			rev_deviceid[1] = hex2ascii((sicp_buf[7]&0x0F));
			rev_deviceid[2] = hex2ascii(((sicp_buf[8]&0xF0)>>4));
			rev_deviceid[3] = hex2ascii((sicp_buf[8]&0x0F));
			rev_deviceid[4] = hex2ascii(((sicp_buf[9]&0xF0)>>4));
			rev_deviceid[5] = hex2ascii((sicp_buf[9]&0x0F));
			rev_deviceid[6] = hex2ascii(((sicp_buf[10]&0xF0)>>4));
			rev_deviceid[7] = hex2ascii((sicp_buf[10]&0x0F));
			for(i = 0;i < 20;i++){
				if(strncmp(ss.st[i].deviceid,rev_deviceid,8)==0){//找到对应device id的sc，说明device info合法
					acceptable = 1;
					ss.st[i].meshid = (((u16)sicp_buf[3])<<8) + (u16)sicp_buf[4];
					if(sicp_buf[11] < 0x0F){
						ss.st[i].model[0] = hex2ascii(sicp_buf[11]);
					}
					else{
						ss.st[i].model[0] = hex2ascii(((sicp_buf[11]&0xF0)>>4));
						ss.st[i].model[1] = hex2ascii((sicp_buf[11]&0x0F));
					}
					if(sicp_buf[12] < 0x0F){
						ss.st[i].firmware[0] = hex2ascii(sicp_buf[12]);
					}
					else{
						ss.st[i].firmware[0] = hex2ascii(((sicp_buf[12]&0xF0)>>4));
						ss.st[i].firmware[1] = hex2ascii((sicp_buf[12]&0x0F));
					}
					ss.st[i].HWTtest = sicp_buf[13];
					sicp_config_cmd(sicp_buf[2],ss.st[i].meshid);
					ready_st_post = 1;
				}		
			}
			if(!acceptable)	{
				sicp_receipt(0x04,sicp_buf[2],ss.st[i].meshid);
				sicp_ble_cmd(0x02,sicp_buf[2],ss.st[i].meshid);
			}
			break;
		default:
			break;
	}
}

//处理回复状态指令
void deal_rev_status(void)
{
	u8 i;
	rev_06 = 1;
	if((sicp_buf[2] >= 50) && (sicp_buf[2] < 69)){//接收到ST回复状态
		ss.st[sicp_buf[2] - 50].ch1_status = sicp_buf[8];
		ss.st[sicp_buf[2] - 50].ch2_status = sicp_buf[9];
		ss.st[sicp_buf[2] - 50].ch3_status = sicp_buf[10];
		ss.st[sicp_buf[2] - 50].ch4_status = sicp_buf[11];
	}
	else if((sicp_buf[2] >= 70) && (sicp_buf[2] < 75)){//接收到SC回复状态
		//寻找sc下挂module id为MDID的模块
		for(i = 0;i < 15;i++){
			if(sicp_buf[7] == ss.sc[sicp_buf[2]-70].slc[i].MDID){
				ss.sc[sicp_buf[2]-70].slc[i].ch1_status = sicp_buf[8];
				ss.sc[sicp_buf[2]-70].slc[i].ch2_status = sicp_buf[9];
				ss.sc[sicp_buf[2]-70].slc[i].ch3_status = sicp_buf[10];
				ss.sc[sicp_buf[2]-70].slc[i].ch4_status = sicp_buf[11];
				break;
			}
		}
		for(i = 0;i < 15;i++){
			if(sicp_buf[7] == ss.sc[sicp_buf[2]-70].spc[i].MDID){
				ss.sc[sicp_buf[2]-70].spc[i].ch1_status = sicp_buf[8];
				ss.sc[sicp_buf[2]-70].spc[i].ch2_status = sicp_buf[9];
				ss.sc[sicp_buf[2]-70].spc[i].ch3_status = sicp_buf[10];
				ss.sc[sicp_buf[2]-70].spc[i].ch4_status = sicp_buf[11];
				break;
			}
		}
	}
}

//处理ST\SC\SLC\SPC汇报故障
void deal_mal_cmd(void)
{
	u8 i;
	u8 rev_message_id = 0;
	u16 rev_mesh_id = 0;
	rev_message_id = sicp_buf[2];
	rev_mesh_id		 = (((u16)sicp_buf[3])<<8) + (u16)sicp_buf[4];
	if(sicp_buf[7] == 0xB4){//说明接收到ST汇报故障
		rev_st_mal = 1;
		for(i = 0;i < 20; i++){
			if(ss.st[i].meshid == rev_mesh_id){
				ss.st[i].HWTtest = sicp_buf[9];
				sicp_receipt(0x02,rev_message_id,rev_mesh_id);
				break;
			}
		}
	}
	else if(sicp_buf[7] == 0xB1){//说明接收到sc汇报故障
		rev_sc_mal = 1;
		for(i = 0; i < 5; i++){
			if(ss.sc[i].meshid == rev_mesh_id){
				ss.sc[i].HWTtest = sicp_buf[9];
				sicp_receipt(0x02,rev_message_id,rev_mesh_id);
				break;
			}
		}
	}
	else if(sicp_buf[7] == 0xB2){//说明接收到slc汇报故障
		for(i = 0;i < 5;i++){
			if(ss.sc[i].meshid == rev_mesh_id){
				ss.sc[i].slc[sicp_buf[9]].HWTtest = sicp_buf[10];
				sicp_receipt(0x02,rev_message_id,rev_mesh_id);
				break;
			}
		}
	}
	else if(sicp_buf[7] == 0xB3){//说明接收到spc汇报故障
		for(i = 0;i < 5;i++){
			if(ss.sc[i].meshid == rev_mesh_id){
				ss.sc[i].spc[sicp_buf[9]].HWTtest = sicp_buf[10];
				sicp_receipt(0x02,rev_message_id,rev_mesh_id);
				break;
			}
		}
	
	}
}

//处理SC汇报电量
void deal_sc_data_cmd(void)
{
	u8 i;
	u8 rev_message_id = 0;
	u16 rev_mesh_id = 0;
	rev_message_id = sicp_buf[2];
	rev_mesh_id		 = (((u16)sicp_buf[3])<<8) + (u16)sicp_buf[4];
	for(i = 0; i < 5;i++){
		if(ss.sc[i].meshid == rev_mesh_id){
			ss.sc[i].spc[sicp_buf[9]].energy_consum = (((u16)sicp_buf[7])<<8) + (u16)sicp_buf[8];
			ss.sc[i].sense.total_energy_consum += ss.sc[i].spc[sicp_buf[9]].energy_consum;
			if(ss.sc[i].sense.total_energy_consum >= 65536){
				ss.sc[i].sense.total_energy_consum -= 65536;
				send_data_sync(0x43);//主动推送一次数据包含EG
			}
			sicp_receipt(0x02,rev_message_id,rev_mesh_id);
			break;
		}
	}
}


/**********************公共函数****************************/

void sicp_send_message(SICP_Message *tx,u8 pay_len)
{
	u8 payload_len = pay_len;
	USART2_Send_Buf[0] = tx->frame_h1;
	USART2_Send_Buf[1] = tx->frame_h2;
	USART2_Send_Buf[2] = tx->message_id;
	USART2_Send_Buf[3] = tx->mesh_id_H;
	USART2_Send_Buf[4] = tx->mesh_id_L;
	USART2_Send_Buf[5] = 4+payload_len;
	mymemcpy(&USART2_Send_Buf[6],tx->payload,payload_len);
	USART2_Send_Buf[6+payload_len] = Check_Sum(&USART2_Send_Buf[2],USART2_Send_Buf[5]);
	Usart2_Send(USART2_Send_Buf,7+payload_len);
	while(!Usart2_Send_Done);	Usart2_Send_Done = 0;//等待这包数据发送完成
}


/*************SICP回复数据**************************/
void sicp_receipt(u8 type,u8 send_message_id,u16 send_mesh_id)
{
	SICP_Message receipt;
	receipt.frame_h1 = 0xEE;
	receipt.frame_h2 = 0xAA;
	receipt.message_id = send_message_id;
	receipt.mesh_id_H = (u8)((send_mesh_id&0xFF00)>>8);
	receipt.mesh_id_L = (u8)(send_mesh_id&0x00FF);
	receipt.payload[0] = 0xAA;
	receipt.payload[1] = type;
	//receipt.payload[2] = '\0';
	sicp_send_message(&receipt,2);
}

void sicp_ble_cmd(u8 type,u8 send_message_id,u16 send_mesh_id)
{
	SICP_Message ble;
	if(type == 0x01){
		ble.frame_h1 = 0xEE;
		ble.frame_h2 = 0xAA;
		ble.message_id = send_message_id;
		ble.mesh_id_H = (u8)((ss.meshid&0xFF00)>>8);
		ble.mesh_id_L = (u8)(ss.meshid&0x00FF);
		ble.payload[0] = 0xC0;
		ble.payload[1] = type;
		//ble.payload[2] = '\0';
		sicp_send_message(&ble,2);
	}
	else if(type == 0x02){
		ble.frame_h1 = 0xEE;
		ble.frame_h2 = 0xAA;
		ble.message_id = send_message_id;
		ble.mesh_id_H = (u8)((send_mesh_id&0xFF00)>>8);
		ble.mesh_id_L = (u8)(send_mesh_id&0x00FF);
		ble.payload[0] = 0xC0;
		ble.payload[1] = type;
		//ble.payload[2] = '\0';
		sicp_send_message(&ble,2);
	}
}

void sicp_config_cmd(u8 send_message_id,u16 send_mesh_id)
{
	SICP_Message config;
	u16 config_mesh_id = 0;
	u8 action = 0;
	u8 gesture_H = 0;
	u8 gesture_L = 0;
	u8 i,j;
	for(i = 0; i < ss_cst_count;i++){//查找相应mesh id的cst配置信息
		if(send_mesh_id == ss_cst[i].meshid){
			if(ss_cst[i].type == 1){//配置按键，且本条指令还没被发送过
				ss_cst[i].type = 0;
				for(j = 0;j < 5;j++){//寻找配置的mesh id
					if(strncmp(ss_cst[i].target_id,ss.sc[j].deviceid,10)==0)
						config_mesh_id = ss.sc[j].meshid;
				}
				if(strncmp(ss_cst[i].action,"DM",2)==0)	action = 0x51;
				config.frame_h1 = 0xEE;
				config.frame_h2 = 0xAA;
				config.message_id = send_message_id;
				config.mesh_id_H = (u8)((send_mesh_id&0xFF00)>>8);
				config.mesh_id_L = (u8)(send_mesh_id&0x00FF);
				config.payload[0] = 0x04;
				config.payload[1] = 0x01;
				config.payload[2] = ss_cst[i].key;
				config.payload[3] = 0x00;
				config.payload[4] = (u8)((config_mesh_id&0xFF00) >> 8);
				config.payload[5] = (u8)(config_mesh_id&0x00FF);
				config.payload[6] = ss_cst[i].ch;
				config.payload[7] = action;
				config.payload[8] = ss_cst[i].value;
				//config.payload[9] = '\0';
				sicp_send_message(&config,9);
			}
			if(ss_cst[i].type == 2){//配置手势，且本条指令还没被发送过
				ss_cst[i].type = 0;
				for(j = 0;j < 5;j++){//寻找配置的mesh id
					if(strncmp(ss_cst[i].target_id,ss.sc[j].deviceid,10)==0)
						config_mesh_id = ss.sc[j].meshid;
				}
				if(strncmp(ss_cst[i].action,"DM",2)==0)	action = 0x51;
				switch(ss_cst[i].cond[0]){
					case '1':
						gesture_H |= 0xE0;
						break;
					case '2':
						gesture_H |= 0xF0;
						break;
					case '3':
						gesture_H |= 0xC0;
						break;
					case '4':
						gesture_H |= 0xD0;
						break;
				}
				switch(ss_cst[i].cond[1]){
					case '1':
						gesture_H |= 0x0E;
						break;
					case '2':
						gesture_H |= 0x0F;
						break;
					case '3':
						gesture_H |= 0x0C;
						break;
					case '4':
						gesture_H |= 0x0D;
						break;
				}
				switch(ss_cst[i].cond[2]){
					case '1':
						gesture_L |= 0xE0;
						break;
					case '2':
						gesture_L |= 0xF0;
						break;
					case '3':
						gesture_L |= 0xC0;
						break;
					case '4':
						gesture_L |= 0xD0;
						break;
				}
				config.frame_h1 = 0xEE;
				config.frame_h2 = 0xAA;
				config.message_id = send_message_id;
				config.mesh_id_H = (u8)((send_mesh_id&0xFF00)>>8);
				config.mesh_id_L = (u8)(send_mesh_id&0x00FF);
				config.payload[0] = 0x04;
				config.payload[1] = 0x02;
				config.payload[2] = gesture_H;
				config.payload[3] = gesture_L;
				config.payload[4] = (u8)((config_mesh_id&0xFF00) >> 8);
				config.payload[5] = (u8)(config_mesh_id&0x00FF);
				config.payload[6] = ss_cst[i].ch;
				config.payload[7] = action;
				config.payload[8] = ss_cst[i].value;
				//config.payload[9] = '\0';
				sicp_send_message(&config,9);
			}
		}
	}
}

/*************SICP主动发送**********************/
//网关发送心跳包用于查询及刷新数据   5s发送一包
void sicp_send_heartbeart(void)
{
  u8 i,j;
	SICP_Message heartbeat;
	heartbeat_cnt++;
	if(heartbeat_cnt >= 5){
		heartbeat_cnt = 0;
		//ST心跳包
		for(i = 0; i < 20;i++){ 
			if(ss.st[i].meshid){//mesh id不为空，则进行点名
				heartbeat.frame_h1 = 0xEE;
				heartbeat.frame_h2 = 0xEE;
				heartbeat.message_id = 50+i;
				heartbeat.mesh_id_H = (u8)((ss.st[i].meshid&0xFF00)>>8);
				heartbeat.mesh_id_L = (u8)(ss.st[i].meshid&0x00FF);
				heartbeat.payload[0] = 0x03;
				heartbeat.payload[1] = 0x03;
				//heartbeat.payload[2] = '\0';
				sicp_send_message(&heartbeat,2);
			}
		}
		//SC心跳包
		for(i = 0; i < 5;i++){ 
			if(ss.sc[i].meshid){//mesh id不为空，则进行点名
				heartbeat.frame_h1 = 0xEE;
				heartbeat.frame_h2 = 0xEE;
				heartbeat.message_id = 70+i;
				heartbeat.mesh_id_H = (u8)((ss.sc[i].meshid&0xFF00)>>8);
				heartbeat.mesh_id_L = (u8)(ss.sc[i].meshid&0x00FF);
				heartbeat.payload[0] = 0x03;
				heartbeat.payload[1] = 0x03;
				//heartbeat.payload[2] = '\0';
				sicp_send_message(&heartbeat,2);
			}
		}
		//SLC心跳包
		for(i = 0; i < 5;i++){
			for(j = 0;j < 15;j++){
				if(ss.sc[i].slc[j].meshid){//mesh id不为空，则进行点名
					heartbeat.frame_h1 = 0xEE;
					heartbeat.frame_h2 = 0xEE;
					heartbeat.message_id = 50+i;
					heartbeat.mesh_id_H = (u8)((ss.sc[i].meshid&0xFF00)>>8);
					heartbeat.mesh_id_L = (u8)(ss.sc[i].meshid&0x00FF);
					heartbeat.payload[0] = 0x03;
					heartbeat.payload[1] = 0x03;
					heartbeat.payload[2] = ss.sc[i].slc[j].MDID;
					//heartbeat.payload[3] = '\0';
					sicp_send_message(&heartbeat,3);
				}
			}
		}
		//SPC心跳包
		for(i = 0; i < 5;i++){
			for(j = 0;j < 15;j++){ 
				if(ss.sc[i].spc[j].meshid){//mesh id不为空，则进行点名
					heartbeat.frame_h1 = 0xEE;
					heartbeat.frame_h2 = 0xEE;
					heartbeat.message_id = 50+i;
					heartbeat.mesh_id_H = (u8)((ss.sc[i].meshid&0xFF00)>>8);
					heartbeat.mesh_id_L = (u8)(ss.sc[i].meshid&0x00FF);
					heartbeat.payload[0] = 0x03;
					heartbeat.payload[1] = 0x03;
					heartbeat.payload[2] = ss.sc[i].spc[j].MDID;
					//heartbeat.payload[3] = '\0';
					sicp_send_message(&heartbeat,3);
				}
			}
		}
	}
}

//Gateway	Mesh	ID	Broadcasting	放入main task1s循环中	，每30s发送一次
void sicp_broadcast(void)
{
	SICP_Message bc;
	broadcast_cnt++;
	if(broadcast_cnt >= 30){
		broadcast_cnt = 0;
		bc.frame_h1 = 0xEE;
		bc.frame_h2 = 0xEE;
		bc.message_id = 0x9E;
		bc.mesh_id_H = 0x00;
		bc.mesh_id_L = 0x00;
		bc.payload[0] = 0xB0;
		sicp_send_message(&bc,1);
	}
}




//收到/actions/refresh/CH/AL或者/actions/refresh/CH/CS时发送
void sicp_cmd_data(void)	//获取SS下ST的环境光颜色和亮度
{
	u8 i;
	SICP_Message cmd_data;
	if(rev_ar2){
		rev_ar2 = 0;
		for(i = 0; i < 20;i++){
			if((ss.st[i].meshid) && (!ss.st[i].deviceid[0])){//mesh id不为空，则进行点名
				cmd_data.frame_h1 = 0xEE;
				cmd_data.frame_h2 = 0xEE;
				cmd_data.message_id = i;
				cmd_data.mesh_id_H = (u8)((ss.st[i].meshid&0xFF00)>>8);
				cmd_data.mesh_id_L = (u8)(ss.st[i].meshid&0x00FF);
				cmd_data.payload[0] = 0x03;
				cmd_data.payload[1] = 0x01;
				cmd_data.payload[2] = 0x30;
				cmd_data.payload[3] = 0x32;
				//cmd_data.payload[4] = '\0';
				sicp_send_message(&cmd_data,4);
			}
		}
	}
}
//sicp发送cmd-refresh指令刷新传感器数据，每ssp config/ss指令refr时间发送一次
//收到/actions/refresh 时强致发送一次
void sicp_cmd_refresh(void) //刷新SS下ST的环境光颜色和亮度,SC上的电流
{
	u8 i;
	SICP_Message refresh_data;
	for(i = 0; i < 20;i++){
		if((ss.st[i].meshid) && (!ss.st[i].deviceid[0])){//mesh id不为空，则进行点名
			refresh_data.frame_h1 = 0xEE;
			refresh_data.frame_h2 = 0xEE;
			refresh_data.message_id = 20+i;
			refresh_data.mesh_id_H = (u8)((ss.st[i].meshid&0xFF00)>>8);
			refresh_data.mesh_id_L = (u8)(ss.st[i].meshid&0x00FF);
			refresh_data.payload[0] = 0x03;
			refresh_data.payload[1] = 0x02;
			//refresh_data.payload[2] = '\0';
			sicp_send_message(&refresh_data,2);
		}
	}
	for(i = 0; i < 5;i++){
		if((ss.sc[i].meshid) && (!ss.sc[i].deviceid[0])){//mesh id不为空，则进行点名
			refresh_data.frame_h1 = 0xEE;
			refresh_data.frame_h2 = 0xEE;
			refresh_data.message_id = 40+i;
			refresh_data.mesh_id_H = (u8)((ss.sc[i].meshid&0xFF00)>>8);
			refresh_data.mesh_id_L = (u8)(ss.sc[i].meshid&0x00FF);
			refresh_data.payload[0] = 0x03;
			refresh_data.payload[1] = 0x02;
			//refresh_data.payload[2] = '\0';
			sicp_send_message(&refresh_data,2);
		}
	}
}

//解析ss_ap，取seqid为x的指令存放到action[x-1]中
u8 get_ss_ap(u8 x)
{
	u8 i,j,k;
	u8 type = 0;
	u8 value_1 = 0;
	u8 value_2 = 0;
	u8 value_3 = 0;
	for(i = 0; i < 10;i++){
		if(ss_ap.sl_ap[i].seqid == x){
			type = 1;//表示将发送的指令是SC控制SLC调光
			action[x-1].action_channel = (ss_ap.sl_ap[i].ch&0x0F);
			value_3 = (u8)(ss_ap.sl_ap[i].topos[0] - '0');
			value_2 = (u8)(ss_ap.sl_ap[i].topos[1] - '0');
			value_1 = (u8)(ss_ap.sl_ap[i].topos[2] - '0');
			if(!ss_ap.sl_ap[i].topos[2]){
				action[x-1].action_value = value_1*100 + value_2*10 + value_3;
			}
			else if(!ss_ap.sl_ap[i].topos[1]){
				action[x-1].action_value = value_2*10 + value_3;
			}
			else {
				action[x-1].action_value = value_3;
			}
			action[x-1].action_cmd = 0x50 + ss_ap.sl_ap[i].option.erase;
			action[x-1].action_ext = ss_ap.sl_ap[i].option.duration*10;//action_ext以ms为单位，duration以s为单位
			action[x-1].action_timeout = ss_ap.sl_ap[i].timeout;
			//查找是哪一个SC下的SLC，填入相应的message id
			for(j = 0; j < 5;j++){
				for(k = 0; k < 15;k++){
					if(strncmp(ss_ap.sl_ap[i].sepid,ss.sc[j].slc[k].deviceid,8)==0){
						mymemcpy(action[x-1].deviceid,ss.sc[j].slc[k].deviceid,8);
						action[x-1].message_id = j+75;
						action[x-1].mesh_id_H = (u8)((ss.sc[j].meshid&0xFF00)>>8);
						action[x-1].mesh_id_L = (u8)(ss.sc[j].meshid&0x00FF);
						action[x-1].action_channel += (u8)(ss.sc[j].slc[k].MDID<<4);
					}
				}					
			}
			break;
		}
		else if(ss_ap.sp_ap[i].seqid == x){
			type = 2;//表示将发送的指令是SC控制SPC打开/关闭开关
			action[x-1].action_channel = ss_ap.sp_ap[i].ch&0x0F;
			value_3 = (u8)(ss_ap.sp_ap[i].topos[0] - '0');
			value_2 = (u8)(ss_ap.sp_ap[i].topos[1] - '0');
			if(!ss_ap.sp_ap[i].topos[1]){
				action[x-1].action_value = value_2*10 + value_3;
			}
			else {
				action[x-1].action_value = value_3;
			}
			action[x-1].action_cmd = 0x55;
			action[x-1].action_timeout = ss_ap.sp_ap[i].timeout;
			for(j = 0; j < 5;j++){
				for(k =0;k < 15;k++){
					if(strncmp(ss_ap.sp_ap[i].sepid,ss.sc[j].spc[k].deviceid,8)==0){
						mymemcpy(action[x-1].deviceid,ss.sc[j].spc[k].deviceid,8);
						action[x-1].message_id = j+75;
						action[x-1].mesh_id_H = (u8)((ss.sc[j].meshid&0xFF00)>>8);
						action[x-1].mesh_id_L = (u8)(ss.sc[j].meshid&0x00FF);
						action[x-1].action_channel += (u8)(ss.sc[j].spc[k].MDID<<4);
					}
				}
			}			
			break;
		}
	}
	return type;
}
//接收到eSH qe时发送相应sicp action指令，0x51~0x55,0x56，本函数放入100ms定时中
void sicp_qe_action_cmd(void)
{
	u8 i,j;
	if(ack_qe){
		ack_qe = 0;
		if(ss_qe.MDID){//说明快速执行的是SLC或SPC
			if((ss_qe.action[0] == 'D') && (ss_qe.action[1] == 'M')){
				for(i = 0; i < 5;i++){
					for(j = 0; j < 15;j++){
						//查找是哪一个SC下的哪一个SLC，填入相应的message id
						//if(strncmp(ss_qe.sepid,ss.sc[i].slc[j].deviceid,8)==0){
						if(strncmp(ss_qe.sepid,ss.sc[i].deviceid,8)==0){
							mymemcpy(action_qe.deviceid,ss.sc[i].slc[j].deviceid,8);
							action_qe.message_id = i+100;
							action_qe.mesh_id_H = (u8)((ss.sc[i].meshid&0xFF00)>>8);
							action_qe.mesh_id_L = (u8)(ss.sc[i].meshid&0x00FF);
							action_qe.action_channel = (u8)(ss_qe.MDID<<4) + ss_qe.CH;
							action_qe.action_cmd = 0x51;//qe快速指令默认用Linear
							action_qe.action_value = ss_qe.topos;
							action_qe.action_ext = 0x1E;
							send_action_message(&action_qe);
							break;
						}
					}
				}
			}
			else if((ss_qe.action[0] == 'W') && (ss_qe.action[1] == 'P')){
				for(i = 0; i < 5;i++){
					for(j = 0; j < 15;j++){
						//查找是哪一个SC下的哪一个SPC，填入相应的message id
						//if(strncmp(ss_qe.sepid,ss.sc[i].spc[j].deviceid,8)==0){
						if(strncmp(ss_qe.sepid,ss.sc[i].deviceid,8)==0){
							mymemcpy(action_qe.deviceid,ss.sc[i].spc[j].deviceid,8);
							action_qe.message_id = i+100;
							action_qe.mesh_id_H = (u8)((ss.sc[i].meshid&0xFF00)>>8);
							action_qe.mesh_id_L = (u8)(ss.sc[i].meshid&0x00FF);
							action_qe.action_channel = (u8)(ss_qe.MDID<<4) + ss_qe.CH;
							action_qe.action_cmd = 0x55;//qe快速指令默认用Linear
							action_qe.action_value = ss_qe.topos;
							action_qe.action_ext = 0;
							send_action_message(&action_qe);
							break;
						}
					}
				}
			}
		}
		if((ss_qe.action[0] == 'C') && (ss_qe.action[1] == 'P')){//快速执行的是ST
			for(i = 0; i < 20; i++){
				//查找是哪一个ST，填入相应的message id
				if(strncmp(ss_qe.sepid,ss.st[i].deviceid,8)==0){
					mymemcpy(action_qe.deviceid,ss.st[i].deviceid,8);
					action_qe.message_id = i+80;
					action_qe.mesh_id_H = (u8)((ss.st[i].meshid&0xFF00)>>8);
					action_qe.mesh_id_L = (u8)(ss.st[i].meshid&0x00FF);
					action_qe.action_channel = ss_qe.CH;
					action_qe.action_cmd = 0x56;//qe快速指令默认用Linear
					action_qe.action_value = ss_qe.topos;
					action_qe.action_ext = 0;
					send_action_message(&action_qe);
				}
			}
		}
	}
}



void send_action_message(ACTION *ac)
{
	SICP_Message action_cmd;
		action_cmd.frame_h1 = 0xEE;
		action_cmd.frame_h2 = 0xEE;
		action_cmd.message_id = ac->message_id;
		action_cmd.mesh_id_H = ac->mesh_id_H;
		action_cmd.mesh_id_L = ac->mesh_id_L;
		switch(ac->action_cmd){
			case 0x51:
			case 0x52:
			case 0x53:
			case 0x54:
			case 0x55:
			case 0x56:
				action_cmd.payload[0] = ac->action_cmd;
				action_cmd.payload[1] = ac->action_channel;
				action_cmd.payload[2] = ac->action_value;
				action_cmd.payload[3] = ac->action_ext;
				//action_cmd.payload[4] = '\0';
				break;
			case 0x57:
				break;
			default:
				break;
		}
		sicp_send_message(&action_cmd,4);
}

//接收到eSH action_perform时发送相应sicp action指令，0x51~0x55，本函数放入100ms定时中
void sicp_action_cmd(void)
{
	u8 i;
	if(ack_ap_rev_success){
		if(ss_ap.qos > 2){//说明有配置指令
			//按seqid顺序发送指令
			action_type = get_ss_ap(1);
			if(!action_type)	{
				ack_ap_rev_success = 0;//找不到第1条指令，此次ssp接收到actions/perform无效
				return;
			}
			else{
				action[0].action_ing = 1;
				for(i = 2;i <= ss_ap.qos-3;i++)
					get_ss_ap(i);
				ack_ap_rev_success = 0;//转存到action结构体数组后清除ack_ap_rev_success
			}
		}
	}
	if(action[0].action_ing){//等待第1条指令执行发送
		action_timecnt++;
		if(action_timecnt >= action[0].action_timeout*10){
			action[0].action_ing = 0;
			action_timecnt = 0;
			send_action_message(&action[0]);
			action[1].action_ing = 1;
			action_sendcnt++;
			if(action_sendcnt >= ss_ap.qos-2)	{
				for(i = 0; i < 10;i++)	action[i].action_ing = 0;
			}
		}
	}
	if(action[1].action_ing){//等待第2条指令执行发送
		action_timecnt++;
		if(action_timecnt >= action[1].action_timeout*10){
			action[1].action_ing = 0;
			action_timecnt = 0;
			send_action_message(&action[1]);
			action[2].action_ing = 1;
			action_sendcnt++;
			if(action_sendcnt >= ss_ap.qos-2)	{
				for(i = 0; i < 10;i++)	action[i].action_ing = 0;
			}
		}
	}
	if(action[2].action_ing){//等待第1条指令执行发送
		action_timecnt++;
		if(action_timecnt >= action[2].action_timeout*10){
			action[2].action_ing = 0;
			action_timecnt = 0;
			send_action_message(&action[2]);
			action[3].action_ing = 1;
			action_sendcnt++;
			if(action_sendcnt >= ss_ap.qos-2)	{
				for(i = 0; i < 10;i++)	action[i].action_ing = 0;
			}
		}
	}
	if(action[3].action_ing){//等待第1条指令执行发送
		action_timecnt++;
		if(action_timecnt >= action[3].action_timeout*10){
			action[3].action_ing = 0;
			action_timecnt = 0;
			send_action_message(&action[3]);
			action[4].action_ing = 1;
			action_sendcnt++;
			if(action_sendcnt >= ss_ap.qos-2)	{
				for(i = 0; i < 10;i++)	action[i].action_ing = 0;
			}
		}
	}
	if(action[4].action_ing){//等待第1条指令执行发送
		action_timecnt++;
		if(action_timecnt >= action[4].action_timeout*10){
			action[4].action_ing = 0;
			action_timecnt = 0;
			send_action_message(&action[4]);
			action[5].action_ing = 1;
			action_sendcnt++;
			if(action_sendcnt >= ss_ap.qos-2)	{
				for(i = 0; i < 10;i++)	action[i].action_ing = 0;
			}
		}
	}
	if(action[5].action_ing){//等待第1条指令执行发送
		action_timecnt++;
		if(action_timecnt >= action[5].action_timeout*10){
			action[5].action_ing = 0;
			action_timecnt = 0;
			send_action_message(&action[5]);
			action[6].action_ing = 1;
			action_sendcnt++;
			if(action_sendcnt >= ss_ap.qos-2)	{
				for(i = 0; i < 10;i++)	action[i].action_ing = 0;
			}
		}
	}
	if(action[6].action_ing){//等待第1条指令执行发送
		action_timecnt++;
		if(action_timecnt >= action[6].action_timeout*10){
			action[6].action_ing = 0;
			action_timecnt = 0;
			send_action_message(&action[6]);
			action[7].action_ing = 1;
			action_sendcnt++;
			if(action_sendcnt >= ss_ap.qos-2)	{
				for(i = 0; i < 10;i++)	action[i].action_ing = 0;
			}
		}
	}
	if(action[7].action_ing){//等待第1条指令执行发送
		action_timecnt++;
		if(action_timecnt >= action[7].action_timeout*10){
			action[7].action_ing = 0;
			action_timecnt = 0;
			send_action_message(&action[7]);
			action[8].action_ing = 1;
			action_sendcnt++;
			if(action_sendcnt >= ss_ap.qos-2)	{
				for(i = 0; i < 10;i++)	action[i].action_ing = 0;
			}
		}
	}
	if(action[8].action_ing){//等待第1条指令执行发送
		action_timecnt++;
		if(action_timecnt >= action[8].action_timeout*10){
			action[8].action_ing = 0;
			action_timecnt = 0;
			send_action_message(&action[8]);
			action[9].action_ing = 1;
			action_sendcnt++;
			if(action_sendcnt >= ss_ap.qos-2)	{
				for(i = 0; i < 10;i++)	action[i].action_ing = 0;
			}
		}
	}
	if(action[9].action_ing){//等待第1条指令执行发送
		action_timecnt++;
		if(action_timecnt >= action[9].action_timeout*10){
			action[9].action_ing = 0;
			action_timecnt = 0;
			send_action_message(&action[9]);
			action_sendcnt++;
			if(action_sendcnt >= ss_ap.qos-2)	{
				for(i = 0; i < 10;i++)	action[i].action_ing = 0;
			}
		}
	}
}

//收到ssp action backlight时发送LED command，此函数放入Task100ms()
void sicp_led_cmd(void)
{
	u8 i;
	SICP_Message led_cmd;
	if(ack_ab){
		ack_ab = 0;
		for(i = 0; i < 20;i++){
			if(strncmp(ss.st[i].deviceid,ss_ab.sepid,8)){
				switch(ss_ab.mode){
					case 1:
						led_cmd.frame_h1 = 0xEE;
						led_cmd.frame_h2 = 0xEE;
						led_cmd.message_id = 110+i;
						led_cmd.mesh_id_H = (u8)((ss.st[i].meshid&0xFF00)>>8);
						led_cmd.mesh_id_L = (u8)(ss.st[i].meshid&0x00FF);
						led_cmd.payload[0] = 0x09;
						led_cmd.payload[1] = (u8)(ss_ab.mode<<4) + (u8)ss_ab.display;
						led_cmd.payload[2] = ss_ab.density;
						led_cmd.payload[3] = ss_ab.speed;
						for(i = 0; i < ss_ab.color_num;i++){
							led_cmd.payload[4+(i*3)] = (u8)(ascii2hex(ss_ab.color[i][0])<<4)+(u8)(ascii2hex(ss_ab.color[i][1]));
							led_cmd.payload[4+(i*3)+1] = (u8)(ascii2hex(ss_ab.color[i][2])<<4)+(u8)(ascii2hex(ss_ab.color[i][3]));
							led_cmd.payload[4+(i*3)+2] = (u8)(ascii2hex(ss_ab.color[i][4])<<4)+(u8)(ascii2hex(ss_ab.color[i][5]));
						}
						//led_cmd.payload[4+((i-1)*3)+3] = '\0';
						sicp_send_message(&led_cmd,(4+((i-1)*3)+3));
						break;
					case 2:
						led_cmd.frame_h1 = 0xEE;
						led_cmd.frame_h2 = 0xEE;
						led_cmd.message_id = 110+i;
						led_cmd.mesh_id_H = (u8)((ss.st[i].meshid&0xFF00)>>8);
						led_cmd.mesh_id_L = (u8)(ss.st[i].meshid&0x00FF);
						led_cmd.payload[0] = 0x09;
						led_cmd.payload[1] = (u8)(ss_ab.mode<<4) + (u8)ss_ab.display;
						led_cmd.payload[2] = ss_ab.time.in;
						led_cmd.payload[3] = ss_ab.time.duration;
						led_cmd.payload[4] = ss_ab.time.out;
						led_cmd.payload[5] = ss_ab.time.blank;
						for(i = 0; i < ss_ab.color_num;i++){
							led_cmd.payload[6+(i*3)] = (u8)(ascii2hex(ss_ab.color[i][0])<<4)+(u8)(ascii2hex(ss_ab.color[i][1]));
							led_cmd.payload[6+(i*3)+1] = (u8)(ascii2hex(ss_ab.color[i][2])<<4)+(u8)(ascii2hex(ss_ab.color[i][3]));
							led_cmd.payload[6+(i*3)+2] = (u8)(ascii2hex(ss_ab.color[i][4])<<4)+(u8)(ascii2hex(ss_ab.color[i][5]));
						}
						//led_cmd.payload[6+((i-1)*3)+3] = '\0';
						sicp_send_message(&led_cmd,(6+((i-1)*3)+3));	
						break;
					case 3:
						led_cmd.frame_h1 = 0xEE;
						led_cmd.frame_h2 = 0xEE;
						led_cmd.message_id = 110+i;
						led_cmd.mesh_id_H = (u8)((ss.st[i].meshid&0xFF00)>>8);
						led_cmd.mesh_id_L = (u8)(ss.st[i].meshid&0x00FF);
						led_cmd.payload[0] = 0x09;
						led_cmd.payload[1] = (u8)(ss_ab.mode<<4) + (u8)ss_ab.display;
						led_cmd.payload[2] = ss_ab.time.in;
						led_cmd.payload[3] = ss_ab.time.duration;
						for(i = 0; i < ss_ab.color_num;i++){
							led_cmd.payload[4+(i*3)] = (u8)(ascii2hex(ss_ab.color[i][0])<<4)+(u8)(ascii2hex(ss_ab.color[i][1]));
							led_cmd.payload[4+(i*3)+1] = (u8)(ascii2hex(ss_ab.color[i][2])<<4)+(u8)(ascii2hex(ss_ab.color[i][3]));
							led_cmd.payload[4+(i*3)+2] = (u8)(ascii2hex(ss_ab.color[i][4])<<4)+(u8)(ascii2hex(ss_ab.color[i][5]));
						}
						//led_cmd.payload[4+((i-1)*3)+3] = '\0';
						sicp_send_message(&led_cmd,(4+((i-1)*3)+3));	
						break;
					case 4:
					case 5:
						led_cmd.frame_h1 = 0xEE;
						led_cmd.frame_h2 = 0xEE;
						led_cmd.message_id = 110+i;
						led_cmd.mesh_id_H = (u8)((ss.st[i].meshid&0xFF00)>>8);
						led_cmd.mesh_id_L = (u8)(ss.st[i].meshid&0x00FF);
						led_cmd.payload[0] = 0x09;
						led_cmd.payload[1] = (u8)(ss_ab.mode<<4) + 0x0F;
						//led_cmd.payload[2] = '\0';
						sicp_send_message(&led_cmd,2);
						break;
					default:
						break;
					}
				break;//跳出for循环
			}
		}
		success_receipt();
	}
}


//收到ssp alarm时发送Alert command，此函数放入Task100ms()
void sicp_alert_cmd(void)
{
	u8 i;
	SICP_Message alert_cmd;
	if(ack_alarm){
		ack_alarm = 0;
		for(i = 0; i < 20; i++){
			alert_cmd.frame_h1 = 0xEE;
			alert_cmd.frame_h2 = 0xE2;
			alert_cmd.message_id = 130+i;
			alert_cmd.mesh_id_H = (u8)((ss.st[i].meshid&0xFF00)>>8);
			alert_cmd.mesh_id_L = (u8)(ss.st[i].meshid&0x00FF);
			alert_cmd.payload[0] = 0x05;
			alert_cmd.payload[1] = ss_alarm.level;
			//alert_cmd.payload[2] = '\0';
			sicp_send_message(&alert_cmd,2);
		}
		for(i = 0; i < 5;i++){
			alert_cmd.frame_h1 = 0xEE;
			alert_cmd.frame_h2 = 0xEE;
			alert_cmd.message_id = 140+i;
			alert_cmd.mesh_id_H = (u8)((ss.sc[i].meshid&0xFF00)>>8);
			alert_cmd.mesh_id_L = (u8)(ss.sc[i].meshid&0x00FF);
			alert_cmd.payload[0] = 0x05;
			alert_cmd.payload[1] = ss_alarm.level;
			//alert_cmd.payload[2] = '\0';
			sicp_send_message(&alert_cmd,2);
		}
	}
}


