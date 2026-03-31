# InkSight（Waveshare 4.2 精简固件说明版）

## 官方网址
**www.inksight.site**

## 源码来源
本仓库基于原项目修改而来：
- 原项目：<https://github.com/datascale-ai/inksight>
- 当前适配仓库：<https://github.com/xiaosheng123/inksight-WF>

本 README 主要说明当前仓库中 **Waveshare 4.2 寸屏幕精简固件** 的使用方式与现状。

## 当前结论
当前这份适配工作的主要结论如下：

- 已切换到 **Waveshare 4.2 驱动后端**
- 默认构建已去掉大部分旧的 **GxEPD2 / U8g2** 依赖
- **ESP32-C3** 与 **ESP32-WROOM32E** 两条 4.2 寸固件线均已成功编译
- 固件体积已明显瘦身，适合先做真机联调
- 当前重点是先保证 **黑白稳定显示**
- 更复杂的旧日历渲染链，后续再逐步迁回新的显示路径

## 固件发布目录
仓库内已提交的固件位于：

- `firmware/releases/`

### ESP32-C3（4.2 寸）

- `firmware/releases/waveshare_c3_4in2.bin`
- `firmware/releases/waveshare_c3_4in2_merged.bin`

### ESP32-WROOM32E（4.2 寸）

- `firmware/releases/waveshare_wroom32e_4in2.bin`
- `firmware/releases/waveshare_wroom32e_4in2_merged.bin`

## 推荐烧录文件
推荐优先使用 `*_merged.bin`：

- `waveshare_c3_4in2_merged.bin`
- `waveshare_wroom32e_4in2_merged.bin`

原因：

- 已合并 bootloader / partitions / app
- 直接从 `0x0` 烧录即可
- 不容易因为偏移地址写错而翻车

## 平台说明

### 1) ESP32-C3
适用于当前已验证的 C3 接线方案。

已知使用过的 4.2 寸屏幕接线为：

- SCK = GPIO4
- MOSI = GPIO6
- CS = GPIO7
- DC = GPIO1
- RST = GPIO2
- BUSY = GPIO10

### 2) ESP32-WROOM32E
WROOM32E 这条线按仓库中的板级固定引脚配置编译，适用于**带固定屏幕接口**的板卡方案。

注意：
- WROOM32E 开发板若自带屏幕接口，应以板卡原理图、丝印、官方说明为准
- 不建议机械照搬 C3 的杜邦线接法

## 当前构建状态

### 已完成
- `wft_4in2b`（ESP32-C3）编译通过
- `wft_4in2b_wroom32e`（ESP32-WROOM32E）编译通过
- 默认构建已瘦身成功
- 已将 C3 / WROOM32E bin 提交到仓库

### 当前默认方向
- 先使用 **Waveshare 4.2 黑白稳定链路**
- 先求稳，再补复杂页面渲染

## 编译环境
当前仓库在本地使用过的可用 PlatformIO 隔离环境为：

- `.venv-pio`

例如：

```bash
/root/.openclaw/workspace/.venv-pio/bin/pio run -e wft_4in2b
/root/.openclaw/workspace/.venv-pio/bin/pio run -e wft_4in2b_wroom32e
```

## 烧录建议
如果使用 merged 固件，通常可直接从 `0x0` 烧录。

请根据自己的芯片与串口环境，选择对应烧录方式，例如：

```bash
esptool.py --chip esp32c3 --port <PORT> --baud 460800 write_flash 0x0 firmware/releases/waveshare_c3_4in2_merged.bin
```

或：

```bash
esptool.py --chip esp32 --port <PORT> --baud 460800 write_flash 0x0 firmware/releases/waveshare_wroom32e_4in2_merged.bin
```

## 说明
这份 README 现在重点服务于：

- 固件产物位置说明
- C3 / WROOM32E 两套 4.2 寸固件区分
- 当前 Waveshare 精简后端的实际状态说明

如果后续把旧日历渲染完整迁回新链路，再继续补充更完整的功能说明。