//--------------------------------------------------------------------------------------------------------------------------
//                  ;;;,
//        ,;;;;;: ,;;;:.,;..
//         :;;"`,;;;;;;"":;;					单元名称：数据结构单元
//      ..;;:  .;;,.;;:',;;"					备注信息：
//    ,;;;;;;" :;`',;",;;"						
//   :" ,;:` , `:.;;;;;'
//    ,;;;,;:;;,;;;, ,;							
//   :;;;"` `:;;;;;,;,.;.
//   '""   .;;`.;";:;;;;` ;,					
//       .;"`  ::. '   .,;;;
//      ""     ';;;;;;;;;;`
//--------------------------------------------------------------------------------------------------------------------------
#ifndef __TransrvPlat_InterfaceH__
#define __TransrvPlat_InterfaceH__
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
#include "MServiceInterActive.hpp"
//--------------------------------------------------------------------------------------------------------------------------
#pragma pack(1)
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//..........................................................................................................................
//以下是基本日志类型定义
//..........................................................................................................................
#define VIEW	0
#define INFO	1
#define WARN	2
#define ERR		3 
#define REPORT	4
//add by liuqy 20101027 for 新的监控驱动的标志及版本号状态定义 
#define DRV_STATIS_MAGIC       0xF81115
#define DRV_STATIS_VERSION      0

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//..........................................................................................................................
//以下是协议描述
//..........................................................................................................................
typedef struct												//市场请求，适用于全市场数据的请求
{
	unsigned char					MarketID;				//市场编号
} tagCcComm_MarketApply;
//..........................................................................................................................
typedef struct												//市场分段请求，适用于全市场数据分多次分段请求
{
	unsigned char					MarketID;				//市场编号
	unsigned short					ApplyOffset;			//起始序号[大部分协议指商品的序号，其他一部分则指特定数据的序号，具体情况根据请求判断]
} tagCcComm_MarketSubApply;
//..........................................................................................................................
typedef struct
{
	unsigned short					ApplyEnd;				//服务器处理的结束位置[目前处理到了这个商品或某个数据位置]
} tagCcComm_MarketSubResponse;
//..........................................................................................................................
typedef struct												//市场数据更新日期时间[用于请求更新数据]
{
	unsigned long					Date;					//更新日期
	unsigned long					Time;					//更新时间
} tagCcComm_MarketUpdateTime;
//................................................................................................................
typedef struct												//时间信息
{
	unsigned long					CurTime;				//当前市场时间{HHMMSS格式]
} tagCcComm_TimeInfo;
//--------------------------------------------------------------------------------------------------------------------------
//[2、0] 登录行情传输系统服务器

//请求：MAIN_FRAME_HEAD | tagCcComm_CertityApply
//应答：MAIN_FRAME_HEAD

typedef struct												//用户登录请求
{
	char							UserName[32];			//用户名称
	char							PassWord[16];			//用户密码
	char							MacAddr[12];			//本机网卡地址[如果存在多块网卡，则取第一块]
	unsigned long					ClientVersion;			//行情传输客户端版本号[(版本*100)<<16|编译号]
	unsigned short					ClientType;				//客户端类型[对应服务管理的的产品类型编号]
	unsigned char					MarketId;				//登录市场
	char							Reserved[33];			//保留
} tagCcComm_CertityApply;

//注意：
//该请求是采用DES进行加密的，具体密码请向相关人员索取。<2009-1-21 Lumy更改>
//--------------------------------------------------------------------------------------------------------------------------
//[2、1]请求市场信息[对于所有市场全部统一]

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketApply
//应答：MAIN_FRAME_HEAD | 市场信息
//注意：市场信息 = tagCcComm_MarketInfoHead | tagCcComm_MarketInfoDetail | tagCcComm_MarketInfoDetail | ...
//结构tagCcComm_MarketInfoDetail的数量由tagCcComm_MarketInfoHead指定（下同）

typedef struct												//市场信息头部结构
{
	unsigned char					MarketID;				//市场编号
	unsigned char					MarketStatus;			//市场状态[0表示初始化 1表示行情中]
	unsigned long					Date;					//市场日期[YYYYMMDD格式]
	unsigned long					Time;					//市场时间[HHMMSS格式]
	unsigned short					WareCount;				//商品数量[包括指数和个股等所有的商品]
	unsigned char					KindCount;				//类别数量
	unsigned short					Periods[4][2];			//交易时间段描述[最多支持4个时段，每个时段纪录开始和结束时间，精确到分钟，HH * 60 + MM格式]
	unsigned char					PeriodsCount;			//交易时间段
	char							TimeZoom;				//该市场目前所处时区[中国属于+8区]
	unsigned short					DelayTime;				//行情延时时间[单位：秒]
	char							CheckCode [16];			//市场校验码（不是原来单一的码表校验码）<2009-1-21 Lumy更改>
} tagCcComm_MarketInfoHead;

//注意：
//市场校验码 = 码表校验码 + 市场信息（包括所有类别信息）特殊处理后的校验码
//市场信息（包括类别信息）特殊处理 ：即将以下字段在校验时置0，然后进行校验
//unsigned char					MarketStatus;			//市场状态[0表示初始化 1表示行情中]
//unsigned long					Time;				//市场时间[HHMMSS格式]
//char							CheckCode [16];		//市场校验码

//校验码相加参照如下算法：
//开辟一块内存空间 = 整个码表的长度 + 市场信息（包括所有类别信息）的长度，将处理后的市场信息与码表拷贝到该空间中，然后进行MD5校验
//这样可以使不论码表变化还是市场信息变化，三级以上的系统也能够正常进行初始化了。
//传输驱动在与上层连接断开、重新连接成功后，首先请求市场信息，然后比较内存中原有的市场信息与新请求的市场信息校验码是否一致，如果不一致，需要重新请求码表、权证、国债、全市场指数数据和全市场个股数据等等信息，并且通知传输平台断开与下一级本市场的连接。如果一致，则直接请求全市场指数数据和全市场个股数据。

typedef struct
{
	char							KindName[8];			//类别的名称
	unsigned char					PriceRate;				//该类别中价格放大倍数[10的多少次方]
	unsigned short					LotSize;				//该类别中"手"比率
	unsigned short					WareCount;				//该类商品的数量
} tagCcComm_MarketInfoDetail;
//--------------------------------------------------------------------------------------------------------------------------
//[2、2]请求上海证券市场L1、L2、深圳证券市场L1、周边市场、上海黄金的名称代码表

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_ShNameTable | tagCcComm_ShNameTable | ...
//注意：该请求必须支持分段请求，在码表过大、服务器发送缓冲过小、通讯带宽过小的情况下仍然能够请求所有的码表。（下同）

typedef struct
{
	unsigned short					Serial;					//商品序号
	unsigned char					Type;					//该商品类别[对应MarketInfo中的类别]
	char							Code[6];				//商品代码
	char							Name[8];				//商品名称
	char							PreName[4];				//深交所前缀[仅深圳市场，上海市场保留]
} tagCcComm_ShNameTable;
//--------------------------------------------------------------------------------------------------------------------------
//[2、3]请求上海证券市场L1、上海L2、深圳证券市场L1、深圳L2国债利息数据

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_Gzlx | tagCcComm_Gzlx | ...
//注意：该请求必须支持分段请求

typedef struct
{
	char							Code[6];				//商品代码<2009-1-21 Lumy更改>
	unsigned long					StartDate;				//上海[计息日期]深圳[起息日期]，YYYYMMDD格式
	unsigned long					EndDate;				//上海[计息天数]深圳[到期日期,YYYYMMDD格式]
	unsigned long					Yjlx;					//应计利息[放大10000倍]
	unsigned long					Pmlx;					//票面利息[放大10000倍]
} tagCcComm_Gzlx;
//--------------------------------------------------------------------------------------------------------------------------
//[2、4]请求上海证券市场L1、上海L2、深圳证券市场L1、深圳L2权证信息数据

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_Qzxx | tagCcComm_Qzxx | ...
//注意：该请求必须支持分段请求

typedef struct
{
	char							Code[6];				//商品代码<2009-1-21 Lumy更改>
	char							XqCode[6];				//行权代码
	char							BdCode[6];				//标的证券代码
	char							Kind;					//权证类型['C'认购 'P'认沽]
	unsigned __int64				BlanceVolume;			//权证流通股数[股]
	unsigned __int64				TotalVolume;			//权证发行总量[股]
	char							JsMode;					//结算方式["C"表示现金结算方式，"S"表示证券给付结算方式]
	char							XqMode;					//行权方式['A'美式权证行权 'E'欧式权证行权 'B'百慕大式权证行权]
	unsigned long					JsPrice;				//结算价格[放大1000倍]
	unsigned long					XqPrice;				//行权价格[放大1000倍]
	unsigned long					XqScale;				//行权比例[放大10000倍]
	unsigned long					OnDate;					//权证上市日期[YYYYMMDD格式]
	unsigned long					DeDate;					//权证到期日期[YYYYMMDD格式]
	unsigned long					QonDate;				//行权起始日期[YYYYMMDD格式]
	unsigned long					QdeDate;				//行权截止日期(上海表示结算价使用日期)[YYYYMMDD格式]
	unsigned long					UpdateDate;				//本条纪录更新日期[YYYYMMDD格式]
	unsigned long					UpdateTime;				//本条纪录更新时间[HHMMSS格式]
} tagCcComm_Qzxx;
//--------------------------------------------------------------------------------------------------------------------------
//[2、5]请求深圳证券市场L1、深圳证券市场L2当日基金净值[注意：仅深圳证券市场，上海没有该数据]

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_SZJjjz | tagCcComm_SZJjjz | ...
//注意：该请求必须支持分段请求

typedef struct
{
	char							Code[6];				//商品代码<2009-1-21 Lumy更改>
	unsigned long					Price;					//基金净值[放大10000倍]	
} tagCcComm_SZJjjz;
//--------------------------------------------------------------------------------------------------------------------------
//[2、6]请求上海证券市场L1、上海L2、深圳证券市场L1、深圳L2、香港证券市场、国外指数全市场指数快照数据

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_IndexData | tagCcComm_IndexData | ...
//注意：该请求必须支持分段请求

typedef struct
{
	char							Code[6];				//商品代码<2009-1-21 Lumy更改>
	unsigned long					High;					//最高指数[* 放大倍数]
	unsigned long					Open;					//开盘指数[* 放大倍数]
	unsigned long					Low;					//最低指数[* 放大倍数]
	unsigned long					Close;					//昨收指数[* 放大倍数]
	unsigned long					Now;					//最新指数[* 放大倍数]
	double							Amount;					//总成交金额[元]
	unsigned __int64				Volume;					//总成交量[股??]
	//2012-7-12 pdkui  查证下来，上海L1、上海L2、深圳L1、深圳L2中，指数的总成交量，采用的是'手'。而不是'股'.
	/*
	记录如下:
	1, 上海交易所的指数，发布的是'手'。包括show2003和L2流。
	2, 深圳交易所的指数，发布的是'股',包括sjshq和L2流。L1和L2的源驱动，都除以了100。
	也就是说，真正在传输中的数据，指数的成交量是'手'。故更新此处注释.
	*/

} tagCcComm_IndexData;
//--------------------------------------------------------------------------------------------------------------------------
//[2、7]请求上海证券市场L1、深圳证券市场L1、周边市场(仅有个股)个股快照数据

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_StockData5 | tagCcComm_StockData5 | ...
//注意：该请求必须支持分段请求

typedef struct
{
	unsigned long					Price;					//委托价格[* 放大倍数]
	unsigned __int64				Volume;					//委托量[股]
} tagCcComm_BuySell;

