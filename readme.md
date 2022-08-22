#Linux 俄罗斯方块

## 1.函数介绍

### void readme(); //操作介绍  
### void next();	//生成下一个方块
### void setAlarm();	//设置时钟  
### void cancelAlarm();	//关闭时钟  
### void showBg();		//初始化背景   
### void blocksInit();	//初始化图形   
### void debugBlock();	//测试图形   
### void writeInBlock();//将图形显示在棋盘里  
### void printBlock();  //打印棋盘  
### void selectColor(int color);//随机生成方块颜色  
### void execCmd(int cmd);//控制方块移动方向  
### void cancelState(int state);//删除方块上一个状态  
### int judgeMove(int cmd);		//判断cmd方向是否能移动  
### void lastInBlock(); 	//判断方块是否已经落到地部  
### void printNext();	//预览下一个方块  	
### int judgeChange();	//判断是否能进行形态变化  
### int judgeElimate();	//判断是否满足得分与消行的条件  
### void elimateT(int row);	//消行  
### void printScore();	//打印分数  
### int endGame();		//判断游戏是否结束，达到条件直接结束  
### void pauseGame();	//暂停游戏  

##2.遇到的问题
### (1).
