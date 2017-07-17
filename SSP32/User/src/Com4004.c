
/* Includes ------------------------------------------------------------------*/
#define _COM4004_GLOBAL
#include "includes.h"
#include <stdlib.h>


int mystrcmp(unsigned char *str1,const unsigned char *str2)
{
	int ret=0;
	while(!(ret=(*(unsigned char *)str2 - *(unsigned char *)str1)&&*str2))
	{
		str1++;
		str2++;
	}
	if(ret<0)
	ret=-1;
	else if(ret>0)
	ret=1;
	return ret;
}

void mystrncat(char *desc,char *src,u32 len)
{
	while(len--)
	{
		*desc++ = *src++;
	}
}



/**
  * @brief  产生随机数 1~255
  * @param  None.
  * @retval None
  */
u8 random(u8 xxx)  
{  
  u8 value,iii;  
  for(iii=0;iii<xxx;iii++)  
  {  
    value = rand() % (MAX + 1- MIN) + MIN; //产生随机数1~255
	}  
  return value;  
} 



void analyze(void)
{
	if (rev_success)
	{
		rev_success = 0;
		if(ssp_buf[2] == 0x41)	{rev_heartbeat = 1;rev_heart_beat(0x83);}
		else if(ssp_parse(ssp_buf,ssp_length))
		{
			rev_analyze(Rxfr4004.rx_var_header.topic,Rxfr4004.rx_payload);
		}
		//接收到的数据可能是ssp心跳包/数据包/4004心跳包
		if(uartTxSLHead){//链表头不为空
			Usart1_Send(uartTxSLHead->data,uartTxSLHead->len); //uartTxSLHead->haswrite在中断中处理
		}
	}
}

//解析ssp_buf内容，存到Rxfr4004结构体中
u8 ssp_parse(u8 *buf,u16 buf_len)
{
	u8 *temp_buf;
	u32 useful_data_len;
	u8  remaining_len=0;
	u8  topic_len;
	u16 payload_len;
	temp_buf = buf;
	if((temp_buf[0] == 0xBB) && (temp_buf[1] == 0xBB))
	{
		Rxfr4004.rx_fix_header.ch.first_ch_byte = temp_buf[2];
		if((temp_buf[3] & 0x80) == 0x00)
		{
			remaining_len = 1;
			useful_data_len = temp_buf[3];
			if(useful_data_len != buf_len - 4)//接收到的有效数据长度与实际收到的数据不等，错误，不接收本包数据
			{
				return 0;
			}
		}
		else if((temp_buf[4] & 0x80) == 0x00)
		{
			remaining_len = 2;
			//useful_data_len = (temp_buf[3]& 0x7F)*128 + temp_buf[4];
			useful_data_len = (temp_buf[3]-0x80) + temp_buf[4]*128;
			if(useful_data_len != buf_len - 5)//接收到的有效数据长度与实际收到的数据不等，错误，不接收本包数据
			{
				return 0;
			}
		}
		else if((temp_buf[5] & 0x80) == 0x00)
		{
			remaining_len = 3;
			//useful_data_len = (temp_buf[3]& 0x7F)*16384 + (temp_buf[4]& 0x7F)*128 + temp_buf[5];
			useful_data_len = (temp_buf[3]-0x80) + ((temp_buf[4]& 0x7F)*128) + temp_buf[5]*16384;
			if(useful_data_len != buf_len - 6)//接收到的有效数据长度与实际收到的数据不等，错误，不接收本包数据
			{
				return 0;
			}
		}
		else if((temp_buf[6] & 0x80) == 0x00)
		{
			remaining_len = 4;
			//useful_data_len = (temp_buf[3]& 0x7F)*2097126 + (temp_buf[4]& 0x7F)*16384 + (temp_buf[5]& 0x7F)*128 + temp_buf[6];
			useful_data_len = (temp_buf[3]-0x80) + ((temp_buf[4]& 0x7F)*128) + ((temp_buf[5]&0x7F)*16384) + temp_buf[6]*2097152;
			if(useful_data_len != buf_len - 7)//接收到的有效数据长度与实际收到的数据不等，错误，不接收本包数据
			{
				return 0;
			}
		}
		if(remaining_len)
		{
			Rxfr4004.rx_var_header.version = temp_buf[remaining_len+3];
			Rxfr4004.rx_var_header.topic_lengthH = temp_buf[remaining_len+4];
			topic_len = Rxfr4004.rx_var_header.topic_lengthL = temp_buf[remaining_len+5];
			payload_len = useful_data_len - 3 - topic_len;
			mymemcpy(Rxfr4004.rx_var_header.topic,(temp_buf+remaining_len+6),topic_len);
			Rxfr4004.rx_var_header.message_id_H = temp_buf[remaining_len+5+topic_len+1];
			Rxfr4004.rx_var_header.message_id_L = temp_buf[remaining_len+5+topic_len+2];
			mymemcpy(Rxfr4004.rx_payload,(temp_buf+remaining_len+8+topic_len),payload_len);
			return 1;
		}
		else
		{
			return 0;
		}
		
	}
	else
	{
		return 0;
	}

}



//解析topic和padload内容
void rev_analyze(u8 *topic_buf,u8 *cjson_buf)
{
	u8 i,deepsyn_topic=0;
	u8 *temp_topicbuf,*temp_cjsonbuf;
	temp_topicbuf = topic_buf;
	temp_cjsonbuf	= cjson_buf;
	for(i = 0;i < Rxfr4004.rx_var_header.topic_lengthL;i++){
		if(*(temp_topicbuf+i) == 0x2F)//0x2F为'/'字符
			deepsyn_topic++;
	}
	if(deepsyn_topic <= 3)//非深度指令
	{
		if (strncmp(temp_topicbuf,"/actions/perform",Rxfr4004.rx_var_header.topic_lengthL) == 0)
		{
			deal_action_perform(temp_cjsonbuf);
			ack_ap_rev_success = 1;
			ss_ap_message_id_H = Rxfr4004.rx_var_header.message_id_H;
			ss_ap_message_id_L = Rxfr4004.rx_var_header.message_id_L;
			send_message_without_payload(0x85,0x01,ss_ap_message_id_H,ss_ap_message_id_L,0x00);
		}
		else if (strncmp(temp_topicbuf,"/config/ss",Rxfr4004.rx_var_header.topic_lengthL) == 0)
		{
			deal_config_ss(temp_cjsonbuf);//回复在deal函数中
		}
		else if(strncmp(temp_topicbuf,"/actions/refresh",Rxfr4004.rx_var_header.topic_lengthL) == 0)
		{
			ack_ar = 1;
			//回复执行回执,在if(ack_ar)处执行
		}
		else if(strncmp(temp_topicbuf,"/actions/backlight",Rxfr4004.rx_var_header.topic_lengthL) == 0)
		{
			deal_action_backlight(temp_cjsonbuf);
			ack_ab = 1;
			//回复执行回执,在if(ack_ab)处执行
		}
		else if(strncmp(temp_topicbuf,"/data/sync",Rxfr4004.rx_var_header.topic_lengthL) == 0)
		{
			ack_ds = 1;
			send_data_sync(0x83);
		}
		else if(strncmp(temp_topicbuf,"/data/recent",Rxfr4004.rx_var_header.topic_lengthL) == 0)
		{
			ack_dr = 1;//强制刷新，回复内容在task100ms的if(ack_dr)中执行
		}
		else if(strncmp(temp_topicbuf,"/config/ss",Rxfr4004.rx_var_header.topic_lengthL) == 0)
		{
			if(ssp_buf[2] == 0x43)	{
				deal_config_ss(temp_cjsonbuf);//回复在deal函数中	
				ack_cs = 1;
			}
		}
		else if(strncmp(temp_topicbuf,"/data/ir",Rxfr4004.rx_var_header.topic_lengthL) == 0)
		{
			ack_dir_rev_success = 1;
			ss_di_message_id_H = Rxfr4004.rx_var_header.message_id_H;
			ss_di_message_id_L = Rxfr4004.rx_var_header.message_id_L;
			send_message_without_payload(0x83,0x01,ss_di_message_id_H,ss_di_message_id_L,0x00);
		}
		else if(strncmp(temp_topicbuf,"/device/list",Rxfr4004.rx_var_header.topic_lengthL) == 0)
		{
			deal_device_list(temp_cjsonbuf);//回复在deal函数中
			ack_dl = 1;
		}
		else if(strncmp(temp_topicbuf,"/config/mesh",Rxfr4004.rx_var_header.topic_lengthL) == 0)
		{
			deal_config_mesh(temp_cjsonbuf);//回复在deal函数中
			ack_cm = 1;
		}
		else if(strncmp(temp_topicbuf,"/config/st",Rxfr4004.rx_var_header.topic_lengthL) == 0)
		{
			deal_config_st(temp_cjsonbuf);//回复在deal函数中
			ack_cst = 1;
		}
		else if(strncmp(temp_topicbuf,"/device/status",Rxfr4004.rx_var_header.topic_lengthL) == 0)
		{
			ack_des = 1;//回复放在Task100mszhon
			ss_des_message_id_H = Rxfr4004.rx_var_header.message_id_H;
			ss_des_message_id_L = Rxfr4004.rx_var_header.message_id_L;
		}
		else if(strncmp(temp_topicbuf,"/device/info/ss",Rxfr4004.rx_var_header.topic_lengthL) == 0)
		{
			ack_diss = 1;
			//ready_ss_post = 1;
			//send_device_info_sub();
			send_device_info_ss(0x02,Rxfr4004.rx_var_header.message_id_H,Rxfr4004.rx_var_header.message_id_L);
		}
		else if(strncmp(temp_topicbuf,"/config/strategy/hpst",Rxfr4004.rx_var_header.topic_lengthL) == 0)
		{
			deal_config_stragy_hpst(temp_cjsonbuf);//回复在deal函数中
			ack_cshp = 1;
		}
	}
	else//深度指令
	{
		if(strncmp(temp_topicbuf,"/actions/refresh",15) == 0)
		{
			deal_action_refresh(temp_topicbuf);//buf是topic的内容
		}
		else if(strncmp(temp_topicbuf,"/data/sync",10) == 0)
		{
			deal_data_sync(temp_topicbuf);//回复在deal函数中
			ack_ds2 = 1;
		}
		else if(strncmp(temp_topicbuf,"/config/ss",10) == 0)
		{
			deal_deepsyn_config_ss(temp_topicbuf,temp_cjsonbuf);//回复在deal函数中
			ack_cs = 1;
		}
		else if(strncmp(temp_topicbuf,"/device/status",14) == 0)
		{
			deal_device_status(temp_topicbuf);//回复在deal函数中
			//ack_des2 ack_des3在deal_device_status中赋值
		}
		else if(*(temp_topicbuf+1)== 'q' && *(temp_topicbuf+2)=='e')
		{
			deal_qe(temp_topicbuf);//回复在Task100ms中
			ack_qe = 1;
			ss_qe_message_id_H = Rxfr4004.rx_var_header.message_id_H;
			ss_qe_message_id_L = Rxfr4004.rx_var_header.message_id_L;
			
		}
		else if(strncmp(temp_topicbuf,"/alarm",6) == 0)//alarm指令
		{
			dela_alarm(temp_topicbuf);//回复在deal函数中
			ack_alarm = 1;
		}
		else if(strncmp(temp_topicbuf,"/config/strategy/htsp",21) == 0)
		{
			deal_config_strategy_htsp(temp_topicbuf);//回复在deal函数中
		}
	}
}
//解析config_ss指令，数据存储在ss_cs中
void deal_config_ss(u8 *buf)
{
	cJSON *root,*tmp,*level1,*level2;
	root = cJSON_Parse((char*)buf);
	if (root)
	{
		level1 = cJSON_GetObjectItem(root,"system");
		if(level1)
		{
			tmp = cJSON_GetObjectItem(level1,"HPST");
			if(tmp)
			{
				ss_cs.hpst = tmp->valueint;
				//printf("HPST: %d\n",ss_cs.hpst);
			}
			tmp = cJSON_GetObjectItem(level1,"REFR");
			if(tmp)
			{
				ss_cs.refr = tmp->valueint;
				sicp_refr = ss_cs.refr;
				//printf("REFR: %d\n",ss_cs.refr);
			}
		}
		level1 = cJSON_GetObjectItem(root,"wifi");
		if(level1)
		{
			tmp = cJSON_GetObjectItem(level1,"ssid");
			if(tmp && tmp->valuestring)
			{
				mymemcpy(ss_cs.ssid,tmp->valuestring,strlen(tmp->valuestring));
				//printf("ssid: %s\n",ss_cs.ssid);
			}
			tmp = cJSON_GetObjectItem(level1,"passwd");
			if(tmp && tmp->valuestring)
			{
				mymemcpy(ss_cs.paswd,tmp->valuestring,strlen(tmp->valuestring));
				//printf("paswd: %s\n",ss_cs.paswd);
			}
		}
		tmp = cJSON_GetObjectItem(root,"time");
		if(tmp)
		{
			ss_cs.time = tmp->valueint;
			//printf("time: %d\n",ss_cs.time);
		}
		level1 = cJSON_GetObjectItem(root,"location");
		if(level1)
		{
			tmp = cJSON_GetObjectItem(level1,"city");
			if(tmp && tmp->valuestring)
			{
				mymemcpy(ss_cs.city,tmp->valuestring,strlen(tmp->valuestring));
				//printf("city: %s\n",ss_cs.city);
			}
			tmp = cJSON_GetObjectItem(level1,"country");
			if(tmp && tmp->valuestring)
			{
				mymemcpy(ss_cs.country,tmp->valuestring,strlen(tmp->valuestring));
				//printf("country: %s\n",ss_cs.country);
			}
			tmp = cJSON_GetObjectItem(level1,"region");
			if(tmp && tmp->valuestring)
			{
				mymemcpy(ss_cs.region,tmp->valuestring,strlen(tmp->valuestring));
				//printf("region: %s\n",ss_cs.region);
			}
		}
		level1 = cJSON_GetObjectItem(root,"weather");
		if(level1)
		{
			level2 = cJSON_GetObjectItem(level1,"astronomy");
			if (level2)
			{
				tmp = cJSON_GetObjectItem(level2,"sunrise");
				if(tmp && tmp->valuestring)
				{
					mymemcpy(ss_cs.sunrise,tmp->valuestring,strlen(tmp->valuestring));
					//printf("sunrise: %s\n",ss_cs.sunrise);
				}
				tmp = cJSON_GetObjectItem(level2,"sunset");
				if(tmp && tmp->valuestring)
				{
					mymemcpy(ss_cs.sunset,tmp->valuestring,strlen(tmp->valuestring));
					//printf("sunset: %s\n",ss_cs.sunset);
				}
			}
			level2 = cJSON_GetObjectItem(level1,"atmosphere");
			if(level2)
			{
				tmp = cJSON_GetObjectItem(level2,"HM");
				if(tmp && tmp->valuestring)
				{
					mymemcpy(ss_cs.hm,tmp->valuestring,strlen(tmp->valuestring));
					//printf("HM: %s\n",ss_cs.hm);
				}
				tmp = cJSON_GetObjectItem(level2,"PR");
				if(tmp && tmp->valuestring)
				{
					mymemcpy(ss_cs.pr,tmp->valuestring,strlen(tmp->valuestring));
					//printf("PR: %s\n",ss_cs.pr);
				}
				tmp = cJSON_GetObjectItem(level2,"visibility");
				if(tmp && tmp->valuestring)
				{
					mymemcpy(ss_cs.visibility,tmp->valuestring,strlen(tmp->valuestring));
					//printf("visibility: %s\n",ss_cs.visibility);
				}
			}
			level2 = cJSON_GetObjectItem(level1,"condition");
			if (level2)
			{
				tmp = cJSON_GetObjectItem(level2,"code");
				if(tmp && tmp->valuestring)
				{
					mymemcpy(ss_cs.code,tmp->valuestring,strlen(tmp->valuestring));
					//printf("code: %s\n",ss_cs.code);
				}
				tmp = cJSON_GetObjectItem(level2,"TP");
				if(tmp && tmp->valuestring)
				{
					mymemcpy(ss_cs.tp,tmp->valuestring,strlen(tmp->valuestring));
					//printf("TP: %s\n",ss_cs.tp);
				}
			}
			level2 = cJSON_GetObjectItem(level1,"airquality");
			if (level2)
			{
				tmp = cJSON_GetObjectItem(level2,"index");
				if(tmp && tmp->valuestring)
				{
					mymemcpy(ss_cs.index,tmp->valuestring,strlen(tmp->valuestring));
					//printf("index: %s\n",ss_cs.index);
				}
			}
		}			
		cJSON_Delete(root);
		success_receipt();
	}
}




