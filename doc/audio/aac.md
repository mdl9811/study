### 封装格式

* **TT_MP4_RAW就是表示裸AAC码流，没有任何方式的封装**
* **TT_MP4_ADIF和TT_MP4_ADTS分别是ADIF和ADTS格式**
* **TT_MP4_LATM_MCP1和TT_MP4_LATM_MCP0 是LATM封装格式，一个是带内传输StreamMuxConfig，一个是带外传输**
* **TT_MP4_LOAS就是LOAS**

在流媒体中最常用的两种格式**ADTS**，**LATM**

* 编码规格AAC-LC 编码格式ADTS
* 编码规格AAC-LD 编码格式RAW, LATM_MCP1, LATM_MCP0, MP4-LOAS 

不同封装格式不同场景：

* ADIF 一般用于文件存储
* ADTS 用于流媒体或文件存储

* LOAS 流媒体

* LATM 流媒体

  