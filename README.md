Mandelbrot Fractal Generator with Parallelization

This program generates the Mandelbrot fractal and utilizes parallelization to improve performance. Here are the key points:

Mandelbrot Set: The Mandelbrot set is a fascinating mathematical object that exhibits intricate self-similarity. It is defined by iterating a simple equation: (z_{n+1} = z_n^2 + c), where (z_n) is a complex number and (c) represents a point in the complex plane. If the iteration remains bounded, the point is part of the Mandelbrot set.

Parallelization: To speed up the computation, the program divides the image into rows and processes each row concurrently using multiple threads. This takes advantage of multi-core CPUs.

Coloring: The program assigns colors to points based on the number of iterations required for them to escape the specified limit. The color gradient represents the complexity of the fractal.

Output: The resulting Mandelbrot image is saved as a BMP file named “mandelbrot.bmp.”

Execution Time: The execution time is measured and printed to the console.
