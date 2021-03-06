#include "gui.h"
#include "button.h"
#include "PROGBAR.h"
#include "draw_print_file.h"
#include "draw_ui.h"
#include "fontLib.h"
#include "LISTBOX.h"
#include "text.h"
#include "Draw_ready_print.h"
//#include "stm32mks_timer.h"     //skyblue 2016-12-13
#include "tim.h"        //skyblue 2016-12-13
#include "ff.h"
#include "fatfs.h"
//#include "sd_config.h"
#include <string.h>
#include <draw_set.h>
#include "sdio.h"
//#include "sd_usr.h"
#include "sys.h"
#include "pic.h"
#include "usb_host.h"

#include "Marlin.h"
#include "configuration_adv.h"
#if defined(MKS_DLP_BOARD)	//MKS DLP_BOARD主板
#include "fastio_dlp_board.h"
#else
#include "fastio_robin2.h"
#endif  


#include "cardreader.h"

#ifndef GUI_FLASH
#define GUI_FLASH
#endif
extern DMA_HandleTypeDef hdma_sdio_rx;

extern CardReader card;

GUI_HWIN hPrintFileWnd, hAlertWin, hPrintFileDlgWnd;

uint8_t trig_browser_flag;

int8_t curDirLever = 0;

DIR_OFFSET dir_offset[10];

//extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
//extern USBH_HOST                     USB_Host;

extern GUI_FLASH const GUI_FONT GUI_FontHZ_fontHz18;
extern int X_ADD,X_INTERVAL;

//extern uint8_t gcode_cache;
#if _LFN_UNICODE
TCHAR curFileName[150] = "notValid";
#else
char curFileName[150] = "notValid";
#endif

static int8_t pages;
static TEXT_Handle printDlgAskText;
static BUTTON_STRUCT buttonPu, buttonPd, buttonR, buttonF[6], buttonOk, buttonCancle;

uint8_t back_flg = 0;	


uint8_t disp_in_file_dir;


