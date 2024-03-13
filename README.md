<div id="top"></div>

[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]


<!-- PROJECT LOGO -->
<br />
<div align="center">
	<a href="https://github.com/MoonGrt/STM32-fly_control">
	<img src="images/logo.png" alt="Logo" width="80" height="80">
	</a>
<h3 align="center">STM32-fly_control</h3>
	<p align="center">
	This project entails a quadcopter built around the STM32F103C8T6 microcontroller. It utilizes an MPU6050 sensor to continuously gather attitude data, which is then processed using a dual-loop PID control algorithm to maintain stable flight. Additionally, the quadcopter is equipped with Bluetooth connectivity, enabling remote control and monitoring of its operations. Integrating both hardware and software design, this project aims to deliver a reliable and stable quadcopter system suitable for enthusiasts, students, and researchers in the field of unmanned aerial vehicles.
	<br />
	<a href="https://github.com/MoonGrt/STM32-fly_control"><strong>Explore the docs »</strong></a>
	<br />
	<br />
	<a href="https://github.com/MoonGrt/STM32-fly_control">View Demo</a>
	·
	<a href="https://github.com/MoonGrt/STM32-fly_control/issues">Report Bug</a>
	·
	<a href="https://github.com/MoonGrt/STM32-fly_control/issues">Request Feature</a>
	</p>
</div>


<!-- CONTENTS -->
<details open>
  <summary>Contents</summary>
  <ol>
    <li><a href="#file-tree">File Tree</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>


<!-- FILE TREE -->
## File Tree

```
└─ Project
  ├─ EventRecorderStub.scvd
  ├─ flight_control4.zip
  ├─ keilkill.bat
  ├─ Project.uvguix.Moon
  ├─ Project.uvoptx
  ├─ Project.uvprojx
  ├─ README.md
  ├─ /.vscode/
  ├─ /DebugConfig/
  ├─ /Hardware/
  │ ├─ LED.c
  │ ├─ LED.h
  │ ├─ mpu6050.c
  │ └─ mpu6050.h
  ├─ /images/
  ├─ /Library/
  ├─ /Listings/
  ├─ /Math/
  │ ├─ filter.c
  │ ├─ filter.h
  │ ├─ imu.c
  │ ├─ imu.h
  │ ├─ kalman.c
  │ ├─ kalman.h
  │ ├─ myMath.c
  │ ├─ myMath.h
  │ ├─ pid.c
  │ └─ pid.h
  ├─ /Objects/
  ├─ /Start/
  ├─ /System/
  │ ├─ delay.c
  │ ├─ delay.h
  │ ├─ GPIO.h
  │ ├─ I2C.c
  │ ├─ I2C.h
  │ ├─ TIM.c
  │ ├─ TIM.h
  │ ├─ USART.c
  │ └─ USART.h
  └─ /User/
    ├─ ALL_DATA.h
    ├─ ANO_DT.c
    ├─ ANO_DT.h
    ├─ control.c
    ├─ control.h
    ├─ main.c
    ├─ remote.c
    ├─ remote.h
    ├─ stm32f10x_conf.h
    ├─ stm32f10x_it.c
    └─ stm32f10x_it.h

```


<!-- CONTRIBUTING -->
## Contributing
Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.
If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!
1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request
<p align="right">(<a href="#top">top</a>)</p>


<!-- LICENSE -->
## License
Distributed under the MIT License. See `LICENSE` for more information.
<p align="right">(<a href="#top">top</a>)</p>


<!-- CONTACT -->
## Contact
MoonGrt - 1561145394@qq.com
Project Link: [MoonGrt/](https://github.com/MoonGrt/)
<p align="right">(<a href="#top">top</a>)</p>


<!-- ACKNOWLEDGMENTS -->
## Acknowledgments
* [Choose an Open Source License](https://choosealicense.com)
* [GitHub Emoji Cheat Sheet](https://www.webpagefx.com/tools/emoji-cheat-sheet)
* [Malven's Flexbox Cheatsheet](https://flexbox.malven.co/)
* [Malven's Grid Cheatsheet](https://grid.malven.co/)
* [Img Shields](https://shields.io)
* [GitHub Pages](https://pages.github.com)
* [Font Awesome](https://fontawesome.com)
* [React Icons](https://react-icons.github.io/react-icons/search)
<p align="right">(<a href="#top">top</a>)</p>


<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/MoonGrt/STM32-fly_control.svg?style=for-the-badge
[contributors-url]: https://github.com/MoonGrt/STM32-fly_control/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/MoonGrt/STM32-fly_control.svg?style=for-the-badge
[forks-url]: https://github.com/MoonGrt/STM32-fly_control/network/members
[stars-shield]: https://img.shields.io/github/stars/MoonGrt/STM32-fly_control.svg?style=for-the-badge
[stars-url]: https://github.com/MoonGrt/STM32-fly_control/stargazers
[issues-shield]: https://img.shields.io/github/issues/MoonGrt/STM32-fly_control.svg?style=for-the-badge
[issues-url]: https://github.com/MoonGrt/STM32-fly_control/issues
[license-shield]: https://img.shields.io/github/license/MoonGrt/STM32-fly_control.svg?style=for-the-badge
[license-url]: https://github.com/MoonGrt/STM32-fly_control/blob/master/LICENSE

