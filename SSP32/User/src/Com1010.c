
/* Includes ------------------------------------------------------------------*/
#define _COM1010_GLOBAL
#include "includes.h"
#include <stdlib.h>
#include "key_string.h"
#include "sensors.h"

u8 BIT[8] = {0x01, 0x02,0x04,0x08, 0x10, 0x20, 0x40, 0x80};

SICP_handle_t SICP_handle = {0};



/*-------------------------------------------------------------------------
    ��16������ת��Ϊ�����ַ�
-------------------------------------------------------------------------*/
int string_toDec(const char *str)
{
	int dec = 0;
	
	while(*str){
		
		if(*str >= '0' && *str <= '9'){
			dec = dec * 10 + *str -'0';
		}else{
			dec = 0; break;
		}
		str++;
	}
	return dec;

}




/*-------------------------------------------------------------------------
    ��16������ת��Ϊ�����ַ�
-------------------------------------------------------------------------*/
void hex2_toChar(uchar d, char *p)
{
	uchar i;
	i = d /16;
	if(i <= 9){*p = '0' + i;}
	else{*p = 'A' + i - 10;}
	
	i = d %16;
	if(i <= 9){*(p+1) = '0' + i;}
	else{*(p+1) = 'A' + i - 10;}

}

/*-------------------------------------------------------------------------
	��������������ַ�ת��Ϊuchar����
-------------------------------------------------------------------------*/
unsigned char  string_touchar(const char *str)
{
    	unsigned char h = 0;

    	if (*str>='0' && *str<='9') { h += (*str)-'0';}
    	else if (*str>='A' && *str<='F') { h += 10+(*str)-'A';}
    	else if (*str>='a' && *str<='f') { h += 10+(*str)-'a';}
    	else { return 0;}

	str++;
	if(*str != 0){	/* ���ڵڶ����ַ� */
	    	h=h<<4; 
	    	if (*str>='0' && *str<='9') {h += (*str)-'0';}       	
	    	else if (*str>='A' && *str<='F') {h += 10+(*str)-'A';}      	
	    	else if (*str>='a' && *str<='f') {h += 10+(*str)-'a';}       	
	    	else { return 0;}
	}else{
		return h;
	} 

	str++;
	if(*str == 0){return h;}
	else { return 0;}/* �������ַ��������� */

}



/*-------------------------------------------------------------------------
	��������������ַ�ת��Ϊ16������
	�����������ֽڵĴ�����
-------------------------------------------------------------------------*/
unsigned char  string_tohex2(const char *str)
{
    	unsigned char h = 0;

    	if (*str>='0' && *str<='9') { h += (*str)-'0';}
    	else if (*str>='A' && *str<='F') { h += 10+(*str)-'A';}
    	else if (*str>='a' && *str<='f') { h += 10+(*str)-'a';}
    	else { return 0;}

	str++;	
	if(*str != 0){	/* ���еڶ����ַ� */
	    	h=h<<4; 
	    	if (*str>='0' && *str<='9') {h += (*str)-'0';}       	
	    	else if (*str>='A' && *str<='F') {h += 10+(*str)-'A';}      	
	    	else if (*str>='a' && *str<='f') {h += 10+(*str)-'a';}       	
	    	else { return 0;}
	}  

	return h;

}



/*-------------------------------------------------------------------------
    
-------------------------------------------------------------------------*/
u8 XOR_Check(u8 *buf, u16 length)
{
	u16 i;
	u8 result = *buf++;
	
	for(i = 1; i < length; i++)
	{
		result ^= *buf++;
	}
	
	return result;
}


/**
  * @brief  check sum(cumulative sum)
  * @param  u8 *sendbuf,u8 length.
  * @retval u8 result
  */
u8 Check_Sum(u8 *buf,u8 length)
{
	u8 i;
	u8 result = *buf++;
	for(i = 1; i < length; i++)
	{
		result ^= *buf++;
	}
	return result;
}



/*-------------------------------------------------------------------------
    
-------------------------------------------------------------------------*/
void sicp_handle_clear(void)
{
	memset(&SICP_handle, 0, sizeof(SICP_handle));
}


//ʮ��������ת��Ϊ�ַ�
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



//�ַ�ת��Ϊʮ��������
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




/*----------------------------------------------------------------------------
	
-----------------------------------------------------------------------------*/
u8 sicp_get_message_id(void)
{
	static u8 id = 0;
	
	id++;

	if(id == 0){
		id = 1;
	}

	return id;
}


/*-------------------------------------------------------------------------
    ����ط��б����ȷ��
    ȷ�Ͻ��ճɹ���ɾ���ط��б��ж�Ӧ�Ľڵ�
-------------------------------------------------------------------------*/
void reSendList_recv_confirm(u8 msgid)
{

	reSendNode_t *node = reSendHead;

	while(node){

		if(node->msgid == msgid){
			
			deleteNodeFromReSend(node);
			return;
			
		}
		
		node = node->next;
	}

}

/*-------------------------------------------------------------------------
    �����ط�
    ÿ2s��ִ��һ��
-------------------------------------------------------------------------*/
void reSendList_send(void)
{
	u8 num = 0;

	reSendNode_t *node = reSendHead;

	reSendCount++;

	if(reSendCount > 20){
		reSendCount = 0;

		while(node){

			node->msgid = sicp_get_message_id();
			*(node->data + 2) = node->msgid;
			*(node->data + node->len - 1) = Check_Sum(node->data + 2, *(node->data + 5));
			
			if(addNodeToUart2TxSLLast((char *)node->data, node->len) == 0){			
				num++;
			}

			if(num >= 2){			//ÿ����෢��10��
				return;
			}
			
			node = node->next;
		}

	}
		
}


/*-------------------------------------------------------------------------
    reSendFlag = 1��Ҫ�ش�
-------------------------------------------------------------------------*/
void sicp_send_message(SICP_Message *tx, u8 pay_len, u8 reSendFlag)
{
	u8 payload_len = pay_len;
	
	SICP_handle.txBuf[0] = tx->frame_h1;
	SICP_handle.txBuf[1] = tx->frame_h2;
	SICP_handle.txBuf[2] = tx->message_id;
	SICP_handle.txBuf[3] = tx->mesh_id_H;
	SICP_handle.txBuf[4] = tx->mesh_id_L;
	SICP_handle.txBuf[5] = 4 + payload_len;
	
	mymemcpy(&SICP_handle.txBuf[6], tx->payload, payload_len);
	
	SICP_handle.txBuf[6 + payload_len] = Check_Sum(&SICP_handle.txBuf[2], SICP_handle.txBuf[5]);

	if(reSendFlag == 1){
		
		addNodeToReSendLast(SICP_handle.txBuf, 7 + payload_len);
	}

	addNodeToUart2TxSLLast((char *)SICP_handle.txBuf, 7 + payload_len);
	
}