static void cbPrintFileWin(WM_MESSAGE * pMsg) {


uint8_t backup_fileAttr[5];	
int8_t sel_item;
int8_t i,j;

uint32_t choose_ret;	
uint32_t temp;
	uint32_t tick3, tick4;
	uint32_t differ1;	
//int8_t sel_file[30];

//struct PressEvt *press_event;
switch (pMsg->MsgId) {
case WM_PAINT:
//GUI_SetBkColor(GUI_BLUE);
//GUI_Clear();
//GUI_DispString("window");
break;
case WM_TOUCH:
// 	press_event = (struct PressEvt *)pMsg->Data.p;
	
break;
case WM_TOUCH_CHILD:
//  press_event = (struct PressEvt *)pMsg->Data.p;
	
  break;
  case WM_NOTIFY_PARENT:
	if(pMsg->Data.v == WM_NOTIFICATION_RELEASED)
	{
		if(pMsg->hWinSrc == buttonPd.btnHandle)
		{	
			if(dir_offset[curDirLever].cur_page_last_offset > 0)
			{
								
				
				//2015.05.19
				card.Sd_file_cnt = 0;
				card.gcodeFileList.index = 0;

				card.Sd_file_offset = dir_offset[curDirLever].cur_page_last_offset + 1;
				do
				{
					card.ShowSDFiles();
					
				
					if((card.gcodeFileList.listVaild == 1) && (card.gcodeFileList.index == 1))
						dir_offset[curDirLever].cur_page_first_offset = card.Sd_file_offset;
				
					
					if(card.gcodeFileList.index >= 6)
					{
						dir_offset[curDirLever].cur_page_last_offset = card.Sd_file_offset;
						card.Sd_file_offset++;		
					
						break;
					}
					card.Sd_file_offset++;					
					
				} while(card.gcodeFileList.listVaild != 2);

				if(card.gcodeFileList.index != 0)
				{
					//dir_offset[curDirLever].cur_page_last_offset = 0;
					Clear_print_file();
					disp_udisk_files(0);
					dir_offset[curDirLever].curPage++;
					
				}
				
				if(card.gcodeFileList.listVaild == 2)
				{
						dir_offset[curDirLever].cur_page_last_offset = 0;
				}
					
		
				
		//		pages = Sd_display_file_cnt/ MAX_FILES_PER_LEVER+ 1;
			}
		}
		else if(pMsg->hWinSrc == buttonPu.btnHandle)
		{
			if(dir_offset[curDirLever].curPage > 0)
			{
				
				//2015.05.19
				card.Sd_file_cnt = 0;
				card.gcodeFileList.index = 0;
				
				if(dir_offset[curDirLever].cur_page_first_offset >= 5)
					card.Sd_file_offset = dir_offset[curDirLever].cur_page_first_offset - 1;
				
				do 
				{
				
					card.ShowSDFiles();
				
					if((card.gcodeFileList.listVaild == 1) && (card.gcodeFileList.index == 1))
						dir_offset[curDirLever].cur_page_last_offset = card.Sd_file_offset;
										
				
					if(card.gcodeFileList.index >= 6)
					{
						dir_offset[curDirLever].cur_page_first_offset = card.Sd_file_offset;

						if(card.Sd_file_offset)
							card.Sd_file_offset--;
						
						
						break;
					}

					if(card.Sd_file_offset)
						card.Sd_file_offset--;
										
				} while(card.gcodeFileList.listVaild != 2);
				if(card.gcodeFileList.index != 0)
				{
					Clear_print_file();
					disp_udisk_files(1);
					dir_offset[curDirLever].curPage--;
				}
				
			}
		}
		else if(pMsg->hWinSrc == buttonR.btnHandle)
		{
			if(curDirLever == 0)
			{
				#if 0
				if(last_disp_state ==SET_UI)choose_ret=1;
				
				
				last_disp_state = PRINT_FILE_UI;
				Clear_print_file();
				//2015.05.19
				sd.Sd_file_offset = 0;
				sd.Sd_file_cnt = 0;
				//Sd_display_file_cnt = 0;
				if(choose_ret==1)draw_Set();
				else draw_ready_print();
				#endif
				if(gCfgItems.breakpoint_reprint_flg == 1)
				{
					gCfgItems.breakpoint_reprint_flg = 0;
					last_disp_state = SET_UI;
					Clear_print_file();
					card.Sd_file_offset = 0;
					card.Sd_file_cnt = 0;
					draw_return_ui();	
				}
				else
				{
					last_disp_state = PRINT_FILE_UI;
					Clear_print_file();
					//2015.05.19
					card.Sd_file_offset = 0;
					card.Sd_file_cnt = 0;
					//Sd_display_file_cnt = 0;
					draw_ready_print();
				}
				disp_in_file_dir = 0;
			}
			else
			{
				int8_t *ch = 0;
				
				ch =  (int8_t *)strrchr((char *)card.gCurDir, '/');
				if(ch != 0)
				{
					*ch = 0;
					Clear_print_file();
					dir_offset[curDirLever].curPage = 0;
					dir_offset[curDirLever].cur_page_first_offset = 0;
					dir_offset[curDirLever].cur_page_last_offset = 0;
					curDirLever--;
					search_files();
					
				}
				
			}
		}
		else
		{
			for(i = 0; i < 6; i++)
			{
				if(back_flg == 1)
				{
					j = 5 - i;
				}
				else
				{
					j = i;
				}
				if(pMsg->hWinSrc == buttonF[i].btnHandle)
				{
					//if(sd.gcodeFileList.fileName[i][0] != 0)
					if(card.gcodeFileList.fileName[j][0] != 0)
					{
						//if(sd.gcodeFileList.fileAttr[i] == 1) //dir
						if(card.gcodeFileList.fileAttr[j] == 1) //dir
						//if(backup_fileAttr[i] == 1) //dir
						{
							memset(card.gCurDir, 0, sizeof(card.gCurDir));
							//strcpy(sd.gCurDir,  sd.gcodeFileList.fileName[i]);
							strcpy((char *)card.gCurDir,  (const char *)card.gcodeFileList.fileName[j]);
							Clear_print_file();
							curDirLever++;
							search_files();
							
						}
						
						else
						{
							memset(curFileName, 0, sizeof(curFileName));
							//strcpy(curFileName, sd.gcodeFileList.fileName[i]);
							#if _LFN_UNICODE
							wcscpy((wchar_t *)curFileName, (const wchar_t*)card.gcodeFileList.fileName[j]);
							#else
							strcpy(curFileName, (const char *)card.gcodeFileList.fileName[j]);
							#endif
								
							Clear_print_file();
							disp_in_file_dir = 0;
							//draw_dialog(DIALOG_TYPE_PRINT_FILE);
							preview_gcode_prehandle(curFileName);
							draw_Preview();
						}
						break;
					}
				}
			}
		}
	}
	break;
default:
WM_DefaultProc(pMsg);
}
}

