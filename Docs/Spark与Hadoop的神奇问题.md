## Spark

Spark必须要安装Hadoop吗？在运行过程中发现如果没有安装Hadoop可以正常的启动Spark，但是提交一个work后就会无限循环的运行，不知道为什么。于是尝试安装Hadoop。

一些指令的记录：

```shell
spark-class.cmd org.apache.spark.deploy.master.Master
spark-class.cmd org.apache.spark.deploy.worker.Worker spark://localhost:7077
（以上两条在 spark/bin/ 中 运行，针对windows的指令）
E:\softwares\spark-3.5.4-bin-hadoop3\spark-3.5.4-bin-hadoop3\bin\spark-submit.cmd --class "SimpleApp" --master spark://2.0.0.1:7077 target/se3353_25_spark_java-1.0-SNAPSHOT.jar
（这条在Hadoop的dfs 和 Spark的master和worker 启动后运行，注意修改地址为真实地址）
```

在此基础上进行MapReduce的开发。



## Hadoop

尝试安装了无数个版本的Hadoop，都在启动的时候或多或少有一些问题。

1. 使用 **java8**  和 Hadoop 3.x 的版本时，每次启动用到文件（dfs&yarn，有一个manager是正常的）时都会报错：

   ```
   java.lang.UnsatisfiedLinkError: org.apache.hadoop.io.nativeio.NativeIO$Windows.access0(Ljava/lang/String;I)Z
   ```

   这是由于一个windows下插件未能正常加载导致的，一般来说使用了winutil.exe 并 正确配置 hadoop.dll（如加环境变量，复制到 C:\Windows\System32等）后就可以正常使用了，但是我并没有通过这种方式解决，非常奇怪。~~卡了我四五个小时~~ 还有一种方式是直接修改Hadoop的源码，把抛出异常的地方直接改成return true，但是由于没时间学怎么从src来安装Hadoop了，就埋个坑。

2. 使用 **java21**（即我正常配置在环境变量里最常用的版本），会在启动 yarn 时出现错误：

   ```
   Caused by: java.lang.reflect.InaccessibleObjectException: Unable to make protected final java.lang.Class java.lang.ClassLoader.defineClass(java.lang.String,byte[],int,int,java.security.ProtectionDomain) throws java.lang.ClassFormatError accessible: module java.base does not "opens java.lang" to unnamed module @30c15d8b
   ```

   这通常是由于 java 的版本问题导致的，搜索后发现可以通过降版本到 **java8** 来解决，但是降版本后又会出现上述的其他奇奇怪怪的问题，可以运行的东西更少，遂放弃。