/*-------------------------------------------------------------------------
    
-------------------------------------------------------------------------*/
void sicp_send_receipt(u8 type, u8 send_message_id, u16 send_mesh_id)
{
	SICP_Message receipt;
	
	receipt.frame_h1 = 0xEE;
	receipt.frame_h2 = 0xAA;
	receipt.message_id = send_message_id;
	receipt.mesh_id_H = (u8)(send_mesh_id >>8);
	receipt.mesh_id_L = (u8)(send_mesh_id & 0xff);
	receipt.payload[0] = 0xAA;
	receipt.payload[1] = type;

	sicp_send_message(&receipt, 2, 0);
}

/*-------------------------------------------------------------------------
    ����ȷ��
-------------------------------------------------------------------------*/
void sicp_config_st_recv_confirm(u8 num, u8 msgid)
{
	config_st_t	*ptmp;

	ptmp = ss.st[num].config_head;

	while(ptmp){				//����������Ϣ

		if(ptmp->flag.bit.configok == 0 && ptmp->flag.bit.sended == 1){		//��δ���óɹ�

			if(ptmp->msgid == msgid){
				
				ptmp->flag.bit.configok = 1;
				return;

			}
		}
		ptmp = ptmp->next;		
	}

}

/*-------------------------------------------------------------------------
    ����ÿ1����ִ��һ��
-------------------------------------------------------------------------*/
void sicp_config_st_send(void)
{
	u8 i;
	SICP_Message 	config;
	config_st_t	*ptmp;

	for(i = 0; i < ST_NUMS_OF_SS; i++){
		
		if(ss.st[i].meshid){					//�豸����

			ptmp = ss.st[i].config_head;

			while(ptmp){				//����������Ϣ

				if(ptmp->flag.bit.configok == 0){		//��δ���óɹ�

					ptmp->msgid = sicp_get_message_id();
					config.frame_h1 = 0xEE;
					config.frame_h2 = 0xEE;
					config.message_id = ptmp->msgid;
					config.mesh_id_H = (u8)(ss.st[i].meshid >> 8);
					config.mesh_id_L = (u8)(ss.st[i].meshid & 0xff);
					config.payload[0] = 0x04;
					config.payload[1] = ptmp->type;

					if(ptmp->type == 1){			//����
						config.payload[2] = ptmp->key;
						config.payload[3] = 0;

					}else if(ptmp->type == 2){		//����
						config.payload[2] = (u8)(ptmp->cond >> 8);
						config.payload[3] = (u8)(ptmp->cond & 0xff);
					}

					config.payload[4] = (u8)(ptmp->meshid >> 8);
					config.payload[5] = (u8)(ptmp->meshid & 0xff);
					config.payload[6] = ptmp->boardid;
					config.payload[7] = ptmp->action;
					config.payload[8] = ptmp->value;

					sicp_send_message(&config, 9, 0);	

					ptmp->flag.bit.sended = 1;

					return;

				}

				ptmp = ptmp->next;
				
			}
				
		}
		
	}


}

/*----------------------------------------------------------------------------
	���SICP_SEND_HEARTBEAT_TIME��һ��ѭ��
	ѭ����ÿ1���ӷ���һ���豸
-----------------------------------------------------------------------------*/
void sicp_send_heartbeat(void)
{
	static u8 counts = 0;		/* ��ʱ���� */	
	static u8 i = 0;			/* st�豸����  */
	static u8 j = 0;			/* sc�豸����  */

	SICP_Message heartbeat = {0};
	
	counts++;
	if(counts >= SICP_SEND_HEARTBEAT_TIME){
		
		//ST������
		for( ; i < ST_NUMS_OF_SS; i++){ 
			if(ss.st[i].meshid){		//mesh id��Ϊ�գ�����е���
				heartbeat.frame_h1 = 0xEE;
				heartbeat.frame_h2 = 0xEE;
				heartbeat.message_id = sicp_get_message_id();
				heartbeat.mesh_id_H = (u8)(ss.st[i].meshid >> 8);
				heartbeat.mesh_id_L = (u8)(ss.st[i].meshid & 0xff);
				heartbeat.payload[0] = 0x10;
				heartbeat.payload[1] = 0x01;
				heartbeat.payload[2] = ss.st[i].level1;
				heartbeat.payload[3] = ss.st[i].level2;
				sicp_send_message(&heartbeat, 4, 0);
				i++;
				return;
			}
		}
		
		//SC������
		for( ; j < SC_NUMS_OF_SS; j++){ 
			if(ss.sc[j].meshid){		//mesh id��Ϊ�գ�����е���
				ss.sc[j].heartBeatFlag = 1;	/*  �����㲥��־ */
				heartbeat.frame_h1 = 0xEE;
				heartbeat.frame_h2 = 0xEE;
				heartbeat.message_id = sicp_get_message_id();
				heartbeat.mesh_id_H = (u8)(ss.sc[j].meshid >> 8);
				heartbeat.mesh_id_L = (u8)(ss.sc[j].meshid & 0xff);
				heartbeat.payload[0] = 0x10;
				heartbeat.payload[1] = 0x02;
				sicp_send_message(&heartbeat, 2, 0);
				j++;
				return;
			}
		}
		
		//ȫ��������ɺ������
		counts = 0;
		i = 0;	
		j = 0;	

	}
	
}


//�յ�/actions/refresh/CH/AL����/actions/refresh/CH/CSʱ����
void sicp_cmd_data(void)	//��ȡSS��ST�Ļ�������ɫ������
{
	u8 i;
	SICP_Message cmd_data;
	if(rev_ar2){
		rev_ar2 = 0;
		for(i = 0; i < 20;i++){
			if((ss.st[i].meshid) && (!ss.st[i].deviceid[0])){	//mesh id��Ϊ�գ�����е���
				cmd_data.frame_h1 = 0xEE;
				cmd_data.frame_h2 = 0xEE;
				cmd_data.message_id = sicp_get_message_id();
				cmd_data.mesh_id_H = (u8)((ss.st[i].meshid&0xFF00)>>8);
				cmd_data.mesh_id_L = (u8)(ss.st[i].meshid&0x00FF);
				cmd_data.payload[0] = 0x03;
				cmd_data.payload[1] = 0x01;
				cmd_data.payload[2] = 0x30;
				cmd_data.payload[3] = 0x32;

				sicp_send_message(&cmd_data, 4, 0);
			}
		}
	}
}