typedef struct
{
	char							Code[6];				//商品代码<2009-1-21 Lumy更改>
	unsigned long					High;					//最高价格[* 放大倍数]
	unsigned long					Open;					//开盘价格[* 放大倍数]
	unsigned long					Low;					//最低价格[* 放大倍数]
	unsigned long					Close;					//昨收价格[* 放大倍数]
	unsigned long					Now;					//最新价格[* 放大倍数]
	double							Amount;					//总成交金额[元]
	unsigned __int64				Volume;					//总成交量[股]
	unsigned long					Records;				//成交笔数
	unsigned long					HighLimit;				//涨停价格[* 放大倍数]，注意：上海L1市场源头没有，由源驱动进行计算<2009-1-21 Lumy更改>
	unsigned long					LowLimit;				//跌停价格[* 放大倍数] ，注意：上海L1市场源头没有，由源驱动进行计算<2009-1-21 Lumy更改>
	tagCcComm_BuySell				Buy[5];					//买盘
	tagCcComm_BuySell				Sell[5];				//卖盘
	unsigned long					Voip;					//基金模拟净值、权证溢价[放大1000倍][ETF、深圳LOF、权证该值有效]
} tagCcComm_StockData5;


//[2，8]请求扩展名称代码表信息
//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_NameTableEx | tagCcComm_NameTableEx

typedef struct
{
	char							Code[6];				//名称代码表
	unsigned short					MKind;					//币种
	unsigned char					CType;					//详细类别，委托使用的分类
	unsigned char					BType;					//所属类别，A，B股等
	unsigned char					SFlag;					//停牌标记，是否可交易，需要推送
	unsigned char					PLimit;					//涨、跌停是否有效
	unsigned long					Worth;					//每股面值，*放大倍率10000
	unsigned long					ExTts;					//交易单位
	unsigned long					MaxExVol;				//最大交易数量
	unsigned long					MinExVol;				//最小交易数量
} tagCcComm_NameTableEx;


//[2,9]推送扩展名称代码信息的变化
//推送：MAIN_FRAME_HEAD | tagCcComm_TimeInfo | 变化数据
//变化数据 = 数据类型 + 代码 + 数据
//代码	= char[6]
//数据类型 = unsigned char
//类型 0 = 停牌标记(unsigned char)
//类型 1 = 最大交易数量(unsigned long)
//类型 2 = 最小交易数量(unsigned long)
//类型 3 = 每股面值(unsigned long)
//类型 4 = 交易单位(unsigned long)
typedef struct				//pdkui 2014-8-7
{
	char			Code[6];
	unsigned char	SFlag;
}tagCcComm_StopFlag;


//[2、10]请求上海证券市场L1、L2债券指数估值

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_ShZJGZ | tagCcComm_ShZJGZ | ...

typedef struct
{
	char						Code[6];				//商品代码
	unsigned long				BondValuation;			//债券估值 *放大倍率10000
} tagCcComm_ShZJGZ;

//--------------------------------------------------------------------------------------------------------------------------
//[2、11] 香港板块指数快照数据
//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD|tagCcComm_MarketSubResponse| tagCcComm_HkIndexData| tagCcComm_HkIndexData | ...

typedef struct
{
	char						Code[6];				// 商品代码
	unsigned long				High;					// 最高价格[* 放大倍数]
	unsigned long				Open;					// 开盘价格[* 放大倍数]
	unsigned long				Low;					// 最低价格[* 放大倍数]
	unsigned long				Close;					// 昨收价格[* 放大倍数]
	unsigned long				Now;					// 最新价格[* 放大倍数]
	double					Amount;						// 总成交金额[元]
	unsigned __int64			Volume;					// 总成交量[股]
	double					dTotalLTG;					// 流通股本	权息需要该字段
	double					dTotalZGB;					// 总股本	保留
	double					dTotalValue;				// 总市值
	double					dMarketValue;				// 流通市值
	unsigned short				ushTotal;				// 计算总家数[指数、初始化时生成]
	unsigned short				ushRise;				// 上涨家数[指数、盘中不断变化]
	unsigned short				ushDown;				// 下跌家数[指数、盘中不断变化]
	unsigned long				ulEarnRate;				// 市盈率 [×100]
	long						lNetRate;				// 市净率 [×100]
	short					shAvgZF;					// 平均幅度[单位%，×100；如果无单位，则乘10000]
	unsigned __int64		uiVBuy;						// 总委买量[单位：股]
	unsigned __int64		uiVSell;					// 总委卖量[单位：股] 
	char				szUpCode[7];					// 领涨股代码＋领涨股市场编号
} tagCcComm_HkIndexData;

//--------------------------------------------------------------------------------------------------------------------------
//[2、17]客户端请求推送市场数据[对应于所有市场]

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketApply
//应答：MAIN_FRAME_HEAD

//注意：传输系统在收到该请求后就向该客户端推送全市场数据
//--------------------------------------------------------------------------------------------------------------------------
//[2、50]推送上海证券市场L1、深圳证券市场L1、周边市场、国外指数数据<2009-1-21 Lumy更改>

//推送：MAIN_FRAME_HEAD | tagCcComm_TimeInfo | 行情数据块 | 行情数据块 | 行情数据块 | …
//行情数据块 = 行情数据块类型 + 行情数据块具体类型对应的结构（参见下面详细说明）
//行情数据块类型 = unsigned char

//类型 0: 市场信息 =          tagCcComm_MarketStatusInfo
//类型 1：单个指数数据 =	  	tagCcComm_IndexData
//类型 2: 单个商品快照数据 =	tagCcComm_StockData5
//类型 3：深交所前缀变更 =	tagCcComm_SzPerNameChg
//类型 4: 周边市场贡献度 = [2,57]板块贡献度值
//类型 5: 周边市场涨跌幅度 = [2,58]板块涨跌幅度排名

typedef struct
{
	unsigned char					MarketID;				//市场编号
	unsigned long					MarketDate;				//市场日期[YYYYMMDD格式]
	unsigned long					MarketTime;				//市场时间[HHMMSS格式]
	unsigned char					MarketStatus;			//市场状态[0初始化状态 1行情中状态]
} tagCcComm_MarketStatusInfo;

//注意：
//源驱动在行情日期、行情时间、市场状态任何一个发生变化时向下级推送该市场状态信息。
//传输驱动在本驱动确定需要进行完全初始化时需要断开与下级本市场的连接（原来为推送该市场状态信息）。所谓需要完全初始化，就是本驱动与上层连接断开，且重新连接到新的上层，但获取的市场信息中校验码与上次完全不一样，需要重新请求码表、重新分配内存结构等，这是成为完全初始化。<2009-1-21 Lumy更改>

typedef struct
{
	char							Code[6];				//商品代码<2009-1-21 Lumy更改>
	char							PreName[4];				//商品前缀
} tagCcComm_SzPerNameChg;
//--------------------------------------------------------------------------------------------------------------------------
//[2、13]请求上海证券市场L2、深圳L2个股快照数据

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_StockData10 | tagCcComm_StockData10 | ...
//注意：该请求必须支持分段请求

typedef struct
{
	char							Code[6];				//商品代码<2009-1-21 Lumy更改>
	unsigned long					High;					//最高价格[* 放大倍数]
	unsigned long					Open;					//开盘价格[* 放大倍数]
	unsigned long					Low;					//最低价格[* 放大倍数]
	unsigned long					Now;					//最新价格[* 放大倍数]
	double							Amount;					//总成交金额[元]
	unsigned __int64				Volume;					//总成交量[股]
	unsigned long					Records;				//成交笔数
	unsigned long					VBuyPrice;				//委买均价[* 放大倍数]
	unsigned __int64				VBuyVolume;				//委买总量[股]
	unsigned long					VSellPrice;				//委卖均价[* 放大倍数]
	unsigned __int64				VSellVolume;			//委卖总量[股]
	tagCcComm_BuySell				Buy[10];				//买盘
	tagCcComm_BuySell				Sell[10];				//卖盘
	unsigned long					Voip;					//基金模拟净值[放大1000倍][ETF、深圳LOF该值有效]
	unsigned long					Close;					//昨收价格[* 放大倍数]
	unsigned long					HighLimit;				//涨停价格[* 放大倍数]
	unsigned long					LowLimit;				//跌停价格[* 放大倍数]
} tagCcComm_StockData10;
//--------------------------------------------------------------------------------------------------------------------------
//[2、19]请求上海证券市场L2、委托排名数据

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_OrderRankingData | tagCcComm_OrderRankingData | ...
//注意：该请求必须支持分段请求

typedef struct
{
	char							Code[6];				//商品代码<2009-1-21 Lumy更改>
	unsigned __int64				VBuySellVol;			//委托总量[股]
	unsigned __int64				TradeVol;				//交易总量[股]
	double							TradeAmount;			//交易总金额[元]
} tagCcComm_OrderRankingDetail;

typedef struct
{
	unsigned char					WareType;				//商品类别[将L2商品类别转换成为MarketInfo中的商品类别] <2009-1-21 Lumy更改>
	unsigned char					Side;					//交易方向[0 买 1 卖]
	tagCcComm_OrderRankingDetail	Records[10];			//队列数据
} tagCcComm_OrderRankingData;
//--------------------------------------------------------------------------------------------------------------------------
//[2、21]请求上海证券市场L2、即时撤单数据

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_CancelRankData | tagCcComm_CancelRankData | ...
//注意：该请求必须支持分段请求

typedef struct
{
	char							Code[6];				//商品代码<2009-1-21 Lumy更改>
	unsigned long					Time;					//委托时间[HHMMSS格式]
	unsigned long					Price;					//委托价格[* 放大倍数]
	unsigned __int64				Volume;					//委托数量[股]
} tagCcComm_CancelRankDetail;

typedef struct
{
	unsigned char					WareType;				//商品类别[将L2商品类别转换成为MarketInfo中的商品类别] <2009-1-21 Lumy更改>
	unsigned char					Side;					//交易方向[0 买 1 卖]
	tagCcComm_CancelRankDetail		Records[10];			//队列数据
} tagCcComm_CancelRankData;
//--------------------------------------------------------------------------------------------------------------------------
//[2、23]请求上海证券市场L2、累计撤单数据

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_TotalCancelRank | tagCcComm_TotalCancelRank | ...
//注意：该请求必须支持分段请求

typedef struct
{
	char							Code[6];				//商品代码<2009-1-21 Lumy更改>
	unsigned __int64				Volume;					//撤单总量[股]
} tagCcComm_TotalCancelDetail;

typedef struct
{
	unsigned char					WareType;				//商品类别[将L2商品类别转换成为MarketInfo中的商品类别] <2009-1-21 Lumy更改>
	unsigned char					Side;					//交易方向[0 买 1 卖]
	tagCcComm_TotalCancelDetail		Records[10];			//队列数据
} tagCcComm_TotalCancelRank;
//--------------------------------------------------------------------------------------------------------------------------
//[2、42]推送上海L2行情数据

//推送：MAIN_FRAME_HEAD | tagCcComm_TimeInfo | 行情数据块 | 行情数据块 | 行情数据块 | …
//行情数据块 = 行情数据块类型 + 行情数据块具体类型对应的结构（参见下面详细说明）
//行情数据块类型 = unsigned char

//类型 0: 市场信息 =            tagCcComm_MarketStatusInfo
//类型 2：单个指数数据 =	  	tagCcComm_IndexData
//类型 3: 逐笔成交 =			tagCcComm_ShL2TransData
//类型 4：委托排名 =			tagCcComm_OrderRankingData
//类型 5：即时撤单 =			tagCcComm_CancelRankData
//类型 6：累计撤单 =			tagCcComm_TotalCancelRank
//类型 7：委托队列 =    (L2)	tagCcComm_OrderQueueHead | tagCcComm_OrderVolDetail | tagCcComm_OrderVolDetail | ...
//                      (L2fast)tagCcComm_OrderQueueHeadL2Fast | tagCcComm_OrderPrice | tagCcComm_OrderVolDetail | ...  | tagCcComm_OrderPrice | tagCcComm_OrderVolDetail ...    
//类型 8：盘前虚拟价格 =	  	tagCcComm_VirtualPrice
//类型 25 单个商品快照数据 =	tagCcComm_StockData10

typedef struct
{
	char							Code[6];				//商品代码<2009-1-21 Lumy更改>
	unsigned long					TransNo;				//交易序号
	unsigned long					TransTime;				//交易时间[HHMMSS格式]
	unsigned long					TransPrice;				//交易价格[* 放大倍数]
	unsigned __int64				TransVolume;			//成交量[股]
} tagCcComm_ShL2TransData;

