
/* Includes ------------------------------------------------------------------*/
#define _COM4004_GLOBAL

#include "includes.h"
#include <stdlib.h>
#include "sensors.h"
#include "ir.h"


devList_t devList = {0};

/* 用来解析qe指令 */
qe_t ssp_qe = {0};

/* 用来解析topic */
topic_t *topic_head = NULL;
topic_t *topic_last = NULL;


int mystrcmp(unsigned char *str1, const unsigned char *str2)
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
    value = rand() % (RANDOM_MAX_NUM + 1- RANDOM_MIN_NUM) + RANDOM_MIN_NUM; //产生随机数1~255
	}  
  return value;  
} 



/*----------------------------------------------------------------------------
	
-----------------------------------------------------------------------------*/
u16 ssp_get_message_id(void)
{
	static u16 id = 0;
	
	id++;

	if(id == 0){
		id = 1;
	}

	return id;
}


/*----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/
void ssp_data_st_to_sicp_message(u8 st_num)
{
	u8 len = 0, cnt = 0;
	SICP_Message msg;
	
	msg.frame_h1 = 0xEE;
	msg.frame_h2 = 0xEE;
	msg.message_id = sicp_get_message_id();
	msg.mesh_id_H = (u8)(ss.st[st_num].meshid >> 8);
	msg.mesh_id_L = (u8)(ss.st[st_num].meshid & 0xff);
	msg.payload[0] = 0x37;	
	msg.payload[1] = ss.st[st_num].level1;
	msg.payload[2] = ss.st[st_num].level2;
	
	sicp_send_message(&msg, 3, 1);
		
}

/*-------------------------------------------------------------------------
    要求必须要检测到4个key值
-------------------------------------------------------------------------*/
void ssp_data_st(u8 *payload)
{
	cJSON *root, *tmp, *devjson, *onejson, *newjson;
	int i, devsize, ret = RECEIPT_CODE_FAILED;
	u8 num, level1, level2, cnt = 0;
	
	root = cJSON_Parse((char*)payload);
	if(root){
		
		devjson = root->child;
		while(devjson){
			
			cnt = 0;			
			num = get_series_number(GET_SERIES_WHEN_EXSIT, GET_SERIES_ST, 0, devjson->string); 
			if(num != GET_SERIES_ERROR){
				level1 = 0;				
				level2 = 0;
				tmp = cJSON_GetObjectItem(devjson, "key0");
				if(tmp){
					cnt++;
					if(tmp->valueint == (-1)){
						level1 |= ((MPR121_LONG_TOUCH_INVALID & 0x0f) << 4);
					}else{					
						level1 |= (u8)((tmp->valueint & 0x0f) << 4);
					}
				}
				tmp = cJSON_GetObjectItem(devjson, "key1");
				if(tmp){
					cnt++;
					if(tmp->valueint == (-1)){
						level1 |= (MPR121_LONG_TOUCH_INVALID & 0x0f);
					}else{					
						level1 |= (u8)(tmp->valueint & 0x0f);
					}
				}				
				tmp = cJSON_GetObjectItem(devjson, "key2");
				if(tmp){
					cnt++;
					if(tmp->valueint == (-1)){
						level2 |= ((MPR121_LONG_TOUCH_INVALID & 0x0f) << 4);
					}else{					
						level2 |= (u8)((tmp->valueint & 0x0f) << 4);
					}
				}
				tmp = cJSON_GetObjectItem(devjson, "key3");
				if(tmp){
					cnt++;
					if(tmp->valueint == (-1)){
						level2 |= (MPR121_LONG_TOUCH_INVALID & 0x0f);
					}else{					
						level2 |= (u8)(tmp->valueint  & 0x0f);
					}										
				}

				if(cnt == 4){			
					ret = RECEIPT_CODE_SUCCESS;			/* 只要有一个设备解析成功就回复成功 */
					if((level1 != ss.st[num].level1) || (level2 != ss.st[num].level2)){		/* 存在有不一样的key */
						ss.st[num].level1 = level1;
						ss.st[num].level2 = level2;
						ssp_data_st_to_sicp_message(num);
					}
				}	
				
			}

			devjson = devjson->next;

		}

		cJSON_Delete(root);

	}

	ssp_recepit_response(ret);	/* 回复receipt回执 */	

}



/*----------------------------------------------------------------------------
	//推送传感器数据st/sc/slc/slc及ss自身传感器
	//type为0x43时为主动推送，type为0x83时是被动推送(接收到/data/sync)
-----------------------------------------------------------------------------*/
void ssp_data_sync(SSP_SEND_TYPE type)
{
	u8 i;
	cJSON *cs,*sub_cs;
	char *payload;
	char *topic;

	if(type == SSP_GET) topic = "";
	if(type == SSP_POST) topic = "/data/sync";
	
	cs = cJSON_CreateObject();    
	if(cs ){
		sub_cs=cJSON_CreateObject();
		if(sub_cs){
			cJSON_AddNumberToObject(sub_cs, "HM", (int)sensors_value.sht3x.humidity);
			cJSON_AddNumberToObject(sub_cs, "TP", (int)(sensors_value.sht3x.temperature * 10));
			cJSON_AddNumberToObject(sub_cs, "PT", (int)sensors_value.pm25_density);
			cJSON_AddNumberToObject(sub_cs, "SM", sensors_value.smokeModule_value);
			cJSON_AddNumberToObject(sub_cs, "PR", 0);		//未有传感器
			cJSON_AddNumberToObject(sub_cs, "CO", sensors_value.co_ppm);
			cJSON_AddNumberToObject(sub_cs, "CD", sensors_value.co2_density);
			cJSON_AddNumberToObject(sub_cs, "VO", sensors_value.voc_ppm);
			cJSON_AddItemToObject(cs, ss.deviceid, sub_cs);

		}

		for(i = 0; i < ST_NUMS_OF_SS; i++){
			if(ss.st[i].meshid){		
				sub_cs=cJSON_CreateObject();
				if(sub_cs){
					cJSON_AddNumberToObject(sub_cs, "CS", ((int)ss.st[i].sense.color_sense_H << 16) + ((int)ss.st[i].sense.color_sense_M << 8) + ss.st[i].sense.color_sense_L);
					cJSON_AddNumberToObject(sub_cs, "AL", ss.st[i].sense.ambient_light);
					cJSON_AddItemToObject(cs, ss.st[i].deviceid, sub_cs);			
				}
			}
		}
		
		payload = cJSON_PrintUnformatted(cs);
		if(payload){
			if(type == SSP_GET) 	ssp_send_message(0x83, 1, 0, topic, Rxfr4004.rx_var_header.msgid, 0, payload);
			if(type == SSP_POST) ssp_send_message(0x43, 1, 10, topic, ssp_get_message_id(), 0, payload);
			myfree(payload);
		}	
		cJSON_Delete(cs);


	}


}


/*----------------------------------------------------------------------------
	qe指令转化成sicp串口数据
	ssp_qe
-----------------------------------------------------------------------------*/
void ssp_qe_to_sicp_message(void)
{
	u8 len = 0, cnt = 0;
	SICP_Message msg;
	
	msg.frame_h1 = 0xEE;
	msg.frame_h2 = 0xEE;
	msg.message_id = sicp_get_message_id();
	msg.mesh_id_H = (u8)(ssp_qe.meshid >> 8);
	msg.mesh_id_L = (u8)(ssp_qe.meshid & 0xff);
	msg.payload[0] = ssp_qe.cmd;
	
	switch(ssp_qe.cmd){
		
		case 0x51:
		case 0x55:
		case 0x56:
			msg.payload[1] = ssp_qe.mdid_channel[0];
			msg.payload[2] = ssp_qe.topos;
			msg.payload[3] = ssp_qe.duration;
			len = 4;
			break;
			
		case 0x57:
		case 0x58:			
			cnt = 0;
			while(ssp_qe.mdid_channel[cnt]){
				msg.payload[cnt + 2] = ssp_qe.mdid_channel[cnt];
				cnt++;			
			}
			msg.payload[cnt + 2] = ssp_qe.topos;
			msg.payload[cnt + 3] = ssp_qe.duration;			
			msg.payload[1] = cnt;
			len = cnt + 4;
			break;

		default:
			break;
			
	}
	
	sicp_send_message(&msg, len, 1);
		
}


/*----------------------------------------------------------------------------
	qe指令转化成sicp串口数据
	ssp_qe
-----------------------------------------------------------------------------*/
void ssp_qe_to_sicp_cmd(void)
{
	u8 num;
	
	ssp_qe.cmd = 0;
	
	if(ssp_qe.action[0] == 'D' || ssp_qe.action[0] == 'W' ){			/* DM, WP, DMM, WPM */

		num = get_series_number(GET_SERIES_WHEN_EXSIT, GET_SERIES_SC, 0, ssp_qe.sepid); 
		if(num != GET_SERIES_ERROR){
			
			ssp_qe.meshid = ss.sc[num].meshid;

			if(ssp_qe.action[0] == 'D' && ssp_qe.action[1] == 'M' ){
				
				if(ssp_qe.action[2] == 0){
					ssp_qe.cmd = 0x51;
				}
				if(ssp_qe.action[2] == 'M'){
					ssp_qe.cmd = 0x57;
				}
			}		
			if(ssp_qe.action[0] == 'W' && ssp_qe.action[1] == 'P' ){

				if(ssp_qe.action[2] == 0){
					ssp_qe.cmd = 0x55;
				}
				if(ssp_qe.action[2] == 'M'){
					ssp_qe.cmd = 0x58;
				}
			}	
			
		}
	}	
		
	if((ssp_qe.action[0] == 'C') && (ssp_qe.action[1] == 'P')){		//快速执行的是ST

		num = get_series_number(GET_SERIES_WHEN_EXSIT, GET_SERIES_ST, 0, ssp_qe.sepid);	
		if(num != GET_SERIES_ERROR){
			ssp_qe.meshid = ss.st[num].meshid;
			ssp_qe.cmd = 0x56;
		}
	}

	if(ssp_qe.cmd){
		ssp_qe_to_sicp_message();
	}		
	
}