//sicp����cmd-refreshָ��ˢ�´��������ݣ�ÿssp config/ssָ��refrʱ�䷢��һ��
//�յ�/actions/refresh ʱǿ�·���һ��
void sicp_cmd_refresh(void) //ˢ��SS��ST�Ļ�������ɫ������,SC�ϵĵ���
{
	u8 i;
	SICP_Message refresh_data;
	
	for(i = 0; i < 20;i++){
		if((ss.st[i].meshid) && (!ss.st[i].deviceid[0])){//mesh id��Ϊ�գ�����е���
			refresh_data.frame_h1 = 0xEE;
			refresh_data.frame_h2 = 0xEE;
			refresh_data.message_id = sicp_get_message_id();
			refresh_data.mesh_id_H = (u8)((ss.st[i].meshid&0xFF00)>>8);
			refresh_data.mesh_id_L = (u8)(ss.st[i].meshid&0x00FF);
			refresh_data.payload[0] = 0x03;
			refresh_data.payload[1] = 0x02;

			sicp_send_message(&refresh_data, 2, 0);
		}
	}
	for(i = 0; i < 5;i++){
		if((ss.sc[i].meshid) && (!ss.sc[i].deviceid[0])){//mesh id��Ϊ�գ�����е���
			refresh_data.frame_h1 = 0xEE;
			refresh_data.frame_h2 = 0xEE;
			refresh_data.message_id = sicp_get_message_id();
			refresh_data.mesh_id_H = (u8)((ss.sc[i].meshid&0xFF00)>>8);
			refresh_data.mesh_id_L = (u8)(ss.sc[i].meshid&0x00FF);
			refresh_data.payload[0] = 0x03;
			refresh_data.payload[1] = 0x02;

			sicp_send_message(&refresh_data, 2, 0);
		}
	}
}



//�յ�ssp action backlightʱ����LED command���˺�������Task100ms()
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
						led_cmd.message_id = sicp_get_message_id();
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
						sicp_send_message(&led_cmd, 4 + ((i-1)*3) + 3, 0);
						break;
					case 2:
						led_cmd.frame_h1 = 0xEE;
						led_cmd.frame_h2 = 0xEE;
						led_cmd.message_id = sicp_get_message_id();
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
						sicp_send_message(&led_cmd, (6+((i-1)*3)+3), 0);	
						break;
					case 3:
						led_cmd.frame_h1 = 0xEE;
						led_cmd.frame_h2 = 0xEE;
						led_cmd.message_id = sicp_get_message_id();
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
						sicp_send_message(&led_cmd, (4+((i-1)*3)+3), 0);	
						break;
					case 4:
					case 5:
						led_cmd.frame_h1 = 0xEE;
						led_cmd.frame_h2 = 0xEE;
						led_cmd.message_id = sicp_get_message_id();
						led_cmd.mesh_id_H = (u8)((ss.st[i].meshid&0xFF00)>>8);
						led_cmd.mesh_id_L = (u8)(ss.st[i].meshid&0x00FF);
						led_cmd.payload[0] = 0x09;
						led_cmd.payload[1] = (u8)(ss_ab.mode<<4) + 0x0F;
						//led_cmd.payload[2] = '\0';
						sicp_send_message(&led_cmd, 2, 0);
						break;
					default:
						break;
					}
				break;//����forѭ��
			}
		}
		
		ssp_recepit_response(RECEIPT_CODE_SUCCESS);
		
	}
}


/*-------------------------------------------------------------------------
    
-------------------------------------------------------------------------*/
void sicp_alarm_cmd(void)
{
	u8 i;
	SICP_Message alarm_cmd;
		
	alarm_cmd.frame_h1 = 0xEE;
	alarm_cmd.frame_h2 = 0xEE;
	alarm_cmd.payload[0] = 0x05;
	alarm_cmd.payload[1] = ss.alarm_level;


	for(i = 0; i < ST_NUMS_OF_SS; i++){

		if(ss.st[i].meshid){
			
			alarm_cmd.message_id = sicp_get_message_id();
			alarm_cmd.mesh_id_H = (u8)(ss.st[i].meshid >> 8);
			alarm_cmd.mesh_id_L = (u8)(ss.st[i].meshid & 0xff);
						
			sicp_send_message(&alarm_cmd, 2, 0);

		}
	}
	
	for(i = 0; i < SC_NUMS_OF_SS; i++){
		
		if(ss.sc[i].meshid){
			
			alarm_cmd.message_id = sicp_get_message_id();
			alarm_cmd.mesh_id_H = (u8)(ss.sc[i].meshid >> 8);
			alarm_cmd.mesh_id_L = (u8)(ss.sc[i].meshid & 0xff);
						
			sicp_send_message(&alarm_cmd, 2, 0);

		}
	}
	
}