extern ApplicationTypeDef Appli_state;
FATFS fs;
#if 0
void draw_print_file()
{
	
	
	//int titleHeight = 30;
//	int buttonHeight = 30;
//	int buttonWidth = LCD_WIDTH/4;
	//int imgHeight = LCD_HEIGHT - titleHeight;

	
	int i;
	
	if(disp_state_stack._disp_state[disp_state_stack._disp_index] != PRINT_FILE_UI)
	{
		disp_state_stack._disp_index++;
		disp_state_stack._disp_state[disp_state_stack._disp_index] = PRINT_FILE_UI;
	}
	disp_state = PRINT_FILE_UI;

	GUI_SetBkColor(gCfgItems.background_color);
	GUI_SetColor(gCfgItems.title_color);
	GUI_Clear();
	#if 0
	if(gCfgItems.language == LANG_COMPLEX_CHINESE)
	{
		GUI_SetFont(&GUI_FontHZ16);
	}
	else if(gCfgItems.language == LANG_SIMPLE_CHINESE)
	{
		GUI_SetFont(&FONT_TITLE);
	}
	else
	{
		GUI_SetFont(&GUI_FontHZ_fontHz18);
	}
	#endif
#if 0

	GUI_DispStringAt(creat_title_text(),  TITLE_XPOS, TITLE_YPOS);
	
	if(gCfgItems.language == LANG_ENGLISH)
	{
		GUI_SetFont(&GUI_FontHZ_fontHz18);
		GUI_DispStringAt("Loading......", 300, 200);
	}
	else if(gCfgItems.language == LANG_COMPLEX_CHINESE)
	{		
		GUI_DispStringAt("正在載入......", 300, 200);
	}	
	else
	{		
		GUI_DispStringAt("正在载入......", 300, 200);
	}
#endif	
	
	hPrintFileWnd = 0;
	//hPrintFileWnd = WM_CreateWindow(0, titleHeight, LCD_WIDTH, imgHeight, WM_CF_SHOW, cbPrintFileWin, 0);

	curDirLever = 0;	
	dir_offset[curDirLever].curPage = 0;
	
#if 1
	card.Sd_file_offset = 0;
	card.Sd_file_cnt = 0;

	//Sd_display_file_cnt = 0;

	memset(dir_offset, 0, sizeof(dir_offset));
	//tan0914
	//GUI_SetFont(&FONT_TITLE);

	//if(gCfgItems.fileSysType == FILE_SYS_SD)
{
	memset(card.gCurDir, 0, sizeof(card.gCurDir));
	//
	#if _LFN_UNICODE
	card.gCurDir[0] = '1';
	card.gCurDir[1] = ':';
	#else
	//strcpy((char *)sd.gCurDir, "1:/gcode_files");
                
    //strcpy((char *)sd.gCurDir, "1:");      //skyblue-modify
    //if ((SD_DET_IP == SD_DETECT_INVERTED)&&(sd.usbactive == false))
    if((gCfgItems.fileSysType == FILE_SYS_SD))
    {
		//MX_SDIO_SD_Init();
		if(SD_DET_IP == SD_DETECT_INVERTED)
		{
      		
    		//FATFS_LinkDriver_sd(&SD_Driver, SD_Path);
			f_mount(&fs, (TCHAR const*)SD_Path, 0);	
		}
		strcpy((char *)card.gCurDir, "1:");      //skyblue-modify
    }
    else
    {
      	
		//FATFS_LinkDriver_usb(&USBH_Driver, USBH_Path);
        f_mount(&fs, (TCHAR const*)USBH_Path, 0);
		strcpy((char *)card.gCurDir, "0:");
        //Appli_state = APPLICATION_DISCONNECT;
	    //if((Appli_state == APPLICATION_READY))
	    {
	        //FATFS_LinkDriver_usb(&USBH_Driver, USBH_Path);
	        //f_mount(&fs, (TCHAR const*)USBH_Path, 0);
	        //Appli_state = APPLICATION_DISCONNECT;
	    }				
    }
	
	#endif
	//SD_Init();
	//SD_Initialize();
}	    
               
	
 #endif	
	search_files();
	
}