//解析config_ss深度同步指令，数据存储在ss_cs中
void deal_deepsyn_config_ss(u8 *buf1,u8 *buf2)//buf1是topic内容,buf2是payload内容
{
	char first_ch;
	//char *key;
	cJSON *root,*level1,*level2,*tmp;
	first_ch = (char)(*(buf1+12));
	//key = mymalloc(20);
	root = cJSON_Parse((char*)buf2);
	switch(first_ch)
	{
		case 's':
			if(root){
				tmp = cJSON_GetObjectItem(root,"HPST");	
				if(tmp){
					ss_cs.hpst = tmp->valueint;
				}
				tmp = cJSON_GetObjectItem(root,"HEFR");	
				if(tmp){
					ss_cs.refr = tmp->valueint;
				}
				cJSON_Delete(root);
			}
			break;
		case 'w':
			if(*(buf1+12) == 'i') {	//wifi
				if(root){
					tmp = cJSON_GetObjectItem(root,"ssid");
					if(tmp && tmp->valuestring){
						mymemcpy(ss_cs.ssid,tmp->valuestring,strlen(tmp->valuestring));
					}
					tmp = cJSON_GetObjectItem(root,"paswd");
					if(tmp && tmp->valuestring){
						mymemcpy(ss_cs.paswd,tmp->valuestring,strlen(tmp->valuestring));
					}
					cJSON_Delete(root);
				}
			}
			else	{									//weather
				if(root){
					level1 = cJSON_GetObjectItem(root,"weather");
					if(level1){
						level2 = cJSON_GetObjectItem(level1,"atmosphere");
						if(level2){
							tmp = cJSON_GetObjectItem(level2,"HM");
							if(tmp && tmp->valuestring){
								mymemcpy(ss_cs.hm,tmp->valuestring,strlen(tmp->valuestring));
							}
							tmp = cJSON_GetObjectItem(level2,"PR");
							if(tmp && tmp->valuestring){
								mymemcpy(ss_cs.pr,tmp->valuestring,strlen(tmp->valuestring));
							}
							tmp = cJSON_GetObjectItem(level2,"visibility");
							if(tmp && tmp->valuestring){
								mymemcpy(ss_cs.visibility,tmp->valuestring,strlen(tmp->valuestring));
							}
						}
						level2 = cJSON_GetObjectItem(level1,"astronomy");
						if(level2){
							tmp = cJSON_GetObjectItem(level2,"sunrise");
							if(tmp && tmp->valuestring){
								mymemcpy(ss_cs.sunrise,tmp->valuestring,strlen(tmp->valuestring));
							}
							tmp = cJSON_GetObjectItem(level2,"sunset");
							if(tmp && tmp->valuestring){
								mymemcpy(ss_cs.sunset,tmp->valuestring,strlen(tmp->valuestring));
							}
						}
						level2 = cJSON_GetObjectItem(level1,"condition");
						if(level2){
							tmp = cJSON_GetObjectItem(level2,"code");
							if(tmp && tmp->valuestring){
								mymemcpy(ss_cs.code,tmp->valuestring,strlen(tmp->valuestring));
							}
							tmp = cJSON_GetObjectItem(level2,"TP");
							if(tmp && tmp->valuestring){
								mymemcpy(ss_cs.tp,tmp->valuestring,strlen(tmp->valuestring));
							}
						}
						level2 = cJSON_GetObjectItem(level1,"airquality");
						if(level2){
							tmp = cJSON_GetObjectItem(level2,"index");
							if(tmp && tmp->valuestring){
								mymemcpy(ss_cs.index,tmp->valuestring,strlen(tmp->valuestring));
							}
						}
					}
					cJSON_Delete(root);
				}
			}
			break;
		case 't':
			if(root){
				tmp = cJSON_GetObjectItem(root,"time");
				if(tmp){
					ss_cs.time = tmp->valueint;
				}
				cJSON_Delete(root);
			}
			break;
		case 'l':
			if(root){
				level1 = cJSON_GetObjectItem(root,"location");
				if(level1){
					tmp = cJSON_GetObjectItem(level1,"city");
					if(tmp && tmp->valuestring){
						mymemcpy(ss_cs.city,tmp->valuestring,strlen(tmp->valuestring));
					}
					tmp = cJSON_GetObjectItem(level1,"country");
					if(tmp && tmp->valuestring){
						mymemcpy(ss_cs.country,tmp->valuestring,strlen(tmp->valuestring));
					}
					tmp = cJSON_GetObjectItem(level1,"region");
					if(tmp && tmp->valuestring){
						mymemcpy(ss_cs.region,tmp->valuestring,strlen(tmp->valuestring));
					}
				}
				cJSON_Delete(root);
			}
			break;
		default:
			break;
	}
	success_receipt();
}

//解析action_perform指令，数据存储在ss_ap中
void deal_action_perform(u8 *buf)
{
	cJSON *root,*tmp,*level1,*level2,*level3;
	int i,cmd_size;
	int sep_type;//sep_type为1时是SL，为2时是SC
	root = cJSON_Parse((char*)buf);
	if (root)
	{
		tmp = cJSON_GetObjectItem(root,"qos");
		if(tmp)
		{
			ss_ap.qos = tmp->valueint;
		}
		level1 = cJSON_GetObjectItem(root,"cmd");	
		if (level1)
		{
			if(level1->type == cJSON_Array)
			{
				cmd_size = cJSON_GetArraySize(level1);
				if(cmd_size)
				{
					for(i = 0;i < cmd_size;i++)
					{
						level2 = cJSON_GetArrayItem(level1,i);
						if(level2)
						{
							tmp = cJSON_GetObjectItem(level2,"sepid");
							if(tmp && tmp->valuestring)
							{
								if (*tmp->valuestring == 'S')
									if(*(tmp->valuestring+1) == 'L')
									{sep_type = 1;mymemcpy(ss_ap.sl_ap[ss_ap.sl_num].sepid,tmp->valuestring,strlen(tmp->valuestring));ss_ap.sl_num++;}
									else
									{sep_type = 2;mymemcpy(ss_ap.sp_ap[ss_ap.sp_num].sepid,tmp->valuestring,strlen(tmp->valuestring));ss_ap.sp_num++;}
							}
							tmp = cJSON_GetObjectItem(level2,"seqid");
							if(tmp)
							{
								if(sep_type == 1)	ss_ap.sl_ap[ss_ap.sl_num].seqid = tmp->valueint;
								else if(sep_type == 2)	ss_ap.sp_ap[ss_ap.sp_num].seqid = tmp->valueint;
							}
							tmp = cJSON_GetObjectItem(level2,"CH");
							if(tmp)
							{
								if(sep_type == 1)	ss_ap.sl_ap[ss_ap.sl_num].ch = tmp->valueint;
								else if(sep_type == 2)	ss_ap.sp_ap[ss_ap.sp_num].ch = tmp->valueint;
							}
							tmp = cJSON_GetObjectItem(level2,"action");
							if(tmp && tmp->valuestring)
							{
								if(sep_type == 1)	mymemcpy(ss_ap.sl_ap[ss_ap.sl_num].action,tmp->valuestring,strlen(tmp->valuestring));
								else if(sep_type == 2)	mymemcpy(ss_ap.sp_ap[ss_ap.sp_num].action,tmp->valuestring,strlen(tmp->valuestring));
							}
							tmp = cJSON_GetObjectItem(level2,"topos");
							if(tmp && tmp->valuestring)
							{
								if(sep_type == 1)	mymemcpy(ss_ap.sl_ap[ss_ap.sl_num].topos,tmp->valuestring,strlen(tmp->valuestring));
								else if(sep_type == 2)	mymemcpy(ss_ap.sp_ap[ss_ap.sp_num].topos,tmp->valuestring,strlen(tmp->valuestring));
							}
							level3 = cJSON_GetObjectItem(level2,"option");
							if(level3)
							{
								tmp = cJSON_GetObjectItem(level3,"durations");
								if(tmp)
								{
									ss_ap.sl_ap[ss_ap.sl_num].option.duration = tmp->valueint;
								}
								tmp = cJSON_GetObjectItem(level3,"erase");
								if(tmp)
								{
									ss_ap.sl_ap[ss_ap.sl_num].option.erase = tmp->valueint;
								}
							}
							tmp = cJSON_GetObjectItem(level2,"stseq");
							if(tmp)
							{
								if(sep_type == 1)	ss_ap.sl_ap[ss_ap.sl_num].stseq = tmp->valueint;
								else if(sep_type == 2)	ss_ap.sp_ap[ss_ap.sp_num].stseq = tmp->valueint;
								
							}
							tmp = cJSON_GetObjectItem(level2,"timeout");
							if(tmp)
							{
								if(sep_type == 1)	ss_ap.sl_ap[ss_ap.sl_num].timeout = tmp->valueint;
								else if(sep_type == 2)	ss_ap.sp_ap[ss_ap.sp_num].timeout = tmp->valueint;
							}
						}
					}
				}
			}
			else if(level1->type == cJSON_Object)//此时只有一个seqid，即只控制SL或SP
			{
				tmp = cJSON_GetObjectItem(level1,"sepid");
				if(tmp && tmp->valuestring)
				{
					if (*tmp->valuestring == 'S')
						if(*tmp->valuestring+1 == 'L')
						{sep_type = 1;mymemcpy(ss_ap.sl_ap[0].sepid,tmp->valuestring,strlen(tmp->valuestring));ss_ap.sl_num++;}
						else
						{sep_type = 2;mymemcpy(ss_ap.sp_ap[0].sepid,tmp->valuestring,strlen(tmp->valuestring));ss_ap.sp_num++;}
				}
				tmp = cJSON_GetObjectItem(level2,"seqid");
				if(tmp)
				{
					if(sep_type == 1)	ss_ap.sl_ap[0].seqid = tmp->valueint;
					else if(sep_type == 2)	ss_ap.sp_ap[0].seqid = tmp->valueint;
				}
				tmp = cJSON_GetObjectItem(level2,"CH");
				if(tmp)
				{
					if(sep_type == 1)	ss_ap.sl_ap[0].ch = tmp->valueint;
					else if(sep_type == 2)	ss_ap.sp_ap[0].ch = tmp->valueint;
				}
				tmp = cJSON_GetObjectItem(level2,"action");
				if(tmp && tmp->valuestring)
				{
					if(sep_type == 1)	mymemcpy(ss_ap.sl_ap[0].action,tmp->valuestring,strlen(tmp->valuestring));
					else if(sep_type == 2)	mymemcpy(ss_ap.sp_ap[0].action,tmp->valuestring,strlen(tmp->valuestring));
				}
				tmp = cJSON_GetObjectItem(level2,"topos");
				if(tmp && tmp->valuestring)
				{
					if(sep_type == 1)	mymemcpy(ss_ap.sl_ap[0].topos,tmp->valuestring,strlen(tmp->valuestring));
					else if(sep_type == 2)	mymemcpy(ss_ap.sp_ap[0].topos,tmp->valuestring,strlen(tmp->valuestring));
				}
				level3 = cJSON_GetObjectItem(level2,"option");
				if(level3)
				{
					tmp = cJSON_GetObjectItem(level3,"durations");
					if(tmp)
					{
						ss_ap.sl_ap[0].option.duration = tmp->valueint;
					}
					tmp = cJSON_GetObjectItem(level3,"erase");
					if(tmp)
					{
						ss_ap.sl_ap[0].option.erase = tmp->valueint;
					}
				}
				tmp = cJSON_GetObjectItem(level2,"stseq");
				if(tmp)
				{
					if(sep_type == 1)	ss_ap.sl_ap[0].stseq = tmp->valueint;
					else if(sep_type == 2)	ss_ap.sp_ap[0].stseq = tmp->valueint;
					
				}
				tmp = cJSON_GetObjectItem(level2,"timeout");
				if(tmp)
				{
					if(sep_type == 1)	ss_ap.sl_ap[0].timeout = tmp->valueint;
					else if(sep_type == 2)	ss_ap.sp_ap[0].timeout = tmp->valueint;
				}
			}
		}
		cJSON_Delete(root);
	}
}


//解析action_refresh深度同步指令
void deal_action_refresh(u8 *buf)//buf是topic的内容
{
	ss_ar.channel = (u8)(*(buf+15) - '0')*10 + (u8)(*(buf+16) - '0');
	ss_ar.ch[0] = *(buf+18);
	ss_ar.ch[1] = *(buf+19);
	if(((ss_ar.ch[0] == 'A') && (ss_ar.ch[1] == 'L')) || (ss_ar.ch[0] == 'C') && (ss_ar.ch[1] == 'S'))//刷新ST环境光颜色和强度
		{rev_ar2 = 1;success_receipt();}
	else//刷新的是SS的传感器
		{rev_ar1 = 1;success_receipt();}
}
//解析action_backlight指令，数据存储在ss_ab中

void deal_action_backlight(u8 *buf)
{
	cJSON *root,*tmp,*level1;
	int i,cmd_size;
	root = cJSON_Parse((char*)buf);
	if (root)
	{
		tmp = cJSON_GetObjectItem(root,"sepid");
		if(tmp && tmp->valuestring){
			mymemcpy(ss_ab.sepid,tmp->valuestring,8);
		}
		tmp = cJSON_GetObjectItem(root,"mode");
		if(tmp){
			ss_ab.mode = tmp->valueint;
		}
		level1 = cJSON_GetObjectItem(root,"color");	
		if(level1){
			if(level1->type == cJSON_Array){
				cmd_size = cJSON_GetArraySize(level1);
				ss_ab.color_num = cmd_size;
				if(cmd_size){
					for(i = 0;i < cmd_size;i++)
					{
						tmp = cJSON_GetArrayItem(level1,i);
						if(tmp && tmp->valuestring){
							mymemcpy((ss_ab.color+i),tmp->valuestring,strlen(tmp->valuestring));
						}
					}
				}
			}
		}
		level1 = cJSON_GetObjectItem(root,"time");
		if(level1){
			tmp = cJSON_GetObjectItem(level1,"in");
			if(tmp){
				ss_ab.time.in = tmp->valueint;
			}
			tmp = cJSON_GetObjectItem(level1,"duration");
			if(tmp){
				ss_ab.time.duration = tmp->valueint;
			}
			tmp = cJSON_GetObjectItem(level1,"out");
			if(tmp){
				ss_ab.time.out = tmp->valueint;
			}
			tmp = cJSON_GetObjectItem(level1,"blank");
			if(tmp){
				ss_ab.time.in = tmp->valueint;
			}
		}
		tmp = cJSON_GetObjectItem(root,"density");
		if(tmp){
			if (tmp->valueint != cmd_size)	ss_ab.density = cmd_size;
			else														ss_ab.density = tmp->valueint;
		}
		tmp = cJSON_GetObjectItem(root,"speed");
		if(tmp){
			ss_ab.speed = tmp->valueint;
		}
		tmp = cJSON_GetObjectItem(root,"display");
		if(tmp){
			ss_ab.display = tmp->valueint;
		}
		cJSON_Delete(root);
	}	
}