/*----------------------------------------------------------------------------
	GET_SERIES_TO_SAVE:���ѱ��棬��ȡ�����λ�ã���δ���棬��ȡ���Ա����λ��
					�����߶�û��λ�ã��򷵻�0xff
	GET_SERIES_WHEN_EXSIT:��ȡ�ѱ����λ�ã���δ���棬�򷵻�0xff
-----------------------------------------------------------------------------*/
u8 get_series_number(GET_SERIES_WAY way, GET_SERIES_OBJECT object, u16 meshid, char *deviceid)
{
	u8 i, num = GET_SERIES_ERROR;

	if(way == GET_SERIES_TO_SAVE){
		
		if(object == GET_SERIES_ST){
			
			for(i = 0; i<ST_NUMS_OF_SS; i++){
				if(ss.st[i].meshid == meshid){	/* ��sc�Ѿ����� */				
					break;
				}
			}			
			if(i == ST_NUMS_OF_SS){ 			/* ��sc��δ��������һ�δ�����λ�ý��б���  */ 		
				for(i = 0; i<ST_NUMS_OF_SS; i++){
					if(ss.st[i].meshid == 0){	/* ��δ�������ݵ�λ�� */				
						break;
					}
				}
			}
			num = (i == ST_NUMS_OF_SS) ? GET_SERIES_ERROR : i;

		}else{

			for(i = 0; i<SC_NUMS_OF_SS; i++){
				if(ss.sc[i].meshid == meshid){	/* ��sc�Ѿ����� */				
					break;
				}
			}			
			if(i == SC_NUMS_OF_SS){ 	/* ��sc��δ��������һ�δ�����λ�ý��б���  */ 		
				for(i = 0; i<SC_NUMS_OF_SS; i++){
					if(ss.sc[i].meshid == 0){			/* ��δ�������ݵ�λ�� */				
						break;
					}
				}
			}
			num = (i == SC_NUMS_OF_SS) ? GET_SERIES_ERROR : i;
			
		}

	}

	if(way == GET_SERIES_WHEN_EXSIT){

		switch(object){			
			case GET_SERIES_SC:
				for(i = 0; i < SC_NUMS_OF_SS; i++){				
					if(strcmp(ss.sc[i].deviceid, deviceid) == 0){	/* ��sc�Ѿ����� */				
						break;
					}
				}	
				num = (i == SC_NUMS_OF_SS) ? GET_SERIES_ERROR : i;
				break;

			case GET_SERIES_ST:
				for(i = 0; i<ST_NUMS_OF_SS; i++){				
					if(strcmp(ss.st[i].deviceid, deviceid) == 0){	/* ��sc�Ѿ����� */				
						break;
					}
				}	
				num = (i == ST_NUMS_OF_SS) ? GET_SERIES_ERROR : i;
				break;
					
//			case GET_SERIES_SL:			
//				for(i = 0; i<SC_NUMS_OF_SS; i++){
//					for(j = 0; j<SL_NUMS_OF_SC; j++){				
//						if(strcmp(ss.sc[i].slc[j].deviceid, deviceid) == 0){	/* ��sc�Ѿ����� */				
//							break;
//						}
//					}								
//				}
//				num = ((i == SC_NUMS_OF_SS) && (j == SL_NUMS_OF_SC)) ? GET_SERIES_ERROR : (i*16 + j);
//				break;

//			case GET_SERIES_SP:
//				for(i = 0; i<SC_NUMS_OF_SS; i++){
//					for(j = 0; j<SP_NUMS_OF_SC; j++){				
//						if(strcmp(ss.sc[i].spc[j].deviceid, deviceid) == 0){	/* ��sc�Ѿ����� */				
//							break;
//						}
//					}								
//				}
//				num = ((i == SC_NUMS_OF_SS) && (j == SP_NUMS_OF_SC)) ? GET_SERIES_ERROR : (i*16 + j);
//				break;
			default:	
				num = 0xff;
				break;

		}

	}

	return num;

}


/*----------------------------------------------------------------------------
	��ȡ�豸�����λ�ã����ȡʧ���򷵻� GET_SERIES_ERROR
-----------------------------------------------------------------------------*/
u8 get_device_number(GET_SERIES_OBJECT object, u16 meshid)
{
	u8 i, num = GET_SERIES_ERROR;

	if(object == GET_SERIES_ST){
		
		for(i = 0; i < ST_NUMS_OF_SS; i++){
			if(ss.st[i].meshid == meshid){			/* ��sc�Ѿ����� */				
				break;
			}
		}			
		num = (i == ST_NUMS_OF_SS) ? GET_SERIES_ERROR : i;
	}

	if(object == GET_SERIES_SC){
		
		for(i = 0; i < SC_NUMS_OF_SS; i++){
			if(ss.sc[i].meshid == meshid){			/* ��sc�Ѿ����� */				
				break;
			}
		}			
		num = (i == SC_NUMS_OF_SS) ? GET_SERIES_ERROR : i;
	}



	return num;

}


/*----------------------------------------------------------------------------
	������������
-----------------------------------------------------------------------------*/
void sicp_ble_ctrl_cmd(u8 type, u16 mesh_id)
{
	SICP_Message ble;

	ble.frame_h1 = 0xEE;
	ble.frame_h2 = 0xEE;
	ble.message_id = sicp_get_message_id();
	ble.mesh_id_H = (u8)((mesh_id >> 8) & 0xff);
	ble.mesh_id_L = (u8)(mesh_id & 0xff);
	ble.payload[0] = 0xC0;
	ble.payload[1] = type;
	
	sicp_send_message(&ble, 2, 0);	

}



/*----------------------------------------------------------------------------
	�ϵ�5s֮���ٿ�ʼ�㲥
	ѭ���㲥 ÿ100msִ��һ��
	��һ�ּ��1ms		timeCnt = 10
	�ڶ��ּ��2s		timeCnt = 20
	�����ּ��4s		timeCnt = 40
	ѭ������֮���ٽ��й㲥
-----------------------------------------------------------------------------*/
void sicp_broadcast_loop(void)
{
	static u8 timeCnt = 0;		/* ѭ����������ʾ�ڼ���ѭ�� */
	static u8 loopCnt = 0;		/* ѭ����������ʾ�ڼ���ѭ�� */
	static u8 devNum = 0;		/* �豸��� */
	static u8 devType = 1;		/* �豸���ͣ�1��ʾsc��2��ʾst */

	SICP_Message bc;
	u8 *pdata;	
	u8 timeOut = (loopCnt == 1 ? 10 : (loopCnt == 2 ? 20 : 40)); 

	if(loopCnt == 0){		/* �ϵ�5s���ٽ��й㲥 */

		timeCnt++;
		if(timeCnt >= 50){
			loopCnt = 1;
			timeCnt = 0;
		}

	}else{

		if(loopCnt <= SICP_BROADCAST_LOOP_NUMS && devList.flag){			/* �豸�б���Ч */
			timeCnt++;

			if(timeCnt >= timeOut){	/* ���ʱ�䵽 */
				timeCnt = 0;

				pdata = NULL;
				if(devType == 1){	/* sc */
					
					for( ; devNum < devList.scSize; devNum++){
						if(devList.scBroadcastRecvFlag[devNum] == 0){
							pdata = devList.scDeviceID[devNum];
							devNum++;
							break;
						}
					}
					if(devNum == devList.scSize){
						devNum = 0;
						devType = 2;	/* �㲥��ϣ����Ź㲥st */								
					}							
				}

				if(pdata == NULL){
					if(devType == 2){	/* st */
					
						for( ; devNum < devList.stSize; devNum++){
							if(devList.stBroadcastRecvFlag[devNum] == 0){
								pdata = devList.stDeviceID[devNum];
								devNum++;
								break;
							}
						}
						if(devNum == devList.stSize){
							devNum = 0;
							devType = 1;	/* �㲥��ϣ���ʼ��һ�ֵĹ㲥 */
							loopCnt++;						
						}	
					}
				}

				if(pdata){
					
					bc.frame_h1 = 0xEE;
					bc.frame_h2 = 0xEE;
					bc.message_id = 0x00;
					bc.mesh_id_H = 0x00;
					bc.mesh_id_L = 0x00;
					bc.payload[0] = 0xB0;
					bc.payload[1] = *pdata;
					bc.payload[2] = *(pdata + 1);
					bc.payload[3] = *(pdata + 2);
					bc.payload[4] = *(pdata + 3);			
					sicp_send_message(&bc, 5, 0);
				}

			}

		}

		if(loopCnt > SICP_BROADCAST_LOOP_NUMS){
			ss.flag.bit.sicp_broadcast_finish = 1;
		}

	}
	
}

