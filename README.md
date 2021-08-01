# **Hey! Mask!**

> [![Release](https://img.shields.io/badge/Release-v1.0-2F9D27?style=for-the-badge&logo=GitLab&logoColor=white)](https://gitlab.com/BanB3515/HeyMask/-/releases)

---

> ## **[Hey! Mask! 로드맵](https://www.notion.so/banb/Hey-Mask-3148ffa35f3a4d53a6f3d350dcce538d)**

---

> ## **목차**
>
> 1. [Hey! Mask!란?](#Hey-Mask란)
> 2. [시연 영상](#시연-영상)
> 3. [개발 정보](#개발-정보)
> 4. [빌드 방법](#빌드-방법)

---

> ### **Hey! Mask!란?**
>
> -   마스크를 착용하지 않은 사람을 탐지하면 탐지된 프레임을 캡처 후 [HeyMaskViewer](https://github.com/banb3515/HeyMaskViewer)로 전송합니다.
> -   뷰어에서 마스크를 착용하지 않았던 사람들을 확인할 수 있습니다.

---

> ### **시연 영상**
>
> [![YouTube](https://img.youtube.com/vi/MhC4nj6zl-4/0.jpg)](https://www.youtube.com/watch?v=MhC4nj6zl-4)

---

> ### **개발 정보**
>
> -   언어 - [C++](https://isocpp.org/) 14
> -   라이브러리
>     -   [mpg123](https://www.mpg123.de/)
>     -   [libao](https://www.xiph.org/ao/)
>     -   [OpenCV 4.5.2](https://opencv.org/releases/)
>     -   [YOLOv4](https://github.com/AlexeyAB/darknet)

---

> ### **빌드 방법**
>
> -   필요 라이브러리 - [라이브러리 설치 (Ubuntu)](https://www.notion.so/Hey-Mask-3148ffa35f3a4d53a6f3d350dcce538d#67e2f52458144e38b6299406e1047475)
>     -   [mpg123](https://www.mpg123.de/)
>     -   [libao](https://www.xiph.org/ao/)
>     -   [OpenCV 4.5.2](https://opencv.org/releases/)
>     -   [CUDA 10.1](https://developer.nvidia.com/cuda-toolkit-archive) (CUDA 사용 시)
>     -   [cuDNN 7.6.5 for CUDA 10.1](https://developer.nvidia.com/cudnn) (CUDA 사용 시)
>
> ```bash
> git clone https://gitlab.com/BanB3515/HeyMask.git
> cd HeyMask
> mkdir build && cd build
> # CUDA 미 사용 시, cmake -D USE_CUDA=0 ..
> cmake ..
> make
> ./HeyMask ../heymask.ini
> ```
