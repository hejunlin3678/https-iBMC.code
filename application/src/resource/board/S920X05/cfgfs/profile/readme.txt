PRODUCT_XX_XX.xml 加载说明：
通过Connector.Id区分是TaiShan200 2X80系列/TaiShan200 5X80系列。
通过Connector.AuxId区分1P/2P、AK与非AK机型等。

0x18寄存器Bit1:7判断1620服务器机型，
Bit7区分是TaiShan 200 1P/2P类型(2180/2280,5280/5180)
Bit6区分是TaiShan 200 AK/非AK类型
Bit5-1当前未使用，默认值为0。
7'b0000_000(0x00)：2280 V2/5280 V2
7'b0010_000(0x10)：1280 V2
7'b0100_000(0x20)：2280 V2/5280 V2 AK机型
7'b1000_000(0x40)：2180 V2
7'b1100_000(0x60)：2180 V2 AK机型

机型与PRODUCT_xx_xx.xml的对应关系：
				Id		  AuxId
						bit7   bit6
PRODUCT_00		2X80	0		0			TaiShan 200 (Model 2280)
PRODUCT_01		5X80	0		0			TaiShan 200 (Model 5280)
PRODUCT_00_20	2X80	0		1			TaiShan 200K (Model 2280K)
PRODUCT_01_20	5X80	0		1			TaiShan 200K (Model 5280K)
PRODUCT_00_40	2X80	1		0			TaiShan 200 (Model 2180)						
PRODUCT_00_60	2X80	1		1			TaiShan 200K (Model 2180K)
PRODUCT_00_ff   非此ID范围时机型为NA
PRODUCT_01_ff   非此ID范围时机型为NA

新增boardid为a9的机型适配AK、40核TaiShan 200机型：
PRODUCT_02		2X80	0		0			TaiShan 200 (Model 2280)
PRODUCT_03		5X80	0		0			TaiShan 200 (Model 5280)
PRODUCT_02_20	2X80	0		1			TaiShan 200K (Model 2280K)
PRODUCT_03_20	5X80	0		1			TaiShan 200K (Model 5280K)
PRODUCT_02_40	2X80	1		0			TaiShan 200 (Model 2180)						
PRODUCT_02_60	2X80	1		1			TaiShan 200K (Model 2180K)
PRODUCT_02_ff   非此ID范围时机型为NA
PRODUCT_03_ff   非此ID范围时机型为NA

*****************UniqueID与ProductName对应关系******************

配置XML	            UniqueID	ProductName
PRODUCT_00.xml	    0x0201b900	TaiShan 200 (Model 2280)
PRODUCT_00_10.xml	0x0201b904	TaiShan 200 (Model 1280)
PRODUCT_00_20.xml	0x0201b90a	TaiShan 200K (Model 2280K)
PRODUCT_00_40.xml	0x0201b902	TaiShan 200 (Model 2180)
PRODUCT_00_60.xml	0x0201b90c	TaiShan 200K (Model 2180K)
PRODUCT_00_ff.xml	0x0201b900	NA
PRODUCT_01.xml	    0x0201b901	TaiShan 200 (Model 5280)
PRODUCT_01_20.xml	0x0201b90b	TaiShan 200K (Model 5280K)
PRODUCT_01_ff.xml	0x0201b901	NA
		
PRODUCT_02.xml	    0x0201a900	TaiShan 200 (Model 2280)
PRODUCT_02_10.xml	0x0201a904	TaiShan 200 (Model 1280)
PRODUCT_02_20.xml	0x0201a90a	TaiShan 200K (Model 2280K)
PRODUCT_02_40.xml	0x0201a902	TaiShan 200 (Model 2180)
PRODUCT_02_60.xml	0x0201a90c	TaiShan 200K (Model 2180K)
PRODUCT_02_ff.xml	0x0201a900	NA
PRODUCT_03.xml	    0x0201a901	TaiShan 200 (Model 5280)
PRODUCT_03_20.xml	0x0201a90b	TaiShan 200K (Model 5280K)
PRODUCT_03_ff.xml	0x0201a901	NA	
		
PRODUCT_00_a5.xml	0x0201a500	TaiShan 200 (Model 2280)
PRODUCT_01_a5.xml	0x0201a501	TaiShan 200 (Model 5280)