#else
//volatile uint8_t usb_test_enable = 1;
void draw_print_file()
{
 
	
	int i;
	
	if(disp_state_stack._disp_state[disp_state_stack._disp_index] != PRINT_FILE_UI)
	{
		disp_state_stack._disp_index++;
		disp_state_stack._disp_state[disp_state_stack._disp_index] = PRINT_FILE_UI;
	}
	disp_state = PRINT_FILE_UI;

	GUI_SetBkColor(gCfgItems.background_color);
	GUI_SetColor(gCfgItems.title_color);
	GUI_Clear();
	#if 0
	if(gCfgItems.language == LANG_COMPLEX_CHINESE)
	{
		GUI_SetFont(&GUI_FontHZ16);
	}
	else if(gCfgItems.language == LANG_SIMPLE_CHINESE)
	{
		GUI_SetFont(&FONT_TITLE);
	}
	else
	{
		GUI_SetFont(&GUI_FontHZ_fontHz18);
	}
	#endif
#if 0

	GUI_DispStringAt(creat_title_text(),  TITLE_XPOS, TITLE_YPOS);
	
	if(gCfgItems.language == LANG_ENGLISH)
	{
		GUI_SetFont(&GUI_FontHZ_fontHz18);
		GUI_DispStringAt("Loading......", 300, 200);
	}
	else if(gCfgItems.language == LANG_COMPLEX_CHINESE)
	{		
		GUI_DispStringAt("正在載入......", 300, 200);
	}	
	else
	{		
		GUI_DispStringAt("正在载入......", 300, 200);
	}
#endif	
	
	hPrintFileWnd = 0;
	//hPrintFileWnd = WM_CreateWindow(0, titleHeight, LCD_WIDTH, imgHeight, WM_CF_SHOW, cbPrintFileWin, 0);

	curDirLever = 0;	
	dir_offset[curDirLever].curPage = 0;
	
#if 1
	card.Sd_file_offset = 0;
	card.Sd_file_cnt = 0;

	//Sd_display_file_cnt = 0;

	memset(dir_offset, 0, sizeof(dir_offset));
	//tan0914
	//GUI_SetFont(&FONT_TITLE);

	//if(gCfgItems.fileSysType == FILE_SYS_SD)
{
	memset(card.gCurDir, 0, sizeof(card.gCurDir));
	//
	#if _LFN_UNICODE
	card.gCurDir[0] = '1';
	card.gCurDir[1] = ':';
	#else
	//strcpy((char *)sd.gCurDir, "1:/gcode_files");
                
    //strcpy((char *)sd.gCurDir, "1:");      //skyblue-modify
    //if ((SD_DET_IP == SD_DETECT_INVERTED)&&(sd.usbactive == false))
    gCfgItems.fileSysType = FILE_SYS_USB;
    if((gCfgItems.fileSysType == FILE_SYS_SD))
    {
		//MX_SDIO_SD_Init();
		if(SD_DET_IP == SD_DETECT_INVERTED)
		{
      		
    		//FATFS_LinkDriver_sd(&SD_Driver, SD_Path);
			f_mount(&fs, (TCHAR const*)SD_Path, 0);	
		}
		strcpy((char *)card.gCurDir, "1:");      //skyblue-modify
    }
    else
    {
/*---------------------*/
#if 0
	usb_test_enable  = 1;
if(usb_test_enable)
{
	card.usbOK = false;
	VUSB_ENA_OP = 0;
	HAL_Delay(100);
	VUSB_ENA_OP = 1;
	HAL_Delay(1000);
	
	for(int i=0;i<10000;i++)
		{
	  	MX_USB_HOST_Process();
		card.checkFilesys(FILE_SYS_USB);
		if(card.usbOK) 
			break;
                //HAL_Delay(10);
		}
/*
	if(card.usbOK)
		{
	  	card.initusb();
	  	card.mksConfiguration();
 		}
*/
}
#endif
/*---------------------*/      	
		//FATFS_LinkDriver_usb(&USBH_Driver, USBH_Path);
        f_mount(&fs, (TCHAR const*)USBH_Path, 0);
		strcpy((char *)card.gCurDir, "0:");
        //Appli_state = APPLICATION_DISCONNECT;
	    //if((Appli_state == APPLICATION_READY))
	    {
	        //FATFS_LinkDriver_usb(&USBH_Driver, USBH_Path);
	        //f_mount(&fs, (TCHAR const*)USBH_Path, 0);
	        //Appli_state = APPLICATION_DISCONNECT;
	    }				
    }
	
	#endif
	//SD_Init();
	//SD_Initialize();
}	    
               
	
 #endif	
	search_files();
	
}

#endif