/*----------------------------------------------------------------------------
	//解析qe深度命令
	topic 为从qe之后开始, 字符串的顺序不可改变
	/qe/sepid/SCAA55AB80/action/DMM/MD/9,10/CH/3,3/topos/00/duration/20
-----------------------------------------------------------------------------*/
int ssp_qe_analyze(topic_t *topic)
{
	int cnt = 0;
	int ret = 0;
	u8 data;
	topic_t *tmp;
	char * pString;

	if(!topic || !(topic->string)){return -1;}
	
	memset(&ssp_qe, 0, sizeof(ssp_qe));		//清空ssp_qe结构体变量

	if(strcmp(topic->string, key_sepid) == 0){			/* sepid */
		
		tmp = topic->next;
		if(tmp && tmp->string){	
			
			strcpy(ssp_qe.sepid, tmp->string);		/* SCAA55AB80 */

			tmp = tmp->next;
			if(tmp && tmp->string){ 			
				if(strcmp(tmp->string, key_action) == 0){ 		/* action */
					
					tmp = tmp->next;
					if(tmp && tmp->string){ 				

						strcpy(ssp_qe.action, tmp->string);	/* DM, WP, UR , DMM, WPM */

						if(*(tmp->string) == 'U' && *(tmp->string) == 'R'){		/* UR */
							
							ret = 0x10000000;
							tmp = tmp->next;							
							if(tmp && tmp->string){ 						
								if(strcmp(tmp->string, "type") == 0){			/* type */
									tmp = tmp->next;							
									if(tmp && tmp->string){ 						
										ret++;
										ssp_qe.type = string_toDec(tmp->string);	
									}									
								}									
							}

							if(tmp){
								tmp = tmp->next;							
								if(tmp && tmp->string){ 						
									if(strcmp(tmp->string, "code") == 0){		/* code */
										tmp = tmp->next;							
										if(tmp && tmp->string){ 						
											ret++;
//											strcpy(ssp_qe.code, tmp->string);	
										}									
									}									
								}
							}					
						
						}
						else if(*(tmp->string) == 'C' && *(tmp->string) == 'P'){	/* CP */
						
							ret = 0x20000000;
							tmp = tmp->next;							
							if(tmp && tmp->string){ 						
								if(*(tmp->string) == 'C' ){ 					/* C */
									tmp = tmp->next;							
									if(tmp && tmp->string){ 						
										ret++;
										ssp_qe.mdid_channel[0]= *(tmp->string) - '0';
									}									
								}									
							}

							if(tmp){
								tmp = tmp->next;							
								if(tmp && tmp->string){ 						
									if(strcmp(tmp->string, "topos") == 0){					/* topos */
										tmp = tmp->next;							
										if(tmp && tmp->string){ 						
											ret++;
											if(*(tmp->string) == 'F' && *(tmp->string + 1) == 'F'){
												ssp_qe.topos = 100;
											}else{
												ssp_qe.topos = (*(tmp->string) - '0') * 10 + (*(tmp->string + 1) -'0');
											}							
										}									
									}									
								}
							}
						}
						else{											/* DM, WP, DMM, WPM */

							ret = 0x30000000;
				
							tmp = tmp->next;							
							if(tmp && tmp->string){ 						
								if(*(tmp->string) == 'M' && *(tmp->string + 1) == 'D'){		/* MD */
									tmp = tmp->next;							
									if(tmp && tmp->string){ 						
										ret++;
										pString = tmp->string;
										data = 0;
										cnt = 0;
										while(*pString){
											
											if(*pString == ','){										
												ssp_qe.mdid_channel[cnt++] = (data << 4);										
												data = 0;									
											}else{
												data = (data * 10) + (*pString - '0');
											}
											pString++;
										}									
										ssp_qe.mdid_channel[cnt] = (data << 4); 									
									}									
								}									
							}

							if(tmp){
								tmp = tmp->next;							
								if(tmp && tmp->string){ 						
									if(*(tmp->string) == 'C' && *(tmp->string + 1) == 'H'){ 	/* CH */
										tmp = tmp->next;							
										if(tmp && tmp->string){ 						
											ret++;
											pString = tmp->string;
											cnt = 0;
											while(*pString){
												
												if(*pString != ','){											
													ssp_qe.mdid_channel[cnt++] += (*pString - '0'); 									
												}
												pString++;
											}								
										}									
									}									
								}
							}

							if(tmp){
								tmp = tmp->next;							
								if(tmp && tmp->string){ 						
									if(strcmp(tmp->string, "topos") == 0){					/* topos */
										tmp = tmp->next;							
										if(tmp && tmp->string){ 						
											ret++;
											if(*(tmp->string) == 'F' && *(tmp->string + 1) == 'F'){
												ssp_qe.topos = 100;
											}else{
												ssp_qe.topos = (*(tmp->string) - '0') * 10 + (*(tmp->string + 1) -'0');
											}							
										}									
									}									
								}
							}
							
							if(tmp){
								tmp = tmp->next;							
								if(tmp && tmp->string){ 						
									if(strcmp(tmp->string, "duration") == 0){				/* duration */
										tmp = tmp->next;							
										if(tmp && tmp->string){ 						
											ret++;
											ssp_qe.duration = (u8)string_toDec(tmp->string);									
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
	return ret;
	
}


/*----------------------------------------------------------------------------
	//解析alarm深度指令
-----------------------------------------------------------------------------*/
void ssp_alarm_analyze(topic_t *topic)		
{
	
	if(!strcmp(topic->string, key_sepid)){				//sepid

		if(topic->next){

			topic = topic->next;

			if(!strcmp(topic->string, ss.deviceid)){		//
			
				if(topic->next){
			
					topic = topic->next;
						
					if(!strcmp(topic->string, "lv")){		//
					
						if(topic->next){
					
							topic = topic->next;

							ss.alarm_level = *(topic->string) -'0';
							
							ssp_recepit_response(RECEIPT_CODE_SUCCESS);
							
						}						
					}
		
				}
				
				
			}

		}
		
	}


}



/*----------------------------------------------------------------------------
	清空某个st中的config_st链表
-----------------------------------------------------------------------------*/
void ssp_config_st_clear(u8 num)
{
	config_st_t *pNext;
	
	while(ss.st[num].config_head){		
		
		pNext = ss.st[num].config_head->next;
		myfree(ss.st[num].config_head);
		ss.st[num].config_head = pNext;
	}
	ss.st[num].config_last = NULL;

}

/*-------------------------------------------------------------------------
	添加topic节点，每个节点代表一个字符串
-------------------------------------------------------------------------*/
int ssp_config_st_add_node(u8 num, config_st_t *config)
{
	int ret = -1;
	config_st_t * newNode;
	
	newNode = (config_st_t *)mymalloc(sizeof(config_st_t)); if(!newNode){ return -1;}

	if(newNode){
		
		newNode->next 		= NULL;
		newNode->flag.byte	= 0;		
		newNode->type 		= config->type;
		newNode->key 		= config->key;
		newNode->cond		= config->cond;
		newNode->meshid 	= config->meshid;
		newNode->boardid 	= config->boardid;
		newNode->action 	= config->action;
		newNode->value		= config->value;
		
		if(!ss.st[num].config_last){		/* 链表尾是否为空? */
			ss.st[num].config_head = newNode;
			ss.st[num].config_last = newNode;
		}else{
			ss.st[num].config_last->next = newNode;	/* add node */
			ss.st[num].config_last = newNode;		/* new list end */
		}
		
		ret = 0;
	}	

	return ret;
	
}


/*-------------------------------------------------------------------------   
	//解析config_st指令
-------------------------------------------------------------------------*/
void ssp_config_st_analyze(u8 *payload)
{
	cJSON *root,*tmp,*devs,*config_json;
	int i, size;
	
	u8 st_num, num, ret;
	config_st_t config = {0};
//	u16 gesture[4] = {0x0e, 0x0f, 0x0c, 0x0d};

	root = cJSON_Parse((char*)payload);

	if(root){

		devs = root->child;
		
		while(devs && devs->string && devs->type == cJSON_Array){

			st_num = get_series_number(GET_SERIES_WHEN_EXSIT, GET_SERIES_ST, 0, devs->string);	
			if(st_num != GET_SERIES_ERROR){

				ssp_config_st_clear(st_num);			//清除掉原有的配置列表

				size = cJSON_GetArraySize(devs);
				if(size){

					for(i = 0; i < size; i++){
						config_json = cJSON_GetArrayItem(devs, i);
						
						if(config_json){
							
							memset(&config, 0, sizeof(config));							
							tmp = cJSON_GetObjectItem(config_json, key_type);						//type
							if(tmp){	
								ret = 0;
								config.type = (u8)tmp->valueint;
								if((config.type == 0x01) || (config.type == 0xf1)){		//设置按键或取消按键(只需要key就可以)

									tmp = cJSON_GetObjectItem(config_json, "key");					//key
									if(tmp){
										config.key = tmp->valueint;
										ret = 1;
									} 
								}
								if((config.type == 0x02) || (config.type == 0xf2)){ 	//设置手势或取消手势(只需要cond就可以)
															
									tmp = cJSON_GetObjectItem(config_json, "cond");				//cond
									if(tmp && tmp->valuestring){
//										config.cond |= gesture[*(tmp->valuestring) - '1'] << 12 ;
//										config.cond |= gesture[*(tmp->valuestring + 1) - '1'] << 8 ;
//										config.cond |= gesture[*(tmp->valuestring + 2) - '1'] << 4 ;	
										config.cond |= (u16)(*(tmp->valuestring) - '0' ) << 12 ;
										config.cond |= (u16)(*(tmp->valuestring + 1) - '0' ) << 8 ;
										config.cond |= (u16)(*(tmp->valuestring + 2) - '0' ) << 4 ;
										ret = 1;
									} 															
								
								}
								if(ret == 0){continue;}

								if((config.type == 0x01) || (config.type == 0x02)){		//若是设置动作的话，则继续获取其他数据

									ret = 0;
									tmp = cJSON_GetObjectItem(config_json, "targetID");				//targetID
									if(tmp && tmp->valuestring){
										if(*(tmp->valuestring) == 'S' && *(tmp->valuestring + 1) == 'C'){
											num = get_series_number(GET_SERIES_WHEN_EXSIT, GET_SERIES_SC, 0, tmp->valuestring);	
											if(num != GET_SERIES_ERROR){
												ret = 1;
												config.meshid = ss.sc[num].meshid;																										
											}
										}
									}
									if(ret == 0){continue;}

									ret = 0;
									tmp = cJSON_GetObjectItem(config_json, key_action);				//action
									if(tmp && tmp->valuestring){
										if(*(tmp->valuestring) == 'D' && *(tmp->valuestring + 1) == 'M'){
											config.action = 0x51;
											ret = 1;
										}else if(*(tmp->valuestring) == 'W' && *(tmp->valuestring + 1) == 'P'){
											config.action = 0x55;
											ret = 1;
										}
									}
									if(ret == 0){continue;}

									ret = 0;
									tmp = cJSON_GetObjectItem(config_json, key_MDID);				//MDID
									if(tmp){
										config.boardid = ((u8)tmp->valueint) << 4;
										tmp = cJSON_GetObjectItem(config_json, key_CH);			//CH
										if(tmp){
											config.boardid |= (u8)tmp->valueint;
											ret = 1;
										}		
									}
									if(ret == 0){continue;}

									if(config.type == 0x02){
										ret = 0;
										tmp = cJSON_GetObjectItem(config_json, key_value);				//value
										if(tmp){
											config.value = (u8)tmp->valueint;	
											ret = 1;
										}
										if(ret == 0){continue;}
									}

								}


								ssp_config_st_add_node(st_num, &config);

							}
							
						}
						
					}

				}
								
			}			
			
			devs = devs->next;
			
		}
		
		cJSON_Delete(root);
				
	}
	
}


/*-------------------------------------------------------------------------
    
-------------------------------------------------------------------------*/
void ssp_action_ir(u8 *payload)
{
	cJSON *root,*tmp, *raw;
	int i, type, size, ret = RECEIPT_CODE_ANALYZE_ERROR;
	u32 len = 0;
	u8 buf[IR_SEND_LEN] = {0};
	
	root = cJSON_Parse((char*)payload);
	if (root){
		
		tmp = cJSON_GetObjectItem(root, key_type);
		if(tmp){
			type = tmp->valueint;

			if(type == 0){
				
				raw = cJSON_GetObjectItem(root, "raw");
				if(raw && (raw->type == cJSON_Array)){		/* 原始代码 */

					size = cJSON_GetArraySize(raw);
					if((size <= IR_RAW_BIT_MAX) &&  ((size % 2) == 1)){		/* 第一位是表示1，最后一位也必须是1 */
						ret = 0;
						for(i = 0; i < size; i++){
							tmp = cJSON_GetArrayItem(raw, i);
							if(tmp && (tmp->valueint < IR_RAW_LEN_MAX)){
								
								buf[IR_POSITION_DATA + i * 2] = tmp->valueint / 256;
								buf[IR_POSITION_DATA + i * 2 + 1] = tmp->valueint % 256;															
							}else{
								ret = RECEIPT_CODE_ANALYZE_ERROR;
								break;
							}
						}
						if(ret == 0){		//解析无错误
							len = size * 2;
							buf[0] = 0x7e;
							buf[1] = 0x7e;
							buf[2] = ir_get_message_id();
							buf[3] = (len + 5) / 256;
							buf[4] = (len + 5) % 256;
							buf[5] = 0x41;
							buf[6] = (u8)type;
							buf[len + 5 + 2] = XOR_Check(&buf[2], (u16)(len + 5));
							//将msgid保留下来以便回复
							addNodeTo_IRTxSLLast((char *)buf, len + 5 + 3, Rxfr4004.rx_var_header.msgid);

						}						
					}
					
				}

			}
			else{				
				tmp = cJSON_GetObjectItem(root, key_code);	/* 红外编码 */
				if(tmp && tmp->valuestring){
					len = strlen(tmp->valuestring);
					if((len <= IR_RAW_BIT_MAX) &&  ((len % 2) == 0)){
						len = len /2;
						for(i = 0; i < len; i++){
							buf[IR_POSITION_DATA + i] = string_tohex2(tmp->valuestring + 2 * i);
						}
						buf[0] = 0x7e;
						buf[1] = 0x7e;
						buf[2] = ir_get_message_id();
						buf[3] = (len + 5) / 256;
						buf[4] = (len + 5) % 256;
						buf[5] = 0x41;
						buf[6] = (u8)type;
						buf[len + 5 + 2] = XOR_Check(&buf[2], (u16)(len + 5));
						//将msgid保留下来以便回复
						addNodeTo_IRTxSLLast((char *)buf, len + 5 + 3, Rxfr4004.rx_var_header.msgid);
						ret = 0;
											
					}
			
				}	
				
			}
			
		}		
		
		cJSON_Delete(root);
		
	}	

	if(ret > 0){
		ssp_recepit_response(ret);
	}
	
}


/*-------------------------------------------------------------------------
    
-------------------------------------------------------------------------*/
void ssp_action_learn_ir(void)
{
	u8 buf[10] = {0};
	u8 len = 5;

	buf[0] = 0x7e;
	buf[1] = 0x7e;
	buf[2] = ir_get_message_id();
	buf[3] = 0;
	buf[4] = len;
	buf[5] = 0x42;
	buf[6] = 0x01;
	buf[len + 2] = XOR_Check(&buf[2], (u16)len);
	
	//将msgid保留下来以便回复
	addNodeTo_IRTxSLLast((char *)buf, len + 3, Rxfr4004.rx_var_header.msgid);

}





/*----------------------------------------------------------------------------
	清空topic链表
-----------------------------------------------------------------------------*/
void ssp_topic_clear(void)
{
	topic_t *pNext;
	
	while(topic_head){		
		pNext = topic_head->next;
		if(topic_head->string){myfree(topic_head->string);} 
		myfree(topic_head);
		topic_head = pNext;
	}
	topic_last = NULL;

}

/*-------------------------------------------------------------------------
	添加topic节点，每个节点代表一个字符串
-------------------------------------------------------------------------*/
int ssp_topic_add_node(char *psave, int length)
{
	int ret = -1;
	topic_t * newNode;
	char *string;
	
	newNode = (topic_t *)mymalloc(sizeof(topic_t)); if(!newNode){ return -1;}
	string = (char *)mymalloc(length + 1); if(!string){myfree(newNode); return -1;}

	if(newNode && string){
		newNode->next = NULL;
		newNode->len = length;

		mymemcpy(string, psave, length);
		*(string + length) = 0;		/* 字符串结束符 */
		
		newNode->string = string;
		
		if(!topic_last){		/* 链表尾是否为空? */
			topic_head = newNode;
			topic_last = newNode;
		}else{
			topic_last->next = newNode;	/* add node */
			topic_last = newNode;		/* new list end */
		}
		
		ret = 0;
	}	

	return ret;
	
}




/*----------------------------------------------------------------------------
	//解析qe深度命令
	‘/’与‘/’之间的字符串长度默认不超过100
-----------------------------------------------------------------------------*/
void ssp_topic_analyze(char *topic)//buf是topic内容
{
	int len;
	char *node = topic;	
	
	ssp_topic_clear();

	len = 0;
	while(*(topic++)){
		len++;		
		if(*topic == '/'){
			ssp_topic_add_node(node + 1, len -1);
			node = topic;
			len = 0;
		}
	}	
	ssp_topic_add_node(node + 1, len -1);

}



/*----------------------------------------------------------------------------
	//解析topic和payload
-----------------------------------------------------------------------------*/
void ssp_topic_payload_analyze(u8 *topic, u8 *payload)
{	
	topic_t *topicTmp;

	ssp_topic_analyze((char *)topic);	

	//快速判断 
	if(*(topic_head->string) == 'q'){

		if(*(topic_head->string + 1) == 'e'){					/* /qe */
			
			cmd_led_flag = 1;			
			
			ssp_qe_analyze(topic_head->next);
			ssp_qe_to_sicp_cmd();

			ssp_recepit_response(RECEIPT_CODE_ANALYZE_OK);	/* qe 直接回复receipt回执 */	
						
		}
	}
	else if(*(topic_head->string) == 'd'){

		if(strcmp(topic_head->string, "data") == 0){
			
			topicTmp = topic_head->next;
		
			if(strcmp(topicTmp->string, "sync") == 0){		/* /data/sync */
				
				ssp_data_sync(SSP_GET);
			}
			else if(strcmp(topicTmp->string, "st") == 0){		/* /data/st */	
				ssp_data_st(payload);
			
			}
			else if(strcmp(topicTmp->string, "recent") == 0){	/* /data/recent */	
		
				ack_dr = 1; 			
				//强制刷新，回复内容在task100ms的if(ack_dr)中执行				
			}
			else if(strcmp(topicTmp->string, "ir") == 0){		/* /data/ir */	
				
				
//				ssp_send_message(0x83, 1, 0, NULL, Rxfr4004.rx_var_header.msgid, 0, NULL);
			}
		
		}
		else if(strcmp(topic_head->string, "device") == 0){

			topicTmp = topic_head->next;
			
			if(strcmp(topicTmp->string, "status") == 0){		/* /device/status */
				
				ssp_device_status_response(topic);//回复在deal函数中				
			}
			else if(strcmp(topicTmp->string, "malfunction") == 0){ /* /device/malfunction */	

				ssp_device_malfunction_response(Rxfr4004.rx_var_header.msgid);
			
			}
			else if(strcmp(topicTmp->string, "list") == 0){	/* /device/list */	

				ss.flag.bit.recv_devinfo_list = 1;
				ssp_device_list_recv(payload);
				
//				ssp_check_device_legality();
//				devList.times = 0;				
			}
			else if(strcmp(topicTmp->string, "info") == 0){		
				
				if(topicTmp->next){
						
					topicTmp = topicTmp->next;
					if(strcmp(topicTmp->string, "ss") == 0){			/* /device/info/ss */
						
						ssp_device_info_ss(SSP_GET, Rxfr4004.rx_var_header.msgid);
					}
					else if(strcmp(topicTmp->string, "sub") == 0){	/* /device/info/sub */	
						
						ssp_device_info_sub_response(Rxfr4004.rx_var_header.msgid);
					}
				}
			}else{

			}

		}

	}
	else if(*(topic_head->string) == 'a'){
		
		if(strcmp(topic_head->string, "actions") == 0){
			
			cmd_led_flag = 1;			
			
			topicTmp = topic_head->next;
			
			if(topicTmp){
				
				if(strcmp(topicTmp->string, "ir") == 0){				/* /actions/ir */
					ssp_action_ir(payload);
					
				}
				else if(strcmp(topicTmp->string, "learn") == 0){		

					if(topicTmp->next){
						topicTmp = topicTmp->next;
						if(strcmp(topicTmp->string, "ir") == 0){		/* /actions/learn/ir */
							ssp_action_learn_ir();
						}						
					}								
				}
				else if(strcmp(topicTmp->string, "perform") == 0){	/* /actions/perform */
								
				}
				else if(strcmp(topicTmp->string, "refresh") == 0){		/* /actions/refresh */
				
					if(topicTmp->next){ 			//深度指令					
						deal_action_refresh(topic);
					}else{
						ack_ar = 1;
					}
					
				}
				else if(strcmp(topicTmp->string, "backlight") == 0){	/* /actions/backlight */
					
					deal_action_backlight(payload);
					ack_ab = 1;
				}



			}

		}
		else if(strcmp(topic_head->string, "alarm") == 0){			/* /alarm */
			
			if(topic_head->next){
				
				ssp_alarm_analyze(topic_head->next);
				sicp_alarm_cmd();

			}

		}

	}
	else if(*(topic_head->string) == 'c'){

		if(strcmp(topic_head->string, "config") == 0){
			
			topicTmp = topic_head->next;
		
			if(*(topicTmp->string) == 's'){

				if(*(topicTmp->string + 1) == 's'){						/* /config/ss */
					
					ss.flag.bit.recv_config_ss = 1;
					if(topicTmp->next){					//深度指令						
						deal_deepsyn_config_ss(topic, payload);
					}else{
						deal_config_ss(payload);		
					}
					
				}
				else if(*(topicTmp->string + 1) == 't'){					/* /config/st */

					if(ss.flag.bit.request_config_st == 1){			//主动请求的/config/st
						ss.flag.bit.recv_config_st = 1;
//						ssp_config_st_analyze(payload);					
						ssp_recepit_response(RECEIPT_CODE_ANALYZE_OK);
						
					}
												
				}
				else if(strcmp(topicTmp->string, "strategy") == 0){
					
					if(topicTmp->next){
						
						topicTmp = topicTmp->next;
						if(strcmp(topicTmp->string, "hpst") == 0){		/* /config/strategy/hpst */
							
//							deal_config_stragy_hpst(payload);		
						}
						else if(strcmp(topicTmp->string, "htsp") == 0){	/* /config/strategy/htsp */	
							
//							deal_config_strategy_htsp(topic);
						}
					}
					
				}
				
			}
			else if(strcmp(topicTmp->string, "mesh") == 0){
				
				deal_config_mesh(payload);		
				ack_cm = 1;			
			}
		
		}

	}

}



/*----------------------------------------------------------------------------
	//解析ssp 数据包
-----------------------------------------------------------------------------*/
u8 ssp_package_analyze(u8 *buf, u16 buf_len)
{
	u8 *temp_buf;
	u32 useful_data_len = 0;
	u8  remaining_len=0;
	u8  topic_len = 0;
	u16 payload_len = 0;

	temp_buf = buf;
	
	memset(&Rxfr4004, 0, sizeof(Rxfr4004));

	if((temp_buf[0] == 0xBB) && (temp_buf[1] == 0xBB)){	
		
		if((temp_buf[3] & 0x80) == 0x00){
			remaining_len = 1;
			useful_data_len = temp_buf[3];
		}
		else if((temp_buf[4] & 0x80) == 0x00){
			remaining_len = 2;
			useful_data_len = (temp_buf[3]-0x80) + temp_buf[4]*128;
		}
		else if((temp_buf[5] & 0x80) == 0x00){
			remaining_len = 3;
			useful_data_len = (temp_buf[3]-0x80) + ((temp_buf[4]& 0x7F)*128) + temp_buf[5]*16384;
		}
		else if((temp_buf[6] & 0x80) == 0x00){
			remaining_len = 4;
			useful_data_len = (temp_buf[3]-0x80) + ((temp_buf[4]& 0x7F)*128) + ((temp_buf[5]&0x7F)*16384) + temp_buf[6]*2097152;
		}
		
		if(remaining_len){
			
			Rxfr4004.rx_fix_header.ch.first_ch_byte = temp_buf[2];
			Rxfr4004.rx_var_header.version = temp_buf[remaining_len+3];
			Rxfr4004.rx_var_header.topic_lengthH = temp_buf[remaining_len+4];
			topic_len = Rxfr4004.rx_var_header.topic_lengthL = temp_buf[remaining_len+5];
			payload_len = useful_data_len - 3 - topic_len;
			mymemcpy(Rxfr4004.rx_var_header.topic, (temp_buf+remaining_len + 6), topic_len);
			Rxfr4004.rx_var_header.msgid = temp_buf[remaining_len+5+topic_len+1] * 256 + temp_buf[remaining_len+5+topic_len+2];
			mymemcpy(Rxfr4004.rx_payload, (temp_buf+remaining_len+8+topic_len), payload_len);			
			return 1;
		}
		else{
			return 0;
		}
		
	}
	else{
		return 0;
	}

}


/*----------------------------------------------------------------------------
	//回复心跳包
-----------------------------------------------------------------------------*/
void ssp_heart_beat(u8 fix1)
{
	u8 send_buf[2];
	
	send_buf[0] = fix1;
	send_buf[1] = 0x00;
	addNodeToUartTxSLLast((char *)send_buf, 2);
	
}


/*----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/
void recv_4004_analyze(void)
{
	if (rev_success){
		
		rev_success = 0;	
		
		if(ssp_length){				//接收到0xbb 0xbb 开头的数据
			
			if(ssp_buf[2] == 0x41){
				
				ssp_heart_beat(0x83);
				
			}else if(ssp_package_analyze(ssp_buf, ssp_length)){
				
				ssp_topic_payload_analyze(Rxfr4004.rx_var_header.topic, Rxfr4004.rx_payload);
			}

		}

		if(Usart1_Delay_Cnt <= 10){		/* 20ms以内 */
		
			if(uartTxSLHead){	//链表头不为空
				Usart1_Send((u8 *)uartTxSLHead->data, uartTxSLHead->len); //uartTxSLHead->haswrite在中断中处理			
			}

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
				tmp = cJSON_GetObjectItem(level2, key_code);
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
		ssp_recepit_response(RECEIPT_CODE_SUCCESS);
		
	}
}




//解析config_ss深度同步指令，数据存储在ss_cs中
void deal_deepsyn_config_ss(u8 *buf1,u8 *buf2)//buf1是topic内容,buf2是payload内容
{
	char first_ch;
	cJSON *root,*level1,*level2,*tmp;
	first_ch = (char)(*(buf1+12));
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
							tmp = cJSON_GetObjectItem(level2, key_code);
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
	ssp_recepit_response(RECEIPT_CODE_SUCCESS);
	
}


//解析action_refresh深度同步指令
void deal_action_refresh(u8 *buf)//buf是topic的内容
{
	ss_ar.channel = (u8)(*(buf+15) - '0')*10 + (u8)(*(buf+16) - '0');
	ss_ar.ch[0] = *(buf+18);
	ss_ar.ch[1] = *(buf+19);
	if(((ss_ar.ch[0] == 'A') && (ss_ar.ch[1] == 'L')) || (ss_ar.ch[0] == 'C') && (ss_ar.ch[1] == 'S'))//刷新ST环境光颜色和强度
		{rev_ar2 = 1; ssp_recepit_response(RECEIPT_CODE_SUCCESS);}
	else//刷新的是SS的传感器
		{rev_ar1 = 1; ssp_recepit_response(RECEIPT_CODE_SUCCESS);}
}


//解析action_backlight指令，数据存储在ss_ab中

void deal_action_backlight(u8 *buf)
{
	cJSON *root,*tmp,*level1;
	int i,cmd_size;
	root = cJSON_Parse((char*)buf);
	if (root)
	{
		tmp = cJSON_GetObjectItem(root, key_sepid);
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
				//fyl
//				if(!ss.sc[i].slc[j].deviceid[0]){//该deviceid不为空，已经保存有配置信息
//					tmp = cJSON_GetObjectItem(root,ss.sc[i].slc[j].deviceid);
//					if(tmp)		ss.sc[i].slc[j].meshid = (u16)tmp->valueint;
//				}
			}
		}
		
		for(i = 0; i < 5;i++){			//判断是不是配置spc
			for(j = 0;j < 15;j++){

				//fyl
//				if(!ss.sc[i].spc[j].deviceid[0]){//该deviceid不为空，已经保存有配置信息
//					tmp = cJSON_GetObjectItem(root,ss.sc[i].spc[j].deviceid);
//					if(tmp) 	ss.sc[i].spc[j].meshid = (u16)tmp->valueint;
//				}

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
		ssp_recepit_response(RECEIPT_CODE_SUCCESS);
	}
	
}





/*----------------------------------------------------------------------------
	//
-----------------------------------------------------------------------------*/
void ssp_calcu_remain_length(Txmessage *tx)
{
	u32 remainLen;
	
	remainLen = tx->tx_payload_len;
	
	if(tx->tx_var_header.ext_message_id){
		
		remainLen += 4;
	}else{
		remainLen += 3;
	}
	
	if(tx->tx_var_header.topic_lengthL){
//		remainLen += strlen(tx->tx_var_header.topic) + 2;	//2是topic length的长度
		remainLen += tx->tx_var_header.topic_lengthL + 2;	//2是topic length的长度
	}

	if(remainLen < 128){					// 1字节
	
		tx->tx_fix_header.r_length_len = 1;
		tx->tx_fix_header.r_length[0] = (u8)remainLen;

	}else if(remainLen < 16384){			//有2个字节的r_length
	
		tx->tx_fix_header.r_length_len = 2;
		tx->tx_fix_header.r_length[1] = (u8)(remainLen % 128) | 0x80;
		tx->tx_fix_header.r_length[0] = (u8)(remainLen /128) ;

	}else if(remainLen < 2097152){			//有3个字节的r_length
	
		tx->tx_fix_header.r_length_len = 3;
		tx->tx_fix_header.r_length[2] = (u8)(remainLen % 16384);
		remainLen -= tx->tx_fix_header.r_length[2];
		tx->tx_fix_header.r_length[1] = (u8)(remainLen % 128);
		remainLen -= tx->tx_fix_header.r_length[1];
		tx->tx_fix_header.r_length[0] = (u8)(remainLen % 128);

		tx->tx_fix_header.r_length[2] |= 0x80;
		tx->tx_fix_header.r_length[1] |= 0x80;

	}else{							//有4个字节长的r_length

		tx->tx_fix_header.r_length_len = 4;
		tx->tx_fix_header.r_length[3] = (u8)(remainLen % 2097152);
		remainLen -= tx->tx_fix_header.r_length[3];
		tx->tx_fix_header.r_length[2] = (u8)(remainLen % 128);
		remainLen -= tx->tx_fix_header.r_length[2];
		tx->tx_fix_header.r_length[1] = (u8)(remainLen % 128);
		remainLen -= tx->tx_fix_header.r_length[1];
		tx->tx_fix_header.r_length[0] = (u8)(remainLen % 128);
		
		tx->tx_fix_header.r_length[3] |= 0x80;
		tx->tx_fix_header.r_length[2] |= 0x80;
		tx->tx_fix_header.r_length[1] |= 0x80;

	}
	
}


/*----------------------------------------------------------------------------
	发送Txto4004内容
	 BB BB 83 05 01 21 44 7B 7D 0A 0A 
-----------------------------------------------------------------------------*/
void ssp_add_node_to_uart(Txmessage *tx)
{
	u8 *send_buf;
	int txlen = 0;
	char * temp_send_buf;
	
	temp_send_buf = (char *)mymalloc(1024);
	if(temp_send_buf && tx){
		
		memset(temp_send_buf, 0, 1024);
			
		send_buf = (u8 *)temp_send_buf;
		
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
			mymemcpy(send_buf, tx->tx_var_header.topic, tx->tx_var_header.topic_lengthL);
			txlen += tx->tx_var_header.topic_lengthL;
			send_buf += tx->tx_var_header.topic_lengthL;
		}
		*send_buf++ = (u8)(tx->tx_var_header.msgid >> 8);
		*send_buf++ = (u8)(tx->tx_var_header.msgid & 0xff);
		txlen += 2; 
		if(tx->tx_var_header.ext_message_id){
			*send_buf++ = (tx->tx_var_header.ext_message_id);
			txlen++;
		}
		
		if(tx->tx_payload_len){
			memcpy(send_buf, tx->tx_payload, tx->tx_payload_len);
			txlen += tx->tx_payload_len;
		}
		
		addNodeToUartTxSLLast(temp_send_buf, txlen);
		myfree(temp_send_buf);

	}

}


/*----------------------------------------------------------------------------
	//初始化Txto4004
-----------------------------------------------------------------------------*/
void ssp_send_message(u8 firstByte, u8 ver, u8 topic_len, char *topic, u16 msgid, u8 ext_msgid, char *payload)
{
	mymemset(&Txto4004, 0, sizeof(Txto4004));

	Txto4004.tx_fix_header.ch.first_ch_byte = firstByte;
	Txto4004.tx_var_header.version = ver;
	Txto4004.tx_var_header.topic_lengthH = 0;
	Txto4004.tx_var_header.topic_lengthL = topic_len;

	if(topic_len){
		mymemcpy(Txto4004.tx_var_header.topic, topic, topic_len);
	}
	
	Txto4004.tx_var_header.msgid = msgid;


	if(ext_msgid){
		Txto4004.tx_var_header.ext_message_id = ext_msgid;
	}

	Txto4004.tx_payload_len = payload ? strlen(payload) : 0;

	ssp_calcu_remain_length(&Txto4004);

	if(payload){
		mymemcpy(Txto4004.tx_payload, payload, Txto4004.tx_payload_len);
	}

	ssp_add_node_to_uart(&Txto4004);
	
}






/**************************************ss主动发送给esh函数**********************************************/
//获取SS的配置信息，什么时候主动推送？
void send_config_ss(void)
{
	cJSON *cs,*sub_cs,*sub_cs2;
	char *payload;
	char *topic = "/config/ss";
	
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
	cJSON_AddStringToObject(sub_cs2, key_code,ss_cs.code);
	cJSON_AddStringToObject(sub_cs2,"TP",ss_cs.tp);
	cJSON_AddItemToObject(sub_cs,"airquality",sub_cs2=cJSON_CreateObject());
	cJSON_AddStringToObject(sub_cs2,"index",ss_cs.index);

	payload = cJSON_PrintUnformatted(cs);
	if(payload){
		ssp_send_message(0x83, 1, 10, topic, ssp_get_message_id(), 0, payload);
		myfree(payload);
	}	
	cJSON_Delete(cs);
	
}



/*----------------------------------------------------------------------------
	主动上报设备状态
-----------------------------------------------------------------------------*/
void ssp_device_status_post(DEVICE_TYPE devType, u8 i)
{	
	u8 j;
	cJSON *sub_cs, *root, *array;
	char *payload = NULL;
	char *topic = "/device/status";

	
	if(devType == DEVICE_SC){	
			
		if(ss.sc[i].meshid){	//设备存在
			
			root = cJSON_CreateObject();		
			if(root){

				array = cJSON_CreateArray();
				if(array){
					for(j = 0; j < SL_NUMS_OF_SC; j++){
						if(ss.sc[i].slc[j].MDID){		//device id和mesh id不为空才发送状态
							sub_cs = cJSON_CreateObject();
							if(sub_cs){
								cJSON_AddNumberToObject(sub_cs,  key_MDID, ss.sc[i].slc[j].MDID);
								cJSON_AddNumberToObject(sub_cs,  key_type, 1);
								cJSON_AddNumberToObject(sub_cs,  key_C1, ss.sc[i].slc[j].ch1_status);
								cJSON_AddNumberToObject(sub_cs,  key_C2, ss.sc[i].slc[j].ch2_status);
								cJSON_AddItemToArray(array, sub_cs);
					
							}					
						}			
					}
					
					for(j = 0; j < SP_NUMS_OF_SC; j++){
						if(ss.sc[i].spc[j].MDID){		//device id和mesh id不为空才发送状态				
							sub_cs = cJSON_CreateObject();
							if(sub_cs){
								cJSON_AddNumberToObject(sub_cs, key_MDID, ss.sc[i].spc[j].MDID);
								cJSON_AddNumberToObject(sub_cs, key_type, 2);
								cJSON_AddNumberToObject(sub_cs, key_C1, ss.sc[i].spc[j].ch1_status);
								cJSON_AddNumberToObject(sub_cs, key_C2, ss.sc[i].spc[j].ch2_status);
								cJSON_AddNumberToObject(sub_cs, key_C3, ss.sc[i].spc[j].ch3_status);
								cJSON_AddItemToArray(array, sub_cs);							
							}					
						}
					}						
					cJSON_AddItemToObject(root, ss.sc[i].deviceid, array);
					
				}
					
			}
			
		}
		
	}
		
	if(root){
		payload = cJSON_PrintUnformatted(root);
		if(payload){
			ssp_send_message(0x43, 1, 14, topic, ssp_get_message_id(), 0, payload);
			myfree(payload);
		}	
		cJSON_Delete(root);
	}
		
}



/*----------------------------------------------------------------------------
	只支持获取某 SS 下所有 SEP (SL 和 SP)/ 获取某 SS 下某一个 SEP
-----------------------------------------------------------------------------*/
void ssp_device_status_response(u8 *buf)	//buf是topic内容
{
	u8 i, j, len;
	cJSON *cs,*sub_cs, *array;
	char *payload;
	char *topic = "";
	
	len = strlen((char *)buf);
	cs = NULL;
	
	/* SS 下所有 SEP (SL 和 SP) */
	if(len == 14){
		
		cs = cJSON_CreateObject();		
		if(cs){
			for(i = 0; i < SC_NUMS_OF_SS; i++){	
				if(ss.sc[i].meshid){
					array = cJSON_CreateArray();
					if(array){
						for(j = 0; j < SL_NUMS_OF_SC; j++){
							if(ss.sc[i].slc[j].MDID){ 		//device id和mesh id不为空才发送状态
								sub_cs = cJSON_CreateObject();
								if(sub_cs){
									cJSON_AddNumberToObject(sub_cs,  key_MDID, ss.sc[i].slc[j].MDID);
									cJSON_AddNumberToObject(sub_cs,  key_type, 1);
									cJSON_AddNumberToObject(sub_cs,  key_C1, ss.sc[i].slc[j].ch1_status);
									cJSON_AddNumberToObject(sub_cs,  key_C2, ss.sc[i].slc[j].ch2_status);
									cJSON_AddItemToArray(array, sub_cs);
						
								}					
							}			
						}
						
						for(j = 0; j < SP_NUMS_OF_SC; j++){
							if(ss.sc[i].spc[j].MDID){ 		//device id和mesh id不为空才发送状态				
								sub_cs = cJSON_CreateObject();
								if(sub_cs){
									cJSON_AddNumberToObject(sub_cs, key_MDID, ss.sc[i].spc[j].MDID);
									cJSON_AddNumberToObject(sub_cs, key_type, 2);
									cJSON_AddNumberToObject(sub_cs, key_C1, ss.sc[i].spc[j].ch1_status);
									cJSON_AddNumberToObject(sub_cs, key_C2, ss.sc[i].spc[j].ch2_status);
									cJSON_AddNumberToObject(sub_cs, key_C3, ss.sc[i].spc[j].ch3_status);
									cJSON_AddItemToArray(array, sub_cs);							
								}					
							}
						}						
						cJSON_AddItemToObject(cs, ss.sc[i].deviceid, array);

					}
				}								
			}			
		}
		
	}

	/* SS 下某一个 SEP */
	
	if(cs){
		payload = cJSON_PrintUnformatted(cs);
		if(payload){
			ssp_send_message(0x83, 1, 0, topic, Rxfr4004.rx_var_header.msgid, 0, payload);
			myfree(payload);
		}	
		cJSON_Delete(cs);
	}
	
}

/*----------------------------------------------------------------------------
	解析device_list指令
-----------------------------------------------------------------------------*/
void ssp_devList_update(void)
{
	int i;
	cJSON *tmp, *onejson;
	
	if(devList.flag == 1){

		/* 更新devList的其他信息 */
		devList.scSize = 0;
		devList.stSize = 0;
		if(devList.sc){ devList.scSize = cJSON_GetArraySize(devList.sc);}	
		if(devList.st){ devList.stSize = cJSON_GetArraySize(devList.st);}	
		if(devList.scSize > SC_NUMS_OF_SS){ devList.scSize = SC_NUMS_OF_SS;}
		if(devList.stSize > ST_NUMS_OF_SS){ devList.stSize = ST_NUMS_OF_SS;}

		memset(devList.scDeviceID, 0, sizeof(devList.scDeviceID));
		memset(devList.stDeviceID, 0, sizeof(devList.stDeviceID));
		
		for(i = 0; i < devList.scSize; i++){
			onejson = cJSON_GetArrayItem(devList.sc, i);
			if(onejson){
				tmp = cJSON_GetObjectItem(onejson, key_deviceID);						
				if(tmp && tmp->valuestring){
					devList.scDeviceID[i][0] = string_tohex2(tmp->valuestring + 2);
					devList.scDeviceID[i][1] = string_tohex2(tmp->valuestring + 4);
					devList.scDeviceID[i][2] = string_tohex2(tmp->valuestring + 6);
					devList.scDeviceID[i][3] = string_tohex2(tmp->valuestring + 8);					
				}
			}		
		}

		for(i = 0; i < devList.stSize; i++){
			onejson = cJSON_GetArrayItem(devList.st, i);
			if(onejson){
				tmp = cJSON_GetObjectItem(onejson, key_deviceID);						
				if(tmp && tmp->valuestring){
					devList.stDeviceID[i][0] = string_tohex2(tmp->valuestring + 2);
					devList.stDeviceID[i][1] = string_tohex2(tmp->valuestring + 4);
					devList.stDeviceID[i][2] = string_tohex2(tmp->valuestring + 6);
					devList.stDeviceID[i][3] = string_tohex2(tmp->valuestring + 8);					
				}
			}		
		}

	}

}


/*----------------------------------------------------------------------------
	解析device_list指令
-----------------------------------------------------------------------------*/
void ssp_device_list_recv(u8 *payload)
{
	cJSON *root, *tmp, *devjson, *onejson, *newjson;
	int i, devsize, ret = 0;
	
	root = cJSON_Parse((char*)payload);
	if(root){
		
		devjson = NULL;		
		tmp = cJSON_GetObjectItem(root, key_deviceID);
		if(tmp && tmp->valuestring){
			if(strcmp(ss.deviceid, tmp->valuestring) == 0){	//配置的ID与本SS的ID相同才进行下一步
				devjson = cJSON_GetObjectItem(root, "devices");
			}
		}
				
		if(devjson && (devjson->type == cJSON_Array)){

			/* sl不存在，则创建sl列表 */
//			if(devList.sl){ 	cJSON_Delete(devList.sl);} 	devList.sl = cJSON_CreateArray();	
//			if(devList.sp){ 	cJSON_Delete(devList.sp);}	devList.sp = cJSON_CreateArray();
			if(devList.sc){ 	cJSON_Delete(devList.sc);}	devList.sc = cJSON_CreateArray();
			if(devList.st){ 	cJSON_Delete(devList.st);}	devList.st = cJSON_CreateArray();
			devList.flag = 0;
			devsize = cJSON_GetArraySize(devjson);
			if(devsize){
				for(i = 0; i < devsize; i++){
					onejson = cJSON_GetArrayItem(devjson, i);
					if(onejson){
						tmp = cJSON_GetObjectItem(onejson, key_deviceID);						
						if(tmp && tmp->valuestring){
//							if(*(tmp->valuestring + 1) == 'L'){
//								ret = 1;								
//								if(devList.sl){			/* 确保sl存在  */
//									newjson = cJSON_CreateObject();
//									cJSON_AddStringToObject(newjson, key_deviceID, tmp->valuestring);	
//									cJSON_AddItemToArray(devList.sl, newjson);
//									devList.flag = 1;
//								}																								
//							}
//							
//							if(*(tmp->valuestring + 1) == 'P'){
//								ret = 1;
//								if(devList.sp){		
//									newjson = cJSON_CreateObject();
//									cJSON_AddStringToObject(newjson, key_deviceID, tmp->valuestring);	
//									cJSON_AddItemToArray(devList.sp, newjson);
//									devList.flag = 1;
//								}	
//							}

							if(*(tmp->valuestring + 1) == 'C'){
								ret = 1;
								if(devList.sc){			
									newjson = cJSON_CreateObject();
									cJSON_AddStringToObject(newjson, key_deviceID, tmp->valuestring);	
									cJSON_AddItemToArray(devList.sc, newjson);
									devList.flag = 1;
								}	
							}
							
							if(*(tmp->valuestring + 1) == 'T'){
								ret = 1;
								if(devList.st){			
									newjson = cJSON_CreateObject();
									cJSON_AddStringToObject(newjson, key_deviceID, tmp->valuestring);	
									cJSON_AddItemToArray(devList.st, newjson);
									devList.flag = 1;
								}	
							}						
						}					
					}				
				}
				
			}
			
		}
		
		cJSON_Delete(root);
		
		if(ret == 1){
			ssp_recepit_response(RECEIPT_CODE_SUCCESS);
		}else{
			ssp_recepit_response(RECEIPT_CODE_ERROR);
		}

		ssp_devList_update();
		
	}
	
}


/*----------------------------------------------------------------------------
	主动上报 i,设备序列
-----------------------------------------------------------------------------*/
void ssp_device_info_sub_post(DEVICE_TYPE devType, u8 i)
{
	cJSON *dev, *root, *array;
	int j;
	char *payload = NULL;
	char *topic = "/device/info/sub";

	root = cJSON_CreateArray();	
	if(root){
		
		if(devType == DEVICE_ST){
			if(ss.st[i].meshid){	//设备存在				
				dev = cJSON_CreateObject(); 
				if(dev){
					cJSON_AddStringToObject(dev, key_deviceID, ss.st[i].deviceid);
					cJSON_AddNumberToObject(dev, key_model, ss.st[i].model);
					cJSON_AddNumberToObject(dev, key_firmware, ss.st[i].firmware);
					cJSON_AddNumberToObject(dev, key_HWtest, ss.st[i].HWTtest);
					cJSON_AddNumberToObject(dev, key_meshID, ss.st[i].meshid);
					cJSON_AddItemToArray(root, dev);
				}
			}
		}
	
		if(devType == DEVICE_SC){		
			if(ss.sc[i].meshid){	//设备存在				
				dev = cJSON_CreateObject(); 
				if(dev){
					cJSON_AddStringToObject(dev, key_deviceID, ss.sc[i].deviceid);
					cJSON_AddNumberToObject(dev, key_model, ss.sc[i].model);
					cJSON_AddNumberToObject(dev, key_firmware, ss.sc[i].firmware);
					cJSON_AddNumberToObject(dev, key_HWtest, ss.sc[i].HWTtest);
					cJSON_AddNumberToObject(dev, key_meshID, ss.sc[i].meshid);
					
					array = cJSON_CreateArray();
					if(array){
						for(j = 0; j < SL_NUMS_OF_SC; j++){
							if(ss.sc[i].slc[j].MDID){
								cJSON_AddItemToArray(array, cJSON_CreateNumber(ss.sc[i].slc[j].MDID));
							}
						}
						cJSON_AddItemToObject(dev, "SLC", array);						
					}
		
					array = cJSON_CreateArray();
					if(array){
						for(j = 0; j < SP_NUMS_OF_SC; j++){
							if(ss.sc[i].spc[j].MDID){
								cJSON_AddItemToArray(array, cJSON_CreateNumber(ss.sc[i].spc[j].MDID));
							}
						}
						cJSON_AddItemToObject(dev, "SPC", array);						
					}					
					cJSON_AddItemToArray(root, dev);
					
				}
			}
		}
	
		payload = cJSON_PrintUnformatted(root);
		if(payload){
			ssp_send_message(0x43, 1, 16, topic, ssp_get_message_id(), 0, payload);
			myfree(payload);
		}	
		cJSON_Delete(root);

	}

}



/*----------------------------------------------------------------------------
	回复请求 
-----------------------------------------------------------------------------*/
void ssp_device_info_sub_response(u16 msgid)
{
	cJSON *array, *dev, *root;
	int i, j;
	char *payload = NULL;
	char *topic = "";;

	root = cJSON_CreateArray();
	
	if(root){

		for(i = 0; i < ST_NUMS_OF_SS; i++){			
			if(ss.st[i].meshid){	//设备存在				
				dev = cJSON_CreateObject(); 
				if(dev){
					cJSON_AddStringToObject(dev, key_deviceID, ss.st[i].deviceid);
					cJSON_AddNumberToObject(dev, key_model, ss.st[i].model);
					cJSON_AddNumberToObject(dev, key_firmware, ss.st[i].firmware);
					cJSON_AddNumberToObject(dev, key_HWtest, ss.st[i].HWTtest);
					cJSON_AddNumberToObject(dev, key_meshID, ss.st[i].meshid);
					cJSON_AddItemToArray(root, dev);
				}
			}
		}
		
		for(i = 0; i < SC_NUMS_OF_SS; i++){			
			if(ss.sc[i].meshid){	//设备存在				
				dev = cJSON_CreateObject(); 
				if(dev){
					cJSON_AddStringToObject(dev, key_deviceID, ss.sc[i].deviceid);
					cJSON_AddNumberToObject(dev, key_model, ss.sc[i].model);
					cJSON_AddNumberToObject(dev, key_firmware, ss.sc[i].firmware);
					cJSON_AddNumberToObject(dev, key_HWtest, ss.sc[i].HWTtest);
					cJSON_AddNumberToObject(dev, key_meshID, ss.sc[i].meshid);
					
					array = cJSON_CreateArray();
					if(array){
						for(j = 0; j < SL_NUMS_OF_SC; j++){
							if(ss.sc[i].slc[j].MDID){
								cJSON_AddItemToArray(array, cJSON_CreateNumber(ss.sc[i].slc[j].MDID));
							}
						}
						cJSON_AddItemToObject(dev, "SLC", array);						
					}

					array = cJSON_CreateArray();
					if(array){
						for(j = 0; j < SP_NUMS_OF_SC; j++){
							if(ss.sc[i].spc[j].MDID){
								cJSON_AddItemToArray(array, cJSON_CreateNumber(ss.sc[i].spc[j].MDID));
							}
						}
						cJSON_AddItemToObject(dev, "SPC", array);						
					}					
					cJSON_AddItemToArray(root, dev);
					
				}
			}
		}

		payload = cJSON_PrintUnformatted(root);
		if(payload){
			ssp_send_message(0x83, 1, 0, topic, msgid, 0, payload);
			myfree(payload);
		}	
		cJSON_Delete(root);

	}

}


/*----------------------------------------------------------------------------
	device_info_ss
	
-----------------------------------------------------------------------------*/
void ssp_device_info_ss(SSP_SEND_TYPE type, u16 msgid)
{
	cJSON *cs;
	char *payload;
	char *topic;
	char topic1[] = "/device/info/ss";
	char topic2[] = "";
	
	if(type == SSP_POST) topic = topic1;
	else			  	topic = topic2;

	cs = cJSON_CreateObject(); 
	if(cs){
		
		cJSON_AddStringToObject(cs, key_deviceID, ss.deviceid);
		cJSON_AddNumberToObject(cs, key_model, ss.model);
		cJSON_AddNumberToObject(cs, key_firmware, ss.firmware);
		cJSON_AddNumberToObject(cs, key_HWtest, ss.HWTtest.byte);
		cJSON_AddNumberToObject(cs, key_meshID, ss.meshid);
		cJSON_AddStringToObject(cs, key_macWiFi, ss.macwifi);
		
		/* test */
//		cJSON_AddNumberToObject(cs, key_macWiFi,sizeof(devList.scDeviceID));
	
		payload = cJSON_PrintUnformatted(cs);
		if(payload){
			if(type == SSP_POST)	ssp_send_message(0x43, 1, 15, topic, ssp_get_message_id(), 0, payload);
			else				ssp_send_message(0x83, 1, 0, topic, msgid, 0, payload);
			myfree(payload);
		}	
		cJSON_Delete(cs);

	}

}

/*----------------------------------------------------------------------------
	应答esh的故障查询
	发送说有设备的故障信息，包括没有故障的设备
-----------------------------------------------------------------------------*/
void ssp_device_malfunction_response(u16 msgid)
{
	cJSON *cs, *root, *array;
	char *payload;
	char *topic = "";
	u8 i, j, scCount;

	root = cJSON_CreateObject();	
	if(root){		
//		if(ss.HWTtest.byte){							/* ss */
			cs = cJSON_CreateObject(); 
			if(cs){
				cJSON_AddNumberToObject(cs, key_fw, ss.firmware);
				cJSON_AddNumberToObject(cs, key_sts, ss.HWTtest.byte);
				cJSON_AddItemToObject(root, ss.deviceid, cs);	
			}
//		}

		for(i = 0; i < ST_NUMS_OF_SS; i++){			/* st */

//			if(ss.st[i].meshid && ss.st[i].HWTtest){		/* 设备存在且有故障  */
			if(ss.st[i].meshid){		/* 设备存在且有故障  */

				cs = cJSON_CreateObject(); 
				if(cs){
					cJSON_AddNumberToObject(cs, key_fw, ss.st[i].firmware);
					cJSON_AddNumberToObject(cs, key_sts, ss.st[i].HWTtest);
					cJSON_AddItemToObject(root, ss.st[i].deviceid, cs);	
				}
			}
		}
		
		for(i = 0; i < SC_NUMS_OF_SS; i++){			/* sc */

			if(ss.sc[i].meshid){				/* 设备存在 */

				scCount = 0;
				array = cJSON_CreateArray(); 
				if(array){
//					if(ss.sc[i].HWTtest){			/* sc */
						cs = cJSON_CreateObject(); 
						if(cs){
							cJSON_AddNumberToObject(cs, key_MDID, 0);
							cJSON_AddNumberToObject(cs, key_fw, ss.sc[i].firmware);
							cJSON_AddNumberToObject(cs, key_sts, ss.sc[i].HWTtest);						
							cJSON_AddItemToArray(array, cs);
							scCount++;
						}
//					}
					
					for(j = 0; j < SL_NUMS_OF_SC; j++){ 		/* slc */
//						if(ss.sc[i].slc[j].MDID && ss.sc[i].slc[j].HWTtest){
						if(ss.sc[i].slc[j].MDID){

							cs = cJSON_CreateObject(); 
							if(cs){
								cJSON_AddNumberToObject(cs, key_MDID, ss.sc[i].slc[j].MDID);
								cJSON_AddNumberToObject(cs, key_sts, ss.sc[i].slc[j].HWTtest);						
								cJSON_AddItemToArray(array, cs);
								scCount++;
							}
						}
					}

					for(j = 0; j < SP_NUMS_OF_SC; j++){ 		/* slc */
//						if(ss.sc[i].spc[j].MDID && ss.sc[i].spc[j].HWTtest){
						if(ss.sc[i].spc[j].MDID){

							cs = cJSON_CreateObject(); 
							if(cs){
								cJSON_AddNumberToObject(cs, key_MDID, ss.sc[i].spc[j].MDID);
								cJSON_AddNumberToObject(cs, key_sts, ss.sc[i].spc[j].HWTtest);						
								cJSON_AddItemToArray(array, cs);
								scCount++;
							}
						}
					}			

					if(scCount == 0){		/* sc下面设备都没有故障 */
						cJSON_Delete(array); 
					}else{
						cJSON_AddItemToObject(root, ss.sc[i].deviceid, array);					
					}	
					
				}

			}
		
		}
					
		payload = cJSON_PrintUnformatted(root);
		if(payload){
			ssp_send_message(0x83, 1, 0, topic, msgid, 0, payload);
			myfree(payload);
		}	
		cJSON_Delete(root); 
		
	}



}

/*----------------------------------------------------------------------------
	//SS向eSH报告ST/SC/SLC/SPC故障
-----------------------------------------------------------------------------*/
void ssp_device_malfunction_post(DEVICE_TYPE devType, u8 num, u8 mdid)
{
	cJSON *cs, *root, *array;
	char *payload;
	char *topic = "/device/malfunction";
	
	root = cJSON_CreateObject(); 	
	if(root){		

		switch(devType){				
			case DEVICE_SS:
				cs = cJSON_CreateObject(); 
				if(cs){
					cJSON_AddNumberToObject(cs, key_fw, ss.firmware);
					cJSON_AddNumberToObject(cs, key_sts, ss.HWTtest.byte);
					cJSON_AddItemToObject(root, ss.deviceid, cs);	
				}					
				break;
				
			case DEVICE_SC:
				array = cJSON_CreateArray(); 
				if(array){
					cs = cJSON_CreateObject(); 
					if(cs){
						cJSON_AddNumberToObject(cs, key_MDID, mdid);
						cJSON_AddNumberToObject(cs, key_fw, ss.sc[num].firmware);
						cJSON_AddNumberToObject(cs, key_sts, ss.sc[num].HWTtest);						
						cJSON_AddItemToArray(array, cs);	
					}					
					cJSON_AddItemToObject(root, ss.sc[num].deviceid, array);					
				}				
				break;

			case DEVICE_SL:
				array = cJSON_CreateArray(); 
				if(array){
					cs = cJSON_CreateObject(); 
					if(cs){
						cJSON_AddNumberToObject(cs, key_MDID, mdid);
						cJSON_AddNumberToObject(cs, key_sts, ss.sc[num].slc[mdid -1].HWTtest);
						cJSON_AddItemToArray(array, cs);	
					}					
					cJSON_AddItemToObject(root, ss.sc[num].deviceid, array);					
				}
				break;				
			
			case DEVICE_SP:
				array = cJSON_CreateArray(); 
				if(array){
					cs = cJSON_CreateObject(); 
					if(cs){
						cJSON_AddNumberToObject(cs, key_MDID, mdid);
						cJSON_AddNumberToObject(cs, key_sts, ss.sc[num].spc[mdid -1].HWTtest);
						cJSON_AddItemToArray(array, cs);	
					}					
					cJSON_AddItemToObject(root, ss.sc[num].deviceid, array);					
				}	
				break;
				
			case DEVICE_ST:
				cs = cJSON_CreateObject(); 
				if(cs){
					cJSON_AddNumberToObject(cs, key_fw, ss.st[num].firmware);
					cJSON_AddNumberToObject(cs, key_sts, ss.st[num].HWTtest);
					cJSON_AddItemToObject(root, ss.st[num].deviceid, cs);	
				}				
				break;

			default:
				break;

		}
		
		payload = cJSON_PrintUnformatted(root);
		if(payload){
			ssp_send_message(0x43, 1, 19, topic, ssp_get_message_id(), 0, payload);
			myfree(payload);
		}	
		cJSON_Delete(root);	
		
	}
	
}

/*----------------------------------------------------------------------------
	检测ss的故障并上报
	每一秒钟检测一次
-----------------------------------------------------------------------------*/
void ssp_device_malfunction_detect_ss(void)
{
	static u8 time3minCnt = 0;
	static u8 time5sCnt = 0;

	time3minCnt++;
	time5sCnt++;

	/* sht30 */
	if(sensors_value.sht3x.errCnt >= 3){			/* 连续三次或以上读取错误 */
		ss.HWTtest.bit.f_SHT30 = 1;
	}else{
		ss.HWTtest.bit.f_SHT30 = 0;
	}

	/* pm2.5 检测方法，1.通讯周期不正常GP2Y1023_value.com_cnt < 75，2.通讯波形不正常
	*/
	if((GP2Y1023_value.com_cnt > 75) && (GP2Y1023_value.output_us > OUTPUT_AT_NO_DUST) && (GP2Y1023_value.output_us < OUTPUT_AT_MAX_DUST)){
		ss.HWTtest.bit.f_PM25 = 0;
	}else{		
		ss.HWTtest.bit.f_PM25 = 1;
	}
	GP2Y1023_value.com_cnt = 0;



	/* 1010 */
	if(ss.flag.bit.ble_en == 0){				/* ble未连接网络 */	
		ss.HWTtest.bit.f_ble = 1;
	}else{
		if(ss.malDetect.bit.f_ble_network_status == 1){
			ss.HWTtest.bit.f_ble = 0;
		}		
	}

	if(time5sCnt >= 5){						/* 5s */
		time5sCnt = 0;

		/* co */
		if(sensors_value.co_recv_cnt > 0){			/* 有数据接收 */
			ss.HWTtest.bit.f_CO = 0;
		}else{
			ss.HWTtest.bit.f_CO = 1;
		}
		sensors_value.co_recv_cnt = 0;

		/* co2 */
		if(sensors_value.co2_recv_cnt > 0){			/* 有数据接收 */
			ss.HWTtest.bit.f_CO2 = 0;
		}else{
			ss.HWTtest.bit.f_CO2 = 1;
		}
		sensors_value.co2_recv_cnt = 0;


	}

	
	if(time3minCnt >= 3 * 60){				/* 3min */
		time3minCnt = 0;
		
		if(ss.malDetect.bit.f_ble_network_status == 0){	/* 当前3min内没有接收到1010的心态帧 */	
			ss.HWTtest.bit.f_ble = 1;
		}
		ss.malDetect.bit.f_ble_network_status = 0;
	}


}

/*----------------------------------------------------------------------------
	检测ss的故障并上报
	每一秒钟检测一次
-----------------------------------------------------------------------------*/
void ssp_device_malfunction_detect(void)
{
	static u32 ss_HWTest = 0;
	static u8 sc_HWTest[SC_NUMS_OF_SS] = {0};
	static u8 st_HWTest[ST_NUMS_OF_SS] = {0};

	static u8 i = 0;
	static u8 j = 0;

	ssp_device_malfunction_detect_ss();

	if(ss.HWTtest.byte != ss_HWTest){					/* ss */
		ss_HWTest = ss.HWTtest.byte;						/* 保证故障不会被重复发送 */
		ssp_device_malfunction_post(DEVICE_SS, 0, 0);
		return;
	}
	
	for( ; i < SC_NUMS_OF_SS; i++){				/* sc */
		if(ss.sc[i].meshid){						
			if(ss.sc[i].HWTtest != sc_HWTest[i]){
				sc_HWTest[i] = ss.sc[i].HWTtest;		/* 保证故障不会被重复发送 */
				ssp_device_malfunction_post(DEVICE_SC, i, 0);
				return;
			}
		}
	}
	
	for( ; j < ST_NUMS_OF_SS; j++){				/* st */
		if(ss.st[j].meshid){						
			if(ss.st[j].HWTtest != st_HWTest[j]){
				st_HWTest[j] = ss.st[j].HWTtest;		/* 保证故障不会被重复发送 */
				ssp_device_malfunction_post(DEVICE_ST, j, 0);
				return;
			}
		}
	}

	/* 重复循环 */ 
	i = 0;
	j = 0;

}


/*----------------------------------------------------------------------------
	device_info_ss
	
-----------------------------------------------------------------------------*/
void ssp_config_st_request(void)
{

	char *topic = "/config/st";
	ssp_send_message(0x43, 1, (u8)strlen(topic), topic, ssp_get_message_id(), 0, NULL);
	
}




//Real Time Reporting
//SS转发ST发出检测到运动的指令
void send_rt(void)
{
	cJSON *cs,*sub_cs,*sub_cs2,*sub_cs3;
	char *payload;
	char *topic = "/rt";
	
	if(rev_st_rt){
		rev_st_rt = 0;
		cs = cJSON_CreateObject();

		if((ss_rt.px.isPX)&&(!ss_rt.eg.isEG)){
			ss_rt.px.isPX = 0;
			cJSON_AddItemToObject(cs, key_report, sub_cs=cJSON_CreateObject());
			cJSON_AddStringToObject(sub_cs, key_sepid,ss_rt.sepid);
			cJSON_AddStringToObject(sub_cs, key_type,"PX");
			cJSON_AddNumberToObject(sub_cs, key_value,ss_rt.px.value);
		}
		else if((!ss_rt.px.isPX)&&(ss_rt.eg.isEG)){
			ss_rt.px.isPX = 0;
			cJSON_AddItemToObject(cs, key_report,sub_cs=cJSON_CreateObject());
			cJSON_AddStringToObject(sub_cs, key_sepid,ss_rt.sepid);
			cJSON_AddNumberToObject(sub_cs,"MD",ss_rt.eg.MD);
			cJSON_AddStringToObject(sub_cs, key_type, "EG");
			cJSON_AddNumberToObject(sub_cs, key_value, ss_rt.eg.value);
		}
		
		if(ss_rt.cp.isCP){
			ss_rt.cp.isCP = 0;
			cJSON_AddItemToObject(cs, key_report,sub_cs=cJSON_CreateObject());
			cJSON_AddStringToObject(sub_cs, key_sepid,ss_rt.sepid2);
			cJSON_AddNumberToObject(sub_cs,"MD",ss_rt.MD);
			cJSON_AddStringToObject(sub_cs, key_type,"CP");
			cJSON_AddNumberToObject(sub_cs, key_value,ss_rt.gt.ch);
			cJSON_AddItemToObject(cs, key_action,sub_cs=cJSON_CreateArray());
			cJSON_AddItemToArray(sub_cs,sub_cs2 = cJSON_CreateObject());
			cJSON_AddStringToObject(sub_cs2, key_sepid,ss_rt.sepid);
			cJSON_AddNumberToObject(sub_cs2, key_CH,ss_rt.gt.ch);
			cJSON_AddStringToObject(sub_cs2, key_action,ss_rt.gt.action);
			cJSON_AddStringToObject(sub_cs2,"topos",ss_rt.gt.topos);
			cJSON_AddItemToObject(sub_cs2,"option",sub_cs3 = cJSON_CreateObject());
			cJSON_AddNumberToObject(sub_cs3,"duration",ss_rt.gt.option_duration);
			cJSON_AddNumberToObject(sub_cs3,"erase",ss_rt.gt.option_erase);
			cJSON_AddNumberToObject(sub_cs2,"timeout",ss_rt.gt.timeout);
		}
		
		if(ss_rt.gt.isGT){
			ss_rt.gt.isGT = 0;
			cJSON_AddItemToObject(cs, key_report,sub_cs=cJSON_CreateObject());
			cJSON_AddStringToObject(sub_cs, key_sepid,ss_rt.sepid2);
			cJSON_AddNumberToObject(sub_cs,"MD",ss_rt.MD);
			cJSON_AddStringToObject(sub_cs, key_type,"GT");
			cJSON_AddNumberToObject(sub_cs, key_value,ss_rt.gt.ch);
			cJSON_AddItemToObject(cs, key_action,sub_cs=cJSON_CreateArray());
			cJSON_AddItemToArray(sub_cs,sub_cs2 = cJSON_CreateObject());
			cJSON_AddStringToObject(sub_cs2, key_sepid,ss_rt.sepid);
			cJSON_AddNumberToObject(sub_cs2, key_CH,ss_rt.gt.ch);
			cJSON_AddStringToObject(sub_cs2, key_action,ss_rt.gt.action);
			cJSON_AddStringToObject(sub_cs2,"topos",ss_rt.gt.topos);
			cJSON_AddItemToObject(sub_cs2,"option",sub_cs3 = cJSON_CreateObject());
			cJSON_AddNumberToObject(sub_cs3,"duration",ss_rt.gt.option_duration);
			cJSON_AddNumberToObject(sub_cs3,"erase",ss_rt.gt.option_erase);
			cJSON_AddNumberToObject(sub_cs2,"timeout",ss_rt.gt.timeout);
		}

		payload = cJSON_PrintUnformatted(cs);
		if(payload){
			ssp_send_message(0x43,0x01,0x03,topic, ssp_get_message_id(), 0,payload);
			myfree(payload);		
		}	
		cJSON_Delete(cs);

	}

}



/*----------------------------------------------------------------------------
	接收到sc的电量上报之后，直接给esh转发
-----------------------------------------------------------------------------*/
void ssp_energy_consum_post(u8 sc_num, u8 addr)
{
	cJSON *cs, *report;
	char *payload = NULL;
	char *topic = "/rt";

	cs = cJSON_CreateObject();    
	if(cs){
		
		report = cJSON_CreateObject();	
		if(report){
			cJSON_AddStringToObject(report, "SEPID", ss.sc[sc_num].deviceid);
			cJSON_AddNumberToObject(report, key_MDID, addr + 1);
			cJSON_AddStringToObject(report, key_type, "EG");	
			cJSON_AddNumberToObject(report, key_value, ss.sc[sc_num].spc[addr].energy_consum);
			cJSON_AddItemToObject(cs, key_report, report);
		}
				payload = cJSON_PrintUnformatted(cs);
		if(payload){
			ssp_send_message(0x43, 1, 3, topic, ssp_get_message_id(), 0, payload);	
			myfree(payload);
		}	
		cJSON_Delete(cs);
		
	}

}


/*----------------------------------------------------------------------------
	运动检测是否发生变化
	每一秒钟检测一次
-----------------------------------------------------------------------------*/
void ssp_motion_detect_post(void)
{
	static u8 motion_tmp = 0;
	
	cJSON *cs, *report;
	char *payload = NULL;
	char *topic = "/rt";

	if(motion_tmp != sensors_value.removeFlag){
		motion_tmp = sensors_value.removeFlag;
		
		cs = cJSON_CreateObject();   
		
		if(cs){
			
			report = cJSON_CreateObject();	
			if(report){
				cJSON_AddStringToObject(report, "SEPID", ss.deviceid);
				cJSON_AddStringToObject(report, key_type, "MI");	
				cJSON_AddNumberToObject(report, key_value, motion_tmp);
				cJSON_AddItemToObject(cs, key_report, report);
			}			
			payload = cJSON_PrintUnformatted(cs);
			if(payload){
				ssp_send_message(0x43, 1, 3, topic, ssp_get_message_id(), 0, payload);	
				myfree(payload);
			}
			cJSON_Delete(cs);
			
		}
		
	}

}



/*----------------------------------------------------------------------------
	烟雾检测上报
-----------------------------------------------------------------------------*/
void ssp_smoke_detect_post(u8 value)
{	
	cJSON *cs, *report;
	char *payload = NULL;
	char *topic = "/rt";
		
	cs = cJSON_CreateObject();   
	
	if(cs){
		
		report = cJSON_CreateObject();	
		if(report){
			cJSON_AddStringToObject(report, "SEPID", ss.deviceid);
			cJSON_AddStringToObject(report, key_type, "SM");	
			cJSON_AddNumberToObject(report, key_value, value);
			cJSON_AddItemToObject(cs, key_report, report);
		}			
		payload = cJSON_PrintUnformatted(cs);
		if(payload){
			ssp_send_message(0x43, 1, 3, topic, ssp_get_message_id(), 0, payload);	
			myfree(payload);
		}
		cJSON_Delete(cs);
		
	}

}


/*----------------------------------------------------------------------------
	接收到st的手势识别或者按键或滑动等级上报后
	向 ss上报 
-----------------------------------------------------------------------------*/
void ssp_gesture_or_keypad_post(u8 st_num, REPORT_TYPE type, u8 keyPad, u16 value)
{
	cJSON *cs, *report;
	char *payload = NULL;
	char *topic = "/rt";
	
	cs = cJSON_CreateObject();    
	if(cs){
		
		report = cJSON_CreateObject();	
		if(report){
			cJSON_AddStringToObject(report, "SEPID", ss.st[st_num].deviceid);
			if(type == GESTURE_REPORT){		
				cJSON_AddStringToObject(report, key_type, "GT");	
				cJSON_AddNumberToObject(report, key_value, value);

			}else if(type == KEYPAD_REPORT){
				cJSON_AddStringToObject(report, key_type, "CP");	
				cJSON_AddNumberToObject(report, key_value, value);
				cJSON_AddNumberToObject(report, key_keyPad, keyPad);

			}else if(type == SLIPPAD_REPORT){
				cJSON_AddStringToObject(report, key_type, "CB");	
				cJSON_AddNumberToObject(report, key_value, value);
				cJSON_AddNumberToObject(report, key_keyPad, keyPad);

			}else{

			}
			cJSON_AddItemToObject(cs, key_report, report);
			
		}
		
		payload = cJSON_PrintUnformatted(cs);
		if(payload){
			ssp_send_message(0x43, 1, 3, topic, ssp_get_message_id(), 0, payload);	
			myfree(payload);
		}	
		cJSON_Delete(cs);
		
	}

}

/*----------------------------------------------------------------------------
	ssp中recepit回复
-----------------------------------------------------------------------------*/
void ssp_recepit_response(int code)
{
	cJSON *cs;
	char *payload;
	char *topic = "";
	u8	ret = 0x83;
	
	cs = cJSON_CreateObject();	
	cJSON_AddNumberToObject(cs, key_code, code);

	if(code == RECEIPT_CODE_SUCCESS){
		cJSON_AddStringToObject(cs, key_msg, key_0x0200000);
	}
	
	if(code == RECEIPT_CODE_ANALYZE_OK){
		cJSON_AddStringToObject(cs, key_msg, key_0x0200001);
		ret = 0x85;
	}

	if(code == RECEIPT_CODE_FAILED){
		cJSON_AddStringToObject(cs, key_msg, key_0x0400000);
		ret = 0x85;
	}
	if(code == RECEIPT_CODE_ANALYZE_ERROR){
		cJSON_AddStringToObject(cs, key_msg, key_0x0400001);
		ret = 0x85;
	}

	
	if(code == RECEIPT_CODE_ERROR){
		cJSON_AddStringToObject(cs, key_msg, key_0x0401F08);
		ret = 0x85;
	}

	payload = cJSON_PrintUnformatted(cs);
	if(payload){
		ssp_send_message(ret, 1, 0, topic, Rxfr4004.rx_var_header.msgid, 0, payload);
		myfree(payload);
	}	
	cJSON_Delete(cs);

	
}



/*-------------------------------------------------------------------------
    主动发送数据
	struct{
	        u8 ble_en:1;		//1010连接mesh网络，正常工作
	        u8 tcp_en:1;		//4004建立tcp连接
	        u8 post_devinfo_ss:1;
	        u8 recv_devinfo_list:1;
	        u8 recv_config_ss:1;
		u8 sicp_broadcast_finish:1;
	        u8 request_config_st:1;
	        u8 recv_config_st:1;

  	}bit; 
-------------------------------------------------------------------------*/
void ssp_send_data_active(void)
{
	
	if(ss.flag.bit.post_devinfo_ss == 0){			//还没发送device info ss

		if(ss.flag.bit.ble_en && ss.flag.bit.tcp_en){	//1010和4004工作正常

			ssp_device_info_ss(SSP_POST, ssp_get_message_id()); 
			ss.flag.bit.post_devinfo_ss = 1;
		}
	}



	if(ss.flag.bit.request_config_st == 0 && ss.flag.bit.sicp_broadcast_finish == 1 ){	//还没发送 request_config_st

		if(ss.flag.bit.ble_en && ss.flag.bit.tcp_en){	//1010和4004工作正常
		
			ss.flag.bit.request_config_st = 1;
			ssp_config_st_request();
			
		}
	}

//if(ss.flag.bit.request_config_st == 0 ){	//还没发送 request_config_st
//	
//	ss.flag.bit.request_config_st = 1;
//	ssp_config_st_request();

//}

}



/*----------------------------------------------------------------------------
	检查设备的合法性
	暂只检查st sc的合法性，sc下面的slc spc与sc的合法性一致
-----------------------------------------------------------------------------*/
void ssp_check_device_legality(void)
{
	int i, j, size;
	cJSON *devjson, *tmp;
//	char *test;

	if(devList.flag){		/* 设备列表有效 */
		
		if(devList.st){		/* st列表存在 */			
			size = devList.stSize;
			if(size > 0){
				for(i = 0; i < ST_NUMS_OF_SS; i++){
					if(ss.st[i].meshid > 0){			/* 设备存在 */
						for(j = 0; j < size; j++){							
							devjson = cJSON_GetArrayItem(devList.st, j);
							if(devjson){
								tmp = cJSON_GetObjectItem(devjson, key_deviceID);							
								if(tmp && tmp->valuestring){
									if(!strcmp(ss.st[i].deviceid, tmp->valuestring)){		/* 相同 */
										ss.st[i].legalFlag = 1;										
										break;									
									}
								}							
							}
						}
						if(j == size){				/* 找不到相同的设备ID，设备不合法，踢出去 */	
							sicp_ble_ctrl_cmd(0x02, ss.st[i].meshid);
							memset(&ss.st[i], 0, sizeof(ST));	
						}
					}											
				}
			}			
		}

		if(devList.sc){ 		/* sc列表存在 */	
			size = devList.scSize;			
			if(size > 0){
				for(i = 0; i < SC_NUMS_OF_SS; i++){
					if(ss.sc[i].meshid > 0){			/* 设备存在 */
						/* sc */
						for(j = 0; j < size; j++){							
							devjson = cJSON_GetArrayItem(devList.sc, j);
							if(devjson){
								tmp = cJSON_GetObjectItem(devjson, key_deviceID);							
								if(tmp && tmp->valuestring){
									if(!strcmp(ss.sc[i].deviceid, tmp->valuestring)){		/* 相同 */
										ss.sc[i].legalFlag = 1; 									
										break;									
									}
								}							
							}
						}
						if(j == size){				/* 找不到相同的设备ID，设备不合法，踢出去 */	
							sicp_ble_ctrl_cmd(0x02, ss.sc[i].meshid);		/* 同时将sc下面的slc\spc踢出去 */	
							memset(&ss.sc[i], 0, sizeof(SC));	
						}						
					}											
				}
			}

			/* test */
//			test = cJSON_PrintUnformatted(devList.sc);
//			if(test){				
//				addNodeToUartTxSLLast(test, strlen(test));
//			}
			
		}

	}

}









