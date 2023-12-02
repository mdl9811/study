**此模块 音频解码 使用fdk-aac opus 视频解码使用NV 后续可以使用AMD等等，还会封装一些ffmpeg音视频编码器 做比较**

* aac

  > https://blog.csdn.net/u010140427/article/details/127773460
  > https://blog.csdn.net/dong_beijing/article/details/87939673
  >
  > aac配置Audio Specific Config 俗称ASC 
  >
  > 如果解码器解 TT_MP4_RAW格式的数据 需要使用aacDecoder_ConfigRaw 接口 
  >
  > 先说明一下aacDecoder_ConfigRaw 第二个参数生产规则，为什么RAW需要调用这个接口，
  >
  > 第一其他格式如：ADTS都是一种封装格式，所以带有音频的各种参数声道数、采样率等等，RAW只是aac编码后的原始数据，
  >
  > 所以不具备各种参数，解码器不知道，所以需要告诉解码器编码的各种参数才能解码，aacDecoder_ConfigRaw 接口就充当这个角色，这个接口的第二个参数是可变的，它的生产规则是：
  >
  > * type Frequencies_index channnel  这是三个参数 第一个参数type代表lc、eld等等，index是采样率的索引有一个表对应上就可以了，channel是声道数，如何将这几个参数放到一起呢？
  >
  > * Frequencies_index 和channel不需要做其他考虑它俩都占4位，就是一个字节
  >
  > * 特殊考虑的是type，如果大于31是一种情况，小于等于31是一种情况，如果小于等于31占用五位，大于占用11
  >
  >   1. 举例说明，参数：lc模式、48000采样率、双声道：
  >
  >      * lc在type中是2 二进制就是 00010，48000 index在表中是3二进制是0011、双声道是2二进制表示0010三个连起来就是00010 0011 0010 000 后三位是填充位数 所以最后的参数就是 0x11 0x90
  >
  >   2. 举例说明，参数：eld模式、48000采样率、双声道：
  >
  >      * eld在type中是39大于31所以占11位数，前五位不变 11111 代表31 后六位是39-32=8（因为数字从0开始的共有32个数） 所以后六位二进制000111 所以前11位的格式是5位 + 6位 ,48000 index在表中是3二进制是0011、双声道是2二进制表示0010, 所以连起来就是11111 000111 0011 0010 00000 参数：0xf8 0xe6 0x40
  >
  >      

>**Audio Object Types**
>
>- NULL: 0
>
>- AAC Main: 1
>
>- AAC LC (Low Complexity): 2
>
>- AAC SSR (Scalable Sample Rate): 3
>
>- AAC LTP (Long Term Prediction): 4
>
>- SBR (Spectral Band Replication): 5
>
>- AAC Scalable: 6
>
>- TwinVQ: 7
>
>- CELP (Code Excited Linear Prediction): 8
>
>- HVXC (Harmonic Vector eXcitation Coding): 9
>
>- Reserved: 10
>
>- TTSI (Text-To-Speech Interface): 11
>
>- Main Synthesis: 12
>
>- Wavetable Synthesis: 13
>
>- General MIDI: 14
>
>- Algorithmic Synthesis and Audio FX: 15
>
>- ER (Error Resilient) AAC LC: 17
>
>- Reserved: 18
>
>- ER AAC LTP: 19
>
>- ER AAC Scalable: 20
>
>- ER TwinVQ: 21
>
>- ER BSAC (Bit-Sliced Arithmetic Coding): 22
>
>- ER AAC LD (Low Delay): 23
>
>- ER CELP: 24
>
>- ER HVXC: 25
>
>- ER HILN (Harmonic and Individual Lines plus Noise): 26
>
>- ER Parametric: 27
>
>- SSC (SinuSoidal Coding): 28
>
>- PS (Parametric Stereo): 29
>
>- MPEG Surround: 30
>
>- Escape Value: 31
>
>  * 32: Layer-1
>
>  * 33: Layer-2
>    34: Layer-3
>    35: DST (Direct Stream Transfer)
>    36: ALS (Audio Lossless)
>    37: SLS (Scalable LosslesS)
>    38: SLS non-core
>    39: ER AAC ELD (Enhanced Low Delay)
>    40: SMR (Symbolic Music Representation) Simple
>    41: SMR Main
>    42: USAC (Unified Speech and Audio Coding) (no SBR)
>    43: SAOC (Spatial Audio Object Coding)
>    44: LD MPEG Surround
>    45: USAC
>
>  
>
>  ****

>**Sampling Frequencies**
>
>- 96000 Hz: 0
>- 88200 Hz: 1
>- 64000 Hz: 2
>- 48000 Hz: 3
>- 44100 Hz: 4
>- 32000 Hz: 5
>- 24000 Hz: 6
>- 22050 Hz: 7
>- 16000 Hz: 8
>- 12000 Hz: 9
>- 11025 Hz: 10
>- 8000 Hz: 11
>- 7350 Hz : 12
>
>