/*----------------------------------------------------------------------------
	sicp_recv_DD
-----------------------------------------------------------------------------*/
void sicp_recv_DD(void)
{
	switch(SICP_handle.rxBuf[4]){
		case 0x01://����״̬֡
			ns.signal = SICP_handle.rxBuf[5];
			ns.connect = SICP_handle.rxBuf[6];
			ns.phones = SICP_handle.rxBuf[7];
			ss.meshid = ((u16)SICP_handle.rxBuf[8] << 8)  + SICP_handle.rxBuf[9]; 		
			ns.host_meshid_H = SICP_handle.rxBuf[10];
			ns.host_meshid_L = SICP_handle.rxBuf[11];
			
			ss.flag.bit.ble_en = (SICP_handle.rxBuf[6] == 0x02) ? 1 : 0;
			ss.malDetect.bit.f_ble_network_status = 1;		/* ���յ�״̬֡ */
				
			break;
		case 0x02:
			system_init();
			sicp_handle_clear();
			break;
		case 0x03:
			system_init();
			sicp_handle_clear();
			ns.signal = 0x00;
			ns.connect = 0x00;
			ns.phones = 0x00;
			ss.meshid = 0x80FF;
			ns.host_meshid_H = 0x00;
			ns.host_meshid_L = 0x00;
			break;
		default:
			break;
			
	}

}

/*----------------------------------------------------------------------------
	//ST�ظ�����
-----------------------------------------------------------------------------*/
void sicp_recv_heart_beat_st(u16 meshid)
{
	u8 num;
	
	num = get_device_number(GET_SERIES_ST, meshid);
	if(num != GET_SERIES_ERROR){


	}

}



/*----------------------------------------------------------------------------
	//ST�ظ�����������
[16:30:11.728]�ա���EE AA 01 80 10 0E 11 0C A8 0C CC CC CC CC C0 05 E3 
[16:30:12.952]�ա���EE AA 02 80 0D 0D 11 CC CC CC C3 CC CC CC C0 90
-----------------------------------------------------------------------------*/
void sicp_recv_heart_beat_sc(u16 meshid)
{
	u8 i, tmp, len, cnt, num, highlow;
	
	num = get_device_number(GET_SERIES_SC, meshid);
	if(num != GET_SERIES_ERROR){

		tmp = 0;
		cnt = 7;
		len = 15;
		highlow = 1;
		for(i = 0; i < 15; i++){
						
			if(highlow){					
				tmp = SICP_handle.rxBuf[cnt] >> 4;
				highlow = 0;
			}else{
				tmp = SICP_handle.rxBuf[cnt] & 0x0f;	
				highlow = 1;
			}
				
			if(tmp != 0x0c){		/* �豸���� */
				
				if(tmp & 0x08){ 	/* slc */
					ss.sc[num].slc[i].ch1_status = 0;
					ss.sc[num].slc[i].ch2_status = 0;
					if(tmp & 0x01){ /* �� */
						ss.sc[num].slc[i].ch1_status = SICP_handle.rxBuf[len++];
					}
					if(tmp & 0x02){
						ss.sc[num].slc[i].ch2_status = SICP_handle.rxBuf[len++];
					}
				}else{			/* spc */
					ss.sc[num].spc[i].ch1_status = 0;
					ss.sc[num].spc[i].ch2_status = 0;
					ss.sc[num].spc[i].ch3_status = 0;
					if(tmp & 0x01){ /* �� */
						ss.sc[num].spc[i].ch1_status = 99;
					}
					if(tmp & 0x02){
						ss.sc[num].spc[i].ch2_status = 99;
					}
					if(tmp & 0x04){
						ss.sc[num].spc[i].ch3_status = 99;
					}
				}				
			}

			if(highlow){
				cnt++;
			}
			
		}

		if(ss.sc[num].heartBeatFlag){				/* ������Ϊ�����㲥�ظ� */
			ss.sc[num].heartBeatFlag = 0;
		}else{								/* ������Ϊsc�����ϱ������ݣ�Ҫ�ϱ��豸״̬ */			
			ssp_device_status_post(DEVICE_SC, num);
		}

	}

}

/*----------------------------------------------------------------------------
	//ST�ظ�����������
-----------------------------------------------------------------------------*/
void sicp_recv_cmd_data(u16 meshid)
{
	u8 num;

	num = get_device_number(GET_SERIES_ST, meshid);
	if(num != GET_SERIES_ERROR){
		ss.st[num].sense.ambient_light = SICP_handle.rxBuf[7];
		ss.st[num].sense.color_sense_H = SICP_handle.rxBuf[8];
		ss.st[num].sense.color_sense_M = SICP_handle.rxBuf[9];
		ss.st[num].sense.color_sense_L = SICP_handle.rxBuf[10];
	}

}