#if 1
void search_files()
{
	uint32_t tick1, tick2;
#if 1	
//	dir_offset[curDirLever].curPage = 0;
	GUI_UC_SetEncodeUTF8();

	GUI_SetColor(gCfgItems.state_msg_textcolor);
#if 0	
	if(gCfgItems.language == LANG_ENGLISH)
	{
		GUI_SetFont(&GUI_FontHZ_fontHz18);
		GUI_DispStringAt(creat_title_text(), X_ADD, X_INTERVAL);
		GUI_DispStringAt("Loading......", 300, 200);
	}
	else if(gCfgItems.language == LANG_COMPLEX_CHINESE)
	{
		GUI_DispStringAt(creat_title_text(), X_ADD, X_INTERVAL);
		GUI_DispStringAt("正在載入......", 300, 200);
	}
	else
	{
		GUI_DispStringAt(creat_title_text(), X_ADD, X_INTERVAL);
		GUI_DispStringAt("正在载入......", 300, 200);
	}
#endif
	//GUI_DispStringAt(creat_title_text(), TITLE_XPOS, TITLE_YPOS);
	GUI_DispStringAt(file_menu.file_loading, (LCD_WIDTH-64)/2, imgHeight/2);

	tick1 = getTick();
	
	GUI_UC_SetEncodeNone();
	//while(1)
	{

		card.Sd_file_cnt = 0;
	
		card.gcodeFileList.listVaild = 0;
		card.gcodeFileList.index = 0;
		memset(card.gcodeFileList.fileAttr, 0, sizeof(card.gcodeFileList.fileAttr));
		memset(card.gcodeFileList.fileName, 0, sizeof(card.gcodeFileList.fileName));

		//gcode_cache = 0;
		//if(gCfgItems.fileSysType == FILE_SYS_SD)
		{					

			card.Sd_file_offset = dir_offset[curDirLever].cur_page_first_offset;
			do
			{
			#if 0//tan_mask
				if(gCfgItems.fileSysType == FILE_SYS_SD)
				{
					card.ShowSDFiles();
				}
				else
				{
					card.ShowUSBFiles();
				}
			#endif	
				card.ShowSDFiles();
			
				if((card.gcodeFileList.listVaild == 1) && (card.gcodeFileList.index == 1))
					dir_offset[curDirLever].cur_page_first_offset = card.Sd_file_offset;
			
				
				if(card.gcodeFileList.index >= 6)
				{
					dir_offset[curDirLever].cur_page_last_offset = card.Sd_file_offset;
					card.Sd_file_offset++;					
					break;
				}
				card.Sd_file_offset++;
				
				
				
			} while(card.gcodeFileList.listVaild != 2);
			
		}
		/*
		else if(gCfgItems.fileSysType == FILE_SYS_USB)
		{
		//	memset(sd.gCurDir, 0, sizeof(sd.gCurDir));
		//	strcpy(sd.gCurDir, "0:");
		
			display_udisk_files();
			//gFile_Browser(sd.gCurDir);
		}
		*/
	/*	if(sd.gcodeFileList.index != 0)
			break;
		
		tick2 = getTick();
		if(getTickDiff(tick1, tick2) >= 2000)
		{
			break;
		}*/
	}
	if(card.gcodeFileList.index == 0)
	{
		//buttonF[0] = BUTTON_CreateEx(0,  0, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1,hPrintFileWnd, BUTTON_CF_SHOW, 0, 103);
		//BUTTON_SetText(buttonF[0], "鏃犳枃浠?);
		
		GUI_UC_SetEncodeUTF8();
		//GUI_Exec();
		GUI_ClearRect(100, 80, 600, 400);
		if(curDirLever == 0)
		{
		#if 0
			if(gCfgItems.language == LANG_ENGLISH)
			{
				GUI_SetFont(&GUI_FontHZ_fontHz18);
				GUI_DispStringAt("No files found!", 330, 200);
				GUI_DispStringAt("Please insert the sd card or udisk!", 230, 250);
			}	
			else if(gCfgItems.language == LANG_COMPLEX_CHINESE)
			{
				GUI_DispStringAt("無文件! ", 350, 200);
				GUI_DispStringAt("請插入SD卡 / U盤!", 310, 250);
			}
			else
			{
				GUI_DispStringAt("无文件 !", 350, 200);
				GUI_DispStringAt("请插入SD卡 / U盘!", 310, 250);
			}
			#endif
			GUI_DispStringAt(file_menu.no_file_and_check, (LCD_WIDTH-200)/2, imgHeight/2);
		}
		else
		{
		#if 0
			if(gCfgItems.language == LANG_ENGLISH)
			{
				GUI_SetFont(&GUI_FontHZ_fontHz18);
				GUI_DispStringAt("No files found!", 300,200);
			}	
			else if(gCfgItems.language == LANG_COMPLEX_CHINESE)
			{
				GUI_DispStringAt("無文件! ", 300, 200);
			}
			else
			{
				GUI_DispStringAt("无文件 ! ",300, 200);
			}
			#endif
			GUI_DispStringAt(file_menu.no_file, (LCD_WIDTH-64)/2,imgHeight/2);
		}

/*-------------------------------------*/
#if 0
	if(curDirLever == 0)	//skyblue modify 2018-10
		{
		card.usbOK = false;
		SERIAL_ECHO_START();
		SERIAL_ECHOLNPGM("USB removed for null");
		FATFS_UnLinkDriver(USBH_Path);
		VUSB_ENA_OP = 0;
		MX_USB_HOST_Init();
		HAL_Delay(500);
		VUSB_ENA_OP = 1;
		}
#endif	
/*-------------------------------------*/
		tick2 = getTick();
		while(1)
		{
			tick1 = getTick();
			if(getTickDiff(tick1, tick2) >= 3000)
			{
			
				if(curDirLever == 0)
				{
/*-------------------------------------*/	
#if 0
					for(int i=0;i<10000;i++)		//skyblue modify 2018-10
						{
						MX_USB_HOST_Process();
						card.checkFilesys(FILE_SYS_USB);
						if(card.usbOK) 
							break;
						}
					if(card.usbOK)
						{
						card.initusb();
						}
#endif					
/*-------------------------------------*/				
					#if 0
					last_disp_state = PRINT_FILE_UI;
					Clear_print_file();
					draw_ready_print();
					#endif
					disp_in_file_dir = 0;
					if(gCfgItems.breakpoint_reprint_flg == 1)
					{
						last_disp_state = SET_UI;
						Clear_print_file();
						draw_return_ui();
					}
					else
					{
						last_disp_state = PRINT_FILE_UI;
                        HAL_DMA_Abort(&hdma_sdio_rx);
						Clear_print_file();
						draw_ready_print();
					}
				}
				else
				{
					int8_t *ch = 0;
					
					ch =  (int8_t *)strrchr((const char *)card.gCurDir, '/');
					if(ch != 0)
					{
						*ch = 0;
						Clear_print_file();
						dir_offset[curDirLever].curPage = 0;
						dir_offset[curDirLever].cur_page_first_offset = 0;
						dir_offset[curDirLever].cur_page_last_offset = 0;
						curDirLever--;
						search_files();
						
					}
					
				}
			//Clear_print_file();
				//draw_ready_print();

			
				break;
			}
		}
		return;
		
	}

	GUI_ClearRect(100, 80, 600, 400);
	//pages = sd.gcodeFileList.index / 5 + 1;
//	pages = Sd_display_file_cnt/ MAX_FILES_PER_LEVER+ 1;

	disp_udisk_files(0);

	#endif
}
#endif
void disp_udisk_files(int seq)
{
	#if _LFN_UNICODE
	TCHAR tmpStr[100] = {0};
	#else
	int8_t tmpStr[100] = {0};
	#endif
	int8_t *tmpStr1;
	int8_t *tmpStr2;
	int8_t i, j;

/*	if(hPrintFileWnd  !=  0)
	{
		Clear_print_file();
	}*/
	GUI_UC_SetEncodeUTF8();
	//GUI_DispStringAt(creat_title_text(), TITLE_XPOS, TITLE_YPOS);
	

	hPrintFileWnd = WM_CreateWindow(0, titleHeight, LCD_WIDTH, imgHeight, WM_CF_SHOW, cbPrintFileWin, 0);

	//buttonPu.btnHandle = BUTTON_CreateEx(BTN_X_PIXEL+INTERVAL_V*2,  BTN_Y_PIXEL+INTERVAL_H,BTN_X_PIXEL, BTN_Y_PIXEL,hPrintFileWnd, BUTTON_CF_SHOW, 0, 104);
	//buttonPd.btnHandle = BUTTON_CreateEx(BTN_X_PIXEL*2+INTERVAL_V*3,  BTN_Y_PIXEL+INTERVAL_H,BTN_X_PIXEL, BTN_Y_PIXEL,hPrintFileWnd, BUTTON_CF_SHOW, 0, 105);
	//buttonR.btnHandle = BUTTON_CreateEx(BTN_X_PIXEL*3+INTERVAL_V*4, BTN_Y_PIXEL+INTERVAL_H,BTN_X_PIXEL, BTN_Y_PIXEL,hPrintFileWnd, BUTTON_CF_SHOW, 0, 106);
	#if defined(TFT70)
	buttonPu.btnHandle = BUTTON_CreateEx(598,0,194,142,hPrintFileWnd, BUTTON_CF_SHOW, 0, 104);//alloc_win_id());
	buttonPd.btnHandle = BUTTON_CreateEx(598,145,194,142,hPrintFileWnd, BUTTON_CF_SHOW, 0, 105);//alloc_win_id());
	buttonR.btnHandle = BUTTON_CreateEx(598,290,194,142,hPrintFileWnd, BUTTON_CF_SHOW, 0, 106);//alloc_win_id());
    #endif
    //#elif defined(TFT35)
	buttonPu.btnHandle = BUTTON_CreateEx(386,15,90,90,hPrintFileWnd, BUTTON_CF_SHOW, 0, 104);//alloc_win_id());
	buttonPd.btnHandle = BUTTON_CreateEx(386,115,90,90,hPrintFileWnd, BUTTON_CF_SHOW, 0, 105);//alloc_win_id());
	buttonR.btnHandle = BUTTON_CreateEx(386,215,90,90,hPrintFileWnd, BUTTON_CF_SHOW, 0, 106);//alloc_win_id());
	//#endif
#if VERSION_WITH_PIC	

	BUTTON_SetBmpFileName(buttonPu.btnHandle, "bmp_pageUp.bin",1);
	BUTTON_SetBmpFileName(buttonPd.btnHandle, "bmp_pageDown.bin",1);
	#if defined(TFT70)
	BUTTON_SetBmpFileName(buttonR.btnHandle, "bmp_return.bin",1);
	BUTTON_SetBitmapEx(buttonPu.btnHandle, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonPd.btnHandle, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonR.btnHandle, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
#endif
	//#elif defined(TFT35)
	BUTTON_SetBmpFileName(buttonR.btnHandle, "bmp_back90x90.bin",1);

	BUTTON_SetBitmapEx(buttonPu.btnHandle, 0, &bmp_struct_90X90,BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonPd.btnHandle, 0, &bmp_struct_90X90,BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonR.btnHandle, 0, &bmp_struct_90X90,BMP_PIC_X, BMP_PIC_Y);	

	//#endif
	BUTTON_SetBkColor(buttonPd.btnHandle, BUTTON_CI_PRESSED, gCfgItems.btn_color);
	BUTTON_SetBkColor(buttonPd.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.btn_color);
	//BUTTON_SetTextColor(buttonPd.btnHandle, BUTTON_CI_PRESSED, gCfgItems.btn_textcolor);
	//BUTTON_SetTextColor(buttonPd.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.btn_textcolor);

	BUTTON_SetBkColor(buttonPu.btnHandle, BUTTON_CI_PRESSED, gCfgItems.btn_color);
	BUTTON_SetBkColor(buttonPu.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.btn_color);
	//BUTTON_SetTextColor(buttonPu.btnHandle, BUTTON_CI_PRESSED, gCfgItems.btn_textcolor);
	//BUTTON_SetTextColor(buttonPu.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.btn_textcolor);

	BUTTON_SetBkColor(buttonR.btnHandle, BUTTON_CI_PRESSED, gCfgItems.btn_color);
	BUTTON_SetBkColor(buttonR.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.btn_color);
	//BUTTON_SetTextColor(buttonR.btnHandle, BUTTON_CI_PRESSED, gCfgItems.back_btn_textcolor);
	//BUTTON_SetTextColor(buttonR.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.back_btn_textcolor);
	
	//因为"上一页"和"下一页"的编码与文件名的编码不一致；
	//所以采用图标显示而不显示文字。
	#if 0
	if(gCfgItems.multiple_language != 0)
	{
		BUTTON_SetText(buttonPd.btnHandle, file_menu.page_down);
		BUTTON_SetText(buttonPu.btnHandle, file_menu.page_up);
		BUTTON_SetText(buttonR.btnHandle, common_menu.text_back);
	}
	#endif
#endif	

	GUI_Exec();

	GUI_UC_SetEncodeNone();

	for(i = 0; i < 6; i++)
	{
		if(seq)
		{
			j = 5 - i;
			back_flg = 1;
		}
		else
		{
			j = i;
			back_flg = 0;
		}
		
		//backup_fileAttr[i] = sd.gcodeFileList.fileAttr[j];
		
		if(i  >= card.gcodeFileList.index)
		{
			break;
		}
	
				
		if(i < 3)
		{
			buttonF[i].btnHandle = BUTTON_CreateEx(6+10*i+116*i,15, 116, 140,hPrintFileWnd, BUTTON_CF_SHOW, 0, 103 + i);
		}
		else
		{
			buttonF[i].btnHandle = BUTTON_CreateEx(6+116*(i-3)+10*(i-3),165, 116, 140,hPrintFileWnd, BUTTON_CF_SHOW, 0, 107 + i);
		}

		BUTTON_SetFont(buttonF[i].btnHandle,&GUI_FontHZ16);
		

#if 1
		memset(tmpStr, 0, sizeof(tmpStr));
		//2015.05.19
		//cutFileName(sd.gcodeFileList.fileName[curPage * 5 + i], 16, 8,  tmpStr);
		#if _LFN_UNICODE
		cutFileName((TCHAR *)card.gcodeFileList.fileName[j], 24, 12,  tmpStr);
       	#else
		cutFileName((char *)card.gcodeFileList.fileName[j], 24, 12,  (char *)tmpStr);
        #endif        

		/*tmpStr1 = (uint8_t *)strstr((uint8_t *)sd.gcodeFileList.fileName[curPage * 5 + i], "/");
		tmpStr2 = tmpStr1;
		tmpStr2++;
	
		while(1)
		{
			
			tmpStr1 = (uint8_t *)strstr(tmpStr2, "/");
			if(tmpStr1 == 0)
			{
				strncpy(tmpStr, tmpStr2, 5);
				break;
			}
			tmpStr1++;
			tmpStr2 = (uint8_t *)strstr(tmpStr1, "/");
			if(tmpStr2 == 0)
			{
				strncpy(tmpStr, tmpStr1, 5);
				break;
			}
			tmpStr2++;
			
		}*/
#if VERSION_WITH_PIC	
/*//2015.05.19
		if(sd.gcodeFileList.fileAttr[i + curPage * 5] == 1) //dir
		{
			BUTTON_SetBmpFileName(&buttonF[i], "bmp_dir.bin");
			BUTTON_SetBitmapEx(buttonF[i], 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
		}
		else
		{
			BUTTON_SetBmpFileName(&buttonF[i], "bmp_file.bin");
			BUTTON_SetBitmapEx(buttonF[i], 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
		}
*/
	BUTTON_SetBkColor(buttonF[i].btnHandle, BUTTON_CI_PRESSED, gCfgItems.filename_background_color);
	BUTTON_SetBkColor(buttonF[i].btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.filename_background_color);
	BUTTON_SetTextColor(buttonF[i].btnHandle, BUTTON_CI_UNPRESSED,gCfgItems.filename_color);
	BUTTON_SetTextColor(buttonF[i].btnHandle, BUTTON_CI_PRESSED,gCfgItems.filename_color);

	if(card.gcodeFileList.fileAttr[j] == 1) //dir
	//if(backup_fileAttr[i] == 1) //dir
	{
		//#if defined(TFT70)
		BUTTON_SetBmpFileName(buttonF[i].btnHandle, "bmp_dir.bin",1);
		BUTTON_SetBitmapEx(buttonF[i].btnHandle, 0, &bmp_struct_90X90,13,25);
		//#elif defined(TFT35)
		//BUTTON_SetBmpFileName(buttonF[i].btnHandle, "bmp_dir.bin",1);
		//BUTTON_SetBitmapEx(buttonF[i].btnHandle, 0, &bmp_struct_100,FILE_PRE_PIC_X_OFFSET, FILE_PRE_PIC_Y_OFFSET);		
		//#endif

	}
	else
	{
		if(have_pre_pic((char *)card.gcodeFileList.fileName[j]))
		{
			BUTTON_SetBmpFileNamePath(buttonF[i].btnHandle, NULL,card.gcodeFileList.fileName[j],1);
			#if defined(TFT35)
			BUTTON_SetBitmapEx(buttonF[i].btnHandle, 0, &bmp_struct_116X116,0, 0);
			#endif			
		}
		else
		{
			BUTTON_SetBmpFileName(buttonF[i].btnHandle, "bmp_file.bin",1);
			#if defined(TFT35)
			BUTTON_SetBitmapEx(buttonF[i].btnHandle, 0, &bmp_struct_90X90,13, 25);
			#endif			
		}
		#if defined(TFT70)
		BUTTON_SetBitmapEx(buttonF[i].btnHandle, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
		#endif
	}

#endif
		BUTTON_SetTextAlign(buttonF[i].btnHandle, GUI_TA_BOTTOM | GUI_TA_HCENTER);
		BUTTON_SetText(buttonF[i].btnHandle, (char const *)(tmpStr));
        #endif

	}
	
	GUI_Exec();
	GUI_UC_SetEncodeUTF8();

}

void Clear_print_file()
{
	GUI_SetBkColor(gCfgItems.background_color);
	if(WM_IsWindow(hPrintFileWnd))
	{
		WM_DeleteWindow(hPrintFileWnd);
		//GUI_Exec();
	}
	
	GUI_Clear();
}