typedef struct
{
	char							Code[6];				//商品代码<2009-1-21 Lumy更改>
	unsigned long					virtualprice;			//虚拟价格[* 放大倍数]
	unsigned __int64				virtualvolume;			//虚拟匹配量[股]
	unsigned __int64				buyvirtualvol;			//买方虚拟匹配量[股]
	unsigned __int64				sellvirtualvol;			//卖方虚拟匹配量[股]
} tagCcComm_VirtualPrice;

typedef struct
{
	char							Code[6];				//商品代码
	unsigned char					Side;					//买卖方向[0 买 1 卖]
	unsigned long					Price;					//委托价格[* 放大倍数]
	unsigned short					VolCount;				//委托分量数量[注意：如果该值超过50，那么后面接着的分量只能为50个]
} tagCcComm_OrderQueueHead;

typedef struct
{
	char							Code[6];				//商品代码
	unsigned char					Side;					//买卖方向[0 买 1 卖]
	unsigned char					PriceCount;				//价位数
} tagCcComm_OrderQueueHeadL2Fast;

typedef struct
{
	unsigned long					Price;					//委托价格[* 放大倍数]
	unsigned short					VolCount;				//委托分量数量[注意：如果该值超过200，那么后面接着的分量只能为200个]
} tagCcComm_OrderPrice;

typedef struct
{
	unsigned __int64				Volume;					//分量数量
} tagCcComm_OrderVolDetail;

//--------------------------------------------------------------------------------------------------------------------------
//[2、27]请求香港证券市场、名称代码表

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_HkNameTable | tagCcComm_HkNameTable | ...
//注意：该请求必须支持分段请求

typedef struct
{
	unsigned short					Serial;					//商品序号
	unsigned char					Type;					//该商品类型
	char							Code[6];				//商品代码
	char							EnglishName[16];		//英文名称
	char							ChineseName[16];		//中文名称
	char							SName[8];				//中文简称
	unsigned long					ListDate;				//上市日期
	unsigned long					OffDate;				//下市日期
	unsigned long					SpreadValue;			//差价[* 放大倍数]
	unsigned long					LotSize;				//该商品的"手"比率
	unsigned char					Currency;				//货币种类
	unsigned char					PriceTb;				//使用哪张价位表
	float							NrRate;					//无风险利率
} tagCcComm_HkNameTable;
//--------------------------------------------------------------------------------------------------------------------------
//[2、28]请求香港证券市场、权证信息数据

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_QzxxHK | tagCcComm_QzxxHK | ...
//注意：返回结果中的tagCcComm_MarketUpdateTime结构，表示服务器端本市场权证信息的更新时间。
//注意：该请求必须支持分段请求

typedef struct
{
	char							Code[6];				//商品代码
	char							XqCode[6];				//行权代码
	char							BdCode[6];				//标的证券代码
	char							Kind;					//权证类型['C'认购 'P'认沽，牛熊证为'c'、'p']
	unsigned __int64				BlanceVolume;			//权证流通股数[股]
	unsigned __int64				TotalVolume;			//权证发行总量[股]
	char							JsMode;					//结算方式["C"表示现金结算方式，"S"表示证券给付结算方式]
	char							XqMode;					//行权方式['A'美式权证行权 'E'欧式权证行权 'B'百慕大式权证行权,牛熊证行权方式'N'或者'R']
	unsigned long					JsPrice;				//结算价格[放大1000倍]
	unsigned long					XqPrice;				//行权价格[放大1000倍]
	unsigned long					XqScale;				//行权比例[放大1000倍]
	unsigned long					CallPrice;				//牛熊证强制回收价[放大1000倍]
	unsigned __int64				JhVolume;				//权证街货量 
	unsigned long					OnDate;					//权证上市日期[YYYYMMDD格式]
	unsigned long					DeDate;					//权证到期日期[YYYYMMDD格式]
	unsigned long					QonDate;				//行权起始日期[YYYYMMDD格式]
	unsigned long					QdeDate;				//行权截止日期(上海表示结算价使用日期)[YYYYMMDD格式]
	unsigned long					UpdateDate;				//本条纪录更新日期[YYYYMMDD格式]
	unsigned long					UpdateTime;				//本条纪录更新时间[HHMMSS格式]
} tagCcComm_QzxxHK;
//--------------------------------------------------------------------------------------------------------------------------
//[2、29]请求香港证券市场、关联个股数据

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | 某商品关联个股数据 | 某商品关联个股数据 | ...
//其中：某商品关联个股数据 = tagCcComm_HKRelateTable | tagCcComm_HKRelateCode | tagCcComm_HKRelateCode | ...
//注意：该请求必须支持分段请求

typedef struct
{
	char							Code[6];				//商品代码<2009-1-21 Lumy更改>
	unsigned short					Count;					//关联商品的数量[最多500个，即后面tagCcComm_HKRelateCode结构的数量]
} tagCcComm_HKRelateTable;

typedef struct
{
	unsigned char					MarketID;				//市场编号<2009-1-21 Lumy更改>
	char							Code[6];				//商品代码<2009-1-21 Lumy更改>
} tagCcComm_HKRelateCode;
//--------------------------------------------------------------------------------------------------------------------------
//[2、30]请求香港证券市场、经纪商列表数据

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_HKBrokerTable | tagCcComm_HKBrokerTable | ...
//注意：该请求必须支持分段请求

typedef struct
{
	unsigned short 					BrokerSerial;			//序号
	char							Name[8];				//经纪商简称
	char							FullName[32];			//经纪商全称
	unsigned short       			StartNumber;			//起始经纪号码
	unsigned short 					EndNumber;				//结束经纪号码
	unsigned long					UpdateDate;				//本条纪录更新日期[YYYYMMDD格式]
	unsigned long					UpdateTime;				//本条纪录更新时间[HHMMSS格式]
} tagCcComm_HKBrokerTable;
//--------------------------------------------------------------------------------------------------------------------------
//[2、31]请求香港证券市场、全市场个股快照数据(5档数据)[逐步被2,32协议取代]

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_HKStockData | tagCcComm_HKStockData | ...
//注意：该请求必须支持分段请求
typedef struct
{
	unsigned __int64				Volume;					//委托数量[股]
	unsigned short					Records;				//委托笔数
} tagCcComm_HKBuySell;

#ifdef __3X_HK__
typedef struct
{
	char							Code[6];				//商品代码<2009-1-21 Lumy更改>
	unsigned long					High;					//最高价格[* 放大倍数]
	unsigned long					Open;					//开盘价格[* 放大倍数]
	unsigned long					Low;					//最低价格[* 放大倍数]
	unsigned long					Close;					//昨收价格[* 放大倍数]
	unsigned long					Now;					//最新价格[* 放大倍数]
	double							Amount;					//总成交金额[元]
	unsigned __int64				Volume;					//总成交量[股]
	unsigned long					VBuyPrice;				//委买价格[* 放大倍数]
	tagCcComm_HKBuySell				VBuy[5];				//5档买盘
	unsigned long					VSellPrice;				//委卖价格[* 放大倍数]
	tagCcComm_HKBuySell				VSell[5];				//5档卖盘
} tagCcComm_HKStockData;
#else
typedef struct
{
	char							Code[6];				//商品代码<2009-1-21 Lumy更改>
	unsigned long					High;					//最高价格[* 放大倍数]
	unsigned long					Open;					//开盘价格[* 放大倍数]
	unsigned long					Low;					//最低价格[* 放大倍数]
	unsigned long					Close;					//昨收价格[* 放大倍数]
	unsigned long					Now;					//最新价格[* 放大倍数]
	double							Amount;					//总成交金额[元]
	unsigned __int64				Volume;					//总成交量[股]
	unsigned long					VBuyPrice;				//委买价格[* 放大倍数]
	tagCcComm_HKBuySell				VBuy[5];				//5档买盘
	unsigned long					VSellPrice;				//委卖价格[* 放大倍数]
	tagCcComm_HKBuySell				VSell[5];				//5档卖盘
	unsigned long					Voip;					//权证溢价
} tagCcComm_HKStockData;
#endif

//--------------------------------------------------------------------------------------------------------------------------
//[2、32]请求香港证券市场、全市场个股快照数据

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_HKStockDataEx | tagCcComm_HKStockDataEx | ...
//注意：该请求必须支持分段请求
typedef struct
{
	char							Code[6];				//商品代码<2009-1-21 Lumy更改>
	unsigned long					High;					//最高价格[* 放大倍数]
	unsigned long					Open;					//开盘价格[* 放大倍数]
	unsigned long					Low;					//最低价格[* 放大倍数]
	unsigned long					Close;					//昨收价格[* 放大倍数]
	unsigned long					Now;					//最新价格[* 放大倍数]
	double							Amount;					//总成交金额[元]
	unsigned __int64				Volume;					//总成交量[股]
	unsigned long					VBuyPrice;				//委买价格[* 放大倍数]
	tagCcComm_HKBuySell				VBuy[10];				//5档买盘
	unsigned long					VSellPrice;				//委卖价格[* 放大倍数]
	tagCcComm_HKBuySell				VSell[10];				//5档卖盘
	unsigned long					Voip;					//权证溢价
} tagCcComm_HKStockDataEx;
//--------------------------------------------------------------------------------------------------------------------------
//[2、33]请求香港证券市场、名称代码表 
//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_HkIdxNameTable | tagCcComm_HkIdxNameTable | ...
//注意：该请求必须支持分段请求
typedef struct
{
	unsigned short					Serial;					//商品序号
	unsigned char					Type;					//该商品类型
	char							Code[6];				//商品代码
	char							EnglishName[16];		//英文名称
	char							ChineseName[16];		//中文名称
	char							SName[8];				//中文简称
	unsigned long					ListDate;				//上市日期
	unsigned long					OffDate;				//下市日期
	unsigned long					SpreadValue;			//差价[* 放大倍数]
	unsigned long					LotSize;				//该商品的"手"比率
	unsigned char					Currency;				//货币种类
	unsigned char					PriceTb;				//使用哪张价位表
	float							NrRate;					//无风险利率
	unsigned char					uchFlags;				//特别标志，0x01:热点指数(该标志来源于信龙的板块配置文件) 
} tagCcComm_HkIdxNameTable;
//--------------------------------------------------------------------------------------------------------------------------
//[2、35]请求香港证券市场、全市场经济排位数据

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | 某商品经济排位数据 | 某商品经济排位数据 | ...
//其中：某商品经济排位数据 = tagCcComm_HKJjpwDataRequest | tagCcComm_HKJjpwDetail | tagCcComm_HKJjpwDetail | ...
//注意：该请求必须支持分段请求

typedef struct
{
	char							Code[6];				//商品代码<2009-1-21 Lumy更改>
	unsigned char					BCount;					//买方档数
	unsigned char					ACount;					//卖方档数
} tagCcComm_HKJjpwDataRequest;

typedef struct
{
	char							Code[6];				//商品代码<2009-1-21 Lumy更改>
	bool							Side;					//买卖方向[true 买 false 卖]
	unsigned char					Count;					//有效数量[最多40档]
} tagCcComm_HKJjpwDataPush;

typedef struct
{
	short							S_Broker;				//S_Broker<0，表示买卖档位。S_Broker>=0，表示经纪号码
} tagCcComm_HKJjpwDetail;
//--------------------------------------------------------------------------------------------------------------------------
//[2、37]请求香港价位表

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_PriceTable | tagCcComm_PriceTable | tagCcComm_PriceTable
//注意后接三个tagCcComm _PriceTable
//注意：该请求必须支持分段请求

typedef struct
{
	unsigned long					PriceTo;				//价格去向, 隐含3位小数
	unsigned short					SpreadValue;			//前一个价格去向（或者价格来源） 及当前价格去向, 隐含3位小数
} tagCcComm_PriceItem; 

typedef struct
{
	char							SpreadTableCode[2];		//价位代码
	unsigned long 					PriceFrom;				//价位表的最低定单价格，隐含3位小数。
	unsigned short					ItemsNumber;			//项目列表中的项目数量。1~52
   	tagCcComm_PriceItem				PriceItem[52];			//价位方向
} tagCcComm_PriceTable;
//--------------------------------------------------------------------------------------------------------------------------
//[2、38]请求香港证价特定财务数据

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | 某商品特定财务数据 |　某商品特定财务数据　| 某商品特定财务数据　|...
//注意：该请求必须支持分段请求

