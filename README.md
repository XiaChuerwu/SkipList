# SkipList存储引擎
基于跳表实现的轻量键值对数据库🌠🌠🌠。在随机读写情况下，该引擎每秒在多个并发线程下的累计执行成功次数都可以保持在30000左右。

# 提供接口

* insertElement （插入数据）
* deleteElement （删除数据）
* searchElement （查询数据）
* showSkipList  （展示已存数据）
* dumpFile      （数据落盘）
* loadFile      （加载数据）
* getSize       （返回数据规模）

# 环境要求
* Linux操作系统
* C++11编程语言

# 等待的优化
* delete的时候没有释放所有new出来的内存
* 引擎的key和value必须是int型和string型，可以考虑优化
* 可以考虑进一步改造为分布式存储服务
* 落盘数据时，如果原key存在不会覆盖掉而是直接在文件末位追加
* loadFile函数有小bug待优化