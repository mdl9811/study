**此模块 音频编码 使用fdk-aac opus 视频编码使用NV 后续可以使用AMD等等，还会封装一些ffmpeg音视频编码器 做比较**

* **AUDIO**
  
  * pub[公共知识库]
  
  > [音频编码（PCM、G711A、G711U、AAC）理解-CSDN博客](https://blog.csdn.net/weixin_37779156/article/details/102813455)【优先】
  >
  > [音频编码：入门看这篇就够了丨音视频基础 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/499760382)
  > https://blog.csdn.net/qq_41824928/article/details/108124382
  > https://zhuanlan.zhihu.com/p/52314087#:~:text=%E8%AE%A1%E7%AE%97%E5%85%AC%E5%BC%8F%EF%BC%9ABits%3D%E9%87%87%E6%A0%B7%E7%8E%87times%E4%BD%8D%E6%B7%B1times%E9%80%9A%E9%81%93times%E6%97%B6%E9%95%BF%20%28%E7%A7%92%29%20%E4%BD%8D%E6%B7%B1%E5%BD%B1%E5%93%8D%E4%BF%A1%E5%8F%B7%E7%9A%84%E4%BF%A1%E5%99%AA%E6%AF%94%E5%92%8C%E5%8A%A8%E6%80%81%E8%8C%83%E5%9B%B4%EF%BC%8C%E4%B9%9F%E5%86%B3%E5%AE%9A%E4%BA%86%E6%96%87%E4%BB%B6%E7%9A%84%E5%A4%A7%E5%B0%8F%EF%BC%8C%E7%90%86%E8%AE%BA%E4%B8%8A%E6%9D%A5%E8%AF%B4%EF%BC%8C%E4%BD%8D%E6%B7%B1%E8%B6%8A%E9%AB%98%EF%BC%8C%E8%B4%A8%E9%87%8F%E8%B6%8A%E5%A5%BD%EF%BC%8C%E5%90%8C%E6%97%B6%E5%85%B6%E6%89%80%E7%94%9F%E6%88%90%E7%9A%84%E6%96%87%E4%BB%B6%E4%B9%9F%E8%B6%8A%E5%A4%A7%E3%80%82,16Bit%EF%BC%9A%20%E5%8A%A8%E6%80%81%E8%8C%83%E5%9B%B4%E5%A4%A7%E6%A6%82%E6%98%AF96dB%EF%BC%8C%E9%80%82%E7%94%A8%E4%BA%8E%E6%99%AE%E9%80%9A%E6%B5%81%E8%A1%8C%E6%AD%8C%E6%9B%B2%E3%80%82%2024Bit%EF%BC%9A%20%E5%8A%A8%E6%80%81%E8%8C%83%E5%9B%B4%E5%A4%A7%E6%A6%82%E6%98%AF144dB%EF%BC%8C%E4%B8%80%E8%88%AC%E7%94%A8%E4%BA%8E%E7%94%B5%E5%BD%B1%E9%85%8D%E4%B9%90%EF%BC%8C%E4%BA%A4%E5%93%8D%E4%B9%90%E5%9B%A2%E7%AD%89%E7%AD%89%E5%A4%A7%E5%8A%A8%E6%80%81%E7%9A%84%E9%9F%B3%E9%A2%91%E4%BF%A1%E5%8F%B7%E3%80%82
  
  * *aac*
  
    > [AAC 文件解析及解码流程 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/347992887)
    >
    > [音视频探索(2)：AAC编码解析 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/584316764)
    >
    > [libfdk_aac编码AAC-CSDN博客](https://blog.csdn.net/u010140427/article/details/127765173)[优先看]
  
    注解：
  
     1. AAC是新一代的音频有损压缩技术，它通过一些附加的编码技术（比如PS,SBR等），衍生出了LC-AAC,HE-AAC,HE-AACv2三种主要的编码，LC-AAC就是比较传统的AAC，相对而言，主要用于中高码率(>=80Kbps)，HE-AAC(相当于AAC+SBR)主要用于中低码(<=80Kbps)，而新近推出的HE-AACv2(相当于AAC+SBR+PS)主要用于低码率(<=48Kbps）,事实上大部分编码器设成<=48Kbps自动启用PS技术，而>48Kbps就不加PS,就相当于普通的HE-AAC。
  
     2. AAC的音频文件格式有ADIF ＆ ADTS
  
     3. 音质，顺序大概是libfdk_aac>libfaac>aac>libvo_aacenc
  
     4. 支持采样率：
  
        >  8000  11025  12000  16000  22050  24000  32000  44100  48000  64000  88200  96000
  
    5. 说明一下编码码率：64k、96k
    6. AAC-ELD的范围是最广的，包括 1024，512，480，其次是AAC-LD范围为:1024,512,480,那么对AAC-LC来说就只有1024了。(注意这里的长度是采样点的个数)。 
  
  * *opus*
  
    > 
  
  * *ffmpeg*
  
    > 
  
* **VIDOE**

   * *NV*

     > 

   * *ffmpeg*

     > 



