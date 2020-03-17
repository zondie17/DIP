# Point Processing

> 张祎维 计71 2017013640

## 1. 改变亮度 Brightness Change

分别将`bgr`值改变`g`之后，得到亮度改变的图，计算公式如下：
$$
J_k(r,c)=max(min(I_k(r,c)+g,255),0), k\in{1,2,3}
$$

#### 源图

<img src="./image/car.jpg" style="zoom:25%;" />

#### 亮度增加$50$时

<img src="./image/car_dimmer+50.jpg" style="zoom:25%;" />

#### 亮度减少$50$时

<img src="./image/car_dimmer-50.jpg" style="zoom:25%;" />

## 2. 对比度放缩 Contrast Stretch

分别将`bgr`值从相应原本的最小像素值和最大像素值`minI,maxI`范围，放缩到指定的最小像素值和最大像素值`minJ,maxJ`范围大小，其计算公式如下：
$$
J(r,c)=(M_J-m_J)\frac{I(r,c)-m_I}{M_I-m_I}+m_J
$$
其中
$$
m_I=min[I(r,c)], M_I=max[I(r,c)]\\
m_J=min[J(r,c)], M_J=max[J(r,c)]
$$

#### 源图

<img src="./image/car.jpg" style="zoom:25%;" />

#### 对于`rgb`分别放缩到$(0,250),(50,200),(100,150)$

<img src="./image/car_contrast_rgb.jpg" style="zoom:25%;" />

#### 对于`bgr`分别放缩到$(0,250),(50,200),(100,150)$

<img src="./image/car_contrast_bgr.jpg" style="zoom:25%;" />

## 3. 伽马校正 Gamma Correction

伽马校正可以改变直方图分布的分布曲线，改变图像色阶。

计算过程即将像素值从$[0,255]$线性压缩到$[0,1]$，之后取$\gamma$次幂校正后，再线性的放大到$[0,255]$范围。

其计算公式如下：
$$
J(r,c) = 255 [\frac{I(r,c)}{255}]^\gamma
$$
处理结果如下：

#### 源图

<img src="./image/car.jpg" style="zoom:25%;" />

#### $\gamma=0.5$时

<img src="./image/car_gamma_0.5.jpg" style="zoom:25%;" />

#### $\gamma=2.0$时

<img src="./image/car_gamma_2.0.jpg" style="zoom:25%;" />

## 4. 直方图均衡 Histogram Equalization

此处处理的时亮度的直方图均衡，即将`bgr`的直方图分布进行均衡处理，使得颜色分布更为均匀。

其映射函数`LUT`为`bgr`直方图分布的`CDF`线性映射到$[0,255]$

处理结果如下：

#### 源图

<img src="./image/car.jpg" style="zoom:25%;" />

#### `bgr`均衡处理

<img src="./image/car_equalize.jpg" style="zoom:25%;" />

#### 源图直方图

<img src="./image/pic/car.png" alt="1" style="zoom:85%;" />

#### 均衡处理后的直方图

<img src="./image/pic/car_equalize.png" style="zoom:85%;" />

## 5. 直方图匹配 Histogram Matching

对源图和目标图的`CDF`进行分析可进行相应的匹配规定化，使源图的`bgr`直方图和目标图的`bgr`直方图更相近。

可将源图中位于某一像素值的数量与目标图中位于相同像素值数量最相近的两个像素值配对，以此作为`LUT`。

其处理结果如下：

#### 源图

<img src="./image/car.jpg" style="zoom:25%;" />

#### 目标图

<img src="./image/target.jpg" style="zoom:24%;" />

#### 结果图

<img src="./image/matchImg.jpg" style="zoom:25%;" />

#### 源图直方图

<img src="./image/pic/car.png" style="zoom:85%;" />

#### 目标图直方图

<img src="./image/pic/target.png" style="zoom:85%;" />

#### 结果图直方图

<img src="./image/pic/matchImg.png" style="zoom:85%;" />

## 6. 遇到的问题

&emsp;&emsp;在处理直方图的均衡的时候，对于此处的`LUT`的效用开始不是很理解，之后通过和计72程佳文的交流以及重复阅读课件，理解了`LUT`的工作作用。

