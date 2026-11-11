# SoftwareAES / Pong 22H374

合并 UniXNU SoftwareAES 的 `NativeAES.cpp`、`OriginalAESABI.h` 和原许可证。实现 AES-CBC 软件密钥接口；不实现 Apple UID/GID 等硬件密钥，不提供 SEP 或安全存储。

`scripts/build-pong-crypto18.py` 使用当前22H374原始 IOCryptoAcceleratorFamily：以实际编译的181项接口、162项共享方法、AES-CBC父类、metaclass getter 和两种构造/析构形式定位绑定，再加当前prelink slide。没有复用上游其他OS的绝对地址。生成的绑定和恢复镜像只留本机。

PongPower归档链接该对象；C++ metaclass初始化延后到原始IOAESAccelerator父类真正注册之后。原始corecrypto启动保持不变。只有真实CBC自检/接口检查成功才发布NativeAES；依赖失败记录原因并退出可选启动，不阻塞开机。

该实现只提供已实现的软件AES算法。它不会替换已有keychain/配对记录、伪造硬件密钥成功或声称硬件AES加速。