/*----------------------------------------------------------------------------
	//ST�ظ�����������
-----------------------------------------------------------------------------*/
void sicp_recv_receipt(u16 meshid)
{
	u8 num, mdid;
	DEVICE_TYPE devType = DEVICE_NULL;
	
	num = get_device_number(GET_SERIES_SC, meshid);
	if(num != GET_SERIES_ERROR){
		if(SICP_handle.rxBuf[7] == 0x05 ){		//ִ��֮�󷵻�״̬			
			devType = DEVICE_SC;
			mdid = SICP_handle.rxBuf[8] - 1;
			if(ss.sc[num].slc[mdid].MDID){
				ss.sc[num].slc[mdid].ch1_status = SICP_handle.rxBuf[9];
				ss.sc[num].slc[mdid].ch2_status = SICP_handle.rxBuf[10];
				ss.sc[num].slc[mdid].ch3_status = SICP_handle.rxBuf[11];
				ss.sc[num].slc[mdid].ch4_status = SICP_handle.rxBuf[12];
			}
			if(ss.sc[num].spc[mdid].MDID){
				ss.sc[num].spc[mdid].ch1_status = SICP_handle.rxBuf[9];
				ss.sc[num].spc[mdid].ch2_status = SICP_handle.rxBuf[10];
				ss.sc[num].spc[mdid].ch3_status = SICP_handle.rxBuf[11];
				ss.sc[num].spc[mdid].ch4_status = SICP_handle.rxBuf[12];
			}															
		}

	}

	if(num == GET_SERIES_ERROR){
		num = get_device_number(GET_SERIES_ST, meshid);
		if(num != GET_SERIES_ERROR){
			
			if(SICP_handle.rxBuf[7] == 0x05 ){		//ִ��֮�󷵻�״̬	
			
				devType = DEVICE_ST;
				ss.st[num].ch1_status = SICP_handle.rxBuf[8];
				ss.st[num].ch2_status = SICP_handle.rxBuf[9];
				ss.st[num].ch3_status = SICP_handle.rxBuf[10];
				ss.st[num].ch4_status = SICP_handle.rxBuf[11];
			}


			if(SICP_handle.rxBuf[7] == 0x03 || SICP_handle.rxBuf[7] == 0x01){	

				//�ж��Ƿ�Ϊconfig st�����û�ִ
				sicp_config_st_recv_confirm(num, SICP_handle.rxBuf[2]);
				
			}

			
			if(SICP_handle.rxBuf[7] == 0x02){	//���ջ�ִ���жϲ��������Ƿ�ִ�гɹ�


			}
			
			
		}
	}

	//״̬�ϱ�
	if(devType != DEVICE_NULL){
		ssp_device_status_post(devType, num);
	}



}

/*----------------------------------------------------------------------------
	//����SC�㱨����
-----------------------------------------------------------------------------*/
void sicp_recv_energy_consum(u16 meshid, u8 msgid)
{
	u8 num, mdid;

	mdid = SICP_handle.rxBuf[9] -1;
	num = get_device_number(GET_SERIES_SC, meshid);
	if(num != GET_SERIES_ERROR){
		ss.sc[num].spc[mdid].energy_consum = SICP_handle.rxBuf[7]* 256 + SICP_handle.rxBuf[8];
		ss.sc[num].sense.total_energy_consum += ss.sc[num].spc[mdid].energy_consum;		
		ssp_energy_consum_post(num, mdid);	

	}
	/* �����Ƿ���ڸ��豸����Ϣ�����ظ����ջ�ִ
		����scһֱ���� */
	sicp_send_receipt(0x02, msgid, meshid);
}


/*-------------------------------------------------------------------------
    ���յ��¼��ϱ��Ĺ���
    ������ϲ���esh�ϱ�
-------------------------------------------------------------------------*/
void sicp_recv_malfunction(void)
{
	u8 i, num, ret = 3;
	u8 rev_message_id = 0;
	u16 rev_mesh_id = 0;
	DEVICE_TYPE devType = DEVICE_NULL;
	
	rev_message_id 	= SICP_handle.rxBuf[2];
	rev_mesh_id	= (((u16)SICP_handle.rxBuf[3]) <<8 ) + (u16)SICP_handle.rxBuf[4];

	if(SICP_handle.rxBuf[7] == 0xB4){				//st

		num = get_device_number(GET_SERIES_ST, rev_mesh_id);
		if(num != GET_SERIES_ERROR){
			ss.st[num].HWTtest = SICP_handle.rxBuf[9];
			devType = DEVICE_ST;
			ret = 2;		
		}
	}
	else if(SICP_handle.rxBuf[7] == 0xB1 || SICP_handle.rxBuf[7] == 0xB2 || SICP_handle.rxBuf[7] == 0xB3){			//sc��slc��spc

		num = get_device_number(GET_SERIES_SC, rev_mesh_id);
		if(num != GET_SERIES_ERROR){
			if(SICP_handle.rxBuf[7] == 0xB1){			/* sc */
				devType = DEVICE_SC;
				ss.sc[num].HWTtest = SICP_handle.rxBuf[9];
			}else if(SICP_handle.rxBuf[7] == 0xB2){	/* slc */
				devType = DEVICE_SL;
				ss.sc[num].slc[SICP_handle.rxBuf[8] -1].HWTtest = SICP_handle.rxBuf[9];
			}else{						/* spc */
				devType = DEVICE_SP;
				ss.sc[num].spc[SICP_handle.rxBuf[8] -1].HWTtest = SICP_handle.rxBuf[9];
			}
			ret = 2;		
		}
	}
	else{

	}
	
	sicp_send_receipt(ret, rev_message_id, rev_mesh_id);	/* ���ͻ�ִ */

	//״̬�ϱ�
	if(devType != DEVICE_NULL){
		ssp_device_malfunction_post(devType, num, SICP_handle.rxBuf[8]);
	}
	
}

/*----------------------------------------------------------------------------
	//ST�㱨 ����
-----------------------------------------------------------------------------*/
void sicp_recv_data_gesture(u16 meshid, u8 msgid)
{
	u8 num, ret = 0x03;
	u16 value = 0;

	num = get_device_number(GET_SERIES_ST, meshid);
	if(num != GET_SERIES_ERROR){
//		ss.st[num].sense.handgusture_H = SICP_handle.rxBuf[7];
//		ss.st[num].sense.handgusture_L = SICP_handle.rxBuf[8];

		value = (SICP_handle.rxBuf[7] >> 4) * 100 + (SICP_handle.rxBuf[7] & 0x0f) * 10 + (SICP_handle.rxBuf[8] >> 4);

		ssp_gesture_or_keypad_post(num, GESTURE_REPORT, 0, value);
		ret = 0x02;

	}
	sicp_send_receipt(ret, msgid, meshid);

}

/*----------------------------------------------------------------------------
	//ST�㱨 ��������
-----------------------------------------------------------------------------*/
void sicp_recv_data_keypad(u16 meshid, u8 msgid)
{
	u8 num, ret = 0x03;
	
	num = get_device_number(GET_SERIES_ST, meshid);
	if(num != GET_SERIES_ERROR){
		
//		if((SICP_handle.rxBuf[7] & 0x01) == 0x01)	ss.st[num].ch1_status = SICP_handle.rxBuf[8];
//		if((SICP_handle.rxBuf[7] & 0x02) == 0x02)	ss.st[num].ch2_status = SICP_handle.rxBuf[8];
//		if((SICP_handle.rxBuf[7] & 0x04) == 0x04)	ss.st[num].ch3_status = SICP_handle.rxBuf[8];

		ssp_gesture_or_keypad_post(num, KEYPAD_REPORT, SICP_handle.rxBuf[7], (u16)SICP_handle.rxBuf[8]);
		ret = 0x02;
	}
	sicp_send_receipt(ret, msgid, meshid);
	
}

