# InkSight（Waveshare 4.2 精简固件说明版）

## 官方网址
**www.inksight.site**

## 源码来源
本仓库基于原项目修改而来：
- 原项目：<https://github.com/datascale-ai/inksight>
- 当前适配仓库：<https://github.com/xiaosheng123/inksight-WF>

本 README 主要说明当前仓库中 **Waveshare 4.2 寸屏幕精简固件** 的使用方式与现状。

## 目标屏幕型号（务必确认）

当前适配工作默认针对这块墨水屏：

- **WFT0420CZ15LW** —— 微雪 4.2" 三色 e-Paper，分辨率 400×300  
  - 官方资料通常以名称 **“4.2inch e-Paper (B) V2”** 出现在 PDF 中：  
    `4.2inch_e-Paper_(B)_V2.pdf`  
  - 本仓库后续提到的 “4.2 屏 / Waveshare 4.2 三色” 如无特别说明，均指 **WFT0420CZ15LW 这一 WF 开头型号**。

如果你手上的屏不是这一型号（例如 4in2G、4.2" 纯黑白等），请谨慎直接使用本仓库固件。

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

### ESP32-C3（WFT0420CZ15LW 4.2"）

新命名（推荐使用）：

- `firmware/releases/WFT0420CZ15LW_esp32c3_inksight.bin`
- `firmware/releases/WFT0420CZ15LW_esp32c3_inksight_merged.bin`

旧命名（兼容保留）：

- `firmware/releases/waveshare_c3_4in2.bin`
- `firmware/releases/waveshare_c3_4in2_merged.bin`

### ESP32-WROOM32E（WFT0420CZ15LW 4.2"）

基于淘宝官方板卡引脚映射（BUSY→9, RST→10, DC→11, DIN→12, SCK→15, CS→16）：

- `firmware/releases/WFT0420CZ15LW_wroom32e_tb_board.bin`
- `firmware/releases/WFT0420CZ15LW_wroom32e_tb_board_merged.bin`

旧命名（仓库早期版本保留）：

- `firmware/releases/waveshare_wroom32e_4in2.bin`
- `firmware/releases/waveshare_wroom32e_4in2_merged.bin`

## 推荐烧录文件
推荐优先使用带 `*_merged.bin` 后缀的固件：

- `WFT0420CZ15LW_esp32c3_inksight_merged.bin`
- `WFT0420CZ15LW_wroom32e_tb_board_merged.bin`

原因：

- 已合并 bootloader / partitions / app
- 直接从 `0x0` 烧录即可
- 不容易因为偏移地址写错而翻车

## 平台说明

### 1) ESP32-C3
适用于当前已验证的 C3 接线方案，对应 WFT0420CZ15LW 屏幕。

已知使用过的 4.2 寸屏幕接线为：

- SCK = GPIO4
- MOSI = GPIO6
- CS = GPIO7
- DC = GPIO1
- RST = GPIO2
- BUSY = GPIO10

### 2) ESP32-WROOM32E
WROOM32E 这条线按仓库中的板级固定引脚配置编译，适用于**带固定屏幕接口**的板卡方案。

当前使用的引脚映射（来自淘宝官方详情页）：

- BUSY → GPIO9
- RST  → GPIO10
- DC   → GPIO11
- DIN  → GPIO12
- SCK  → GPIO15
- CS   → GPIO16

注意：
- 若你的 WROOM32E 开发板自带屏幕接口，应以板卡原理图、丝印、官方说明为准
- 不建议机械照搬 C3 的杜邦线接法

## 当前构建状态

### 已完成
- `wft_4in2b`（ESP32-C3, WFT0420CZ15LW）编译通过
- `wft_4in2b_wroom32e`（ESP32-WROOM32E, WFT0420CZ15LW 淘宝板）编译通过
- 默认构建已瘦身成功
- 已将 C3 / WROOM32E bin 提交到 `firmware/releases/`

### 当前默认方向
- 先使用 **Waveshare 4.2 黑白稳定链路**（面向 WFT0420CZ15LW）
- 先求稳，再补复杂页面渲染与三色显示

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
esptool.py --chip esp32c3 --port <PORT> --baud 460800 \
  write_flash 0x0 firmware/releases/WFT0420CZ15LW_esp32c3_inksight_merged.bin
```

或：

```bash
esptool.py --chip esp32 --port <PORT> --baud 460800 \
  write_flash 0x0 firmware/releases/WFT0420CZ15LW_wroom32e_tb_board_merged.bin
```

## 说明
这份 README 现在重点服务于：

- 固件产物位置说明
- C3 / WROOM32E 两套 WFT0420CZ15LW 4.2 寸固件区分
- 当前 Waveshare 精简后端的实际状态说明

如果后续把旧日历渲染完整迁回新链路，再继续补充更完整的功能说明。
