# EC-STM32F4-22200259

![Language](https://img.shields.io/badge/Language-C%20%2F%20C%2B%2B-blue)
![Platform](https://img.shields.io/badge/Platform-PlatformIO-orange)
![MCU](https://img.shields.io/badge/MCU-STM32F411RE%20(ARM%20Cortex--M4)-red)
![Framework](https://img.shields.io/badge/Framework-CMSIS-green)

한동대학교 전자제어공학부 **임베디드 컨트롤러(Embedded Controller)** 강의 실습 및 하드웨어 제어 라이브러리 개발 저장소입니다.

* **Author:** 22200259 명금준
* **Target Board:** STMicroelectronics NUCLEO-F411RE
* **Toolchain:** PlatformIO on VS Code (CMSIS)

---

## 🛠️ Hardware & Environment

* **MCU:** STM32F411RE (ARM Cortex-M4 with FPU, 84MHz)
* **IDE / Build System:** Visual Studio Code, PlatformIO
* **Driver Framework:** CMSIS (Direct Register Level Programming)

---

## 📁 Repository Structure

```text
├── include/       # 직접 설계 및 모듈화한 STM32 드라이버 라이브러리 (.h, .c)
│   ├── ecGPIO2.h / .c      # GPIO Digital In/Out 드라이버
│   ├── ecRCC2.h / .c       # HSI(16MHz) / PLL(84MHz) 클럭 설정
│   ├── ecEXTI2.h / .c      # 외부 인터럽트(EXTI) 설정 드라이버
│   ├── ecSysTick2.h / .c   # SysTick 하드웨어 기반 ms/us 정밀 지연 타이머
│   ├── ecStepper2.h / .c   # 28BYJ-48 스테퍼 모터 FSM 드라이버 (Full/Half)
│   └── ecUART2.h / .c      # USART 통신 드라이버
│
├── lab/           # 정규 실습 과제 (LAB)
│   ├── LAB_GPIO_7segment/  # 7-Segment 카운터 및 제어 실습
│   ├── LAB_EXTI/           # EXTI 외부 버튼 인터럽트 실습
│   ├── LAB_Stepper_Motor/  # 28BYJ-48 스테퍼 모터 FSM 및 정밀 RPM 제어
│   └── LAB_USART/          # USART 시리얼 통신 실습
│
├── tutorial/      # 기초 학습 및 테스트 튜토리얼 소스 코드
└── platformio.ini # 환경별 빌드 타깃 설정 파일
```

---

## 🚀 Key Libraries & Features

| 모듈명 | 주요 기능 및 특징 |
| :--- | :--- |
| **`ecGPIO2`** | GPIO 핀 모드(INPUT/OUTPUT/AF), 풀업/풀다운, 속도, ODR/IDR 레지스터 제어 |
| **`ecRCC2`** | PLL 클럭 트리 설정을 통한 $84\text{ MHz}$ 고속 시스템 클럭 초기화 |
| **`ecSysTick2`** | SysTick 24비트 하드웨어 카운터(`VAL`) 기반 Wrap-around 보정 **초정밀 `delay_us()`** 구현 |
| **`ecStepper2`** | 28BYJ-48 기어 감속비(1:64)를 고려한 Full(4상)/Half(8상) 무어 FSM, 일시정지 및 과열 방지 |
| **`ecEXTI2`** | 버튼(PC13) 폴링/인터럽트 처리 및 엣지 트리거 설정 |
| **`ecUART2`** | 비동기 직렬 통신(USART) 송수신 드라이버 |

---

## 💻 How to Build & Run

1. VS Code에서 PlatformIO 확장 프로그램을 설치합니다.
2. 저장소를 클론한 후 프로젝트 폴더를 엽니다.
3. `platformio.ini`의 `default_envs`를 실행할 프로젝트로 변경하거나 PlatformIO 태스크 창에서 원하는 환경을 선택합니다.
   ```ini
   [platformio]
   default_envs = LAB_Stepper_Motor
   ```
4. **Build:** `Ctrl + Alt + B` (또는 하단 체크 버튼)
5. **Upload:** `Ctrl + Alt + U` (또는 하단 화살표 버튼)