/*----------------------------------------------------------------------------
	//ST�㱨 ��������
-----------------------------------------------------------------------------*/
void sicp_recv_data_slippad(u16 meshid, u8 msgid)
{
	u8 num, ret = 0x03;

	num = get_device_number(GET_SERIES_ST, meshid);
	if(num != GET_SERIES_ERROR){
//		if((SICP_handle.rxBuf[7] & 0x01) == 0x01)	ss.st[num].ch1_status = SICP_handle.rxBuf[8];
//		if((SICP_handle.rxBuf[7] & 0x02) == 0x02)	ss.st[num].ch2_status = SICP_handle.rxBuf[8];
//		if((SICP_handle.rxBuf[7] & 0x04) == 0x04)	ss.st[num].ch3_status = SICP_handle.rxBuf[8];

		ssp_gesture_or_keypad_post(num, SLIPPAD_REPORT, SICP_handle.rxBuf[7], (u16)SICP_handle.rxBuf[8]);
		ret = 0x02;
		
	}
	sicp_send_receipt(ret, msgid, meshid);
	
}

/*----------------------------------------------------------------------------
	
-----------------------------------------------------------------------------*/
void sicp_recv_data_action_notify(u16 meshid, u8 msgid)
{
	u8 num, mdid;
	u16 sc_meshid;

	if(SICP_handle.rxBuf[7] == 0x01) {rev_st_rt = 1; ss_rt.cp.isCP = 1;}
	if(SICP_handle.rxBuf[7] == 0x02) {rev_st_rt = 1; ss_rt.gt.isGT = 1;}

	sc_meshid = SICP_handle.rxBuf[10] * 256 + SICP_handle.rxBuf[11];
	mdid = (SICP_handle.rxBuf[13] >> 4) -1;

	num = get_device_number(GET_SERIES_SC, sc_meshid);
	if(num != GET_SERIES_ERROR){

		switch(SICP_handle.rxBuf[12]){
			case 0x51:					//ST��������DM���첽֪ͨ

				if(ss.sc[num].slc[mdid].MDID){
					mymemcpy(ss_rt.sepid2, ss.sc[num].deviceid,10);
//					mymemcpy(ss_rt.sepid, ss.sc[num].slc[mdid].deviceid,10);
					ss_rt.MD = ss.sc[num].slc[mdid].MDID;
					ss_rt.gt.action[0] = 'D';ss_rt.gt.action[1] = 'M';
					ss_rt.gt.ch = SICP_handle.rxBuf[13];
					ss_rt.gt.topos[0] = hex2ascii(SICP_handle.rxBuf[14] /10);
					ss_rt.gt.topos[1] = hex2ascii(SICP_handle.rxBuf[14] %10);
					ss_rt.gt.option_duration = SICP_handle.rxBuf[15] /10;
					ss_rt.gt.option_erase  = 1;
					ss_rt.gt.timeout = 3;
					switch((SICP_handle.rxBuf[13] & 0x0f)){
						case 1:
							ss.sc[num].slc[mdid].ch1_status = SICP_handle.rxBuf[14];
							break;
						case 2:
							ss.sc[num].slc[mdid].ch2_status = SICP_handle.rxBuf[14];
							break;
						case 3:
							ss.sc[num].slc[mdid].ch3_status = SICP_handle.rxBuf[14];
							break;
						default:
							break;
					}
				
				}		
				sicp_send_receipt(0x02, msgid, meshid);

				break;
				
			case 0x55:

				if(ss.sc[num].spc[mdid].MDID == ((SICP_handle.rxBuf[13] & 0xf0) >> 4)){
					mymemcpy(ss_rt.sepid2, ss.sc[num].deviceid, 10);
//					mymemcpy(ss_rt.sepid, ss.sc[num].spc[mdid].deviceid, 10);
					ss_rt.MD = ss.sc[num].spc[mdid].MDID;
					ss_rt.gt.action[0] = 'W';ss_rt.gt.action[1] = 'P';
					ss_rt.gt.ch = SICP_handle.rxBuf[13];
					ss_rt.gt.topos[0] = hex2ascii(SICP_handle.rxBuf[14] /10);
					ss_rt.gt.topos[1] = hex2ascii(SICP_handle.rxBuf[14] %10);
					ss_rt.gt.option_duration = SICP_handle.rxBuf[15] /10;
					ss_rt.gt.option_erase  = 1;
					ss_rt.gt.timeout = 3;
					switch((SICP_handle.rxBuf[13]&0x0f)){
						case 1:
							ss.sc[num].spc[mdid].ch1_status = SICP_handle.rxBuf[14];
							break;
						case 2:
							ss.sc[num].spc[mdid].ch2_status = SICP_handle.rxBuf[14];
							break;
						case 4:
							ss.sc[num].spc[mdid].ch3_status = SICP_handle.rxBuf[14];
							break;
						default:
							break;
					}
					
				}
			
				sicp_send_receipt(0x02, msgid, meshid);
	
				break;
				
			default:
				sicp_send_receipt(0x03, msgid, meshid);
				break;
				
		}

	}

}


