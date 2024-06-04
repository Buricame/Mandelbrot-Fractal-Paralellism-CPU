// ParalelOdev1.cpp : Bu dosya 'main' işlevi içeriyor. Program yürütme orada başlayıp biter.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <thread>
#include <chrono>

using namespace std;
using namespace std::chrono;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const int WIDTH = 6400;
const int HEIGHT = 4800;
const int MAX_ITER = 1000;
const double LIMIT = 2.0;

void mandelbrotRow(int startRow, int endRow, vector<vector<int>>& image) {
    for (int row = startRow; row < endRow; row++) {
        for (int col = 0; col < WIDTH; col++) {
            double c_re = (col - WIDTH / 2.0) * 4.0 / WIDTH;
            double c_im = (row - HEIGHT / 2.0) * 4.0 / HEIGHT;

            double x = 0, y = 0;
            int iteration = 0;
            while (x * x + y * y <= LIMIT * LIMIT && iteration < MAX_ITER) {
                double x_new = x * x - y * y + c_re;
                y = 2 * x * y + c_im;
                x = x_new;
                iteration++;
            }

            if (iteration < MAX_ITER) {
                double ratio = (double)iteration / MAX_ITER;
                int red = (int)(9 * (1 - ratio) * ratio * ratio * 255);
                int green = (int)(15 * (1 - ratio) * (1 - ratio) * ratio * 255);
                int blue = (int)(8.5 * (1 - ratio) * (1 - ratio) * (1 - ratio) * ratio * 255);

                image[row][col] = (red << 16) + (green << 8) + blue; // BMP formatı: 0x00RRGGBB
            }
            else {
                image[row][col] = 0;
            }
        }
    }
}

int main() {
    vector<vector<int>> image(HEIGHT, vector<int>(WIDTH, 0));

    auto start = high_resolution_clock::now(); // Başlangıç zamanı

    int numThreads = thread::hardware_concurrency(); // Donanım üzerindeki iş parçacığı sayısını al
    numThreads = 4;
    cout << "Cekirdek Sayisi: " << numThreads << endl;

    vector<thread> threads;
    int rowsPerThread = HEIGHT / numThreads;
    for (int i = 0; i < numThreads; i++) {
        int startRow = i * rowsPerThread;
        int endRow = (i == numThreads - 1) ? HEIGHT : (i + 1) * rowsPerThread;
        threads.emplace_back(mandelbrotRow, startRow, endRow, ref(image));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    auto stop = high_resolution_clock::now(); // Bitiş zamanı
    auto duration = duration_cast<milliseconds>(stop - start); // Geçen süre

    ofstream outFile("mandelbrot.bmp", ios::binary);
    if (!outFile) {
        cerr << "Error: Unable to create mandelbrot.bmp" << endl;
        return 1;
    }

    // BMP dosyası başlığı
    const int BMP_HEADER_SIZE = 54;
    int fileSize = BMP_HEADER_SIZE + WIDTH * HEIGHT * 3;
    char header[BMP_HEADER_SIZE] = {
        'B', 'M',
        fileSize & 0xFF, (fileSize >> 8) & 0xFF, (fileSize >> 16) & 0xFF, (fileSize >> 24) & 0xFF,
        0, 0, 0, 0,
        BMP_HEADER_SIZE, 0, 0, 0,
        40, 0, 0, 0,
        WIDTH & 0xFF, (WIDTH >> 8) & 0xFF, (WIDTH >> 16) & 0xFF, (WIDTH >> 24) & 0xFF,
        HEIGHT & 0xFF, (HEIGHT >> 8) & 0xFF, (HEIGHT >> 16) & 0xFF, (HEIGHT >> 24) & 0xFF,
        1, 0,
        24, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0 
    };
    outFile.write(header, BMP_HEADER_SIZE);

    // BMP veri alanı (BGR sırası ile)
    for (int row = HEIGHT - 1; row >= 0; row--) {
        for (int col = 0; col < WIDTH; col++) {
            char pixel[3] = {
                (char)(image[row][col] & 0xFF),
                (char)((image[row][col] >> 8) & 0xFF),
                (char)((image[row][col] >> 16) & 0xFF)
            };
            outFile.write(pixel, 3);
        }
        // BMP, her satırı 4-byte tam sayıya hizalamak için gerektiği kadar boş bayt ekler
        const int ALIGNMENT = 4;
        int padding = (4 - (WIDTH * 3) % ALIGNMENT) % ALIGNMENT;
        char padByte = 0;
        outFile.write(&padByte, padding);
    }

    cout << "Mandelbrot set saved as mandelbrot.bmp" << endl;
    cout << "Execution time: " << duration.count() << " milliseconds" << endl;

    return 0;
}