typedef struct
{
	char							Code[6];				//商品代码<2009-1-21 Lumy更改>
	unsigned short					PERatio;				//市盈率   隐含1位小数. 
	unsigned long					CurrentYield;			//派息率   隐含3位小数. 
	unsigned long					MaturityYield;			//证券到期收益, 隐含4位小数
	char							MaturityYieldIndi;		//指出到期收益字段的意义。N-正常, O-超值, U-负面超值, X-不能提供 
	__int64							Share_Out;				//发行股数 
	__int64							Market_Cap;				//总市值 
} tagCcomm_FinanceData;
//--------------------------------------------------------------------------------------------------------------------------
//[2、40]推送港股市场的新闻信息

//推送：MAIN_FRAME_HEAD | tagCcComm_TimeInfo | 新闻索引 | 新闻
//新闻索引 = tagComm_NewsIndex | tagComm_SecuNI | tagComm_SecuNI | ．．．
//新闻　＝　tagComm_NewsPage　| 内容（８０字节）| 内容（８０字节） |　．．．

typedef struct
{
	char							Code[6];				//商品代码<2009-1-21 Lumy更改>
} tagComm_SecuNI;

typedef struct
{
	unsigned char					NewsType;				//新闻类型
	unsigned short					NewsId;					//新闻唯一id
	unsigned char     				SecurityNumber;			//代码个数
}tagComm_NewsIndex;

typedef struct
{		
	unsigned __int64				DateTime;				//时间/日期
	unsigned char					LineNum;				//行数
}tagComm_NewsPage;

typedef struct
{
	char							LineContext[80];		//新闻内容
}tagCcomm_PGContext;
//--------------------------------------------------------------------------------------------------------------------------
//[2、41]推送港股行情数据

//推送：MAIN_FRAME_HEAD | tagCcComm_TimeInfo | 行情数据块 | 行情数据块 | 行情数据块 | …
//行情数据块 = 行情数据块类型 + 行情数据块具体类型对应的结构（参见下面详细说明）
//行情数据块类型 = unsigned char

//类型 1：单个指数数据 =  tagCcComm_IndexData
//类型 2：单个商品经济排位数据 =  tagCcComm_HKJjpwDataPush | tagCcComm_HKJjpwDetail | tagCcComm_HKJjpwDetail | …
//类型 3：单个商品交易明细数据 =  tagCcComm_HKTradeData
//类型13：单个商品全部快照数据 =  tagCcComm_HKStockData
//类型14：市场信息 =  tagCcComm_MarketStatusInfo<2009-1-21 Lumy更改>

//类型 15：单个商品全部快照数据 =  tagCcComm_HKStockDataEx, 取代类型13
typedef struct
{
	char							Code[6];				//商品代码<2009-1-21 Lumy更改>
	unsigned long					Price;					//交易价格[* 放大倍数]
	unsigned __int64				Volume;					//交易数量[股]
	unsigned long					TradeTime;				//交易时间[HHMMSS格式]
	unsigned char					TradeType;				//公共交易类型
} tagCcComm_HKTradeData;
//--------------------------------------------------------------------------------------------------------------------------
//[2、43]港股期货请求码表

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_HKFNameTable | tagCcComm_HKFNameTable | ...
//注意：该请求必须支持分段请求

typedef struct
{
	unsigned short					Serial;					//商品序号
	unsigned char					Type;					//该商品类别[对应MarketInfo中的类别]
	char							Code[6];				//商品代码
	char							CHName[16];				//中文全称
	char							EnName[16];				//英文简称
	char							TargetCode[6];			//标的指数的代码，无标的指数时填空
	unsigned char					TargetMarketID;			//标的指数市场编号(目前，对于新华25指数填入5，恒生指数填入2，0xFF表示未知)
	unsigned long					ContractMultiplier;		//合约乘数
	unsigned long					Settle;					//今结算价格
	__int64							OpenInterest;			//今持仓量(单位：合约)
	unsigned long					Close;					//昨收价格
	unsigned long					PreSettle;				//昨结算价格
	__int64							PreOpenInterest;		//昨持仓量(单位：合约)
} tagCcComm_HKFNameTable;
//--------------------------------------------------------------------------------------------------------------------------
//[2、44]港股期货请求动态数据

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_HKFObjectData | tagCcComm_HKFObjectData | ...
//注意：该请求必须支持分段请求

typedef struct
{
	char							Code[6];				//商品代码<2009-1-21 Lumy更改>
	unsigned long					sequenceID;				//包序号(每个商品独立编号)
	unsigned long					High;					//最高价格[* 放大倍数]
	unsigned long					Open;					//开盘价格[* 放大倍数]
	unsigned long					Low;					//最低价格[* 放大倍数]
	unsigned long					Close;					//昨收价格[* 放大倍数]
	unsigned long					Now;					//最新价格[* 放大倍数]
	unsigned long					CurVolume;				//成交现量
	double							Amount;					//总成交金额[元]
	unsigned __int64				Volume;					//总成交量[股]
	unsigned long					EstimatedSettlement;	//可能结算价格
	unsigned long					Cop;					//预开盘价格[* 放大倍数]
	tagCcComm_BuySell				VBuy[5];				//5档买盘
	tagCcComm_BuySell				VSell[5];				//5档卖盘
} tagCcComm_HKFObjectData;
//--------------------------------------------------------------------------------------------------------------------------
//[2、45]港股期货请求静态数据(盘前初始化时不请求，因为数据已经包含在码表中,收盘时要请求)
				 
//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_HKFStaticData | tagCcComm_HKFStaticData | ...
//注意：该请求必须支持分段请求
				 
typedef struct
{
	char							Code[6];				//商品代码<2009-1-21 Lumy更改>
	unsigned long					Close;					//昨收价格
	unsigned long					Settle;					//今结算价格
	__int64							OpenInterest;			//今持仓量(单位：合约)
	unsigned long					PreSettle;				//昨结算价格
	__int64							PreOpenInterest;		//昨持仓量(单位：合约)
} tagCcComm_HKFStaticData;

//[2、46]推送港股指数行情数据

//推送：MAIN_FRAME_HEAD | tagCcComm_TimeInfo | 行情数据块 | 行情数据块 | 行情数据块 | …
//行情数据块 = 行情数据块类型 + 行情数据块具体类型对应的结构（参见下面详细说明）
//行情数据块类型 = unsigned char
//类型 0: 市场信息 =          tagCcComm_MarketStatusInfo
//类型 1：单个指数数据 =	  	tagCcComm_HkIndexData
//类型 2: 单个商品快照数据 =	tagCcComm_StockData5
//类型 4: 港股板指市场贡献度 = [2,57]板块贡献度值
//类型 5: 港股板指市场涨跌幅度 = [2,58]板块涨跌幅度排名


//--------------------------------------------------------------------------------------------------------------------------
//[2、49]推送港股期货数据<2009-1-21 Lumy更改>

//推送：MAIN_FRAME_HEAD | tagCcComm_TimeInfo | 行情数据块 | 行情数据块 | 行情数据块 | …
//行情数据块 = 行情数据块类型 + 行情数据块具体类型对应的结构（参见下面详细说明）
//行情数据块类型 = unsigned char

//类型 0: 市场信息 =          tagCcComm_MarketStatusInfo
//类型 1：成交明细 =	  		tagCcComm_HKFTradeData
//类型 2: 五档买卖盘 =		tagCcComm_HKFBuySellData;
//类型 3：静态数据 =			tagCcComm_HKFStaticData

typedef struct
{
	char							Code[6];				//商品代码<2009-1-21 Lumy更改>
	unsigned long					sequenceID;				//包序号(每个商品独立编号)
	unsigned long					High;					//最高价格[* 放大倍数]
	unsigned long					Open;					//开盘价格[* 放大倍数]
	unsigned long					Low;					//最低价格[* 放大倍数]
	unsigned long					Close;					//昨收价格[* 放大倍数]
	unsigned long					Now;					//最新价格[* 放大倍数]
	unsigned long					CurVolume;				//成交现量
	double							Amount;					//总成交金额[元]
	unsigned __int64				Volume;					//总成交量[股]
	unsigned long					EstimatedSettlement;	//可能结算价格( 源收到UT包时填如有效值其他各值全填0,如收到TT包时本值为0 )
	unsigned long					Cop;					//预开盘价格
} tagCcComm_HKFTradeData;

typedef struct
{
	char							Code[6];				//商品代码<2009-1-21 Lumy更改>
	unsigned char					Side;					//买卖方向(0 买;1 卖)
	tagCcComm_BuySell				VBuy[5];				//5档买盘
} tagCcComm_HKFBuySellData;

//--------------------------------------------------------------------------------------------------------------------------
//[2、51]请求国外汇率、国外期货名称代码表

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_ForeignFutureNameTable| tagCcComm_ForeignFutureNameTable | ...
//注意：该请求必须支持分段请求，在码表过大、服务器发送缓冲过小、通讯带宽过小的情况下仍然能够请求所有的码表。（下同）

typedef struct
{
	unsigned short					Serial;					//商品序号
	unsigned char					Type;					//该商品类别[对应MarketInfo中的类别]
	char							PriceRate;				//外汇的放大倍率
	char							TimeZoom;				//商品所在时区
	char							Code[6];				//商品代码
	char							Name[8];				//商品名称
	unsigned short					Periods[2][2];			//该指数的行情时间段，最多支持两段,无效段起始、结束时间均段为0xFFFF
} tagCcComm_ForeignFutureNameTable;

//--------------------------------------------------------------------------------------------------------------------------
//[2、52]请求国外期货

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_ForeignFutureData | tagCcComm_ForeignFutureData | ...
typedef struct
{
    char							Code[6];				//商品代码
    unsigned long					High;                   //最高价格[* 放大倍数]
    unsigned long					Open;                   //开盘价格[* 放大倍数]
    unsigned long					Low;                    //最低价格[* 放大倍数]
    unsigned long					Close;                  //昨收价格[* 放大倍数]
    unsigned long					Now;                    //最新价格[* 放大倍数]
	unsigned __int64			    NowVolume;				//现量
    unsigned __int64           		Volume;                 //总成交量[股]
    unsigned __int64            	PreOpenInterest;        //昨日持仓量[股]
    unsigned __int64            	OpenInterest;           //持仓量[股]
	tagCcComm_BuySell				Buy;					//买入
	tagCcComm_BuySell				Sell;					//卖出
} tagCcComm_ForeignFutureData;

//--------------------------------------------------------------------------------------------------------------------------
//[2、53]推送国外期货数据

//推送：MAIN_FRAME_HEAD | tagCcComm_TimeInfo | 行情数据块 | 行情数据块 | 行情数据块 | …
//行情数据块 = 行情数据块类型 + 行情数据块具体类型对应的结构（参见下面详细说明）
//行情数据块类型 = unsigned char

//类型 0: 市场信息 =          tagCcComm_MarketStatusInfo
//类型 1：快照数据 =	  	  tagCcComm_ForeignFutureData

//--------------------------------------------------------------------------------------------------------------------------
//[2、54]请求国外汇率数据

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_ForeignExchangeData | tagCcComm_ForeignExchangeData | ...
typedef struct
{
    char							Code[6];				//商品代码
    unsigned long					High;                   //最高价格[* 放大倍数]
    unsigned long					Open;                   //开盘价格[* 放大倍数]
    unsigned long					Low;                    //最低价格[* 放大倍数]
    unsigned long					Close;                  //昨收价格[* 放大倍数]
    unsigned long					Now;                    //最新价格[* 放大倍数]
    unsigned __int64           		Volume;                 //总成交量[股]
	tagCcComm_BuySell				Buy;					//买入
	tagCcComm_BuySell				Sell;					//卖出
} tagCcComm_ForeignExchangeData;

//--------------------------------------------------------------------------------------------------------------------------
//[2、55]推送国外汇率数据

//推送：MAIN_FRAME_HEAD | tagCcComm_TimeInfo | 行情数据块 | 行情数据块 | 行情数据块 | …
//行情数据块 = 行情数据块类型 + 行情数据块具体类型对应的结构（参见下面详细说明）
//行情数据块类型 = unsigned char

//类型 0: 市场信息 =          tagCcComm_MarketStatusInfo
//类型 1：快照数据 =	  	  tagCcComm_ForeignExchangeData