/*----------------------------------------------------------------------------
	����û�кϷ��豸�б���ֱ�ӱ�����յ������� 
	���Ѵ��ںϷ��豸�б�
//			case 0xB2://SLC
//			
//			rev_deviceid[0] = 'S'; rev_deviceid[1] = 'L';
//			num = get_series_number(GET_SERIES_TO_SAVE, GET_SERIES_SL, recv_meshid, NULL);		
//			if(num != GET_SERIES_ERROR){				
//				strcpy(ss.sc[num].slc[mdid].deviceid, rev_deviceid);
//				ss.sc[num].slc[mdid].meshid = recv_meshid;	
//				ss.sc[num].slc[mdid].model = SICP_handle.rxBuf[11];
//				ss.sc[num].slc[mdid].firmware = SICP_handle.rxBuf[12];
//				ss.sc[num].slc[mdid].HWTtest = SICP_handle.rxBuf[13];
//				ss.sc[num].slc[mdid].MDID = mdid;
//			}						
//			break;
//			
//		case 0xB3://SPC

//			rev_deviceid[0] = 'S'; rev_deviceid[1] = 'P';
//			num = get_series_number(GET_SERIES_TO_SAVE, GET_SERIES_SP, recv_meshid, NULL);		
//			if(num != GET_SERIES_ERROR){				
//				strcpy(ss.sc[num].spc[mdid].deviceid, rev_deviceid);
//				ss.sc[num].spc[mdid].meshid = recv_meshid;	
//				ss.sc[num].spc[mdid].model = SICP_handle.rxBuf[11];
//				ss.sc[num].spc[mdid].firmware = SICP_handle.rxBuf[12];
//				ss.sc[num].spc[mdid].HWTtest = SICP_handle.rxBuf[13];
//				ss.sc[num].spc[mdid].MDID = mdid;
//			}						
//			break;
-----------------------------------------------------------------------------*/
void sicp_recv_device_info(u16 meshid)
{
	u8  	rev_deviceid[12] = {0};
	u8 	i, num = GET_SERIES_ERROR;
	u8 	type = SICP_handle.rxBuf[6];	

	hex2_toChar(SICP_handle.rxBuf[7], (char *)&rev_deviceid[2]);
	hex2_toChar(SICP_handle.rxBuf[8], (char *)&rev_deviceid[4]);
	hex2_toChar(SICP_handle.rxBuf[9], (char *)&rev_deviceid[6]);
	hex2_toChar(SICP_handle.rxBuf[10], (char *)&rev_deviceid[8]);
	rev_deviceid[10] = 0;

	switch(type){
		
		case 0xB1:	//SC
		
			rev_deviceid[0] = 'S'; rev_deviceid[1] = 'C';
			num = get_series_number(GET_SERIES_TO_SAVE, GET_SERIES_SC, meshid, NULL);		
			if(num != GET_SERIES_ERROR){				
				strcpy(ss.sc[num].deviceid, rev_deviceid);
				ss.sc[num].meshid = meshid;	
				ss.sc[num].model = SICP_handle.rxBuf[11];
				ss.sc[num].firmware = SICP_handle.rxBuf[12];
				ss.sc[num].HWTtest = SICP_handle.rxBuf[13];
				/* slc */
				for(i = 1; i < 8; i++){			/* SICP_handle.rxBuf[14] ��λ */
					if(SICP_handle.rxBuf[14] & BIT[i]){		/* �豸����*/
						ss.sc[num].slc[i -1].MDID= i;
					}				
				}
				for(i = 0; i < 8; i++){			/* SICP_handle.rxBuf[15] ��λ */
					if(SICP_handle.rxBuf[15] & BIT[i]){		/* �豸����*/
						ss.sc[num].slc[i + 7].MDID = i + 8;
					}				
				}

				/* spc */
				for(i = 1; i < 8; i++){ 			/* SICP_handle.rxBuf[16] ��λ */
					if(SICP_handle.rxBuf[16] & BIT[i]){		/* �豸����*/
						ss.sc[num].spc[i -1].MDID = i;
					}				
				}
				for(i = 0; i < 8; i++){ 			/* SICP_handle.rxBuf[17] ��λ */
					if(SICP_handle.rxBuf[17] & BIT[i]){		/* �豸����*/
						ss.sc[num].spc[i + 7].MDID = i + 8;
					}				
				}

				for(i = 0; i < devList.scSize; i++){
					if(devList.scBroadcastRecvFlag[i] == 0){
						if(!memcmp(devList.scDeviceID[i], &SICP_handle.rxBuf[7], 4)){				
							devList.scBroadcastRecvFlag[i] = 1;
							break;
						}
					}
				}

			}						
			break;
					
		case 0xB4://ST

			rev_deviceid[0] = 'S'; rev_deviceid[1] = 'T';
			num = get_series_number(GET_SERIES_TO_SAVE, GET_SERIES_ST, meshid, NULL);		
			if(num != GET_SERIES_ERROR){				
				strcpy(ss.st[num].deviceid, rev_deviceid);
				ss.st[num].meshid = meshid;	
				ss.st[num].model = SICP_handle.rxBuf[11];
				ss.st[num].firmware = SICP_handle.rxBuf[12];
				ss.st[num].HWTtest = SICP_handle.rxBuf[13];

				for(i = 0; i<devList.stSize; i++){
					if(devList.stBroadcastRecvFlag[i] == 0){
						if(!memcmp(devList.stDeviceID[i], &SICP_handle.rxBuf[7], 4)){				
							devList.stBroadcastRecvFlag[i] = 1;
							break;
						}
					}
				}

			}	
			break;

		default:
			break;
			
	}
	
}



/*----------------------------------------------------------------------------
	sicp_recv_analyze
-----------------------------------------------------------------------------*/
void sicp_recv_analyze(void)
{
	u8 msgid;
	u16 meshid;
	
	if(SICP_handle.status.bit.recvFlag){
		SICP_handle.status.bit.recvFlag = 0;
		
		if (SICP_handle.rxBuf[0] == 0xDD){				
			sicp_recv_DD();
		}

		if (SICP_handle.rxBuf[0] == 0xEE){
			
			msgid = SICP_handle.rxBuf[2];
			meshid = (((u16)SICP_handle.rxBuf[3]) << 8) + (u16)SICP_handle.rxBuf[4];

			if(ss.meshid != meshid){		/* �����Լ����͵����� */

				switch(SICP_handle.rxBuf[6]){			
					
					case 0xAA: sicp_recv_receipt(meshid); reSendList_recv_confirm(msgid); break;					
					case 0x11: 
						sicp_recv_heart_beat_sc(meshid); 
						if(SICP_handle.rxBuf[1] == 0xAA){reSendList_recv_confirm(msgid);} 	/* ��ΪӦ�����ݲŽ���ȷ�� */
						break;										
					case 0x12: sicp_recv_heart_beat_st(meshid); break;										
					case 0x20: sicp_recv_cmd_data(meshid); break;				//ST�ظ�����������									
					case 0x0A: sicp_recv_malfunction(); break;					//���ϻ㱨
					case 0x2A: sicp_recv_energy_consum(meshid, msgid); break;		//SC�ϱ�����					
					case 0x29: sicp_recv_data_gesture(meshid, msgid); break;	//ST�㱨 ����
					case 0x35: sicp_recv_data_keypad(meshid, msgid); break;		//ST�㱨 ��������
					case 0x36: sicp_recv_data_keypad(meshid, msgid); break;		//ST�㱨 ��������

					case 0x08: sicp_recv_data_action_notify(meshid, msgid); break;	/* �첽֪ͨ */
					case 0xB1:
					case 0xB2:
					case 0xB3:
					case 0xB4: sicp_recv_device_info(meshid); break;	
					
					default: break;
						
				}

			}

		}
	
	}
	
}