//解析data_sync指令，数据存储在ss_ab中
void deal_data_sync(u8 *buf)
{
	ss_ds.channel = (u8)(*(buf+15) - '0')*10 + (u8)(*(buf+16) - '0');
	ss_ds.ch[0] = *(buf+18);
	ss_ds.ch[1] = *(buf+19);
	send_deepin_data_sync();
}

//解析device_list指令，指令存在ss中,SC,SLC,SPC,ST的device id在这里赋值
void deal_device_list(u8 *buf)
{
	cJSON *root,*tmp,*level1,*level2;
	u8 double_break = 0;
	int i,j,k,cmd_size;
	root = cJSON_Parse((char*)buf);
	if(root)
	{
		tmp = cJSON_GetObjectItem(root,"deviceID");
		if(tmp && tmp->valuestring){
			if(strncmp(ss.deviceid,tmp->valuestring,8)==0){//配置的ID与本SS的ID相同才进行下一步
			level1 = cJSON_GetObjectItem(root,"devices");
				if(level1){
					if(level1->type == cJSON_Array){
						cmd_size = cJSON_GetArraySize(level1);
						if(cmd_size){
							for(i = 0;i < cmd_size;i++)
							{
								level2 = cJSON_GetArrayItem(level1,i);
								if(level2){
									tmp = cJSON_GetObjectItem(level2,"deviceID");
									if(tmp && tmp->valuestring){
										if((*tmp->valuestring == 'S')&&(*(tmp->valuestring+1) == 'L')){
											for(j = 0;j < 5;j++){
												for(k=0;k < 15;k++){
														//if(strncmp(ss.sc[j].slc[k].deviceid,tmp->valuestring,8)==0){//寻找到SC下对应ID的SLC
														if(ss.sc[j].slc[k].deviceid[0] == 0x00){//寻找device id为空的SLC
														mymemcpy(ss.sc[j].slc[k].deviceid,(tmp->valuestring+2),8);
														tmp = cJSON_GetObjectItem(level2,"model");
														if(tmp && tmp->valuestring){
															mymemcpy(ss.sc[j].slc[k].model,tmp->valuestring,strlen(tmp->valuestring));
														}
														tmp = cJSON_GetObjectItem(level2,"coord");
														if(tmp && tmp->valuestring){
															mymemcpy(ss.sc[j].slc[k].coord,tmp->valuestring,strlen(tmp->valuestring));
														}
														double_break = 1;
														break;
													}
												}
												if(double_break)	{double_break = 0;break;}
											}
										}
										else if((*tmp->valuestring == 'S')&&(*(tmp->valuestring+1) == 'P')){
											for(j = 0;j < 5;j++){
												for(k=0;k < 15;k++){
														//if(strncmp(ss.sc[j].spc[k].deviceid,tmp->valuestring,8)==0){//寻找到SC下对应ID的SPC
														if(ss.sc[j].spc[k].deviceid[0] == 0x00){//寻找device id为空的SPC
														mymemcpy(ss.sc[j].spc[k].deviceid,(tmp->valuestring+2),8);
														tmp = cJSON_GetObjectItem(level2,"model");
														if(tmp && tmp->valuestring){
															mymemcpy(ss.sc[j].spc[k].model,tmp->valuestring,strlen(tmp->valuestring));
														}
														tmp = cJSON_GetObjectItem(level2,"coord");
														if(tmp && tmp->valuestring){
															mymemcpy(ss.sc[j].spc[k].coord,tmp->valuestring,strlen(tmp->valuestring));
														}
														double_break = 1;
														break;
													}
												}
												if(double_break)	{double_break = 0;break;}
											}
										}
										else if((*tmp->valuestring == 'S')&&(*(tmp->valuestring+1) == 'C')){
											for(j = 0;j < 5;j++){//找寻SS下对应ID的SC
													//if(strncmp(ss.sc[i].deviceid,tmp->valuestring,8)){
													if(ss.sc[j].deviceid[0] == 0x00){//寻找device id为空的SC
													mymemcpy(ss.sc[j].deviceid,(tmp->valuestring+2),8);
													tmp = cJSON_GetObjectItem(level2,"model");
													if(tmp && tmp->valuestring){
														mymemcpy(ss.sc[j].model,tmp->valuestring,strlen(tmp->valuestring));
													}
													tmp = cJSON_GetObjectItem(level2,"coord");
													if(tmp && tmp->valuestring){
														mymemcpy(ss.sc[j].coord,tmp->valuestring,strlen(tmp->valuestring));
													}
													break;
												}	
											}
										}
										else if((*tmp->valuestring == 'S')&&(*(tmp->valuestring+1) == 'T')){
											for(j = 0;j < 20;j++){//找寻SS下deviceid为空的st
												//if(strncmp(ss.st[i].deviceid,tmp->valuestring,8)){
												if(ss.st[j].deviceid[0] == 0x00){//寻找device id为空的ST
													mymemcpy(ss.st[j].deviceid,(tmp->valuestring+2),8);
													tmp = cJSON_GetObjectItem(level2,"model");
													if(tmp && tmp->valuestring){
														mymemcpy(ss.st[j].model,tmp->valuestring,strlen(tmp->valuestring));
													}
													tmp = cJSON_GetObjectItem(level2,"coord");
													if(tmp && tmp->valuestring){
														mymemcpy(ss.st[j].coord,tmp->valuestring,strlen(tmp->valuestring));
													}
													break;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		cJSON_Delete(root);
		success_receipt();
	}
}

//解析config_mesh指令
void deal_config_mesh(u8 *buf)
{
	cJSON *root,*tmp;
	int i,j;
	root = cJSON_Parse((char*)buf);
	if(root)
	{
		for(i = 0; i < 5;i++){//判断是不是配置slc
			for(j = 0;j < 15;j++){
				if(!ss.sc[i].slc[j].deviceid[0]){//该deviceid不为空，已经保存有配置信息
					tmp = cJSON_GetObjectItem(root,ss.sc[i].slc[j].deviceid);
					if(tmp)		ss.sc[i].slc[j].meshid = (u16)tmp->valueint;
				}
			}
		}
		for(i = 0; i < 5;i++){//判断是不是配置spc
			for(j = 0;j < 15;j++)
			if(!ss.sc[i].spc[j].deviceid[0]){//该deviceid不为空，已经保存有配置信息
				tmp = cJSON_GetObjectItem(root,ss.sc[i].spc[j].deviceid);
				if(tmp)		ss.sc[i].spc[j].meshid = (u16)tmp->valueint;
			}
		}
		for(i = 0; i < 5;i++){//判断是不是配置sc
			if(!ss.sc[i].deviceid[0]){//该deviceid不为空，已经保存有配置信息
				tmp = cJSON_GetObjectItem(root,ss.sc[i].deviceid);
				if(tmp)		ss.sc[i].meshid = (u16)tmp->valueint;
			}
		}
		for(i = 0; i < 20;i++){//判断是不是配置st
			if(!ss.st[i].deviceid[0]){//该deviceid不为空，已经保存有配置信息
				tmp = cJSON_GetObjectItem(root,ss.st[i].deviceid);
				if(tmp)		ss.st[i].meshid = (u16)tmp->valueint;
			}
		}
		cJSON_Delete(root);
		success_receipt();
	}
}

//解析config_st指令
void deal_config_st(u8 *buf)
{
	cJSON *root,*tmp,*level1,*level2;
	int i,j,cmd_size;
	int index = 0;
	char deviceid[10];
	root = cJSON_Parse((char*)buf);
	ss_cst_count = 0;
	if(root)
	{
		for(j = 0; j < 20;j++){//寻找要配置的st是否为本ss所管理的
			deviceid[0] = 'S';
			deviceid[1] = 'T';
			mymemcpy((deviceid + 2),ss.st[j].deviceid,8);
			level1 = cJSON_GetObjectItem(root,deviceid);
			if(level1){//是本ss所管理的st
				if(level1->type == cJSON_Array){
					cmd_size = cJSON_GetArraySize(level1);
					if(cmd_size){
						ss_cst_count += cmd_size;
						for(i = 0;i < cmd_size;i++){
							level2 = cJSON_GetArrayItem(level1,i);
							if(level2){
								tmp = cJSON_GetObjectItem(level2,"type");
								if(tmp){								
									if(ss_cst[index].type == 0){//该cst[i]还没有被配置，注意：通过uart2发送给st后type需清零
										ss_cst[index].type = tmp->valueint;
										ss_cst[index].meshid = ss.st[j].meshid;
										tmp = cJSON_GetObjectItem(level2,"key");
										if(tmp)	ss_cst[index].key = tmp->valueint;
										tmp = cJSON_GetObjectItem(level2,"cond");
										if(tmp && tmp->valuestring)	mymemcpy(ss_cst[index].cond,tmp->valuestring,3);
										tmp = cJSON_GetObjectItem(level2,"targetID");
										if(tmp && tmp->valuestring)	mymemcpy(ss_cst[index].target_id,tmp->valuestring,strlen(tmp->valuestring));
										tmp = cJSON_GetObjectItem(level2,"MDID");
										if(tmp)	ss_cst[index].mdid = tmp->valueint;
										tmp = cJSON_GetObjectItem(level2,"CH");
										if(tmp)	ss_cst[index].ch = tmp->valueint;
										tmp = cJSON_GetObjectItem(level2,"action");
										if(tmp && tmp->valuestring)	mymemcpy(ss_cst[index].action,tmp->valuestring,2);
										tmp = cJSON_GetObjectItem(level2,"value");
										if(tmp)	ss_cst[index].value = tmp->valueint;
										else		ss_cst[index].value = 0;
										index++;
									}
								}
							}
						}
					}
				}
			}
			cJSON_Delete(root);
		}
		success_receipt();
	}
}
//解析config_stratgy_hpst命令
void deal_config_stragy_hpst(u8 *buf)
{
	cJSON *root,*tmp,*level1,*level2,*level3;
	int i,j,cmd_size;
	u16 len;
	int sep_type;//sep_type为1时是SL，为2时是SC
	root = cJSON_Parse((char*)buf);
	if (root)
	{
		tmp = cJSON_GetObjectItem(root,"cond");
		if(tmp && tmp->valuestring){
			len = strlen(tmp->valuestring);
			for(j=0;j < 20;j++){
				if(strncmp(tmp->valuestring,ss_cshp[j].cond,len) != 0){
					ss_cshp[j].id = j;
					mymemcpy(ss_cshp[j].cond,tmp->valuestring,len);
					break;
				}
				else{//已经接收过该cond
					return;
				}
			}
		}
		level1 = cJSON_GetObjectItem(root,"cmd");	
		if (level1){
			if(level1->type == cJSON_Array){
				cmd_size = cJSON_GetArraySize(level1);
				if(cmd_size){
					for(i = 0;i < cmd_size;i++){
						level2 = cJSON_GetArrayItem(level1,i);
						if(level2){
							tmp = cJSON_GetObjectItem(level2,"sepid");
							if(tmp && tmp->valuestring){
								if (*tmp->valuestring == 'S')
									if(*(tmp->valuestring+1) == 'L')
									{sep_type = 1;mymemcpy(ss_cshp[j].sl_ap[ss_cshp[j].sl_num].sepid,tmp->valuestring,strlen(tmp->valuestring));ss_cshp[j].sl_num++;}
									else
									{sep_type = 2;mymemcpy(ss_cshp[j].sp_ap[ss_cshp[j].sp_num].sepid,tmp->valuestring,strlen(tmp->valuestring));ss_cshp[j].sp_num++;}
							}
							tmp = cJSON_GetObjectItem(level2,"seqid");
							if(tmp)
							{
								if(sep_type == 1)	ss_cshp[j].sl_ap[ss_cshp[j].sl_num].seqid = tmp->valueint;
								else if(sep_type == 2)	ss_cshp[j].sp_ap[ss_cshp[j].sp_num].seqid = tmp->valueint;
							}
							tmp = cJSON_GetObjectItem(level2,"CH");
							if(tmp)
							{
								if(sep_type == 1)	ss_cshp[j].sl_ap[ss_cshp[j].sl_num].ch = tmp->valueint;
								else if(sep_type == 2)	ss_cshp[j].sp_ap[ss_cshp[j].sp_num].ch = tmp->valueint;
							}
							tmp = cJSON_GetObjectItem(level2,"action");
							if(tmp && tmp->valuestring)
							{
								if(sep_type == 1)	mymemcpy(ss_cshp[j].sl_ap[ss_cshp[j].sl_num].action,tmp->valuestring,strlen(tmp->valuestring));
								else if(sep_type == 2)	mymemcpy(ss_cshp[j].sp_ap[ss_cshp[j].sp_num].action,tmp->valuestring,strlen(tmp->valuestring));
							}
							tmp = cJSON_GetObjectItem(level2,"topos");
							if(tmp && tmp->valuestring)
							{
								if(sep_type == 1)	mymemcpy(ss_cshp[j].sl_ap[ss_cshp[j].sl_num].topos,tmp->valuestring,strlen(tmp->valuestring));
								else if(sep_type == 2)	mymemcpy(ss_cshp[j].sp_ap[ss_cshp[j].sp_num].topos,tmp->valuestring,strlen(tmp->valuestring));
							}
							level3 = cJSON_GetObjectItem(level2,"option");
							if(level3)
							{
								tmp = cJSON_GetObjectItem(level3,"durations");
								if(tmp)
								{
									ss_cshp[j].sl_ap[ss_cshp[j].sl_num].option.duration = tmp->valueint;
								}
								tmp = cJSON_GetObjectItem(level3,"erase");
								if(tmp)
								{
									ss_cshp[j].sl_ap[ss_cshp[j].sl_num].option.erase = tmp->valueint;
								}
							}
							tmp = cJSON_GetObjectItem(level2,"stseq");
							if(tmp)
							{
								if(sep_type == 1)	ss_cshp[j].sl_ap[ss_cshp[j].sl_num].stseq = tmp->valueint;
								else if(sep_type == 2)	ss_cshp[j].sp_ap[ss_cshp[j].sp_num].stseq = tmp->valueint;
								
							}
							tmp = cJSON_GetObjectItem(level2,"timeout");
							if(tmp)
							{
								if(sep_type == 1)	ss_cshp[j].sl_ap[ss_cshp[j].sl_num].timeout = tmp->valueint;
								else if(sep_type == 2)	ss_ap.sp_ap[ss_cshp[j].sp_num].timeout = tmp->valueint;
							}
						}
					}
				}
			}
			else if(level1->type == cJSON_Object)//此时只有一个seqid，即只控制SL或SP
			{
				tmp = cJSON_GetObjectItem(level1,"sepid");
				if(tmp && tmp->valuestring)
				{
					if (*tmp->valuestring == 'S')
						if(*tmp->valuestring+1 == 'L')
						{sep_type = 1;mymemcpy(ss_cshp[j].sl_ap[0].sepid,tmp->valuestring,strlen(tmp->valuestring));ss_cshp[j].sl_num++;}
						else
						{sep_type = 2;mymemcpy(ss_cshp[j].sp_ap[0].sepid,tmp->valuestring,strlen(tmp->valuestring));ss_cshp[j].sp_num++;}
				}
				tmp = cJSON_GetObjectItem(level2,"seqid");
				if(tmp)
				{
					if(sep_type == 1)	ss_cshp[j].sl_ap[0].seqid = tmp->valueint;
					else if(sep_type == 2)	ss_cshp[j].sp_ap[0].seqid = tmp->valueint;
				}
				tmp = cJSON_GetObjectItem(level2,"CH");
				if(tmp)
				{
					if(sep_type == 1)	ss_cshp[j].sl_ap[0].ch = tmp->valueint;
					else if(sep_type == 2)	ss_cshp[j].sp_ap[0].ch = tmp->valueint;
				}
				tmp = cJSON_GetObjectItem(level2,"action");
				if(tmp && tmp->valuestring)
				{
					if(sep_type == 1)	mymemcpy(ss_cshp[j].sl_ap[0].action,tmp->valuestring,strlen(tmp->valuestring));
					else if(sep_type == 2)	mymemcpy(ss_cshp[j].sp_ap[0].action,tmp->valuestring,strlen(tmp->valuestring));
				}
				tmp = cJSON_GetObjectItem(level2,"topos");
				if(tmp && tmp->valuestring)
				{
					if(sep_type == 1)	mymemcpy(ss_cshp[j].sl_ap[0].topos,tmp->valuestring,strlen(tmp->valuestring));
					else if(sep_type == 2)	mymemcpy(ss_cshp[j].sp_ap[0].topos,tmp->valuestring,strlen(tmp->valuestring));
				}
				level3 = cJSON_GetObjectItem(level2,"option");
				if(level3)
				{
					tmp = cJSON_GetObjectItem(level3,"durations");
					if(tmp)
					{
						ss_cshp[j].sl_ap[0].option.duration = tmp->valueint;
					}
					tmp = cJSON_GetObjectItem(level3,"erase");
					if(tmp)
					{
						ss_cshp[j].sl_ap[0].option.erase = tmp->valueint;
					}
				}
				tmp = cJSON_GetObjectItem(level2,"stseq");
				if(tmp)
				{
					if(sep_type == 1)	ss_cshp[j].sl_ap[0].stseq = tmp->valueint;
					else if(sep_type == 2)	ss_cshp[j].sp_ap[0].stseq = tmp->valueint;
					
				}
				tmp = cJSON_GetObjectItem(level2,"timeout");
				if(tmp)
				{
					if(sep_type == 1)	ss_cshp[j].sl_ap[0].timeout = tmp->valueint;
					else if(sep_type == 2)	ss_cshp[j].sp_ap[0].timeout = tmp->valueint;
				}
			}
		}
	cJSON_Delete(root);
	success_receipt();
	}
}

//解析config_strategy_htsp命令并且回复
void deal_config_strategy_htsp(u8 *buf)//buf是topic内容
{
	u8 stid,i,j,sepidnum = 0;
	cJSON *cs,*sub_cs,*sub_cs2,*sub_cs3;
	char *payload;
	char *topic = "/config/strategy/htsp";
	u8 len = (u8)strlen(buf);
	switch(len - 27){
		case 1:
			stid = (u8)(*(buf+27) - '0');
		break;
		case 2:
			stid = (u8)((*(buf+27) - '0')*10) + (u8)(*(buf+28) - '0');
		break;
	}
	if(stid > 20)	return;//id超出范围直接返回
	sepidnum = ss_cshp[stid].sl_num + ss_cshp[stid].sp_num;
	cs = cJSON_CreateObject();
	cJSON_AddStringToObject(cs,"cond",ss_cshp[stid].cond);
	if(sepidnum == 1){//只有1条处理命令
		cJSON_AddItemToObject(cs, "cmd",sub_cs=cJSON_CreateArray());
		cJSON_AddItemToArray(sub_cs,sub_cs2 = cJSON_CreateObject());
		if(ss_cshp[stid].sl_num == 1){
			cJSON_AddNumberToObject(sub_cs2,"seqid",ss_cshp[stid].sl_ap[0].seqid);
			cJSON_AddStringToObject(sub_cs2,"sepid",ss_cshp[stid].sl_ap[0].sepid);
			cJSON_AddNumberToObject(sub_cs2,"CH",ss_cshp[stid].sl_ap[0].ch);
			cJSON_AddStringToObject(sub_cs2,"action",ss_cshp[stid].sl_ap[0].action);
			cJSON_AddStringToObject(sub_cs2,"topos",ss_cshp[stid].sl_ap[0].topos);
			cJSON_AddItemToObject(sub_cs2, "option",sub_cs3=cJSON_CreateObject());
			cJSON_AddNumberToObject(sub_cs3,"duration",ss_cshp[stid].sl_ap[0].option.duration);
			cJSON_AddNumberToObject(sub_cs3,"erase",ss_cshp[stid].sl_ap[0].option.erase);
			cJSON_AddNumberToObject(sub_cs2,"stseq",ss_cshp[stid].sl_ap[0].stseq);
			cJSON_AddNumberToObject(sub_cs2,"timeout",ss_cshp[stid].sl_ap[0].timeout);
		}
		else if(ss_cshp[stid].sp_num == 1){
			cJSON_AddNumberToObject(sub_cs2,"seqid",ss_cshp[stid].sp_ap[0].seqid);
			cJSON_AddStringToObject(sub_cs2,"sepid",ss_cshp[stid].sp_ap[0].sepid);
			cJSON_AddNumberToObject(sub_cs2,"CH",ss_cshp[stid].sp_ap[0].ch);
			cJSON_AddStringToObject(sub_cs2,"action",ss_cshp[stid].sp_ap[0].action);
			cJSON_AddStringToObject(sub_cs2,"topos",ss_cshp[stid].sp_ap[0].topos);
			cJSON_AddNumberToObject(sub_cs2,"stseq",ss_cshp[stid].sp_ap[0].stseq);
			cJSON_AddNumberToObject(sub_cs2,"timeout",ss_cshp[stid].sp_ap[0].timeout);
		}
		payload = cJSON_PrintUnformatted(cs);
		if(payload)	cJSON_Delete(cs);
		init_tx_message(0x43,0x01,0x15,topic,Rxfr4004.rx_var_header.message_id_H,Rxfr4004.rx_var_header.message_id_L,0x00,payload);
		send_message(&Txto4004);
	}
	else if(sepidnum > 1){
		cJSON_AddItemToObject(cs, "cmd",sub_cs=cJSON_CreateArray());
		for(i = 1;i < sepidnum;i++){//sepid从1到sepidnum遍历ss_cshp[stid]中的sl_ap和sp_ap结构，创建相应的cjson数据
			for(j=1;j < ss_cshp[stid].sl_num;j++){
				if(ss_cshp[stid].sl_ap[j].seqid == i){//说明此时找到了seqid对应的sl_ap结构
					cJSON_AddItemToArray(sub_cs,sub_cs2 = cJSON_CreateObject());
					cJSON_AddNumberToObject(sub_cs2,"seqid",ss_cshp[stid].sl_ap[j].seqid);
					cJSON_AddStringToObject(sub_cs2,"sepid",ss_cshp[stid].sl_ap[j].sepid);
					cJSON_AddNumberToObject(sub_cs2,"CH",ss_cshp[stid].sl_ap[j].ch);
					cJSON_AddStringToObject(sub_cs2,"action",ss_cshp[stid].sl_ap[j].action);
					cJSON_AddStringToObject(sub_cs2,"topos",ss_cshp[stid].sl_ap[j].topos);
					cJSON_AddItemToObject(sub_cs2, "option",sub_cs3=cJSON_CreateObject());
					cJSON_AddNumberToObject(sub_cs3,"duration",ss_cshp[stid].sl_ap[j].option.duration);
					cJSON_AddNumberToObject(sub_cs3,"erase",ss_cshp[stid].sl_ap[j].option.erase);
					cJSON_AddNumberToObject(sub_cs2,"stseq",ss_cshp[stid].sl_ap[j].stseq);
					cJSON_AddNumberToObject(sub_cs2,"timeout",ss_cshp[stid].sl_ap[j].timeout);
					break;
				}
			}
			//此时寻找的sepid为"i"的不是sl_ap结构，而是sp_ap结构
			for(j=1;j < ss_cshp[stid].sp_num;j++){
				if(ss_cshp[stid].sl_ap[j].seqid == i){
					cJSON_AddItemToArray(sub_cs,sub_cs2 = cJSON_CreateObject());
					cJSON_AddNumberToObject(sub_cs2,"seqid",ss_cshp[stid].sp_ap[0].seqid);
					cJSON_AddStringToObject(sub_cs2,"sepid",ss_cshp[stid].sp_ap[0].sepid);
					cJSON_AddNumberToObject(sub_cs2,"CH",ss_cshp[stid].sp_ap[0].ch);
					cJSON_AddStringToObject(sub_cs2,"action",ss_cshp[stid].sp_ap[0].action);
					cJSON_AddStringToObject(sub_cs2,"topos",ss_cshp[stid].sp_ap[0].topos);
					cJSON_AddNumberToObject(sub_cs2,"stseq",ss_cshp[stid].sp_ap[0].stseq);
					cJSON_AddNumberToObject(sub_cs2,"timeout",ss_cshp[stid].sp_ap[0].timeout);
					break;
				}
			}
		}
		payload = cJSON_PrintUnformatted(cs);
		if(payload)	cJSON_Delete(cs);
		init_tx_message(0x43,0x01,0x15,topic,Rxfr4004.rx_var_header.message_id_H,Rxfr4004.rx_var_header.message_id_L,0x00,payload);
		send_message(&Txto4004);
	}
	
}


//解析decive_status命令
void deal_device_status(u8 *buf)//buf是topic内容
{
	u8 i,j;
	u8 doule_break = 0;
	cJSON *cs,*sub_cs;
	char *payload;
	char *topic = "";
	u8 type;
	u8 buflen;
	buflen = strlen(buf);
	if((buflen > 14)&&(buflen <= 30))	type = 1;
	else															type = 2;
	if(type == 1){
		mymemcpy(ss_des.sepid,(buf+22),8);
		if(!ss_des.sepid[0]){
			ack_des2 = 1;
			cs = cJSON_CreateObject();
			if(ack_des2){
				ack_des2 = 0;
				for(i = 0;i < 5;i++){
					for(j = 0;j < 15;j++){
						if(strncmp(ss_des.sepid,ss.sc[i].slc[j].deviceid,8) == 0){//找到与ss_des.sepid对应的slc
							cJSON_AddItemToObject(cs, ss.sc[i].slc[j].deviceid,sub_cs=cJSON_CreateObject());
							cJSON_AddNumberToObject(sub_cs,"C1",ss.sc[i].slc[j].ch1_status);
							cJSON_AddNumberToObject(sub_cs,"C2",ss.sc[i].slc[j].ch2_status);
							cJSON_AddNumberToObject(sub_cs,"C3",ss.sc[i].slc[j].ch3_status);
							cJSON_AddNumberToObject(sub_cs,"C4",ss.sc[i].slc[j].ch4_status);
							doule_break = 1;
							break;
						}
						if(strncmp(ss_des.sepid,ss.sc[i].spc[j].deviceid,8) == 0){//找到与ss_des.sepid对应的slc
							cJSON_AddItemToObject(cs, ss.sc[i].spc[j].deviceid,sub_cs=cJSON_CreateObject());
							cJSON_AddNumberToObject(sub_cs,"C1",ss.sc[i].spc[j].ch1_status);
							cJSON_AddNumberToObject(sub_cs,"C2",ss.sc[i].spc[j].ch2_status);
							cJSON_AddNumberToObject(sub_cs,"C3",ss.sc[i].spc[j].ch3_status);
							cJSON_AddNumberToObject(sub_cs,"C4",ss.sc[i].spc[j].ch4_status);
							doule_break = 1;
							break;
						}
					}
					if(doule_break)	{doule_break = 0;break;}
				}
				payload = cJSON_PrintUnformatted(cs);
				if(payload)	cJSON_Delete(cs);
				init_tx_message(0x83,0x01,0x00,topic,ss_des_message_id_H,ss_des_message_id_L,0x00,payload);
				send_message(&Txto4004);
			}
		}
	}
	if(type == 2){
		mymemcpy(ss_des.sepid,(buf+22),8);
		mymemcpy(ss_des.ch,(buf+33),2);
		if((!ss_des.sepid[0]) && (!ss_des.ch[0]))	{
			ack_des3 = 1;
			cs = cJSON_CreateObject();
			if(ack_des3){
				ack_des3 = 0;
				for(i = 0;i < 5;i++){
					for(j = 0;j < 15;j++){
						if(strncmp(ss_des.sepid,ss.sc[i].slc[j].deviceid,8) == 0){//找到与ss_des.sepid对应的slc
							cJSON_AddItemToObject(cs, ss.sc[i].slc[j].deviceid,sub_cs=cJSON_CreateObject());
							if(ss_des.ch[1] == '1')	cJSON_AddNumberToObject(sub_cs,"C1",ss.sc[i].slc[j].ch1_status);
							if(ss_des.ch[1] == '2')	cJSON_AddNumberToObject(sub_cs,"C2",ss.sc[i].slc[j].ch2_status);
							if(ss_des.ch[1] == '3')	cJSON_AddNumberToObject(sub_cs,"C3",ss.sc[i].slc[j].ch3_status);
							if(ss_des.ch[1] == '4')	cJSON_AddNumberToObject(sub_cs,"C4",ss.sc[i].slc[j].ch4_status);
							doule_break = 1;
							break;
						}
						if(strncmp(ss_des.sepid,ss.sc[i].spc[j].deviceid,8) == 0){//找到与ss_des.sepid对应的slc
							cJSON_AddItemToObject(cs, ss.sc[i].spc[j].deviceid,sub_cs=cJSON_CreateObject());
							if(ss_des.ch[1] == '1')	cJSON_AddNumberToObject(sub_cs,"C1",ss.sc[i].spc[j].ch1_status);
							if(ss_des.ch[1] == '2')	cJSON_AddNumberToObject(sub_cs,"C2",ss.sc[i].spc[j].ch2_status);
							if(ss_des.ch[1] == '3')	cJSON_AddNumberToObject(sub_cs,"C3",ss.sc[i].spc[j].ch3_status);
							if(ss_des.ch[1] == '4')	cJSON_AddNumberToObject(sub_cs,"C4",ss.sc[i].spc[j].ch4_status);
							doule_break = 1;
							break;
						}
					}
					if(doule_break)	{doule_break = 0;break;}
				}
				payload = cJSON_PrintUnformatted(cs);
				if(payload)	cJSON_Delete(cs);
				init_tx_message(0x83,0x01,0x00,topic,ss_des_message_id_H,ss_des_message_id_L,0x00,payload);
				send_message(&Txto4004);
			}
		}
	}
}
//解析qe深度命令
void deal_qe(u8 *buf)//buf是topic内容
{
	int i = 10;
	int j = 0;
	int k = 0;
	while(*(buf+i) != 0x2F)	i++; 	
	mymemcpy(ss_qe.sepid,(buf+12),(i-10-2));
	i++;
	if(*(buf + i) == 'M'){///qe/sepid/{$SEPID}/MD/1/CH/52/action/DM/topos/99,//qe/sepid/{$SEPID}/MD/1/CH/52/action/WP/topos/99  
		i += 3;j=i;
		while(*(buf+i) != 0x2F)	i++;
		//MD有i-j个数
		switch(i-j){
			case 1:
				ss_qe.MDID = (int)(*(buf + j) - '0');
				break;
			case 2:
				ss_qe.MDID = (int)(*(buf + j) - '0')*10 + (int)(*(buf + j+1) - '0');
				break;
		}
		i++;
		if(*(buf + i) == 'C'){
			i += 3;j = i;
			while(*(buf+i) != 0x2F)	i++;
			//CH有i-j个数
			switch(i-j){
			case 1:
				ss_qe.CH = (int)(*(buf + j) - '0');
				break;
			case 2:
				ss_qe.CH = /*(int)(*(buf + j) - '0')*10 +*/(int)(*(buf + j+1) - '0');
				break;
			}
			i++;
			//action
			i += 7;
			ss_qe.action[0] = *(buf+i);i++;
			ss_qe.action[1] = *(buf+i);i++;
			//topos
			i += 7;
			if((*(buf + i) == 'F')&&(*(buf + i+1) == 'F'))			ss_qe.topos = 0x64;
			else																							ss_qe.topos = (int)(*(buf + i) - '0')*10 + (int)(*(buf + i+1) - '0');
			/*if((*(buf+i) > 0x30)&&(*(buf+i) <= 0x39))	{ss_qe.topos = (int)(*(buf + i) - '0')*10 + (int)(*(buf + i+1) - '0');}
			else								{
				ss_qe.topos = (int)(*(buf + i) - '0');
			}*/
		}
	}
	else if(*(buf + i) == 'a'){
		//action
		i += 7;
		ss_qe.action[0] = *(buf+i);i++;//可能为UR和CP
		ss_qe.action[1] = *(buf+i);i++;
		i++;
		if(*(buf+i) == 't'){///qe/sepid/{$SEPID}/action/UR/type/8/code/AD7681
			i += 5;j = i;k=0;
			while(*(buf+i) != 0x2F)	i++;
			//type有i-j个数
			switch(i-j){
				case 1:
					ss_qe.type = (int)(*(buf + j) - '0');
					break;
				case 2:
					ss_qe.type = (int)(*(buf + j) - '0')*10 + (int)(*(buf + j+1) - '0');
					break;
			}
			i++;
			i+=5;
			while(*(buf+i))	{ss_qe.code[k++] = *(buf+i);i++;}
		}
		else if(*(buf+i) == 'r'){///qe/sepid/{$SEPID}/action/UR/raw/990-290-200厖 
			i += 4;k=0;
			while(*(buf+i))	{ss_qe.raw[k++] = *(buf+i);i++;}
		}
		else if(*(buf+i) == 'C'){///qe/sepid/{$SEPID}/action/CP/CH/52/topos/99
			i += 3;j = i;
			while(*(buf+i) != 0x2F)	i++;
			//CH有i-j个数
			switch(i-j){
			case 1:
				ss_qe.CH = (int)(*(buf + j) - '0');
				break;
			case 2:
				ss_qe.CH = (int)(*(buf + j) - '0')*10 + (int)(*(buf + j+1) - '0');
				break;
			}
			i++;
			//topos
			i += 7;
			if(*(buf+i) == '9')	ss_qe.topos = (int)(*(buf + i) - '0')*10 + (int)(*(buf + i+1) - '0');
			else								ss_qe.topos = (int)(*(buf + i) - '0');
		}
	}
}

//解析alarm深度指令
void dela_alarm(u8 *buf)//buf时topic内容
{
	int i = 13;
	while(*(buf+i) != 0x2F)	i++; 	
	mymemcpy(ss_alarm.sepid,(buf+13),(i-13));
	i+= 4;
	ss_alarm.level = *(buf + i) - '0';//level值可能为1，2，3
	success_receipt();
}

/**************************************回复ssp函数************************************************/

void success_receipt(void)
{
	/*
	u8 *send_buf,*temp_send_buf;
	u16 send_payload_len;
	char *send_payload_buf;
	cJSON *root;
	root=cJSON_CreateObject();       
	cJSON_AddItemToObject(root, "code", cJSON_CreateString("0x0200000"));     
	cJSON_AddItemToObject(root, "msg", cJSON_CreateString("operation are Successfully Performed")); 
	send_payload_buf =cJSON_PrintUnformatted(root);
	cJSON_Delete(root); 
	send_payload_len = strlen(send_payload_buf);
	mymemcpy(Txto4004.tx_payload,send_payload_buf,send_payload_len);
	send_buf = temp_send_buf = mymalloc(send_payload_len+5);
	*send_buf++ = Txto4004.tx_fix_header.ch.first_ch_byte = 0x83;
	*send_buf++ = (3 + send_payload_len);
	*send_buf++	= Txto4004.tx_var_header.version = 0x01;
	*send_buf++	= Txto4004.tx_var_header.message_id_H = Rxfr4004.rx_var_header.message_id_H;
	*send_buf++	= Txto4004.tx_var_header.message_id_L = Rxfr4004.rx_var_header.message_id_L;
	mystrncat(send_buf,Txto4004.tx_payload,send_payload_len);
	//Usart1_Send(temp_send_buf,(5 + send_payload_len));
	//while(!Usart1_Send_Done);	Usart1_Send_Done = 0;
	addNodeToUartTxSLLast(temp_send_buf,(5 + send_payload_len));
	mymemset(Txto4004.tx_payload,0,send_payload_len);
	//free(send_payload_buf);
	//myfree(send_buf);
	*/
	cJSON *cs;
	char *payload;
	char *topic = "";
	cs = cJSON_CreateObject();
	cJSON_AddItemToObject(cs, "code", cJSON_CreateString("0x0200000"));     
	cJSON_AddItemToObject(cs, "msg", cJSON_CreateString("operation are Successfully Performed")); 
	payload = cJSON_PrintUnformatted(cs);
	if(payload)	cJSON_Delete(cs);
	init_tx_message(0x83,0x01,0x00,topic,Rxfr4004.rx_var_header.message_id_H,Rxfr4004.rx_var_header.message_id_L,0x00,payload);
	send_message(&Txto4004);
}

void error_recepit(void)
{
	/*
	u8 *send_buf,*temp_send_buf;
	u16 send_payload_len;
	char *send_payload_buf;
	cJSON *root;
	root=cJSON_CreateObject();       
	cJSON_AddItemToObject(root, "code", cJSON_CreateString("0x0401F08"));     
	cJSON_AddItemToObject(root, "msg", cJSON_CreateString("Unauthorized Seraph Sense")); 
	send_payload_buf =cJSON_PrintUnformatted(root);
	cJSON_Delete(root); 
	send_payload_len = strlen(send_payload_buf);
	mymemcpy(Txto4004.tx_payload,send_payload_buf,send_payload_len);
	temp_send_buf = send_buf = mymalloc(send_payload_len+5);
	*send_buf++ = Txto4004.tx_fix_header.ch.first_ch_byte = 0x85;
	*send_buf++ = (3 + send_payload_len);
	*send_buf++	= Txto4004.tx_var_header.version = 0x01;
	*send_buf++	= Txto4004.tx_var_header.message_id_H = Rxfr4004.rx_var_header.message_id_H;
	*send_buf++	= Txto4004.tx_var_header.message_id_L = Rxfr4004.rx_var_header.message_id_L;
	mystrncat(send_buf,Txto4004.tx_payload,send_payload_len);
	//Usart1_Send(temp_send_buf,(5 + send_payload_len));
	//while(!Usart1_Send_Done);	Usart1_Send_Done = 0;
	addNodeToUartTxSLLast(temp_send_buf,(5 + send_payload_len));
	mymemset(Txto4004.tx_payload,0,send_payload_len);
	//free(send_payload_buf);
	//myfree(send_buf);
	*/
	cJSON *cs;
	char *payload;
	char *topic = "";
	cs = cJSON_CreateObject();
	cJSON_AddItemToObject(cs, "code", cJSON_CreateString("0x0200000"));     
	cJSON_AddItemToObject(cs, "msg", cJSON_CreateString("operation are Successfully Performed")); 
	payload = cJSON_PrintUnformatted(cs);
	if(payload)	cJSON_Delete(cs);
	init_tx_message(0x85,0x01,0x00,topic,Rxfr4004.rx_var_header.message_id_H,Rxfr4004.rx_var_header.message_id_L,0x00,payload);
	send_message(&Txto4004);
}

void calc_send_r_length(u8 *buf,Txmessage *tx)
{
	u32 padload_len;
	padload_len = strlen(buf);
	if(tx->tx_var_header.ext_message_id){
		padload_len += 4;
	}
	else{
		padload_len += 3;
	}
	if(tx->tx_var_header.topic_lengthL){
		padload_len += strlen(tx->tx_var_header.topic) + 2;//2是topic length的长度
	}
	if ((padload_len / 2097152) > 0)//有4个字节长的r_length
	{	
		tx->tx_fix_header.r_length_len = 4;
		tx->tx_fix_header.r_length[3] = (u8)(padload_len % 2097152);
		padload_len -= tx->tx_fix_header.r_length[3];
		tx->tx_fix_header.r_length[2] = (u8)(padload_len % 128);
		padload_len -= tx->tx_fix_header.r_length[2];
		tx->tx_fix_header.r_length[1] = (u8)(padload_len % 128);
		padload_len -= tx->tx_fix_header.r_length[1];
		tx->tx_fix_header.r_length[0] = (u8)(padload_len % 128);
		tx->tx_fix_header.r_length[3] |= 0x80;
		tx->tx_fix_header.r_length[2] |= 0x80;
		tx->tx_fix_header.r_length[1] |= 0x80;
		/*
		tx->tx_fix_header.r_length[3] = (u8)(padload_len / 2097126) | 0x80;
		padload_len = padload_len % 2097126;
		if((padload_len / 16384) > 0)
		{
			tx->tx_fix_header.r_length[2] = (u8)(padload_len / 16384);
			padload_len = padload_len % 16384;
			if ((padload_len / 128) > 0)
			{
				tx->tx_fix_header.r_length[1] = (u8)(padload_len / 128);
				padload_len = (padload_len % 128);
				
				tx->tx_fix_header.r_length[0] = (u8)padload_len;
			}
		}*/
	}
	else if((padload_len / 16384) > 0)//有3个字节的r_length
	{
			tx->tx_fix_header.r_length_len = 3;
			tx->tx_fix_header.r_length[2] = (u8)(padload_len % 16384);
			padload_len -= tx->tx_fix_header.r_length[2];
			tx->tx_fix_header.r_length[1] = (u8)(padload_len % 128);
			padload_len -= tx->tx_fix_header.r_length[1];
			tx->tx_fix_header.r_length[0] = (u8)(padload_len % 128);
			tx->tx_fix_header.r_length[2] |= 0x80;
			tx->tx_fix_header.r_length[1] |= 0x80;
		
			/*padload_len = padload_len / 128;
			if ((padload_len / 128) > 0)
			{
				tx->tx_fix_header.r_length[1] = (u8)(padload_len % 128) | 0x80;
				padload_len = (padload_len / 128);
				tx->tx_fix_header.r_length[2] = (u8)padload_len;
			}*/
	}
	else if((padload_len / 128) > 0)//有2个字节的r_length
	{
		tx->tx_fix_header.r_length_len = 2;
		tx->tx_fix_header.r_length[1] = (u8)(padload_len % 128) | 0x80;
		padload_len = (u8)padload_len / 128;
		tx->tx_fix_header.r_length[0] = (u8)padload_len ;
	}
	else
	{
		tx->tx_fix_header.r_length_len = 1;
		tx->tx_fix_header.r_length[0] = (u8)padload_len;
	}
}

//清除Txto4004数据
void clear_tx_buf(void)
{
	int i;
	Txto4004.tx_fix_header.ch.first_ch_byte = 0x00;
	for(i = 0;i < 4;i++)	Txto4004.tx_fix_header.r_length[i] = 0x00;
	Txto4004.tx_fix_header.r_length_len = 0x00;
	Txto4004.tx_var_header.version = 0x00;
	Txto4004.tx_var_header.topic_lengthH = 0x00;
	Txto4004.tx_var_header.topic_lengthL = 0x00;
	for(i = 0; i < 30; i++)	Txto4004.tx_var_header.topic[i] = 0x00;
	Txto4004.tx_var_header.message_id_H = 0x00;
	Txto4004.tx_var_header.message_id_L = 0x00;
	Txto4004.tx_var_header.ext_message_id = 0x00;
	for(i = 0; i < 1024;i++)	Txto4004.tx_payload[i] = 0x00;
}
//初始化Txto4004
void init_tx_message(u8 first_ch_byte,u8 version,u8 topic_len,char *topic,u8 message_id_H,u8 message_id_L,u8 ext_message_id,char *payload)
{
	clear_tx_buf();
	Txto4004.tx_fix_header.ch.first_ch_byte = first_ch_byte;
	Txto4004.tx_var_header.version = version;
	Txto4004.tx_var_header.topic_lengthH = 0x00;
	Txto4004.tx_var_header.topic_lengthL = topic_len;
	if(topic_len)
		{mymemcpy(Txto4004.tx_var_header.topic,topic,topic_len);myfree(topic);}
	Txto4004.tx_var_header.message_id_H = message_id_H;
	Txto4004.tx_var_header.message_id_L = message_id_L;
	if(ext_message_id)
		Txto4004.tx_var_header.ext_message_id = ext_message_id;
	calc_send_r_length(payload,&Txto4004);
	mymemcpy(Txto4004.tx_payload,payload,strlen(payload));myfree(payload);
}

//初始化Txto4004数据，根据_REPLY_ type做不同初始化
void init_send_Txmessage(_REPLY_ type)
{
	cJSON *cs;
	char *payload;
	char *topic = "";
	switch(type)
	{
		case CONFIG_SS:
			cs = cJSON_CreateObject();    
			cJSON_AddItemToObject(cs, "code", cJSON_CreateString("0x0401F08"));     
			cJSON_AddItemToObject(cs, "msg", cJSON_CreateString("Unauthorized Seraph Sense")); 
			payload = cJSON_PrintUnformatted(cs);
			if(payload)	cJSON_Delete(cs);
			init_tx_message(0x83,0x01,0x00,topic,0x89,0x02,0x01,payload);
			break;
		default:
			break;
	}
		send_message(&Txto4004);
}
//发送Txto4004内容
void send_message(Txmessage *tx)
{
	u8 *send_buf,*temp_send_buf;
	u16 txlen = 0;
	u16 payloadlen;
	temp_send_buf = mymalloc(1024);
	mymemset(temp_send_buf,0,1024);
	send_buf = temp_send_buf;
	temp_send_buf = send_buf;
	*send_buf++ = 0xBB;
	txlen++;
	*send_buf++ = 0xBB;
	txlen++;
	*send_buf++ = (tx->tx_fix_header.ch.first_ch_byte);
	txlen++;
	while(tx->tx_fix_header.r_length_len--){
		*send_buf++ = tx->tx_fix_header.r_length[tx->tx_fix_header.r_length_len];
		txlen++;
	}
	*send_buf++ = tx->tx_var_header.version;
	txlen++;
	if(tx->tx_var_header.topic_lengthL){
		*send_buf++ = tx->tx_var_header.topic_lengthH;
		txlen++;
		*send_buf++ = tx->tx_var_header.topic_lengthL;
		txlen++;
		mymemcpy(send_buf,tx->tx_var_header.topic,tx->tx_var_header.topic_lengthL);
		txlen += tx->tx_var_header.topic_lengthL;
		send_buf += tx->tx_var_header.topic_lengthL;
	}
	*send_buf++ = (tx->tx_var_header.message_id_H);
	*send_buf++ = (tx->tx_var_header.message_id_L);
	txlen+=2;
	if(tx->tx_var_header.ext_message_id){
		*send_buf++ = (tx->tx_var_header.ext_message_id);
		txlen++;
	}
	if(tx->tx_payload){
		payloadlen = strlen(tx->tx_payload);
		mymemcpy(send_buf,tx->tx_payload,payloadlen);
		txlen+=payloadlen;
		//myfree(tx->tx_payload);
	}
	//Usart1_Send(temp_send_buf,txlen);
	//Usart1_Send(temp_send_buf,txlen);
	//while(!Usart1_Send_Done);	Usart1_Send_Done = 0;
	//myfree(temp_send_buf);
	addNodeToUartTxSLLast(temp_send_buf,txlen);
}


void send_message_without_payload(u8 fix1,u8 version,u8 message_id_h,u8 message_id_l,u8 ex_message_id)
{
	u8 send_buf[6];
	u8 r_length;
	if(ex_message_id)
		r_length = 0x04;
	else
		r_length = 0x03;
	send_buf[0] = fix1;
	send_buf[1] = r_length;
	send_buf[2] = version;
	send_buf[3] = message_id_h;
	send_buf[4] = message_id_l;
	if(ex_message_id)
	{	
		send_buf[5] = ex_message_id;
	}
	//Usart1_Send(send_buf,(r_length + 2));
	//while(!Usart1_Send_Done);	Usart1_Send_Done = 0;
	addNodeToUartTxSLLast(send_buf,(r_length + 2));
}


//回复心跳包
void rev_heart_beat(u8 fix1)
{
	u8 send_buf[2];
	send_buf[0] = fix1;
	send_buf[1] = 0x00;
	//Usart1_Send(send_buf,2);
	addNodeToUartTxSLLast(send_buf,2);
}

/**************************************ss主动发送给esh函数**********************************************/
//获取SS的配置信息，什么时候主动推送？
void send_config_ss(void)
{
	u8 temp_counter,gene_message_id_H,gene_message_id_L;
	cJSON *cs,*sub_cs,*sub_cs2;
	char *payload;
	char *topic = "/config/ss";
	temp_counter = (u8)TIM2->CNT;
	gene_message_id_H = random(temp_counter);
	temp_counter = (u8)TIM2->CNT;
	gene_message_id_L = random(temp_counter);
	cs = cJSON_CreateObject();    
	cJSON_AddItemToObject(cs, "system",sub_cs=cJSON_CreateObject());
	cJSON_AddNumberToObject(sub_cs,"HPST",ss_cs.hpst);
	cJSON_AddNumberToObject(sub_cs,"REFR",ss_cs.refr);
	cJSON_AddItemToObject(cs, "wifi",sub_cs=cJSON_CreateObject());
	cJSON_AddStringToObject(sub_cs,"ssid",ss_cs.ssid);
	cJSON_AddStringToObject(sub_cs,"passwd",ss_cs.paswd);
	cJSON_AddNumberToObject(cs,"time",ss_cs.time);
	cJSON_AddItemToObject(cs, "location",sub_cs=cJSON_CreateObject());
	cJSON_AddStringToObject(sub_cs,"city",ss_cs.city);
	cJSON_AddStringToObject(sub_cs,"country",ss_cs.country);
	cJSON_AddStringToObject(sub_cs,"region",ss_cs.region);
	cJSON_AddItemToObject(cs, "weather",sub_cs=cJSON_CreateObject());
	cJSON_AddItemToObject(sub_cs,"atmosphere",sub_cs2=cJSON_CreateObject());
	cJSON_AddStringToObject(sub_cs2,"HM",ss_cs.hm);
	cJSON_AddStringToObject(sub_cs2,"PR",ss_cs.pr);
	cJSON_AddStringToObject(sub_cs2,"visibility",ss_cs.visibility);
	cJSON_AddItemToObject(sub_cs,"astronomy",sub_cs2=cJSON_CreateObject());
	cJSON_AddStringToObject(sub_cs2,"sunrise",ss_cs.sunrise);
	cJSON_AddStringToObject(sub_cs2,"sunset",ss_cs.sunset);
	cJSON_AddItemToObject(sub_cs,"condition",sub_cs2=cJSON_CreateObject());
	cJSON_AddStringToObject(sub_cs2,"code",ss_cs.code);
	cJSON_AddStringToObject(sub_cs2,"TP",ss_cs.tp);
	cJSON_AddItemToObject(sub_cs,"airquality",sub_cs2=cJSON_CreateObject());
	cJSON_AddStringToObject(sub_cs2,"index",ss_cs.index);
	payload = cJSON_PrintUnformatted(cs);
	if(payload)	cJSON_Delete(cs);
	init_tx_message(0x83,0x01,0x0A,topic,gene_message_id_H,gene_message_id_L,0x01,payload);
	send_message(&Txto4004);
}

//推送传感器数据st/sc/slc/slc及ss自身传感器
//type为0x43时为主动推送，type为0x83时是被动推送(接收到/data/sync)
void send_data_sync(u8 type)
{
	u8 i;
	u8 temp_counter,gene_message_id_H,gene_message_id_L;
	cJSON *cs,*sub_cs;
	char *payload;
	char *topic;
	if(type == 0x83) topic = "";
	else if(type == 0x43) topic = "/data/sync";
	temp_counter = (u8)TIM2->CNT;

	
	gene_message_id_H = random(temp_counter);
	temp_counter = (u8)TIM2->CNT;
	gene_message_id_L = random(temp_counter);
	cs = cJSON_CreateObject();    
	cJSON_AddItemToObject(cs, ss.deviceid,sub_cs=cJSON_CreateObject());
	cJSON_AddNumberToObject(sub_cs,"HM",(int)ss.sense.humidity);
	cJSON_AddNumberToObject(sub_cs,"TP",(int)ss.sense.temperature);
	cJSON_AddNumberToObject(sub_cs,"PT",((int)(ss.sense.pm2_5_H<<8) + (int)ss.sense.pm2_5_L));
	cJSON_AddNumberToObject(sub_cs,"SM",(int)ss.sense.smoke);
	cJSON_AddNumberToObject(sub_cs,"PR",(int)ss.sense.human_sensing);
	cJSON_AddNumberToObject(sub_cs,"MI",(int)ss.sense.motion);
	cJSON_AddNumberToObject(sub_cs,"BT",(int)ss.sense.bt);
	cJSON_AddNumberToObject(sub_cs,"CO",((int)(ss.sense.CO_H<<8) + (int)ss.sense.CO_L));
	cJSON_AddNumberToObject(sub_cs,"CD",((int)(ss.sense.CO2_H<<8) + (int)ss.sense.CO2_L));
	cJSON_AddNumberToObject(sub_cs,"VO",(int)ss.sense.VOC);
	for(i = 0; i < 5; i++){
		if((!ss.sc[i].deviceid[0]) && (ss.sc[i].meshid)){//device id和mesh id都不为空时才认为sc存在，并推送数据
			cJSON_AddItemToObject(cs, ss.sc[i].deviceid,sub_cs=cJSON_CreateObject());
			cJSON_AddNumberToObject(sub_cs,"EG",(int)ss.sc[i].sense.total_energy_consum);
			cJSON_AddNumberToObject(sub_cs,"CT",(int)ss.sc[i].sense.total_current);
			cJSON_AddNumberToObject(sub_cs,"VG",(int)ss.sc[i].sense.voltage);
		}
	}
	for(i = 0; i < 20; i++){
		if((!ss.st[i].deviceid[0]) && (ss.st[i].meshid)){//device id和mesh id都不为空时才认为st存在，并推送数据
			cJSON_AddItemToObject(cs, ss.sc[i].deviceid,sub_cs=cJSON_CreateObject());
			cJSON_AddNumberToObject(sub_cs,"CS",((int)(ss.st[i].sense.color_sense_H<<16) + (int)(ss.st[i].sense.color_sense_M<<8) + (int)ss.st[i].sense.color_sense_L));
			cJSON_AddNumberToObject(sub_cs,"AL",(int)ss.st[i].sense.ambient_light);
		}
	}
	if(payload)	cJSON_Delete(cs);
	if(type == 0x83)	init_tx_message(0x83,0x01,0x00,topic,Rxfr4004.rx_var_header.message_id_H,Rxfr4004.rx_var_header.message_id_L,0x00,payload);
	else if(type == 0x43)	init_tx_message(0x43,0x01,0x0A,topic,gene_message_id_H,gene_message_id_L,0x00,payload);
	send_message(&Txto4004);
}
//推送传感器数据st/sc/slc/slc及ss中某一个传感器
void send_deepin_data_sync(void)
{
	u8 i;
	cJSON *cs,*sub_cs;
	char *payload;
	char *topic = "";
	cs = cJSON_CreateObject();
	if((ss_ds.ch[0] == 'H') && (ss_ds.ch[0] == 'M')){
		cJSON_AddItemToObject(cs, ss.deviceid,sub_cs=cJSON_CreateObject());
		cJSON_AddNumberToObject(sub_cs,"HM",(int)ss.sense.humidity);
	}
	else if((ss_ds.ch[0] == 'T') && (ss_ds.ch[0] == 'P')){
		cJSON_AddItemToObject(cs, ss.deviceid,sub_cs=cJSON_CreateObject());
		cJSON_AddNumberToObject(sub_cs,"TP",(int)ss.sense.temperature);
	}
	else if((ss_ds.ch[0] == 'P') && (ss_ds.ch[0] == 'T')){
		cJSON_AddItemToObject(cs, ss.deviceid,sub_cs=cJSON_CreateObject());
		cJSON_AddNumberToObject(sub_cs,"PT",((int)(ss.sense.pm2_5_H<<8) + (int)ss.sense.pm2_5_L));
	}
	else if((ss_ds.ch[0] == 'S') && (ss_ds.ch[0] == 'M')){
		cJSON_AddItemToObject(cs, ss.deviceid,sub_cs=cJSON_CreateObject());
		cJSON_AddNumberToObject(sub_cs,"SM",(int)ss.sense.smoke);
	}
	else if((ss_ds.ch[0] == 'P') && (ss_ds.ch[0] == 'R')){
		cJSON_AddItemToObject(cs, ss.deviceid,sub_cs=cJSON_CreateObject());
		cJSON_AddNumberToObject(sub_cs,"PR",(int)ss.sense.human_sensing);
	}
	else if((ss_ds.ch[0] == 'M') && (ss_ds.ch[0] == 'I')){
		cJSON_AddItemToObject(cs, ss.deviceid,sub_cs=cJSON_CreateObject());
		cJSON_AddNumberToObject(sub_cs,"MI",(int)ss.sense.motion);
	}
	else if((ss_ds.ch[0] == 'B') && (ss_ds.ch[0] == 'T')){
		cJSON_AddItemToObject(cs, ss.deviceid,sub_cs=cJSON_CreateObject());
		cJSON_AddNumberToObject(sub_cs,"BT",(int)ss.sense.bt);
	}
	else if((ss_ds.ch[0] == 'C') && (ss_ds.ch[0] == 'O')){
		cJSON_AddItemToObject(cs, ss.deviceid,sub_cs=cJSON_CreateObject());
		cJSON_AddNumberToObject(sub_cs,"CO",((int)(ss.sense.CO_H<<8) + (int)ss.sense.CO_L));
	}
	else if((ss_ds.ch[0] == 'C') && (ss_ds.ch[0] == 'D')){
		cJSON_AddItemToObject(cs, ss.deviceid,sub_cs=cJSON_CreateObject());
		cJSON_AddNumberToObject(sub_cs,"CD",((int)(ss.sense.CO2_H<<8) + (int)ss.sense.CO2_L));
	}
	else if((ss_ds.ch[0] == 'V') && (ss_ds.ch[0] == 'O')){
		cJSON_AddItemToObject(cs, ss.deviceid,sub_cs=cJSON_CreateObject());
		cJSON_AddNumberToObject(sub_cs,"VO",(int)ss.sense.VOC);
	}
	else if((ss_ds.ch[0] == 'E') && (ss_ds.ch[0] == 'G')){
		for(i = 0; i < 5;i++){
			cJSON_AddItemToObject(cs, ss.sc[i].deviceid,sub_cs=cJSON_CreateObject());
			cJSON_AddNumberToObject(sub_cs,"EG",(int)ss.sc[i].sense.total_energy_consum);
		}
	}
	else if((ss_ds.ch[0] == 'C') && (ss_ds.ch[0] == 'T')){
		for(i = 0; i < 5;i++){
			cJSON_AddItemToObject(cs, ss.sc[i].deviceid,sub_cs=cJSON_CreateObject());
			cJSON_AddNumberToObject(sub_cs,"CT",(int)ss.sc[i].sense.total_current);
		}
	}
	else if((ss_ds.ch[0] == 'V') && (ss_ds.ch[0] == 'G')){
		for(i = 0; i < 5;i++){
			cJSON_AddItemToObject(cs, ss.sc[i].deviceid,sub_cs=cJSON_CreateObject());
			cJSON_AddNumberToObject(sub_cs,"VG",(int)ss.sc[i].sense.voltage);
		}
	}
	else if((ss_ds.ch[0] == 'C') && (ss_ds.ch[0] == 'S')){
		for(i = 0; i < 20;i++){
			cJSON_AddItemToObject(cs, ss.st[i].deviceid,sub_cs=cJSON_CreateObject());
			cJSON_AddNumberToObject(sub_cs,"CS",((int)(ss.st[i].sense.color_sense_H<<16) + (int)(ss.st[i].sense.color_sense_M<<8) + (int)ss.st[i].sense.color_sense_L));
		}
	}
	else if((ss_ds.ch[0] == 'A') && (ss_ds.ch[0] == 'L')){
		for(i = 0; i < 20;i++){
			cJSON_AddItemToObject(cs, ss.st[i].deviceid,sub_cs=cJSON_CreateObject());
			cJSON_AddNumberToObject(sub_cs,"AL",(int)ss.st[i].sense.ambient_light);
		}
	}
	if(payload)	cJSON_Delete(cs);
	init_tx_message(0x83,0x01,0x00,topic,Rxfr4004.rx_var_header.message_id_H,Rxfr4004.rx_var_header.message_id_L,0x00,payload);
	send_message(&Txto4004);
}
//发送SS的device info
//1.上电3s后主动发送一次
//2.收到esh的device info ss包请求时
void send_device_info_ss(u8 type,u8 message_id_H,u8 message_id_L)
{
	cJSON *cs;
	int i,j;
	char *payload;
	char *topic;
	char topic1[] = "/device/info/ss";
	char topic2[] = "";
	if(type == 0x01) topic = topic1;
	else						 topic = topic2;
	//if(ready_ss_post)
	//{
		//ready_ss_post = 0;
		cs = cJSON_CreateObject(); 
		cJSON_AddStringToObject(cs,"deviceID",ss.deviceid);
		cJSON_AddStringToObject(cs,"model",ss.model);
		cJSON_AddStringToObject(cs,"firmware",ss.firmware);
		cJSON_AddNumberToObject(cs,"HWtest",ss.HWTtest);
		cJSON_AddNumberToObject(cs,"meshID",ss.meshid);
		cJSON_AddStringToObject(cs,"macWiFi",ss.macwifi);
		payload = cJSON_PrintUnformatted(cs);
		if(payload)	cJSON_Delete(cs);
		if(type == 0x01)	init_tx_message(0x43,0x01,0x0F,topic,random((u8)TIM2->CNT),random((u8)TIM2->CNT),0x00,payload);
		else							init_tx_message(0x83,0x01,0x00,topic,message_id_H,message_id_L,0x00,payload);
		send_message(&Txto4004);
	//}
}


//上电收到ST/SC/SLC/SPC后转发给eSH，上电推送自身数据
//uart2接收到ST/SC/SLC/SPC自身数据后，存储在ss.sc[]/ss.st[]/ss.slc[]/ss.spc中
void send_device_info_sub(u8 type,u8 message_id_H,u8 message_id_L)
{
	u8 temp_counter,gene_message_id_H,gene_message_id_L;
	cJSON *cs;
	int i,j;
	char *payload;
	char *topic;
	char topic1[] = "/device/info/sub";
	char topic2[] = "";
	u8 tmp_deviceid[10];
	if(type == 0x01) topic = topic1;
	else						 topic = topic2;
	
	if(ready_st_post){
		ready_st_post = 0;
		for(i = 0;i < 20;i++){
			if((!ss.st[i].posted) && (ss.st[i].deviceid[0]) && (ready_st_post_meshid == ss.st[i].meshid)){//还没有被推送过且ss.st[i].deviceid[]已经有数据
				ss.st[i].posted = 1;
				tmp_deviceid[0] = 'S';
				tmp_deviceid[1] = 'T';
				mymemcpy((tmp_deviceid+2),ss.st[i].deviceid,8);
				cs = cJSON_CreateObject(); 
				cJSON_AddStringToObject(cs,"deviceID",tmp_deviceid);
				cJSON_AddStringToObject(cs,"model",ss.st[i].model);
				cJSON_AddStringToObject(cs,"firmware",ss.st[i].firmware);
				cJSON_AddNumberToObject(cs,"HWtest",ss.st[i].HWTtest);
				cJSON_AddNumberToObject(cs,"meshID",ss.st[i].meshid);
				cJSON_AddStringToObject(cs,"macWiFi",ss.st[i].macwifi);
				payload = cJSON_PrintUnformatted(cs);
				if(payload)	cJSON_Delete(cs);
				if(type == 0x01)	init_tx_message(0x43,0x01,0x10,topic,random((u8)TIM2->CNT),random((u8)TIM2->CNT),0x00,payload);
				else							init_tx_message(0x83,0x01,0x00,topic,message_id_H,message_id_L,0x00,payload);
				send_message(&Txto4004);
			}
		}
	}
	if(ready_sc_post)
	{
		ready_sc_post = 0;
		for(i = 0;i < 5;i++){
			if((!ss.sc[i].posted) && (ss.sc[i].deviceid[0]) &&(ready_sc_post_meshid == ss.sc[i].meshid)){//还没有被推送过且ss.sc[i].deviceid[]已经有数据
				ss.sc[i].posted = 1;
				tmp_deviceid[0] = 'S';
				tmp_deviceid[1] = 'C';
				mymemcpy((tmp_deviceid+2),ss.sc[i].deviceid,8);
				cs = cJSON_CreateObject(); 
				cJSON_AddStringToObject(cs,"deviceID",tmp_deviceid);
				cJSON_AddStringToObject(cs,"model",ss.sc[i].model);
				cJSON_AddStringToObject(cs,"firmware",ss.sc[i].firmware);
				cJSON_AddNumberToObject(cs,"HWtest",ss.sc[i].HWTtest);
				cJSON_AddNumberToObject(cs,"meshID",ss.sc[i].meshid);
				cJSON_AddNumberToObject(cs,"Ndevice",ss.sc[i].Ndevice);
				cJSON_AddStringToObject(cs,"macWiFi",ss.sc[i].macwifi);
				payload = cJSON_PrintUnformatted(cs);
				if(payload)	cJSON_Delete(cs);
				if(type == 0x01)	init_tx_message(0x43,0x01,0x10,topic,random((u8)TIM2->CNT),random((u8)TIM2->CNT),0x00,payload);
				else							init_tx_message(0x83,0x01,0x00,topic,message_id_H,message_id_L,0x00,payload);
				send_message(&Txto4004);
			}
		}
	}
	
	if(ready_slc_post)
	{
		ready_slc_post = 0;
		for(i = 0;i < 5;i++){
			for(j = 0;j < 15;j++){
				if((!ss.sc[i].slc[j].posted) && (ss.sc[i].slc[j].deviceid[0]) && (ready_slc_post_mdid == ss.sc[i].slc[j].MDID)){//还没有被推送过且ss.slc[i].deviceid[]已经有数据
					ss.sc[i].slc[j].posted = 1;
					tmp_deviceid[0] = 'S';
					tmp_deviceid[1] = 'L';
					mymemcpy((tmp_deviceid+2),ss.sc[i].slc[j].deviceid,8);
					cs = cJSON_CreateObject(); 
					cJSON_AddStringToObject(cs,"deviceID",tmp_deviceid);
					cJSON_AddStringToObject(cs,"model",ss.sc[i].slc[j].model);
					cJSON_AddStringToObject(cs,"firmware",ss.sc[i].slc[j].firmware);
					cJSON_AddNumberToObject(cs,"HWtest",ss.sc[i].slc[j].HWTtest);
					cJSON_AddNumberToObject(cs,"meshID",ss.sc[i].meshid);
					cJSON_AddNumberToObject(cs,"MDID",ss.sc[i].slc[j].MDID);
					cJSON_AddStringToObject(cs,"macWiFi",ss.sc[i].slc[j].macwifi);
					payload = cJSON_PrintUnformatted(cs);
					if(payload)	cJSON_Delete(cs);
					if(type == 0x01)	init_tx_message(0x43,0x01,0x10,topic,random((u8)TIM2->CNT),random((u8)TIM2->CNT),0x00,payload);
					else							init_tx_message(0x83,0x01,0x00,topic,message_id_H,message_id_L,0x00,payload);
					send_message(&Txto4004);
				}
			}
		}
	}
	
	if(ready_spc_post)
	{
		ready_spc_post = 0;
		for(i = 0;i < 5;i++){
			for(j = 0;j < 15;j++){
				if((!ss.sc[i].spc[j].posted) && (ss.sc[i].spc[j].deviceid[0]) && (ready_spc_post_mdid == ss.sc[i].spc[j].MDID)){//还没有被推送过且ss.spc[i].deviceid[]已经有数据
					ss.sc[i].spc[j].posted = 1;

					tmp_deviceid[0] = 'S';
					tmp_deviceid[1] = 'P';
					mymemcpy((tmp_deviceid+2),ss.sc[i].spc[j].deviceid,8);
					cs = cJSON_CreateObject(); 
					cJSON_AddStringToObject(cs,"deviceID",tmp_deviceid);
					cJSON_AddStringToObject(cs,"model",ss.sc[i].spc[j].model);
					cJSON_AddStringToObject(cs,"firmware",ss.sc[i].spc[j].firmware);
					cJSON_AddNumberToObject(cs,"HWtest",ss.sc[i].spc[j].HWTtest);
					cJSON_AddNumberToObject(cs,"meshID",ss.sc[i].meshid);
					cJSON_AddNumberToObject(cs,"MDID",ss.sc[i].spc[j].MDID);
					cJSON_AddStringToObject(cs,"macWiFi",ss.sc[i].spc[j].macwifi);
					payload = cJSON_PrintUnformatted(cs);
					if(payload)	cJSON_Delete(cs);
					if(type == 0x01)	init_tx_message(0x43,0x01,0x10,topic,random((u8)TIM2->CNT),random((u8)TIM2->CNT),0x00,payload);
					else							init_tx_message(0x83,0x01,0x00,topic,message_id_H,message_id_L,0x00,payload);
					send_message(&Txto4004);
				}
			}
		}
	}
	myfree(topic);
}

//SS向eSH报告ST/SC/SLC/SPC故障，此函数放到Task100ms中
void send_device_malfunction(void)
{
	u8 temp_counter,gene_message_id_H,gene_message_id_L;
	cJSON *cs;
	int i,j;
	char *payload;
	char *topic = "/device/malfunction";
	temp_counter = (u8)TIM2->CNT;
	gene_message_id_H = random(temp_counter);
	temp_counter = (u8)TIM2->CNT;
	gene_message_id_L = random(temp_counter);
	//SS汇报自己的故障
	if(ss.HWTtest){
		cs = cJSON_CreateObject(); 
		cJSON_AddStringToObject(cs,"deviceID",ss.deviceid);
		cJSON_AddStringToObject(cs,"firmware",ss.firmware);
		cJSON_AddNumberToObject(cs,"HWTtest",ss.HWTtest);
		cJSON_AddNumberToObject(cs,"meshID",ss.meshid);
		payload = cJSON_PrintUnformatted(cs);
		if(payload)	cJSON_Delete(cs);
		init_tx_message(0x43,0x01,0x13,topic,gene_message_id_H,gene_message_id_L,0x00,payload);
		send_message(&Txto4004);
	}
	//SS汇报ST故障
	if(rev_st_mal){
		rev_st_mal = 0;
		for(i = 0;i < 20;i++){
			if(ss.st[i].HWTtest){
				cJSON_AddStringToObject(cs,"deviceID",ss.st[i].deviceid);
				cJSON_AddStringToObject(cs,"firmware",ss.st[i].firmware);
				cJSON_AddNumberToObject(cs,"HWTtest",ss.st[i].HWTtest);
				cJSON_AddNumberToObject(cs,"meshID",ss.st[i].meshid);
				payload = cJSON_PrintUnformatted(cs);
				if(payload)	cJSON_Delete(cs);
				init_tx_message(0x43,0x01,0x13,topic,gene_message_id_H,gene_message_id_L,0x00,payload);
				send_message(&Txto4004);
				ss.st[i].HWTtest = 0;
			}
		}
	}
	//SS汇报SC故障
	if(rev_sc_mal){
		rev_sc_mal = 0;
		for(i = 0;i < 5;i++){
			if(ss.sc[i].HWTtest){
				cJSON_AddStringToObject(cs,"deviceID",ss.sc[i].deviceid);
				cJSON_AddStringToObject(cs,"firmware",ss.sc[i].firmware);
				cJSON_AddNumberToObject(cs,"HWTtest",ss.sc[i].HWTtest);
				cJSON_AddNumberToObject(cs,"meshID",ss.sc[i].meshid);
				payload = cJSON_PrintUnformatted(cs);
				if(payload)	cJSON_Delete(cs);
				init_tx_message(0x43,0x01,0x13,topic,gene_message_id_H,gene_message_id_L,0x00,payload);
				send_message(&Txto4004);
			}
		}
	}
	//SS汇报SLC故障
	if(rev_slc_mal){
		rev_slc_mal = 0;
		for(i = 0;i < 5;i++){
			for(j = 0;j < 15;j++){
				if(ss.sc[i].slc[j].HWTtest){
					cJSON_AddStringToObject(cs,"deviceID",ss.sc[i].slc[j].deviceid);
					cJSON_AddStringToObject(cs,"firmware",ss.sc[i].slc[j].firmware);
					cJSON_AddNumberToObject(cs,"HWTtest",ss.sc[i].slc[j].HWTtest);
					cJSON_AddNumberToObject(cs,"meshID",ss.sc[i].meshid);
					payload = cJSON_PrintUnformatted(cs);
					if(payload)	cJSON_Delete(cs);
					init_tx_message(0x43,0x01,0x13,topic,gene_message_id_H,gene_message_id_L,0x00,payload);
					send_message(&Txto4004);
				}
			}
		}
	}
	//SS汇报SPC故障
	if(rev_spc_mal){
		rev_spc_mal = 0;
		for(i = 0;i < 5;i++){
			for(j = 0;j < 15;j++){
				if(ss.sc[i].spc[j].HWTtest){
					cJSON_AddStringToObject(cs,"deviceID",ss.sc[i].spc[j].deviceid);
					cJSON_AddStringToObject(cs,"firmware",ss.sc[i].spc[j].firmware);
					cJSON_AddNumberToObject(cs,"HWTtest",ss.sc[i].spc[j].HWTtest);
					cJSON_AddNumberToObject(cs,"meshID",ss.sc[i].meshid);
					payload = cJSON_PrintUnformatted(cs);
					if(payload)	cJSON_Delete(cs);
					init_tx_message(0x43,0x01,0x13,topic,gene_message_id_H,gene_message_id_L,0x00,payload);
					send_message(&Txto4004);
				}
			}
		}
	}
}
//Real Time Reporting
//SS转发ST发出检测到运动的指令
void send_rt(void)
{
	u8 temp_counter,gene_message_id_H,gene_message_id_L;
	cJSON *cs,*sub_cs,*sub_cs2,*sub_cs3;
	char *payload;
	char *topic = "/rt";
	temp_counter = (u8)TIM2->CNT;
	gene_message_id_H = random(temp_counter);
	temp_counter = (u8)TIM2->CNT;
	gene_message_id_L = random(temp_counter);
	if(rev_st_rt){
		rev_st_rt = 0;
		cs = cJSON_CreateObject();
		if((ss_rt.px.isPX)&&(ss_rt.eg.isEG)){
			cJSON_AddItemToObject(cs, "report",sub_cs=cJSON_CreateArray());
			cJSON_AddItemToArray(sub_cs,sub_cs2 = cJSON_CreateObject());
			cJSON_AddStringToObject(sub_cs2,"sepid",ss_rt.sepid);
			cJSON_AddStringToObject(sub_cs2,"type","PX");
			cJSON_AddNumberToObject(sub_cs2,"value",ss_rt.px.value);
			cJSON_AddItemToArray(sub_cs,sub_cs2 = cJSON_CreateObject());
			cJSON_AddStringToObject(sub_cs2,"sepid",ss_rt.sepid);
			cJSON_AddNumberToObject(sub_cs2,"MD",ss_rt.eg.MD);
			cJSON_AddStringToObject(sub_cs2,"type","EG");
			cJSON_AddNumberToObject(sub_cs2,"value",ss_rt.eg.value);
		}
		else if((ss_rt.px.isPX)&&(!ss_rt.eg.isEG)){
			cJSON_AddItemToObject(cs, "report",sub_cs=cJSON_CreateObject());
			cJSON_AddStringToObject(sub_cs,"sepid",ss_rt.sepid);
			cJSON_AddStringToObject(sub_cs,"type","PX");
			cJSON_AddNumberToObject(sub_cs,"value",ss_rt.px.value);
		}
		else if((!ss_rt.px.isPX)&&(ss_rt.eg.isEG)){
			cJSON_AddItemToObject(cs, "report",sub_cs=cJSON_CreateObject());
			cJSON_AddStringToObject(sub_cs,"sepid",ss_rt.sepid);
			cJSON_AddNumberToObject(sub_cs,"MD",ss_rt.eg.MD);
			cJSON_AddStringToObject(sub_cs,"type","EG");
			cJSON_AddNumberToObject(sub_cs,"value",ss_rt.eg.value);
		}
		cJSON_AddItemToObject(cs, "action",sub_cs=cJSON_CreateArray());
		if(ss_rt.cp.isCP){
			cJSON_AddItemToArray(sub_cs,sub_cs2 = cJSON_CreateObject());
			cJSON_AddStringToObject(sub_cs2,"sepid",ss_rt.sepid);
			cJSON_AddNumberToObject(sub_cs2,"CH",ss_rt.cp.ch);
			cJSON_AddStringToObject(sub_cs2,"action",ss_rt.cp.action);
			cJSON_AddStringToObject(sub_cs2,"topos",ss_rt.cp.topos);
			cJSON_AddItemToObject(sub_cs2,"option",sub_cs3 = cJSON_CreateObject());
			cJSON_AddNumberToObject(sub_cs3,"duration",ss_rt.cp.option_duration);
			cJSON_AddNumberToObject(sub_cs3,"erase",ss_rt.cp.option_erase);
			cJSON_AddNumberToObject(sub_cs2,"timeout",ss_rt.cp.timeout);
		}
		cJSON_AddItemToObject(cs, "action",sub_cs=cJSON_CreateArray());
		if(ss_rt.gt.isGT){
			cJSON_AddItemToArray(sub_cs,sub_cs2 = cJSON_CreateObject());
			cJSON_AddStringToObject(sub_cs2,"sepid",ss_rt.sepid);
			cJSON_AddNumberToObject(sub_cs2,"CH",ss_rt.gt.ch);
			cJSON_AddStringToObject(sub_cs2,"action",ss_rt.gt.action);
			cJSON_AddStringToObject(sub_cs2,"topos",ss_rt.gt.topos);
			cJSON_AddItemToObject(sub_cs2,"option",sub_cs3 = cJSON_CreateObject());
			cJSON_AddNumberToObject(sub_cs3,"duration",ss_rt.gt.option_duration);
			cJSON_AddNumberToObject(sub_cs3,"erase",ss_rt.gt.option_erase);
			cJSON_AddNumberToObject(sub_cs2,"timeout",ss_rt.gt.timeout);
		}
		payload = cJSON_PrintUnformatted(cs);
		if(payload)	cJSON_Delete(cs);
		init_tx_message(0x43,0x01,0x03,topic,gene_message_id_H,gene_message_id_L,0x00,payload);
		send_message(&Txto4004);
	}
}

/***********************回复函数**************************/
//回复action_perform，此函数放到Task100ms
void rev_action_perform(void)
{
	u8 i,j,k,l;
	cJSON *cs,*sub_cs,*sub_cs2,*sub_cs3;
	char *payload;
	char *topic = "";
	if(rev_action_done){
		rev_action_done = 0;
		cs = cJSON_CreateObject();
		cJSON_AddNumberToObject(cs,"seq",ss_ap.qos - 2);
		cJSON_AddNumberToObject(cs,"success",rev_action_success_cnt);
		rev_action_success_cnt = 0;
		rev_action_fail_cnt = 0;
		cJSON_AddItemToObject(cs, "result",sub_cs=cJSON_CreateArray());
		cJSON_AddItemToObject(cs, "status",sub_cs2=cJSON_CreateObject());
		for(i = 1;i < ss_ap.qos - 2;i++){
			for(j = 0; j < 10;j++){
				if(ss_ap.sl_ap[j].seqid == i){//找到seqid为i的ss_ap下的sl_ap
					for(k = 0; k < 5;k++){
						for(l = 0;l < 15;l++){
							//寻找与ss_ap.sl_ap[j].sepid对应的slc或者spc
							if(strncmp(ss.sc[k].slc[l].deviceid,ss_ap.sl_ap[j].sepid,8) == 0){
								if(ss.sc[k].slc[l].status._flag_bit.bit1){//成功执行指令
									ss.sc[k].slc[l].status._flag_bit.bit1 = 0;
									send_message_without_payload(0x86,0x01,ss_ap_message_id_H,ss_ap_message_id_L,i);
									cJSON_AddItemToArray(sub_cs,sub_cs3 = cJSON_CreateObject());
									cJSON_AddNumberToObject(sub_cs3,"seqid",i);
									cJSON_AddStringToObject(sub_cs3,"code","0x0200000");
									cJSON_AddStringToObject(sub_cs3,"msg","success");
									cJSON_AddItemToObject(sub_cs2,ss.sc[k].slc[l].deviceid,sub_cs3=cJSON_CreateObject());
									cJSON_AddNumberToObject(sub_cs3,"C1",ss.sc[k].slc[l].ch1_status);
									cJSON_AddNumberToObject(sub_cs3,"C2",ss.sc[k].slc[l].ch2_status);
									cJSON_AddNumberToObject(sub_cs3,"C3",ss.sc[k].slc[l].ch3_status);
									
								}
								else if	(ss.sc[k].slc[l].status._flag_bit.bit2){//执行失败
									ss.sc[k].slc[l].status._flag_bit.bit2 = 0;
									send_message_without_payload(0x81,0x01,ss_ap_message_id_H,ss_ap_message_id_L,i);
									cJSON_AddItemToArray(sub_cs,sub_cs3 = cJSON_CreateObject());
									cJSON_AddNumberToObject(sub_cs3,"seqid",i);
									cJSON_AddStringToObject(sub_cs3,"code","0x0408001");
									cJSON_AddStringToObject(sub_cs3,"msg","Request Timeout");
									cJSON_AddItemToObject(sub_cs2,ss.sc[k].slc[l].deviceid,sub_cs3=cJSON_CreateObject());
									cJSON_AddNumberToObject(sub_cs3,"C1",ss.sc[k].slc[l].ch1_status);
									cJSON_AddNumberToObject(sub_cs3,"C2",ss.sc[k].slc[l].ch2_status);
									cJSON_AddNumberToObject(sub_cs3,"C3",ss.sc[k].slc[l].ch3_status);
								}
								break;
							}
							else if(strncmp(ss.sc[k].spc[l].deviceid,ss_ap.sl_ap[j].sepid,8) == 0){
								if(ss.sc[k].spc[l].status._flag_bit.bit1){//成功执行指令
									ss.sc[k].spc[l].status._flag_bit.bit1 = 0;
									send_message_without_payload(0x86,0x01,ss_ap_message_id_H,ss_ap_message_id_L,i);
									cJSON_AddItemToArray(sub_cs,sub_cs3 = cJSON_CreateObject());
									cJSON_AddNumberToObject(sub_cs3,"seqid",i);
									cJSON_AddStringToObject(sub_cs3,"code","0x0200000");
									cJSON_AddStringToObject(sub_cs3,"msg","success");
									cJSON_AddItemToObject(sub_cs2,ss.sc[k].spc[l].deviceid,sub_cs3=cJSON_CreateObject());
									cJSON_AddNumberToObject(sub_cs3,"C1",ss.sc[k].slc[l].ch1_status);
									cJSON_AddNumberToObject(sub_cs3,"C2",ss.sc[k].slc[l].ch2_status);
									cJSON_AddNumberToObject(sub_cs3,"C3",ss.sc[k].slc[l].ch3_status);
								}
								else if	(ss.sc[k].spc[l].status._flag_bit.bit2){//执行失败
									ss.sc[k].spc[l].status._flag_bit.bit2 = 0;
									send_message_without_payload(0x81,0x01,ss_ap_message_id_H,ss_ap_message_id_L,i);
									cJSON_AddItemToArray(sub_cs,sub_cs3 = cJSON_CreateObject());
									cJSON_AddNumberToObject(sub_cs3,"seqid",i);
									cJSON_AddStringToObject(sub_cs3,"code","0x0408001");
									cJSON_AddStringToObject(sub_cs3,"msg","Request Timeout");
									cJSON_AddItemToObject(sub_cs2,ss.sc[k].spc[l].deviceid,sub_cs3=cJSON_CreateObject());
									cJSON_AddNumberToObject(sub_cs3,"C1",ss.sc[k].slc[l].ch1_status);
									cJSON_AddNumberToObject(sub_cs3,"C2",ss.sc[k].slc[l].ch2_status);
									cJSON_AddNumberToObject(sub_cs3,"C3",ss.sc[k].slc[l].ch3_status);
								}
								break;
							}
						}					
					}
				}
				break;
			}
		}
		payload = cJSON_PrintUnformatted(cs);
		if(payload)	cJSON_Delete(cs);
		init_tx_message(0x83,0x01,0x00,topic,ss_ap_message_id_H,ss_ap_message_id_L,0x00,payload);
		send_message(&Txto4004);
	}
	myfree(topic);
}




//回复qe执行结果，此函数放到Task100ms
void rev_qe(void)
{
	u8 i,j;
	u8 double_break = 0;
	cJSON *cs,*sub_cs,*sub_cs2,*sub_cs3;
	char *payload;
	char *topic = "";
	char deviceid[10];
	if(rev_action_done2){
		rev_action_done2 = 0;
		cs = cJSON_CreateObject();
		cJSON_AddNumberToObject(cs,"seq",1);
		cJSON_AddNumberToObject(cs,"success",rev_action_success_cnt);
		rev_action_success_cnt = 0;
		rev_action_fail_cnt = 0;
		cJSON_AddItemToObject(cs, "result",sub_cs=cJSON_CreateArray());
		cJSON_AddItemToObject(cs, "status",sub_cs2=cJSON_CreateObject());
		//查询ST是否接收到qe action回复
		for(i = 0; i < 20;i++){
			if(ss.st[i].status._flag_bit.bit1){//ST收到qe action，且回复执行成功指令
				ss.st[i].status._flag_bit.bit1 = 0;
				cJSON_AddItemToArray(sub_cs,sub_cs3 = cJSON_CreateObject());
				cJSON_AddNumberToObject(sub_cs3,"seqid",1);
				cJSON_AddStringToObject(sub_cs3,"code","0x0200000");
				cJSON_AddStringToObject(sub_cs3,"msg","success");
				deviceid[0] = 'S';deviceid[1] = 'T';
				mymemcpy(deviceid+2,ss.st[i].deviceid,8);
				cJSON_AddItemToObject(sub_cs2,deviceid,sub_cs3=cJSON_CreateObject());
				//cJSON_AddItemToObject(sub_cs2,ss.st[i].deviceid,sub_cs3=cJSON_CreateObject());
				cJSON_AddNumberToObject(sub_cs3,"C1",ss.st[i].ch1_status);
				cJSON_AddNumberToObject(sub_cs3,"C2",ss.st[i].ch2_status);
				cJSON_AddNumberToObject(sub_cs3,"C3",ss.st[i].ch3_status);
				cJSON_AddNumberToObject(sub_cs3,"C4",ss.st[i].ch4_status);
				break;
			}
			else if(ss.st[i].status._flag_bit.bit2){
				ss.st[i].status._flag_bit.bit2 = 0;
				cJSON_AddItemToArray(sub_cs,sub_cs3 = cJSON_CreateObject());
				cJSON_AddNumberToObject(sub_cs3,"seqid",1);
				cJSON_AddStringToObject(sub_cs3,"code","0x0408001");
				cJSON_AddStringToObject(sub_cs3,"msg","Request Timeout");
				deviceid[0] = 'S';deviceid[1] = 'T';
				mymemcpy(deviceid+2,ss.st[i].deviceid,8);
				cJSON_AddItemToObject(sub_cs2,deviceid,sub_cs3=cJSON_CreateObject());
				//cJSON_AddItemToObject(sub_cs2,ss.st[i].deviceid,sub_cs3=cJSON_CreateObject());
				cJSON_AddNumberToObject(sub_cs3,"C1",ss.st[i].ch1_status);
				cJSON_AddNumberToObject(sub_cs3,"C2",ss.st[i].ch2_status);
				cJSON_AddNumberToObject(sub_cs3,"C3",ss.st[i].ch3_status);
				cJSON_AddNumberToObject(sub_cs3,"C4",ss.st[i].ch4_status);
			}
		}
		//查询SC是否接收到qe action回复
		for(i = 0;i < 5;i++){
			for(j = 0; j < 15;j++){
				if(ss.sc[i].slc[j].status._flag_bit.bit1){//SLC收到qe action，且回复执行成功指令
					ss.sc[i].slc[j].status._flag_bit.bit1 = 0;
					cJSON_AddItemToArray(sub_cs,sub_cs3 = cJSON_CreateObject());
					cJSON_AddNumberToObject(sub_cs3,"seqid",1);
					cJSON_AddStringToObject(sub_cs3,"code","0x0200000");
					cJSON_AddStringToObject(sub_cs3,"msg","success");
					deviceid[0] = 'S';deviceid[1] = 'L';
					mymemcpy(deviceid+2,ss.sc[i].slc[j].deviceid,8);
					cJSON_AddItemToObject(sub_cs2,deviceid,sub_cs3=cJSON_CreateObject());
					cJSON_AddNumberToObject(sub_cs3,"C1",ss.sc[i].slc[j].ch1_status);
					cJSON_AddNumberToObject(sub_cs3,"C2",ss.sc[i].slc[j].ch2_status);
					cJSON_AddNumberToObject(sub_cs3,"C3",ss.sc[i].slc[j].ch3_status);
					double_break = 1;
					break;
				}
				else if(ss.sc[i].slc[j].status._flag_bit.bit2){
					ss.sc[i].slc[j].status._flag_bit.bit2 = 0;
					cJSON_AddItemToArray(sub_cs,sub_cs3 = cJSON_CreateObject());
					cJSON_AddNumberToObject(sub_cs3,"seqid",1);
					cJSON_AddStringToObject(sub_cs3,"code","0x0408001");
					cJSON_AddStringToObject(sub_cs3,"msg","Request Timeout");
					deviceid[0] = 'S';deviceid[1] = 'L';
					mymemcpy(deviceid+2,ss.sc[i].slc[j].deviceid,8);
					cJSON_AddItemToObject(sub_cs2,deviceid,sub_cs3=cJSON_CreateObject());
					cJSON_AddNumberToObject(sub_cs3,"C1",ss.sc[i].slc[j].ch1_status);
					cJSON_AddNumberToObject(sub_cs3,"C2",ss.sc[i].slc[j].ch2_status);
					cJSON_AddNumberToObject(sub_cs3,"C3",ss.sc[i].slc[j].ch3_status);
					double_break = 1;
					break;
				}
				if(ss.sc[i].spc[j].status._flag_bit.bit1){//SPC收到qe action，且回复执行成功指令
					ss.sc[i].spc[j].status._flag_bit.bit1 = 0;
					cJSON_AddItemToArray(sub_cs,sub_cs3 = cJSON_CreateObject());
					cJSON_AddNumberToObject(sub_cs3,"seqid",1);
					cJSON_AddStringToObject(sub_cs3,"code","0x0200000");
					cJSON_AddStringToObject(sub_cs3,"msg","success");
					deviceid[0] = 'S';deviceid[1] = 'P';
					mymemcpy(deviceid+2,ss.sc[i].spc[j].deviceid,8);
					cJSON_AddItemToObject(sub_cs2,deviceid,sub_cs3=cJSON_CreateObject());
					cJSON_AddNumberToObject(sub_cs3,"C1",ss.sc[i].spc[j].ch1_status);
					cJSON_AddNumberToObject(sub_cs3,"C2",ss.sc[i].spc[j].ch2_status);
					cJSON_AddNumberToObject(sub_cs3,"C3",ss.sc[i].spc[j].ch3_status);
					double_break = 1;
					break;
				}
				else if(ss.sc[i].spc[j].status._flag_bit.bit2){
					ss.sc[i].spc[j].status._flag_bit.bit2 = 0;
					cJSON_AddItemToArray(sub_cs,sub_cs3 = cJSON_CreateObject());
					cJSON_AddNumberToObject(sub_cs3,"seqid",1);
					cJSON_AddStringToObject(sub_cs3,"code","0x0408001");
					cJSON_AddStringToObject(sub_cs3,"msg","Request Timeout");
					deviceid[0] = 'S';deviceid[1] = 'P';
					mymemcpy(deviceid+2,ss.sc[i].spc[j].deviceid,8);
					cJSON_AddItemToObject(sub_cs2,deviceid,sub_cs3=cJSON_CreateObject());
					cJSON_AddNumberToObject(sub_cs3,"C1",ss.sc[i].spc[j].ch1_status);
					cJSON_AddNumberToObject(sub_cs3,"C2",ss.sc[i].spc[j].ch2_status);
					cJSON_AddNumberToObject(sub_cs3,"C3",ss.sc[i].spc[j].ch3_status);
					double_break = 1;
					break;
				}
			}
			if(double_break)	{double_break = 0;break;}
		}
		payload = cJSON_PrintUnformatted(cs);
		if(payload)	cJSON_Delete(cs);
		init_tx_message(0x83,0x01,0x00,topic,ss_qe_message_id_H,ss_qe_message_id_L,0x00,payload);
		send_message(&Txto4004);
	}
	myfree(topic);
}

//回复device/status指令，此函数放在Task100ms中
void rev_device_status(void)
{
	u8 i,j;
	cJSON *cs,*sub_cs;
	char *payload;
	char *topic = "";
	if(ack_des){
		ack_des = 0;
		cs = cJSON_CreateObject();
		for(i = 0;i < 5;i++){
			for(j = 0;j < 15;j++){
				if((!ss.sc[i].slc[j].deviceid[0]) && (ss.sc[i].slc[j].meshid)){//device id和mesh id不为空才发送状态
					cJSON_AddItemToObject(cs, ss.sc[i].slc[j].deviceid,sub_cs=cJSON_CreateObject());
					cJSON_AddNumberToObject(sub_cs,"C1",ss.sc[i].slc[j].ch1_status);
					cJSON_AddNumberToObject(sub_cs,"C2",ss.sc[i].slc[j].ch2_status);
					cJSON_AddNumberToObject(sub_cs,"C3",ss.sc[i].slc[j].ch3_status);
					cJSON_AddNumberToObject(sub_cs,"C4",ss.sc[i].slc[j].ch4_status);
				}
				if((!ss.sc[i].spc[j].deviceid[0]) && (ss.sc[i].spc[j].meshid)){//device id和mesh id不为空才发送状态
					cJSON_AddItemToObject(cs, ss.sc[i].spc[j].deviceid,sub_cs=cJSON_CreateObject());
					cJSON_AddNumberToObject(sub_cs,"C1",ss.sc[i].spc[j].ch1_status);
					cJSON_AddNumberToObject(sub_cs,"C2",ss.sc[i].spc[j].ch2_status);
					cJSON_AddNumberToObject(sub_cs,"C3",ss.sc[i].spc[j].ch3_status);
					cJSON_AddNumberToObject(sub_cs,"C4",ss.sc[i].spc[j].ch4_status);
				}
			}
		}
		payload = cJSON_PrintUnformatted(cs);
		if(payload)	cJSON_Delete(cs);
		init_tx_message(0x83,0x01,0x00,topic,ss_des_message_id_H,ss_des_message_id_L,0x00,payload);
		send_message(&Txto4004);
	}
	myfree(topic);
}