//[2、56]请求国外指数名称代码表

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_ForeignIndexNameTable0 | tagCcComm_ForeignIndexNameTable0 | ...
//注意：该请求必须支持分段请求，在码表过大、服务器发送缓冲过小、通讯带宽过小的情况下仍然能够请求所有的码表。（下同）
typedef struct
{
	unsigned short					Serial;					//商品序号
	unsigned char					Type;					//该商品类别[对应MarketInfo中的类别]
	char							PriceRate;
	char							TimeZoom;				//商品所在时区
	char							Code[6];				//商品代码
	char							CName[16];				//商品中文名称
	char							EName[40];				//商品英文名称
	unsigned short					Periods[2][2];			//该指数的行情时间段，最多支持两段,无效段起始、结束时间均段为0xFFFF
} tagCcComm_ForeignIndexNameTable;

//[2，57]请求周边市场板块相关贡献度
//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | 板块1名称+板块1的贡献度值 | 板块2名称+板块2的贡献度值 | ........ | 板块n名称+板块n的贡献度值 
//板块名称 = tagCcComm_BlockInfo
//板块贡献度值 = tagCcComm_BlockContributeValue(沪深大盘) | 贡献度排名(前20) | 贡献度排名(后20)

typedef struct
{
	char							Code[6];				//板块代码
	char							Name[8];				//板块名称
} tagCcComm_BlockInfo;


typedef struct
{
	char							Code[6];				//代码,板块对大盘的贡献度时代码为大盘指数代码, 版本贡献度排名中的代码则为成分股代码
	char							MarketId;				//代码所在市场，0表示上海, 1表示深圳
	unsigned long					Price;					//现价, 放大1000倍数
	double							CtbValue;				//贡献值(涨跌幅度)
} tagCcComm_BlockContributeValue;

//[2，58]请求周边市场板块相关涨跌幅排名
//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | 板块1名称+板块1的涨跌幅度排名 (前20)| 板块2名称+板块2的涨跌幅度排名 | ........ | 板块n名称+板块n的涨跌幅度排名 
//板块名称 = tagCcComm_BlockInfo
//涨跌幅度排名 = 涨跌幅度排名(前20) | 涨跌幅度排名(后20)


//[2，59]推送深圳L2市场 //逐步被[2,65]协议取代
//推送：MAIN_FRAME_HEAD | tagCcComm_TimeInfo | 行情数据块 | 行情数据块 | 行情数据块 | …
//行情数据块 = 行情数据块类型 + 行情数据块具体类型对应的结构（参见下面详细说明）
//行情数据块类型 = unsigned char

//类型 0: 市场信息 =            tagCcComm_MarketStatusInfo
//类型 1：单个指数数据 =	  	tagCcComm_IndexData
//类型 2  单个商品快照数据 =	tagCcComm_StockData10
//类型 3: 逐笔成交 =			tagCcComm_SzL2TransData
//类型 4: 逐笔委托 =			tagCcComm_SzL2OderData
//类型 5: 委托队列 =			tagCcComm_OrderQueueHeadSzL2 | tagCcComm_OrderVolDetail | tagCcComm_OrderVolDetail | ...
//类型 6: 前缀变化 =			tagCcComm_SzPerNameChg
//类型 7: 国债利息 =			tagCcComm_Gzlx						

typedef struct
{
	char							Code[6];				//商品代码
	char							TranType;				//成交类别
	unsigned long					TransNo;				//交易序号
	unsigned long					TransTime;				//交易时间[HHMMSS格式]
	unsigned long					TransPrice;				//交易价格[* 放大倍数]
	unsigned __int64				TransVolume;			//成交量[股]
} tagCcComm_SzL2TransData;

typedef struct
{
	char							Code[6];				//商品代码
	char							OrderType;				//委托类别
	unsigned long					OrderNo;				//委托序号
	unsigned long					OrderTime;				//委托时间[HHMMSS格式]
	unsigned long					OrderPrice;				//委托价格[* 放大倍数]
	unsigned __int64				OrderVolume;			//委托量[股]
} tagCcComm_SzL2OderData;


typedef struct
{
	char							Code[6];				//商品代码
	unsigned char					Side;					//买卖方向[0 买 1 卖]
	unsigned long					Price;					//委托价格[* 放大倍数]
	unsigned short					TVolCount;				//委托总笔
	unsigned short					VolCount;				//揭示委托分量数量, 最大200
} tagCcComm_OrderQueueHeadSzL2;

//--------------------------------------------------------------------------------------------------------------------------
//[2、65]压缩的深圳L2市场
//协议内容同[2、59]完全一致, 逐步取代[2,59]

//--------------------------------------------------------------------------------------------------------------------------
//[2,69]协议，用来替代[2,50]协议。作为压缩后的[2,50]
//(pdkui 2012-9-3)

//--------------------------------------------------------------------------------------------------------------------------
//[2,70]请求[上证个股/深圳个股/板块指数]资金流向  (pdkui 2012-9-3)
//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse｜TStockFundFlow ......

//资金流向的量与金额
typedef struct __TSTOCKTRAMTVOL
{
	double					dAmt;				//交易金额
	unsigned __int64		ui64Volume;			//成交量[股]
}TStockTrAmtVol;

//一支股票的资金流向
typedef struct __TSTOCKFUNDFLOW 
{
	char                    Code[6];
	TStockTrAmtVol          stInFlow;			//流入
	TStockTrAmtVol          stOutFlow;			//流出
	TStockTrAmtVol          stBalanceFlow;		//平衡
}TStockFundFlow;

//--------------------------------------------------------------------------------------------------------------------------
//[2, 71]板块指数黄金眼数据  (pdkui 2012-9-3)
//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse｜

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//[2, 76]请求美股码表
//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_BatsNameTable | tagCcComm_BatsNameTable | ...
typedef struct 
{
	unsigned short					Serial;
	unsigned char					Type;
	char							Code[16];
	unsigned long					PreClose;		//昨收[*放大倍数] //昨收来自于收盘矫正文件，不来自于流
}tagCcComm_BatsNameTable;

//[2, 78]请求美股快照
//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_BatsData | tagCcComm_BatsData | ...
//注意: 该请求必须支持分段请求

//[2, 79]推送美股市场数据
//推送：MAIN_FRAME_HEAD | tagCcComm_TimeInfo | 行情数据块 | 行情数据块 | 行情数据块 | …
//行情数据块 = 行情数据块类型 + 行情数据块具体类型对应的结构（参见下面详细说明）
//行情数据块类型 = unsigned char

//类型0： 市场信息	= tagCcComm_MarketStatusInfo
//类型1： 昨收变更  = tagCcComm_BatsPreCloseChg //(仅对 矫正文件中不存在的代码，从流中取昨收)
//类型2： 单个合约快照数据 = tagCcComm_BatsData
typedef struct 
{
	char							Code[16];
	unsigned long					PreClose;		//昨收[*放大倍数]
}tagCcComm_BatsPreCloseChg;

typedef struct 
{
	char							Code[16];
	unsigned long					Open;			//开盘价[*放大倍数]
	unsigned long					High;			//最高价[*放大倍数]
	unsigned long					Low;			//最低价[*放大倍数]
	unsigned long					Now;			//最新价[*放大倍数]
	__int64							Volume;			//成交量
	unsigned long					AvgPrice;		//加权平均价 [*放大倍数]
	unsigned char					TradingFlag;	//交易状态(0=交易中 1=停牌)
	tagCcComm_BuySell				Buy;
	tagCcComm_BuySell				Sell;
}tagCcComm_BatsData;
//--------------------------------------------------------------------------------------------------------------------------


//[2、99]请求商品期货名称代码表

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_CsfexNameTable | tagCcComm_CsfexNameTable | ...
//注意：该请求必须支持分段请求
typedef struct
{
	unsigned short					Serial;					//商品序号
	char							Type;
	char							Code[6];				//商品代码
	char							Name[8];				//商品名称
	unsigned long					LotFactor;				//手
} tagCcComm_CsfexNameTable;

//--------------------------------------------------------------------------------------------------------------------------
//[2、101]请求中金所、商品期货、上海黄金全市场快照数据
//(pdkui: 中金期货添加TimeStamp结构，使用102协议)

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_CffexStockData | tagCcComm_CffexStockData | ...
//注意：该请求必须支持分段请求

typedef struct
{
    char							Code[6];				//商品代码<2009-1-21 Lumy更改>
    unsigned long					High;                   //最高价格[* 放大倍数]
    unsigned long					Open;                   //开盘价格[* 放大倍数]
    unsigned long					Low;                    //最低价格[* 放大倍数]
    unsigned long					PreClose;               //昨收价格[* 放大倍数]
    unsigned long					PreSettlePrice;         //昨日结算价格[* 放大倍数]
    unsigned long					Now;                    //最新价格[* 放大倍数]
    unsigned long					Close;                  //今日收盘价格[* 放大倍数]
    unsigned long					SettlePrice;            //今日结算价格[* 放大倍数]
    unsigned long					UpperPrice;             //涨停价格[* 放大倍数]
    unsigned long					LowerPrice;             //跌停价格[* 放大倍数]
    double							Amount;                 //总成交金额[元]
    unsigned __int64           		Volume;                 //总成交量[股]
    unsigned __int64            	PreOpenInterest;        //昨日持仓量[股]
    unsigned __int64            	OpenInterest;           //持仓量[股]
    tagCcComm_BuySell				Buy[5];                 //委托买盘
    tagCcComm_BuySell				Sell[5];                //委托卖盘
} tagCcComm_CffexStockData;
//--------------------------------------------------------------------------------------------------------------------------
//[2、103]推送中金所股指期货、商品期货、上海黄金行情数据
//(pdkui: 中金期货添加TimeStamp结构，使用105协议)

//推送：MAIN_FRAME_HEAD | tagCcComm_TimeInfo | 行情数据块 | 行情数据块 | 行情数据块 | …
//行情数据块 = 行情数据块类型 + 行情数据块具体类型对应的结构（参见下面详细说明）
//行情数据块类型 = unsigned char

//类型 0: 市场信息 =          tagCcComm_MarketStatusInfo
//类型 1：快照数据 =	  	  tagCcComm_CffexStockData
//--------------------------------------------------------------------------------------------------------------------------

// add by liuqy 20100610 for 增加周边市场的指数推送结构
//modify by liuqy 2010707 for 重新修改指数结构
typedef struct
{
	char						Code[6];					// 商品代码
	unsigned long				High;					// 最高价格[* 放大倍数]
	unsigned long				Open;					// 开盘价格[* 放大倍数]
	unsigned long				Low;					// 最低价格[* 放大倍数]
	unsigned long				Close;					// 昨收价格[* 放大倍数]
	unsigned long				Now;					// 最新价格[* 放大倍数]
	double					Amount;					// 总成交金额[元]
	unsigned __int64			Volume;					// 总成交量[股]
	double					dTotalLTG;			// 流通股本
	double					dTotalZGB;			// 总股本
	//delete by liuqy 20100823 for 不使用以下结构
	/*
	double					dTotalJzc;			// 金资产累加和
	unsigned __int64			uiDtNd;				// 动态年度累加和
	unsigned __int64			uiJtNd;				// 静态年度累加和	
	*/
	double					dTotalValue;				// 总市值
	double					dMarketValue;				// 流通市值
	double					dAdjustedMarketValue;		// 分级靠档流通市值
	unsigned short				ushTotal;					// 计算总家数[指数、初始化时生成]
	unsigned short				ushRise;					// 上涨家数[指数、盘中不断变化]
	unsigned short				ushDown;					// 下跌家数[指数、盘中不断变化]
	//delete by liuqy 20100823 for 不使用以下结构
	//unsigned short				ushExchangeRate;			// 换手率 [单位%，×100；如果无单位，则乘10000]
	unsigned long				ulEarnRate;				// 市盈率 [×100]
	long						lNetRate;					// 市净率 [×100]
	short					shAvgZF;					//平均幅度[单位%，×100；如果无单位，则乘10000]
	unsigned __int64		uiVBuy;					// 总委买量[单位：股]
	unsigned __int64		uiVSell;				// 总委卖量[单位：股] 
	char				szUpCode[7];		// 领涨股代码＋领涨股市场编号
	
} tagCcComm_OtherIndexData;
//add by liuqy 20110826 for 商品期货扩展码表
//[2, 104] 请求商品期货扩展名称代码表信息
//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_CffexRelation | …..
typedef struct
{
unsigned  char 	ExchKind;			//shfe 0  dce 1  zse 2
char 			ContractCode[2];		//合约代码 RB
char 			ContractMap [2][6];	//Map合约规则 8070**
char 			OddEvenFlag ;		// Map合约规则是否区别奇偶 0 不区分 1区分 奇偶
char 			IndexCode[4];			//指数代码 RBZS
char 			IndexMap[6];			//Map指数代码 807099

char 			Reserved[6];			//保留字节
} tagCcComm_CffexRelation ; 
//end add

