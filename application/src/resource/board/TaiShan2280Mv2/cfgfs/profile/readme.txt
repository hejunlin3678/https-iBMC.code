PRODUCT_XX_XX.xml ����˵����
ͨ��Connector.Id������TaiShan200 2X80ϵ��/TaiShan200 5X80ϵ�С�
ͨ��Connector.AuxId����1P/2P��AK���AK���͵ȡ�

0x18�Ĵ���Bit1:7�ж�1620���������ͣ�
Bit7������TaiShan 200 1P/2P����(2180/2280,5280/5180)
Bit6������TaiShan 200 AK/��AK����
Bit5-1��ǰδʹ�ã�Ĭ��ֵΪ0��
7'b0000_000(0x00)��2280 V2/5280 V2
7'b0010_000(0x10)��1280 V2
7'b0100_000(0x20)��2280 V2/5280 V2 AK����
7'b1000_000(0x40)��2180 V2
7'b1100_000(0x60)��2180 V2 AK����

������PRODUCT_xx_xx.xml�Ķ�Ӧ��ϵ��
				Id		  AuxId
						bit7   bit6
PRODUCT_00		2X80	0		0			TaiShan 200 (Model 2280)
PRODUCT_01		5X80	0		0			TaiShan 200 (Model 5280)
PRODUCT_00_20	2X80	0		1			TaiShan 200K (Model 2280K)
PRODUCT_01_20	5X80	0		1			TaiShan 200K (Model 5280K)
PRODUCT_00_40	2X80	1		0			TaiShan 200 (Model 2180)						
PRODUCT_00_60	2X80	1		1			TaiShan 200K (Model 2180K)
PRODUCT_00_ff   �Ǵ�ID��Χʱ����ΪNA
PRODUCT_01_ff   �Ǵ�ID��Χʱ����ΪNA

����boardidΪa9�Ļ�������AK��40��TaiShan 200���ͣ�
PRODUCT_02		2X80	0		0			TaiShan 200 (Model 2280)
PRODUCT_03		5X80	0		0			TaiShan 200 (Model 5280)
PRODUCT_02_20	2X80	0		1			TaiShan 200K (Model 2280K)
PRODUCT_03_20	5X80	0		1			TaiShan 200K (Model 5280K)
PRODUCT_02_40	2X80	1		0			TaiShan 200 (Model 2180)						
PRODUCT_02_60	2X80	1		1			TaiShan 200K (Model 2180K)
PRODUCT_02_ff   �Ǵ�ID��Χʱ����ΪNA
PRODUCT_03_ff   �Ǵ�ID��Χʱ����ΪNA

*****************UniqueID��ProductName��Ӧ��ϵ******************

����XML	            UniqueID	ProductName
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


