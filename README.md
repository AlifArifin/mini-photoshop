# Mini Photoshop
Desktop based app for image processing from scratch (without using any image processing library) such as image operation, image enhancement, edge detection, etc.

## Features
- read file (PGM, PPM, PBM, BMP and raw image) and its properties
- basic image operation
  - negative
  - grayscale
  - image brightening
  - arithmetic operation between 2 image (add, reduce, multiply, divide)
  - boolean operation (AND, OR, NOT)
  - geometric operation (translation, rotating, flipping, zooming)
- histogram and normalized histogram
- image enhancement
  - image brightening
  - contrast stretching
  - log and inverse log transformation
  - power transformation
  - gray-level slicing
  - bit-plane slicing
  - histogram spesification
  - median filtering
  - high pass filtering
  - unsharp masking
  - highbosst filter
- edge detection
  - gradien
  - laplace
  - log of gaussian
  - sobel
  - prewitt
  - roberts
  - canny
- hough transformation for line and circle detection
- binary transformation
- thinning
- vehicle plat number detection

## Getting Started
These instructions will get you a copy of the project up and running on your local machine for development.

### Prerequisites
What things you need to install the software
- [Qt](https://www.qt.io/)
- C++

### Using
Open Qt and choose `Tes2.pro` as a project and run it.

## Built With

- [Qt](https://www.qt.io/) Desktop Application Framework

## Authors
* **Muhammad Alif Arifin** - *Image Processing and Dekstop Application* - [AlifArifin](https://github.com/AlifArifin)