//...................................................................................................................................................
//...................................................................................................................................................
//...................................................................................................................................................
//...................................................................................................................................................
//...................................................................................................................................................

//--------------------------------------------------------------------------------------------------------------------------
//[2、100]请求中金所股指期货名称代码表

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_CffexNameTable | tagCcComm_CffexNameTable | ...
//注意：该请求必须支持分段请求

typedef struct
{
    unsigned short					Serial;                	//商品序号
    unsigned char					Type;                  	//该商品类别[对应MarketInfo中的类别]
	unsigned char					ObjectMId;				//标的指数市场编号[参见数据字典-市场编号]，0xFF表示未知
	unsigned char					ExFlag;					//最后交易日标记,0x01表示是最后交易日只对普通合约有效；其他值暂未定义
    char							Code[6];                //商品代码
    char							Name[8];               	//商品名称
	char							ObjectCode[6];			//标的指数的代码，无标的指数时填空
	unsigned long					ContractMult;			//合约乘数
} tagCcComm_CffexNameTable;
//--------------------------------------------------------------------------------------------------------------------------
//[2、102]请求中金所全市场快照数据

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | ... | ...
//注意：该请求必须支持分段请求

typedef struct
{
	unsigned int					DataTimeStamp;			//时间戳(HHMMSSmmm)
    char							Code[6];				//商品代码<2009-1-21 Lumy更改>
    unsigned long					High;                   //最高价格[* 放大倍数]
    unsigned long					Open;                   //开盘价格[* 放大倍数]
    unsigned long					Low;                    //最低价格[* 放大倍数]
    unsigned long					PreClose;               //昨收价格[* 放大倍数]
    unsigned long					PreSettlePrice;         //昨日结算价格[* 放大倍数]
    unsigned long					Now;                    //最新价格[* 放大倍数]
    unsigned long					Close;                  //今日收盘价格[* 放大倍数]
    unsigned long					SettlePrice;            //今日结算价格[* 放大倍数]
    unsigned long					UpperPrice;             //涨停价格[* 放大倍数]
    unsigned long					LowerPrice;             //跌停价格[* 放大倍数]
    double							Amount;                 //总成交金额[元]
    unsigned __int64           		Volume;                 //总成交量[股]
    unsigned __int64            	PreOpenInterest;        //昨日持仓量[股]
    unsigned __int64            	OpenInterest;           //持仓量[股]
    tagCcComm_BuySell				Buy[5];                 //委托买盘
    tagCcComm_BuySell				Sell[5];                //委托卖盘
} tagCcComm_CffexStockDataEx;

//[2、105]推送中金所市场数据
//推送：MAIN_FRAME_HEAD | tagCcComm_TimeInfo | 行情数据块 | 行情数据块 | 行情数据块 | …
//行情数据块 = 行情数据块类型 + 行情数据块具体类型对应的结构（参见下面详细说明）
//行情数据块类型 = unsigned char

//类型 0: 市场信息 =          tagCcComm_MarketStatusInfo
//类型 2：快照数据 =	  	  tagCcComm_CffexStockDataEx




//...................................................................................................................................................
//...................................................................................................................................................
//...................................................................................................................................................
//...................................................................................................................................................
//2016-6-21
//[2、201]请求商品期货/权名称代码表
//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_CsfexFutOptNameTable | tagCcComm_CsfexFutOptNameTable | ...
//注意：该请求必须支持分段请求
typedef struct
{
	unsigned short			Serial;						//商品序号
	char					Type;						//分类
	char					Code[20];					//合约代码
	char					Name[40];					//合约名称
	unsigned long			LotFactor;					//手

	//unsigned long			PreClosePx;					//合约昨收(如遇除权除息则为调整后的收盘价格)(精确到厘)//[*放大倍数]
	//unsigned long			PreSettlePx;				//合约昨结//[*放大倍数]

	char					Filler[8];
	int						LeavesQty;					//未平仓合约数 = 昨持仓 单位是(张)
	unsigned char			ObjectMId;					//上海期货 0  大连期货 1  郑州期货 2 上海期权 3  大连期权 4  郑州期权 5
	char					UnderlyingCode[6];			//标的证券代码
	char					PriceLimitType;				//涨跌幅限制类型(N 有涨跌幅)(R 无涨跌幅)
	unsigned long			UpLimit;					//当日期权涨停价格(精确到厘) //[*放大倍数]
	unsigned long			DownLimit;					//当日期权跌停价格(精确到厘) //[*放大倍数]
	int						LotSize;					//一手等于几张合约
	unsigned long			ContractMult;				//合约乘数
	unsigned long			XqPrice;					//行权价格(精确到厘) //[*放大倍数] 
	int						StartDate;					//首个交易日(YYYYMMDD)
	int						EndDate;					//最后交易日(YYYYMMDD)
	int						XqDate;						//行权日(YYYYMM)
	int						DeliveryDate;				//交割日(YYYYMMDD)
	int						ExpireDate;					//到期日(YYYYMMDD)
	unsigned short			TypePeriodIndx;				//分类交易时段位置
	unsigned char			EarlyNightFlag;				//日/夜盘标识：1.日盘 2.夜盘
	char 					Reserved[3];				//保留字节
} tagCcComm_CsfexFutOptNameTable;


///< tagCcComm_CnfOptMkInquiry, 期交所做市询价通知
typedef struct
{
    char						Code[20];				///< 商品代码<2009-1-21 Lumy更改>
	char						ForQuoteSysID[21];		///< 询价编号
} tagCcComm_CnfOptMkInquiry;


//2016-6-21
//[2、202]请求商品期货/权全市场快照数据
//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_CsfexFutOptStockData | tagCcComm_CsfexFutOptStockData | ...
//注意：该请求必须支持分段请求
typedef struct
{
	unsigned short				Date;					//日期
	unsigned int				DataTimeStamp;			//时间戳(HHMMSSmmm)
	char						Code[20];				//商品代码<2009-1-21 Lumy更改>
	unsigned long				High;                   //最高价格[* 放大倍数]
	unsigned long				Open;                   //开盘价格[* 放大倍数]
	unsigned long				Low;                    //最低价格[* 放大倍数]
	unsigned long				PreClose;               //昨收价格[* 放大倍数]
	unsigned long				PreSettlePrice;         //昨日结算价格[* 放大倍数]
	unsigned long				Now;                    //最新价格[* 放大倍数]
	unsigned long				Close;                  //今日收盘价格[* 放大倍数]
	unsigned long				SettlePrice;            //今日结算价格[* 放大倍数]
	unsigned long				UpperPrice;             //涨停价格[* 放大倍数]
	unsigned long				LowerPrice;             //跌停价格[* 放大倍数]
	double						Amount;                 //总成交金额[元]
	unsigned __int64			Volume;                 //总成交量[股]
	unsigned __int64			PreOpenInterest;        //昨日持仓量[股]
	unsigned __int64			OpenInterest;           //持仓量[股]
	tagCcComm_BuySell			Buy[5];                 //委托买盘
	tagCcComm_BuySell			Sell[5];                //委托卖盘
} tagCcComm_CsfexFutOptStockData;


//2016-6-21
//[2、203]请求商品期货/权扩展码表
//同[2、104]结构一样

//2016-6-21
//[2、204]推送商品期货/权行情数据
//推送：MAIN_FRAME_HEAD | tagCcComm_TimeInfo | 行情数据块 | 行情数据块 | 行情数据块 | …
//行情数据块 = 行情数据块类型 + 行情数据块具体类型对应的结构（参见下面详细说明）
//行情数据块类型 = unsigned char
//类型 0: 市场信息 =	tagCcComm_MarketStatusInfo
//类型 1：快照数据 =	tagCcComm_CsfexFutOptStockData


//...................................................................................................................................................
//...................................................................................................................................................
//...................................................................................................................................................
//...................................................................................................................................................
//...................................................................................................................................................

//[2、108]请求中金期权码表
typedef struct
{
    unsigned short					Serial;                	//商品序号
    unsigned char					Type;                  	//该商品类别[对应MarketInfo中的类别]
	unsigned char					ObjectMId;				//标的指数市场编号[参见数据字典-市场编号]，0xFF表示未知
	unsigned char					ExFlag;					//最后交易日标记,0x01表示是最后交易日只对普通合约有效；其他值暂未定义
    char							Code[32];               //商品代码,就是商品名称
	char							ObjectCode[6];			//标的指数的代码，无标的指数时填空
	unsigned long					ContractMult;			//合约乘数

	int								ContractUnit;			//合约单位
	unsigned char					SettlementMode;			//结算方式(1-实物,2-现金)
	char							OptionsType;			//期权类型(E-欧式,A-美式)
	int								StartDate;				//首个交易日(YYYYMMDD)
	int								EndDate;				//最后交易日(YYYYMMDD)
	int								XqDate;					//行权日(YYYYMMDD)
	int								DeliveryDate;			//交割日(YYYYMMDD)
	int								ExpireDate;				//到期日(YYYYMMDD)
} tagCcComm_CffexOptNameTable;
//--------------------------------------------------------------------------------------------------------------------------
//[2、109]请求中金期权快照数据

//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_CffexOptStockData | tagCcComm_CffexOptStockData | ...
//注意：该请求必须支持分段请求

typedef struct
{
	unsigned int					DataTimeStamp;			//时间戳(HHMMSSmmm)
    char							Code[32];				//商品代码<2009-1-21 Lumy更改>
    unsigned long					High;                   //最高价格[* 放大倍数]
    unsigned long					Open;                   //开盘价格[* 放大倍数]
    unsigned long					Low;                    //最低价格[* 放大倍数]
    unsigned long					PreClose;               //昨收价格[* 放大倍数]
    unsigned long					PreSettlePrice;         //昨日结算价格[* 放大倍数]
    unsigned long					Now;                    //最新价格[* 放大倍数]
    unsigned long					Close;                  //今日收盘价格[* 放大倍数]
    unsigned long					SettlePrice;            //今日结算价格[* 放大倍数]
    unsigned long					UpperPrice;             //涨停价格[* 放大倍数]
    unsigned long					LowerPrice;             //跌停价格[* 放大倍数]
    double							Amount;                 //总成交金额[元]
    unsigned __int64           		Volume;                 //总成交量[股]
    unsigned __int64            	PreOpenInterest;        //昨日持仓量[股]
    unsigned __int64            	OpenInterest;           //持仓量[股]
    tagCcComm_BuySell				Buy[5];                 //委托买盘
    tagCcComm_BuySell				Sell[5];                //委托卖盘
} tagCcComm_CffexOptStockData;

//[2, 110]中金期权推送数据
//推送：MAIN_FRAME_HEAD | tagCcComm_TimeInfo | 行情数据块 | 行情数据块 | 行情数据块 | …
//行情数据块 = 行情数据块类型 + 行情数据块具体类型对应的结构（参见下面详细说明）
//行情数据块类型 = unsigned char

//类型0： 市场信息	= tagCcComm_MarketStatusInfo
//类型2： 单个合约快照数据 = tagCcComm_CffexOptStockData


//...................................................................................................................................................
//...................................................................................................................................................
//...................................................................................................................................................
//...................................................................................................................................................
//...................................................................................................................................................

