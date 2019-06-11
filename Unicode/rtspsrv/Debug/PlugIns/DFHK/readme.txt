1、HCNetSDK(Windows32)V5.1.5.7
2、PlayCtrl_V7.3.0.40(for_win32)
3、 3D定位接口变更，原DF1500系统不支持。
 [df_jk_camera]
	增加一条记录 通道信息与原摄像机信息一致，增加如下配置，并设置为隐藏，重启管理服务
	- url项设置：CHANNEL=12;(云台协议)PROTOCOL=YIKEDA;(摄像机地址)CAMADDR=12;
