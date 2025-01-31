## Anoconda的安装：

可以不装在c盘的

[andaconda&jupyter](./Jupyter Notebook安装与环境搭建.pdf)

注意配置环境变量，需要添加 /Anaconda3 和 /Anaconda3/Scripts。完成后可以通过 conda --version 来检查。

而后会在开始菜单中看见安装的conda navigator，可以尝试打开jupyter。

由于我使用的是 powershell 居多，需要配置conda的powershell配置。

在管理员模式的powershell（使用 ctrl+shift+enter进入）下执行：

```powershell
conda init powershell
Set-ExecutionPolicy Bypass -Scope CurrentUser
```

然后重新进入就可以使用了。



## 创建新的环境

一般是建议新开一个环境进行操作的。在建环境之前最好确定好各个依赖的版本。

pip的清华镜像源： -i https://pypi.tuna.tsinghua.edu.cn/simple

1. 创建环境：

   ```conda
   conda create --name your_env_name python=x.x
   ```

   可以切换到指定环境、退出环境：

   ```conda
   conda activate your_env_name
   conda deactivate
   ```

   要在新环境中配置jupyter notebook（一般是自己配好的）

   

   在指定的环境下配置 ipykernel，可以分离各个环境的运行程序，操作如下：

   ```
   pip install ipykernel ipython
   python -m ipykernel install --user(只为当前用户添加) --name your_env_name --display-name "Your Kernel Name"
   jupyter kernelspec list/remove 可以查看\删除
   ```

   

   

   若只需要使用CPU进行计算（比较慢），以下环境是可以的：

   ```
   python==3.9.7(在配环境时完成)
   tensorflow==2.12.0
   matplotlib==3.5.2
   numpy==1.23.5
   pandas==1.4.4
   numba==0.56.4
   daal==2021.2.3
   ** TBB的依赖可能有问题，暂时没有解决，但是使用上没有问题。
   ```

   如果使用GPU，首先需要下好NVIDIA的CUDA和CUDNN（参考[配置方法](https://blog.csdn.net/weixin_44779079/article/details/141528972)，pip则可以用这个环境：

```
python==3.9.7 (在配环境时完成)
tensorflow==2.10.0 
tensorflow-gpu==2.10.0 （只支持到CUDA 11.2还是多少，但是这个CUDA和win11可能有兼容性问题，没配成）
matplotlib==3.5.2 
numpy==1.23.5 
pandas==1.4.4 
numba==0.56.4 
daal==2021.2.3
```