//[2, 80]请求上海期权市场名称代码表
//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_ShOptNameTable | tagCcComm_ShOptNameTable | ...
typedef struct 
{
	unsigned long					Serial;				//序号
	unsigned char					Type;				//类别(1 为指数， 2为 期权合约)
	
	char							SecurityID[8];		//产品ID
	char							ContractID[19];		//合约代码
	char							SecuritySymbol[20];	//合约简称
	
	char							OptionType;			//欧式美式(E-欧式  A-美式)
	char							CallOrPut;			//认购认沽(C-   P-)
	
	unsigned long					PreClosePx;			//合约昨收(如遇除权除息则为调整后的收盘价格)(精确到厘)//[*放大倍数]
	unsigned long					PreSettlePx;		//合约昨结//[*放大倍数]
	int								LeavesQty;			//未平仓合约数 = 昨持仓 单位是(张)
	
	char							UnderlyingCode[6];	//标的证券代码
	char							UnderlyingName[8];	//标的证券名称
	char							UnderlyingType[3];	//标的证券类型(EBS -ETF, ASH -A股)
	unsigned long 					UnderlyingClosePx;	//标的证券的昨收 //[*放大倍数]
	
	char							PriceLimitType;		//涨跌幅限制类型(N 有涨跌幅)(R 无涨跌幅)
	unsigned long					UpLimit;			//当日期权涨停价格(精确到厘) //[*放大倍数]
	unsigned long					DownLimit;			//当日期权跌停价格(精确到厘) //[*放大倍数]
	int								LotSize;			//一手等于几张合约
	int								ContractUnit;		//合约单位(经过除权除息调整后的合约单位，一定为整数)
	
	unsigned long					XqPrice;			//行权价格(精确到厘) //[*放大倍数] 
	int								StartDate;			//首个交易日(YYYYMMDD)
	int								EndDate;			//最后交易日(YYYYMMDD)
	int								XqDate;				//行权日(YYYYMMDD)
	int								DeliveryDate;		//交割日(YYYYMMDD)
	int								ExpireDate;			//到期日(YYYYMMDD)
	
	char							UpdateVersion;		//期权合约的版本号(新挂合约是'1')
	
	unsigned long					MarginUnit;			//单位保证金(精确到分)//[*放大100]
	short							MarginRatio;		//保证金比例1(%)
	short							MarginRatio2;		//保证金比例2(%)
	
	int								MinMktFloor;		//单笔市价申报下限
	int								MaxMktFloor;		//单笔市价申报上限
	int								MinLmtFloor;		//单笔限价申报下限
	int								MaxLmtFloor;		//单笔限价申报上限
	
	char							StatusFlag[8];		//期权合约状态(8个字符,详细定义见文档)
}tagCcComm_ShOptNameTable;


//[2, 82]请求上海期权市场快照数据
//注意：该请求必须支持分段请求
//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_ShOptionData | tagCcComm_ShOptionData | ...


//[2,83]推送上海期权市场数据
//推送：MAIN_FRAME_HEAD | tagCcComm_TimeInfo | 行情数据块 | 行情数据块 | 行情数据块 | …
//行情数据块 = 行情数据块类型 + 行情数据块具体类型对应的结构（参见下面详细说明）
//行情数据块类型 = unsigned char

//类型0： 市场信息	= tagCcComm_ShOptMarketStatus
//类型1： 单个合约快照数据 = tagCcComm_ShOptionData

typedef struct
{
	unsigned char					MarketID;				//市场编号
	unsigned long					MarketDate;				//市场日期[YYYYMMDD格式]
	unsigned long					MarketTime;				//市场时间[HHMMSS格式]
	int								RecordNum;				//合约总个数
	char							TradingPhaseCode[8];	//市场状态
} tagCcComm_ShOptMarketStatus;

typedef struct 
{
	unsigned int					DataTimeStamp;		//时间戳(HHMMSSmmm)
	char							SecurityID[8];		//产品代码 (!= 合约代码)
	unsigned long					PreSettlePx;		//合约昨结//[*放大倍数]
	unsigned long					SettlePrice;		//今结 [*放大倍数]，精确3位数
	unsigned long					OpenPx;				//开盘价 [*放大倍数]，精确3位数
	unsigned long					HighPx;				//最高 [*放大倍数]，精确3位数
	unsigned long					LowPx;				//最低 [*放大倍数]，精确3位数
	unsigned long					Now;				//最新 [*放大倍数]，精确3位数
	
	__int64							Volume;				//成交量
	__int64							Amount;				//成交金额 [放大100倍] 2位小数
	char							TradingPhaseCode[4];//交易状态
	
	unsigned long					AuctionPrice;		//波动性中断参考价 [*放大倍数]，精确3位数
	__int64							AuctionQty;			//波动性中断集合竞价虚拟匹配量
	__int64							Position;			//持仓量
	tagCcComm_BuySell				Buy[5];				//委托买
	tagCcComm_BuySell				Sell[5];			//委托卖
}tagCcComm_ShOptionData;


//...................................................................................................................................................
//...................................................................................................................................................
//...................................................................................................................................................
//...................................................................................................................................................
//...................................................................................................................................................
//[2, 112]请求深圳期权市场名称代码表
//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_SzOptNameTable | tagCcComm_SzOptNameTable | ...
typedef struct 
{
	unsigned long					Serial;			//序号
	unsigned char					Type;			//类别(1 为指数， 2为 期权合约)

	char							SecurityID[8];		//产品ID
	char							ContractID[20];		//合约代码
	char							SecuritySymbol[20];	//合约简称

	char							OptionType;		//欧式美式(E-欧式  A-美式 B-百慕大式)
	char							CallOrPut;		//认购认沽(C-   P-)

	unsigned long					PreClosePx;		//合约昨收(如遇除权除息则为调整后的收盘价格)(精确到厘)//[*放大倍数]
	unsigned long					PreSettlePx;		//合约昨结//[*放大倍数]
	int								LeavesQty;		//未平仓合约数 = 昨持仓 单位是(张)

	char							UnderlyingCode[6];	//标的证券代码

	unsigned long					UpLimit;		//当日期权涨停价格(精确到厘) //[*放大倍数]
	unsigned long					DownLimit;		//当日期权跌停价格(精确到厘) //[*放大倍数]
	int								LotSize;		//一手等于几张合约
	int								ContractUnit;		//合约单位(经过除权除息调整后的合约单位，一定为整数)

	unsigned long					XqPrice;		//行权价格(精确到厘) //[*放大倍数] 
	int								StartDate;		//首个交易日(YYYYMMDD)
	int								EndDate;		//最后交易日(YYYYMMDD)
	int								XqDate;			//行权日(YYYYMMDD)
	int								DeliveryMonth;		//交割月份(YYMM)
	int								ExpireDate;		//到期日(YYYYMMDD)
	unsigned long					MarginUnit;		//单位保证金(精确到分)//[*放大100]
}tagCcComm_SzOptNameTable;

//[2, 113]请求深圳期权市场快照数据
//注意：该请求必须支持分段请求
//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_MarketSubResponse | tagCcComm_SzOptionData | tagCcComm_SzOptionData | ...



//[2,115]推送深圳期权市场数据
//推送：MAIN_FRAME_HEAD | tagCcComm_TimeInfo | 行情数据块 | 行情数据块 | 行情数据块 | …
//行情数据块 = 行情数据块类型 + 行情数据块具体类型对应的结构（参见下面详细说明）
//行情数据块类型 = unsigned char

//类型0： 市场信息 = tagCcComm_SzOptMarketStatus
//类型1： 单个合约快照数据 = tagCcComm_SzOptionData
//类型2：深交所前缀变更 = tagCcComm_SzOptPerNameChg

typedef struct
{
	unsigned char					MarketID;		//市场编号
	unsigned long					MarketDate;		//市场日期[YYYYMMDD格式]
	unsigned long					MarketTime;		//市场时间[HHMMSS格式]
	unsigned char					MarketStatus;	//市场状态[0初始化状态 1行情中状态]
} tagCcComm_SzOptMarketStatus;

typedef struct
{
	char							Code[8];		//商品代码
	char							PreName[4];		//商品前缀
	unsigned char					Status[2];		//下标0，买开仓=1(有限制) =0(无限制);  下标1,卖开仓=1(有限制) =0(无限制)
} tagCcComm_SzOptPerNameChg;

typedef struct 
{
	unsigned int					DataTimeStamp;		//时间戳(HHMMSSmmm)
	char							SecurityID[8];		//产品代码 (!= 合约代码)
	unsigned long					PreSettlePx;		//合约昨结//[*放大倍数]
	unsigned long					SettlePrice;		//今结 [*放大倍数]，精确3位数
	unsigned long					OpenPx;			//开盘价 [*放大倍数]，精确3位数
	unsigned long					HighPx;			//最高 [*放大倍数]，精确3位数
	unsigned long					LowPx;			//最低 [*放大倍数]，精确3位数
	unsigned long					Now;			//最新 [*放大倍数]，精确3位数

	__int64							Volume;			//成交量
	__int64							Amount;			//成交金额 [放大100倍] 2位小数
	char							TradingPhaseCode[8];	//交易状态

	__int64							Position;		//持仓量
	tagCcComm_BuySell				Buy[5];			//委托买
	tagCcComm_BuySell				Sell[5];		//委托卖
}tagCcComm_SzOptionData;


typedef struct
{
     char                           Code[8];		///< 商品代码
     short							InquiryStatus;	///< 做市询价状态
} tagCcComm_SzOptMkInquiry;


//...................................................................................................................................................
//...................................................................................................................................................
//...................................................................................................................................................
//...................................................................................................................................................
//...................................................................................................................................................

//[2, 254]请求最近的节假日表
//注意：该请求失败于否不影响后续请求业务的处理
//请求：MAIN_FRAME_HEAD | tagCcComm_MarketSubApply
//应答：MAIN_FRAME_HEAD | tagCcComm_LastHolidayResponse | tagCcComm_LastHolidayTable | tagCcComm_LastHolidayTable | ...
typedef struct 
{
	unsigned short					ushDateCount; //后续表中日期的个数
} tagCcComm_LastHolidayResponse;

typedef struct 
{
	unsigned char					uchIsHoliday; //日期是节假日标志，0表示非，1表示是
	unsigned int					ulDate; //日期，由上面的标志控制该日期是否为节假日
}tagCcComm_LastHolidayTable;


//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//..........................................................................................................................
//以下为驱动动态库接口
//..........................................................................................................................
typedef struct														//驱动基本信息
{
	unsigned char				drvMarketId;						//驱动提供行情的市场编号（总部服务器负责人处申请或查询）
	unsigned short				drvUnitType;						//驱动的单元类型（总部服务器负责人处申请或查询）
	char						drvName[40];						//驱动名称（总部服务器负责人处申请或查询）
	char						drvDescription[128];				//驱动描述
	MControlBaseClass		*	drvControlClassPtr;					//驱动控制类函数指针[具体描述请参见服务管理器接口MServiceInterActive.hpp]
	char						Reserved[81];						//保留
} tagDll_DriverBaseInfo;
//..........................................................................................................................
typedef void (__cdecl tagOnPushDataFun)(		
											unsigned char 		MainType,		//推送数据的主版本号（参见具体驱动协议）
											unsigned char 		ChildType,		//推送数据的子版本号（参见具体驱动协议）
											const char		* 	InBuf,			//推送数据内容缓冲
											unsigned short 		InSize,			//要推送数据的长度
											unsigned char 		Marketid,		//推送数据的市场ID
											unsigned short		UnitNo,			//推送驱动的单元号码，转码服务器可以用到
											bool 				SendDirectFlag	//告诉传输平台对该推送包不需要进行压缩等处理，直接发送就可以了
										);
//..........................................................................................................................
typedef void (__cdecl tagWriteLogFun) (		
											unsigned char 		Type,			//日志类型[0=流水日志、1=信息日志、2=警告日志、3=错误日志]
											unsigned short		SrvUnit, 		//服务管理器分配的单元编号（传输平台会通过其他函数通知驱动）
											unsigned short		LogLevel,		//日志等级[数字越大表示越详细，0级为普通运营级别]
											const char 		* 	Source,			//该日志所属的驱动内部模块名称
											const char		* 	Format, ...		//日志内容
										);		
//..........................................................................................................................
typedef void (__cdecl tagPrintFun) (
											unsigned long		CmdSerail,		//控制命令编号，由传输平台调用时作为参数传入。
											const char 		* 	Format, ... 	//返回的提示信息内容
									);

typedef void (__cdecl tagCloseFun)(
										   unsigned char		Marketid,		//需要关闭的市场ID
										   unsigned short		UnitCode		//驱动的单元编号
									);

typedef int  (__cdecl tagPrepareDataFrame)(unsigned char cMainType,				//主类型
										   unsigned char cChildType,			//子类型
										   const char * lpInBuf,				//原始数据（需要压缩和处理的数据）
										   unsigned short sInSize,				//原始数据大小
										   char * lpOutBuf,						//已经处理好的数据（经过压缩的数据或金融压缩的数据）
										   unsigned short sOutSize);			//lpOutBuf空间大小

typedef int  (__cdecl tagRestoreDataFrame)(	const char * lpInBuf,				//需要还原的压缩数据（经过压缩的数据或金融压缩的数据）
											unsigned short sInSize,				//需要还原的压缩数据长度
											char * * lpOutPtr,					//已经还原的数据地址
											char * lpTempBuf,					//临时处理空间
											unsigned short sTempSize );			//临时处理空间大小
//add by liuqy 20100901 for 增加对单元日志级别检查，级别可以使用返回true,不可用返回false
typedef bool tagSmFun_ChkWriteLogLevel(unsigned short wSrvUnit, unsigned short wLogLevel);
//end add

//..........................................................................................................................
typedef struct
{
	unsigned short				PlatUnitType;						//调用平台类型（传输平台还是转码机，总部服务器负责人处申请或查询）
	unsigned long				PlatVersion;						//调用平台版本号
	unsigned long				DriverUnitCode;						//调用平台注册驱动后，服务管理器分配的单元编号（注意：日志等地方需要使用）
	unsigned long 				SendBufSize;						//发送缓冲长度，传输平台一般为发送缓冲的1/20，转码机为100 * FrameSize
	unsigned short 				FrameSize;							//发送时每个数据包的最大长度，一般为8192
	tagOnPushDataFun		*	PushDataFunPtr;						//向下级推送数据（如快照、逐笔等等，发送变化后进行推送）
	tagWriteLogFun			*	WriteLogFunPtr;						//打印日志（注意：驱动打印日志必须通过该接口，输送到传输平台）
	tagPrintFun				*  	CmdPrintFunPtr;						//打印控制命令返回提示（不换行方式）
	tagPrintFun				*  	CmdPrintlnFunPtr;					//打印控制命令返回提示（换行方式）
	tagCloseFun				*	CloseFunptr;						//通知传输平台关闭与该驱动的连接
	tagPrepareDataFrame		*	PrepareDataFrame;					//对原始数据进行压缩等处理
	tagRestoreDataFrame		*	RestoreDataFrame;					//对处理的数据进行还原
	//add by liuqy 20100901 for 增加一个增加对单元日志级别检查函数接口,同时增加一个版本号和识别码用于下一级检查是否支持某些功能
	unsigned long								ulSvcPlatFlag;					//平台识别码，当该值为"SVCP"的ASCII码时，表示有后面的接口
	unsigned long								ulPlatVer;						//平台当前的版本号，供下一级检查某些功能是否支持
	//增加对单元日志级别检查，级别可以使用返回true,不可用返回false
	tagSmFun_ChkWriteLogLevel				* ChkWriteLogLevel;					//对单元日志级别检查 
	char						Reserved[72];						//保留
	//end add
} tagDll_DataCenterInput;
//..........................................................................................................................
typedef struct
{
	char						Reserved[128];						//保留
} tagDll_DriverOutput;
//..........................................................................................................................
typedef struct
{
	unsigned short 				nCount;								//表示调用平台本次调用GetMarketInfo最大提供pInfo的容量（In）
																	//如果驱动中实际pInfo实际数量大于该数值，应该返回失败。
																	//该值表示tagDll_GetMarketInfoOut结构中所开辟的pInfo结构数量
	char						Reserved[126];						//保留
} tagDll_GetMarketInfoIn;
//..........................................................................................................................
typedef struct
{
	tagCcComm_MarketInfoHead 	*	pHead;							//市场信息头部
	tagCcComm_MarketInfoDetail 	* 	pInfo;							//市场类别信息
	char							Reserved[120];					//保留
} tagDll_GetMarketInfoOut;
//..........................................................................................................................
typedef struct
{
	unsigned char 				MainType;							//请求主类型
	unsigned char 				ChildType; 							//请求子类型
	const char 				* 	ApplyBuf;							//请求内容
	unsigned short 				ApplySize;							//请求长度
	char						Reserved[120];						//保留
} tagDll_GetDataFrameCountIn;
//..........................................................................................................................
typedef struct
{
	char						RequestCompleteFlag;				//本次请求是否完成（如果BeginWork时SendBufSize不够返回所有信息，即只能完成部分）
	char						Reserved[127];						//保留
} tagDll_GetFrameCountOut;
//..........................................................................................................................
typedef struct
{
	unsigned char 				MainType;							//请求主类型
	unsigned char 				ChildType; 							//请求子类型
	const char 				* 	ApplyBuf;							//请求内容
	unsigned short 				ApplySize;							//请求长度
	unsigned short				FrameNo;							//请求第几个数据包的数据
	unsigned short				DataOutBufSize;						//出口参数DataOutBuf的长度，如果实际返回数据长度大于该值，函数返回失败
	char						Reserved[116];						//保留
} tagDll_GetOneDataFrameIn;
//..........................................................................................................................
typedef struct
{
	char					*	DataOutBuf;							//返回数据包数据内容
	char						Reserved[124];						//保留
} tagDll_GetOneDataFrameOut;
//..........................................................................................................................
typedef struct
{
	unsigned long				version;							//驱动版本
	unsigned char 				marketid;							//市场编号
	unsigned char			    marketstatus;						//市场状态
	unsigned long				freshtime; 							//市场刷新时间(秒)
	unsigned long 				marketdate;							//市场日期
	unsigned long 				markettime;							//市场时间
	unsigned short				wareCount;							//该市场商品总数
	unsigned short 				packspeed;							//每秒钟处理的包的个数
	unsigned long  				price;								//首商品的价格
	__int64						amount;								//首商品的成交金额
	unsigned __int64			volumn;								//首商品的成交量
	unsigned long				freshinterval;						//首商品的刷新时间间隔（秒）
	unsigned long				worktime;							//本次连接会话的时间(秒)，库打开的时间 
	char						serverStatus;						//驱动服务状态
	char						HQSourceAddr[80];					//行情源地址或描述，可以为DBF文件路径、服务器域名：端口、IP地址：端口等等
	char						additionInfo[32];					//驱动的描述信息
} tagDll_DriverStatusInfo;
//add by liuqy 20101027 for 新的监控要素，因此需要修改如下结构
typedef struct
{
	union
	{
		struct
		{
			unsigned long m_u24Magic: 24;
			unsigned long m_u8Version:8;
		};
		
		unsigned long				ulHead;				// 监控头
	};
	unsigned long				ulVersion;							//驱动版本
	unsigned char 				uchMarketid;							//市场编号
	unsigned char			    uchMarketstatus;						//市场状态
	unsigned long				ulMarketFreshtime; 							//市场刷新时间间隔(毫秒)
	unsigned long 				ulMarkettime;							//市场时间
	unsigned long 				ulMarketdate;							//市场日期
	unsigned short				ushWareCount;							//该市场商品总数
	unsigned short 				ushPackspeed;							//每秒钟处理的包的个数
	unsigned long  				ulPrice;								//首商品的价格
	__int64						llAmount;								//首商品的成交金额
	unsigned __int64			ullVolumn;								//首商品的成交量
	unsigned long				ulFreshinterval;						//首商品的刷新时间间隔（毫秒）
	unsigned long				ulStartupTime;							//启动时间
	unsigned long				ulStartupDate;							//启动日期
	unsigned long				ulInitializeTime;						//初始化时间
	unsigned char				uchServerStatus;						//驱动服务状态
	unsigned short				ushTotalCommBreakInterval;				//当天断线总时长(秒 最大为65535)
	unsigned short				ushCurCommBreakInterval;				//当前断线时长(秒 最大为65535)
	unsigned char				uchConnectFlag;						//与上一级连接状态
	unsigned short				ushCommSwitchCount;					//通信切换统计
	unsigned long				ulConnectedInterval;				//本次连接成功时长(秒)
	unsigned short				ushConnectPort;						//连接端口
	char						szConnectAddr[16];					//连接地址
} tagDll_NewDriverStatusInfo;

//..........................................................................................................................
typedef struct
{
	unsigned char				MarketId;
	unsigned char				UnitType;
	unsigned short				UnitNo;
	unsigned long				Date;
	unsigned long				Time;
	unsigned long				DelayTime;
	char						IndexFileName[128];	
	char						DataFileName[128];
	char						Reserved[116];
} tagDelay_PreUnitInfo;
//..........................................................................................................................
typedef struct
{
	unsigned char				MarketId;			///< 市场ID
	unsigned char				UnitType;			///< 模块类型
	unsigned short				UnitNo;				///< 模块编号
	char						pszIP[16];			///< 模块ip地址
	unsigned short				nListenPort;		///< 模块监听端口
	unsigned short				nMaxServerNum;		///< 最大可accecpt的连接数
} tagDispatcher_SZL2FastInfo;
//..........................................................................................................................
typedef struct
{
	unsigned char	mainype;	//推送主类型
	unsigned char	childType;	//推送子类型
	unsigned short	size;		//数据文件中的大小
	unsigned int	offset;		//数据文件中的起始位置
	long			time;		//写文件时间
} tagIndexFileRecord;
//..........................................................................................................................
typedef struct
{
	unsigned char				Status0;
	unsigned char				Status1;
	char						Reserved[126];
} tagDelay_ThreadStatus;

//..........................................................................................................................
typedef unsigned long	__stdcall tagGetDllVersion(void);
typedef void			__stdcall tagGetDriverBaseInfo(tagDll_DriverBaseInfo * outdrvinfo);
typedef int				__stdcall tagBeginDriverWork(tagDll_DataCenterInput * In,tagDll_DriverOutput * Out);
typedef void			__stdcall tagEndDriverWork(void);
typedef int				__stdcall tagGetMarketInfo(tagDll_GetMarketInfoIn * In, tagDll_GetMarketInfoOut * Out);
typedef int				__stdcall tagGetDataFrameCount(tagDll_GetDataFrameCountIn * In,tagDll_GetFrameCountOut * Out);
typedef int				__stdcall tagGetOneDataFrame(tagDll_GetOneDataFrameIn * In,tagDll_GetOneDataFrameOut * Out);
typedef bool			__stdcall tagIsDriverWorking(void);
typedef void			__stdcall tagGetDriverStatusInfo(tagDll_DriverStatusInfo * Out);
//add by liuqy 20101027 为监控新要素取新的监控信息
/*********************
 * 函数名：GetNewDriverStatusInfo
 * 功能：	取新的驱动信息
 * 参数：	out_pszDrvStatusInfoBuf	输出-状态信息
 *			in_ulDrvStatusInfoBufSize	输入-状态信息空间大小
 * 返回： 大于0返回数据大小，小于0表示失败
 **********************/
typedef int				__stdcall tagGetNewDriverStatusInfo(char * out_pszDrvStatusInfoBuf, unsigned long in_ulDrvStatusInfoBufSize);


//add by liuqy 20110808 for 增加过虑接口需要的结构
//二级传输连接信息
typedef struct
{
	unsigned long					lLinkNo;					//连接编号
	unsigned long					lIPAddr;					//IP地址
	
	char							szUserName[32];				//账号名称
	char							szMacAddr[12];				//MAC地址
	unsigned long					lClientVersion;				//行情传输客户端版本号
	unsigned char					cMarketID;					//市场编号
	unsigned short					uClientType;				//客户端类型
	
} tagTran2nd_LinkInfo;
//add by liuqy 20110808 for 增加过虑接口处理接口，返回是否发送标志

typedef bool			__stdcall tagSndDataSpecialFilter(tagTran2nd_LinkInfo * in_pstLinkInfo, unsigned char MainType,unsigned char ChildType,
														  const char * InBuf,unsigned short InSize);


//..........................................................................................................................
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------------
#pragma pack()
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------------------------------------------------------
////////////////////////This Code AUTO Mount Point By Append////////////

////////////////////////LINUX newline 请在该注释上面加CODE///////////////////
